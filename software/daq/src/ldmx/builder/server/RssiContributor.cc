// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/RssiContributor.cc
  \brief  RSSI emulation contributor to LDMX event builder
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the LDMX software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the terms 
   ontained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.01.14 jjr Created
 *
\* ---------------------------------------------------------------------- */

#define __STDC_FORMAT_MACROS 1

#include "RssiContributor.hh"
#include "RssiConnection.hh"

#include "ldmx/data/server/GenericGenerator.hh"
#include "ldmx/data/server/GenericBatch.hh"
#include "ldmx/utl/Timestamp.hh"

#include <rogue/protocols/packetizer/CoreV2.h>
#include <rogue/protocols/packetizer/Application.h>
#include <rogue/interfaces/stream/Frame.h>
#include <rogue/interfaces/stream/FrameIterator.h>
#include <rogue/interfaces/stream/Buffer.h>
#include <rogue/Helpers.h>

#include <cinttypes>
#include <cstdlib>

static const size_t NBytesBatch = 32*1024;

/* ---------------------------------------------------------------------- *//*!

  \brief Dummy constructor to satisfy array initialization

\* ---------------------------------------------------------------------- */
RssiContributor::RssiContributor () :
   m_stats           (),
   m_GenericGenerator    (0),
   m_GenericBatch        (malloc (NBytesBatch), NBytesBatch),
   m_neventsPerBatch (0)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Constructor for an RSSI contributor

  \param[in]        serverId  A small integer identifying this contributor
  \param[in]      rceAddress  The address of the source RCE
  \param[in] neventsPerBatch  Number of events per batch
  \param[in]        clientId  The event builder's ip
  \param[in]      clientPort  The port the event builder should connect to
  \param[in]         nframes  The number of frames used to buffer the 
                              incoming data

\* ---------------------------------------------------------------------- */
RssiContributor::RssiContributor (int              serverId,
                                  uint8_t        rceAddress,
                                  int       neventsPerBatch,
                                  const char      *clientIp,
                                  uint16_t       clientPort,
                                  int               nframes) :
   m_connection      (serverId, clientIp, clientPort, nframes),
   m_stats           (),
   m_GenericGenerator    (rceAddress),
   m_GenericBatch        (malloc (NBytesBatch), NBytesBatch),
   m_neventsPerBatch (neventsPerBatch)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Constructor for an RSSI contributor shared pointer

  \param[in]        serverId A small integer identifying this contributor
  \param[in]      rceAddress The address of the source RCE
  \param[in] neventsPerBatch Number of events per batch
  \param[in]        clientId The event builder's ip
  \param[in]      clientPort The port the event builder should connect to
  \param[in]         nframes The number of frames used to buffer the 
                             incoming data

\* ---------------------------------------------------------------------- */
std::shared_ptr<RssiContributor> 
RssiContributor::construct (int           serverId,
                            uint8_t     rceAddress,
                            int    neventsPerBatch,
                            const char   *clientIp,
                            uint16_t    clientPort,
                            int            nframes)
{
   std::shared_ptr<RssiContributor>
      snd = std::make_shared<RssiContributor>(serverId,
                                              rceAddress,
                                              neventsPerBatch,
                                              clientIp, 
                                              clientPort, 
                                              nframes);

   /// --- streamConnect (snd, snd->m_connection.m_pack->application(0));
   rogueStreamConnect (snd, snd->m_connection.m_pack->application(0));
   return snd;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Prototype for a data header
                                                                          */
/* ---------------------------------------------------------------------- */
class Header
{
public:
   Header (uint32_t   nbytes, 
           uint32_t       id, 
           uint32_t sequence, 
           uint64_t  trgTime, 
           uint64_t  sndTime) :
      m_nbytes     (nbytes),
      m_id           (  id),
      m_sequence (sequence),
      m_trgTime   (trgTime),
      m_sndTime   (sndTime)
      {
         return;
      }


public:
   uint32_t   m_nbytes; /*!< The size in bytes                            */
   uint32_t       m_id; /*!< The server identification                    */
   uint32_t m_sequence; /*!< The sequence number                          */
   uint32_t     m_rsvd; /*!< Reserved for future use                      */
   uint64_t  m_trgTime; /*!< The 64-bit trigger time, in nanoseconds      */
   uint64_t  m_sndTime; /*!< The 64-bit send    time, in nanoseconds      */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Fills in some fake data
  \return The number of events in the current batch

  \param[in] nmultisamples The number of multisamples per event
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t RssiContributor::generate (unsigned short int       nevents,
                                    unsigned short int nmultisamples)
{
   m_GenericGenerator.reset    (&m_GenericBatch);
   m_GenericGenerator.generate (&m_GenericBatch, 
                            nevents, 
                            nmultisamples);

   return nevents;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Fills in some fake data one event
  \return A flag indicating whether the Generic batch is filled

  \param[in] nmultisamples The number of multisamples per event
                                                                          */
/* ---------------------------------------------------------------------- */
bool RssiContributor::generateEvent (unsigned short int nmultisamples)
{
   uint32_t nevents = m_GenericBatch.getNevents ();
   if (nevents == 0)  m_GenericGenerator.addHeader (&m_GenericBatch);

   
   nevents = m_GenericGenerator.addEvent (&m_GenericBatch, nmultisamples);
   bool filled = (nevents == m_neventsPerBatch);

   return filled;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Send the current batch of data 
                                                                          */
/* ---------------------------------------------------------------------- */
void RssiContributor::sendBatch ()
{
   // ----------------------------------------------------------
   // Retrieve the address of the Generic batch buffer and it length
   // ----------------------------------------------------------
   void const *buffer = m_GenericBatch.getBuffer ();
   uint32_t    nbytes = m_GenericBatch.getNbytes ();


   Statistics prv = m_stats;
   sendData (reinterpret_cast<const uint8_t *>(buffer), nbytes);
   Statistics cur = m_stats;

   static int Count = 0;
   if (--Count <= 0)
   {
      Statistics::print_title ();
      Count = 32;
   }


   Statistics::print (m_connection.m_serverId,
                      &cur,
                      &prv,
                      true, 
                      '\n');

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Resets/clears the Generic batch
                                                                          */
/* ---------------------------------------------------------------------- */
void RssiContributor::resetBatch ()
{
   m_GenericGenerator.reset (&m_GenericBatch);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief  Sends the specified data

   \param[in]  data  The data to send
   \param[in]  size  The number of bytes to send

\* ---------------------------------------------------------------------- */   
void RssiContributor::sendData (uint8_t const *data, uint32_t size) 
{
   rogue::interfaces::stream::FramePtr frame;
   rogue::interfaces::stream::FrameIterator it;


   //auto volatile &stats  = m_stats;
   Statistics::gettime (&m_stats.m_time);

   // Request frame
   frame = reqFrame(size,true);

   // Get data write iterator
   it = frame->beginWrite();

   std::copy(data,data+size,it);

   // Set new frame size
   frame->setPayload(size);
   
   //Send frame
   sendFrame(frame);
   
   // Update counters
   m_stats.m_evtCount += m_neventsPerBatch;
   m_stats.m_txCount  += 1;
   m_stats.m_txBytes  += size;
   m_stats.m_txLast    = size;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  The constructor to initialize the statistics keep by the
           contributor

\* ---------------------------------------------------------------------- */
RssiContributor::Statistics::Statistics () :
   m_evtCount (0),
   m_txCount  (0),
   m_txBytes  (0),
   m_txLast   (0),
   m_packDrop (0),
   m_rssiDrop (0)
{
   gettime (&m_time);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief  Callable constructor to initialize the statistics keep by the
           contributor

\* ---------------------------------------------------------------------- */
void RssiContributor::Statistics::construct () volatile
{
   m_evtCount = 0;
   m_txCount  = 0;
   m_txBytes  = 0;
   m_txLast   = 0;
   m_packDrop = 0;
   m_rssiDrop = 0;

   gettime (&m_time);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  The constructor to initialize to a set of volatile statistics

   \param[in] stats  The volatile statistics to copy
                                                                          */
/* ---------------------------------------------------------------------- */
RssiContributor::Statistics::
                 Statistics (Statistics volatile &stats) :
   m_evtCount (stats.m_evtCount),
   m_txCount  (stats.m_txCount),
   m_txBytes  (stats.m_txBytes),
   m_txLast   (stats.m_txLast),
   m_packDrop (stats.m_packDrop),
   m_rssiDrop (stats.m_rssiDrop)
{
   m_time = const_cast<const struct timespec &>(stats.m_time);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the current monotonic increasing time

  \param[out]  ts  The returned struct timespec

\* ---------------------------------------------------------------------- */
void RssiContributor::Statistics::gettime (struct timespec *ts)
{
   clock_gettime (CLOCK_MONOTONIC_COARSE, ts);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the current monotonic increasing time

  \param[out]  ts  The returned struct timespec

  \note
   The twist here is that the output timespec is volatile.  This just
   hides the ugly cast.

\* ---------------------------------------------------------------------- */
void RssiContributor::Statistics::gettime (struct timespec volatile *ts)
{
   clock_gettime (CLOCK_MONOTONIC_COARSE, 
                  const_cast<struct timespec *>(ts));
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the difference of two timespecs
  \return The difference in nanoseconds

  \param[in]  start The starting time
  \param[in]   stop The stopping time

\* ---------------------------------------------------------------------- */
int64_t RssiContributor::Statistics::subtime (struct timespec const *start,
                                              struct timespec const  *stop)
{
   int32_t diff_ns =  stop->tv_nsec - start->tv_nsec;
   int64_t elapsed = (stop->tv_sec  - start->tv_sec);
   
   elapsed *= 1e9;
   elapsed += diff_ns;
   
   //printf ("Diff = %" PRIu64 " % "PRId32 "\n", elapsed, diff_ns);
   
   return elapsed;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Prints the periodic statistics

  \param[in]    cur  The current statistics
  \param[in]    prv  The previous statistics; used to form differences and
                     rates
  \param[in] opened  Flag indicating the contributor is connected to the
                     sender
  \param[in]    eol  Either a '\n' or '\r'
                                                                          */
/* ---------------------------------------------------------------------- */
void RssiContributor::Statistics::
     print (int                               which,
            RssiContributor::Statistics const  *cur, 
            RssiContributor::Statistics const  *prv, 
            bool                             opened,
            char                                eol)
{
   // ---------------------------------------------
   // Get the time since the last set of statistics
   // ---------------------------------------------
   double     elapsed = Statistics::subtime (&prv->m_time, 
                                             &cur->m_time);

   uint32_t       cnt = cur->m_evtCount - prv->m_evtCount;
   uint64_t diffBytes = cur->m_txBytes - prv->m_txBytes;
   uint32_t  curBytes = cur->m_txLast;
   uint64_t  totBytes = cur->m_txBytes;
   double        gbps = (((float)(diffBytes * 8)) / elapsed);
   float         rate =   ((float)cnt / elapsed) * 1e9;

  
   // If no change
   if (diffBytes == 0) 
   {
      fflush (stdout);
   }
   else
   {                  
      //         cnt  rate Gbps open drop rssi    drop pack  count         last         total
      printf (" %5u %6u %6.1f %7.3f %9d %8" PRIu32 " %8" PRIu32 " %6" PRIu32 " %8" PRIu32 " %10" PRIu64 "%c",
              which,
              cur->m_evtCount,
              rate,
              gbps, 
              opened, 
              cur->m_rssiDrop, 
              cur->m_packDrop,
              cur->m_txCount,
              curBytes, 
              totBytes,
              eol);
   }


   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Puts of the statistics/status title line

\* ---------------------------------------------------------------------- */
void RssiContributor::Statistics::print_title ()
{
   puts (
   " Which Events   Rate    Gbps Connected DropRssi DropPack  Count Tx  Last   Tx Total\n"
   " ----- ------ ------ ------- --------- -------- -------- ------ -------- ----------");

    return;
}
/* ---------------------------------------------------------------------- */
