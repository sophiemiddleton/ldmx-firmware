// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_RSSI_CONTRIBUTOR_HH__
#define __LDMX_BUILDER_SERVER_RSSI_CONTRIBUTOR_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/RssiContributor.hh
  \brief  RSSI contributor to the LDMX event builder
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

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
 * 2019.01.07 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "RssiConnection.hh"
#include "ldmx/data/server/GenericGenerator.hh"
#include "ldmx/data/server/GenericBatch.hh"
#include <rogue/interfaces/stream/Master.h>
#include <memory>
#include <time.h>

#include <cinttypes>


class RssiConnection;

//! Receive slave data, count frames and total bytes for example purposes.
class RssiContributor : public rogue::interfaces::stream::Master 
{
public:
   RssiContributor ();
   RssiContributor (int               serverId,
                    uint8_t         rceAddress,
                    int         neventPerBatch,
                    const char       *clientIp,
                    uint16_t        clientPort,
                    int                nframes);


   static std::shared_ptr<RssiContributor>
          construct (RssiConnection *connection);

   static std::shared_ptr<RssiContributor>
          construct (int           serverId,
                    uint8_t      rceAddress,
                     int     neventPerBatch,
                     const char   *clientIp,
                     uint16_t    clientPort,
                     int            nframes);

   uint32_t generate      (unsigned short int        nevents,
                           unsigned short int nmultisamples);
                           
   bool     generateEvent (unsigned short int nmultisamples);
   uint32_t generateBatch (unsigned short int nmultisamples);


   void sendBatch  ();
   void sendData   (uint8_t const *data, uint32_t size);
   void resetBatch ();

   class Statistics
   {
   public:
      Statistics ();
      Statistics (Statistics volatile &stats);

   public:
      void construct  () volatile;

   public:
      static void    gettime (struct timespec          *ts);
      static void    gettime (struct timespec volatile *ts);
      static int64_t subtime (struct timespec const *start,
                              struct timespec const  *stop);

   public:
      static void print_title ();
      static void print       (int                              which,
                               RssiContributor::Statistics const *cur,
                               RssiContributor::Statistics const *prv,
                               bool                            opened,
                               char                               eol);

   public:
      struct timespec m_time;  /*!< The timestamp of these statistics     */
      uint32_t    m_evtCount;  /*!< Count of outputgoing events           */
      uint32_t     m_txCount;  /*!< Count of the transmissions (batches)  */
      uint64_t     m_txBytes;  /*!< Total number of bytes sent            */
      uint32_t      m_txLast;  /*!< Number of bytes in last event sent    */
      uint32_t    m_packDrop;  /*!< Number of dropped packets             */
      uint32_t    m_rssiDrop;  /*!< Number of dropped rssi packets        */
   };
   

public:
   RssiConnection    m_connection;  /*!< The connection information       */
   Statistics volatile    m_stats;  /*!< The statistics                   */
   ldmx::data::server::
   GenericGenerator    m_GenericGenerator;  /*!< Generate an Generic batch            */
   ldmx::data::server::
   GenericBatch            m_GenericBatch;  /*!< The memory for the generated data*/
   unsigned short int 
                m_neventsPerBatch;  /*!< Number of events per batch       */
};
/* ---------------------------------------------------------------------- */ 
#endif
