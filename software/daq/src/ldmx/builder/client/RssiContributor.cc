// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/RssiContributor.cc
  \brief  RSSI Contributor to LDMX event builder
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level
   directory of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim
   No part of the LDMX software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the
   terms contained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.02.12 jjr Moved to namespace hps::builder::client
 * 2018.12.19 jjr Created
 *
\* ---------------------------------------------------------------------- */

#define __STDC_FORMAT_MACROS 1

#include "GenericContributor.hh" 
#include "RssiContributor.hh"
#include "RssiConnection.hh"
#include "Contribution.hh"
#include "Contributions.hh"
#include "ldmx/utl/ListsInterlocked.hh"
#include "ldmx/utl/List.hh"
#include "ldmx/utl/Timestamp.hh"
#include "ldmx/utl/Dumper.hh"

#include "ldmx/builder/client/CfgContributor.hh"

#include <rogue/protocols/rssi/Client.h>
#include <rogue/protocols/packetizer/CoreV2.h>
#include <rogue/protocols/packetizer/Application.h>
#include <rogue/interfaces/stream/FrameIterator.h>
#include <rogue/interfaces/stream/Buffer.h>
#include <rogue/protocols/batcher/CoreV1.h>
#include <rogue/protocols/batcher/Data.h>

#include <cinttypes>
#include <iostream>

/* ---------------------------------------------------------------------- */
static void init (void      *initPrms,
                  void        *packet,
                  int     packet_size,
                  int   packet_offset);


static void dumpFrame (std::shared_ptr<rogue::interfaces::stream::Frame> frame)
          __attribute__ ((unused));


static void copyFrame (uint8_t                                            *dst,
                       std::shared_ptr<rogue::interfaces::stream::Frame> frame)
          __attribute__ ((unused));


static void dumpData  (std::shared_ptr<rogue::protocols::batcher::Data>  &data)
          __attribute__ ((unused));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief The parameters to initialize the static portion of the 
         contribution packets
                                                                          */
/* ---------------------------------------------------------------------- */
class InitPrms
{
public:
   InitPrms (ldmx::utl::FixedPacket *fpa, int id) :
      m_fpa (fpa),
      m_id  ( id)
   {
     return;
   }

public:
   ldmx::utl::FixedPacket *m_fpa;
   int                      m_id;
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Packet initializer
  
  \param[in] init_prms     Pointer to the packet initialization parameters
  \param[in] packet        The packet to initialize
  \param[in] packet_size   The size, in bytes, of the packet
  \param[in] packet_offset The offset, in bytes, of the packet's control
                           structure, this is typically a link pointer
                                                                          */
/* ---------------------------------------------------------------------- */
static void init (void *init_prms,
                  void *packet,
                  int   packet_size,
                  int   packet_offset)
{
   Contribution    *ctb = reinterpret_cast<decltype( ctb)>(packet);
   InitPrms const *prms = reinterpret_cast<decltype(prms)>(init_prms);

   ctb->m_fpa  = prms->m_fpa;
   ctb->m_type = Contribution::Type::Generic;
   ctb->m_id   = prms->m_id;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the RSSI Contributor

  \param[in]           id  A small identifying integer 
  \param[in]       cfgCtb  The contribution configuration parameters
  \param[in] contributions The common aggregator for contributions
  \param[in]    packetSize The size of each contribution
                                                                          */
/* ---------------------------------------------------------------------- */
RssiContributor::
RssiContributor (int                                              id,
                 ldmx::builder::client::CfgContributor const &cfgCtb,
                 Contributions                        *contributions,
                 int                                      packetSize) :
   m_connection                          (cfgCtb),
   m_contributions                (contributions),
   m_list   (contributions->m_lists.getList (id)),
   m_stats                                     (),
   m_id                                      (id)
{
   InitPrms initPrms (&m_fpa, id);
   new (&m_fpa) ldmx::utl::FixedPacket (cfgCtb.m_nevents,
                                        packetSize, 
                                        0, 
                                        init, reinterpret_cast<void *>(&initPrms));



   // Carry the pool with the list so contributors can be allocated/freed
   contributions->m_lists.setUser (m_list, &m_fpa);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Callable constructor for the RSSI contributor

  \param[in]           id  A small identifying integer 
  \param[in]           dsc The connection description, basically, ip, port
  \param[in] contributions The common aggregator for contributions
  \param[in]     packetCnt The number of packets to hold contributions
  \param[in]    packetSize The size of each contribution
                                                                          */
/* ---------------------------------------------------------------------- */
std::shared_ptr<RssiContributor> RssiContributor::
     construct (int                                              id,
                ldmx::builder::client::CfgContributor const &cfgCtb,
                Contributions                        *contributions,
                int                                      packetSize)
{
   std::shared_ptr<RssiContributor> 
        rcv = std::make_shared<RssiContributor>(id,
                                                cfgCtb,
                                                contributions,
                                                packetSize);

   /// --- rcv->m_connection.m_pack->application(0)->setSlave (rcv);
   rcv->m_connection.m_pack->application(0)->addSlave (rcv);
   return rcv;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Accepts a new incoming frame

  \param[in] frame  The incoming frame

\* ---------------------------------------------------------------------- */
void RssiContributor::
     acceptFrame (std::shared_ptr<rogue::interfaces::stream::Frame> frame ) 
{
   Statistics::gettime (&m_stats.m_time);

   auto    nbytes     = frame->getPayload();
   m_stats.m_rxLast   = nbytes;
   m_stats.m_rxBytes += nbytes;
   m_stats.m_rxCount += 1;


   m_stats.m_rssiDrop = m_connection.m_rssi->getDropCount();
   m_stats.m_packDrop = m_connection.m_pack->getDropCount();

      
   auto err = frame->getError ();
   if (err)
   {
      std::cout << "Frame error: " << err << std::endl;
   }


   // Batcher decoder
   std::shared_ptr<rogue::protocols::batcher::CoreV1> 
          core = std::make_shared<rogue::protocols::batcher::CoreV1>();

   bool           okay = core->processFrame (frame);
   uint32_t eventCount = core->count        ();
   uint32_t headerSize = core->headerSize   ();
   uint32_t sequence   = core->sequence     ();

   printf ("Status     = %d\n"
           "HeaderSize = %8.8" PRIx32 "\n"
           "EventCount = %8.8" PRIx32 "\n"
           "Sequence   = %8.8" PRIx32 "\n",
           okay,
           headerSize,
           eventCount,
           sequence);

   /// --- dumpFrame (frame);


   // -------------------
   // Notify of reception !!!!
   // -------------------
   //// ----   auto    *list = m_list;
   auto           rcvTime = ldmx::utl::Timestamp::current ();
   ldmx::utl::List lclList;


   // -------------------------------------
   // Extract the individual batcher events
   // and place them on the local list.
   // -------------------------------------
   for (unsigned ievt = 0; ievt < eventCount; ievt++)
   {
      std::shared_ptr<rogue::protocols::batcher::Data> 
                                                &data  = core->record (ievt);
      rogue::interfaces::stream::FrameIterator   dIter = data->begin ();


      // ----------------------------------------
      // Get the event's sequence number and size
      // ----------------------------------------
      
      uint32_t *ptr     = reinterpret_cast<decltype (ptr)>(dIter.ptr ());
      uint32_t sequence = *ptr;
      auto         size = data->size ();


      ///printf ("Id:Sequence:Size = %2" PRIx32 ":%8" PRIx32 ":%8" PRIx32 "\n", 
      ///        m_id, sequence, (uint32_t)size);


      // ---------------------------------------------------------
      // Allocate a node to hold a description of this data record
      // ---------------------------------------------------------
      GenericContributor *generic = new (m_fpa) GenericContributor (    size,
                                  sequence,
                                   rcvTime,
                                      ievt,
                                     core);


      // ----------------------------
      // Place node on the local list
      // ----------------------------
      lclList.insert (generic);
      ///dumpData (data);
   }

   // printf ("Posting events ctbs = %p[%d]\n", 
   ///        (void *)(m_contributions), (int)m_id);
   m_contributions->m_lists.append (m_id, &lclList);


#if 0
   {  // this is more for providing a template on how to do the copy
      uint32_t *dst = reinterpret_cast<decltype (dst)>(malloc (nbytes));
      copyFrame (dst, frame);
      free (dst);
   }
#endif
   
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief  The constructor to initialize the statistics keep by the
           contributor

\* ---------------------------------------------------------------------- */
RssiContributor::Statistics::Statistics () :
   m_rxCount  (0),
   m_rxBytes  (0),
   m_rxLast   (0),
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
   m_rxCount  = 0;
   m_rxBytes  = 0;
   m_rxLast   = 0;
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
RssiContributor::Statistics::Statistics (RssiContributor::Statistics volatile &stats) :
   m_rxCount  (stats.m_rxCount),
   m_rxBytes  (stats.m_rxBytes),
   m_rxLast   (stats.m_rxLast),
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
   int32_t diff_ns = stop->tv_nsec - start->tv_nsec;
   int64_t elapsed = (stop->tv_sec - start->tv_sec);
   
   elapsed *= 1e9;
   elapsed += diff_ns;
   
   //printf ("Diff = %" PRIu64 " % "PRId32 "\n", elapsed, diff_ns);
   
   return elapsed;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

   \brief  Checks if the connection has been established

\* ---------------------------------------------------------------------- */

bool RssiContributor::isConnected () const
{
   bool   connected= m_connection.m_rssi->getOpen ();
   return connected;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Puts of the statistics/status title line

\* ---------------------------------------------------------------------- */
void RssiContributor::Statistics::print_title ()
{
   puts (
   " Sample   Rate    Gbps Connected DropRssi DropPack  Count Rx  Last   Rx Total\n"
   " ------ ------ ------- --------- -------- -------- ------ -------- ----------");

    return;
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
void RssiContributor::Statistics::print (RssiContributor::Statistics const  *cur, 
                                         RssiContributor::Statistics const  *prv, 
                                         bool                          opened,
                                         char                             eol)
{
   static int     Count = 0;
   static bool LastOpen = false;

   // ---------------------------------------------
   // Get the time since the last set of statistics
   // ---------------------------------------------
   //timespec_diff (&prv->m_time, &cur->m_time, &diff);

   double     elapsed = RssiContributor::Statistics::subtime (&prv->m_time, &cur->m_time);
   uint32_t       cnt = cur->m_rxCount - prv->m_rxCount;
   uint64_t diffBytes = cur->m_rxBytes - prv->m_rxBytes;
   uint32_t  curBytes = cur->m_rxLast;
   uint64_t  totBytes = cur->m_rxBytes;
   double        gbps = (((float)(diffBytes * 8)) / elapsed);
   float         rate =   ((float)cnt / elapsed) * 1e9;

  
   // If no change, only update the count
   if (LastOpen == opened && diffBytes == 0) 
   {
      printf (" %6u\r", Count++);
      fflush (stdout);
   }
   else
   {                  
      //         cnt  rate Gbps open drop rssi    drop pack  count         last         total
      printf (" %6u %6.3f %7.3f %6d  %8" PRIu32 " %8" PRIu32 "%6" PRIu32 " %8" PRIu32 " %10" PRIu64 "%c",
              Count++,
              rate,
              gbps, 
              opened, 
              cur->m_rssiDrop, 
              cur->m_packDrop,
              cur->m_rxCount,
              curBytes, 
              totBytes,
              eol);
   }


   LastOpen = opened;

   return;
}
/* ---------------------------------------------------------------------- */


#include <string.h>

/* ---------------------------------------------------------------------- *//*!

  \brief Copy the frame to the specified destination

  \param[out]   dst  The destination
  \param[ in] frame  The frame to copy
                                                                           */
/* ---------------------------------------------------------------------- */
static void copyFrame (uint8_t                                            *dst,
                       std::shared_ptr<rogue::interfaces::stream::Frame> frame)
{
   // Iterators to start and end of frame
   /// --- rogue::interfaces::stream::Frame::iterator iter = frame->beginRead();
   /// --- rogue::interfaces::stream::Frame::iterator  end = frame->endRead();

   rogue::interfaces::stream::FrameIterator iter = frame->begin ();
   rogue::interfaces::stream::FrameIterator  end = frame->end   ();

   

   //Iterate through contigous buffers
   while ( iter != end ) 
   {
      //  Get contigous size
      auto size = iter.remBuffer ();
      

      //  Get the data pointer from current position
      auto *src = iter.ptr ();
      

      //  Copy the data
      memcpy(dst, src, size);
      

      //  Update destination pointer and source iterator
      dst  += size;
      iter += size;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Dumps the frame to the stdout

  \param[in] frame The frame to dump
                                                                          */
/* ---------------------------------------------------------------------- */
static void dumpFrame (std::shared_ptr<rogue::interfaces::stream::Frame> frame)
{
   /// --- rogue::interfaces::stream::Frame::iterator iter = frame->beginRead();
   /// --- rogue::interfaces::stream::Frame::iterator  end = frame->endRead();

   rogue::interfaces::stream::FrameIterator iter = frame->begin();
   rogue::interfaces::stream::FrameIterator  end = frame->end  ();


   ldmx::utl::Dumper dumper;

   // ------------------
   // Dump the raw frame
   // ------------------
   while ( iter != end ) 
   {

      //  Get contigous size
      auto size = iter.remBuffer ();


      // Get the data pointer from current position
      auto           *src = iter.ptr ();
      uint32_t const *ptr = reinterpret_cast<decltype(ptr)>(src);


      // Next
      iter += size;
      bool last = iter == end;
      

      // Print the data
      dumper.dump ("Frame", ptr, size/sizeof (uint32_t), last);


   }
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Dump one batcher data record

  \param[in] data The batcher data record to dump
                                                                          */
/* ---------------------------------------------------------------------- */
static void dumpData (std::shared_ptr<rogue::protocols::batcher::Data> &data)
{

#if 1
   auto size = data->size  ();
   auto iter = data->begin ();   
   uint32_t *dst = reinterpret_cast<decltype (dst)>(malloc (size));
   fromFrame (iter, size, reinterpret_cast<void *>(dst));

   ldmx::utl::Dumper dumper;
   dumper.dump ("Data  ", dst, size/sizeof (uint32_t), true);

   free (dst);
   

#else

   auto iter = data->begin ();
   auto end  = data->end   ();
   ///auto size = data->size  ();

   printf ("Begin:End %p:%p\n", (void *)iter.ptr (), (void *)end.ptr ());
   
   while (iter != end)
   {
      auto           size = iter.remBuffer ();
      auto           *src = iter.ptr ();
      uint32_t const *ptr = reinterpret_cast<decltype(ptr)>(src);

     
      printf ("Data buffer size = %x:%x\n", (int)size, (int)size/4); 

     // Advance and check if last
     iter += size;
     bool last = true; /// iter == end;



     // Print the data
     ///dump ("Data  ", ptr, size, last);


     if (size == 0) break;
   }

#endif
   return;
}
/* ---------------------------------------------------------------------- */
