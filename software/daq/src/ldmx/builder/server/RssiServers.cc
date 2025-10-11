// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/RssiServers.cc
  \brief  Class for the collection RSSI LDMX bulder server emulators
          builder server emulator
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
 * 2019.11.21 jjr Created
 *
\* ---------------------------------------------------------------------- */

#include "RssiServers.hh"
#include "RssiConnection.hh"
#include "RssiContributor.hh"   

#include <rogue/protocols/rssi/Server.h>
#include <rogue/protocols/packetizer/CoreV2.h>
#include "rogue/Logging.h"

#include "ldmx/builder/Logging.hh"
#include "ldmx/builder/server/Configuration.hh"
#include "ldmx/utl/Timestamp.hh"

#include <unistd.h>


/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static int            setLevel (ldmx::builder::Logging::Level level);

static  bool waitForConnection (RssiConnection const   &connection,
                                int                       nseconds);
/* ====================================================================== */




/* ---------------------------------------------------------------------- *//*!

   \brief  Main program to establish and send data from an RSSI stream

   \param[in] cfg  The Event Builder configuration values

\* ---------------------------------------------------------------------- */
RssiServers::RssiServers  (ldmx::builder::server::Configuration const &cfg)
{
   int                          nframes = 16;

   // -------------------------------------------------------
   // Get the count of and contributor connection descriptors
   // -------------------------------------------------------
   const char                     *ip = cfg.m_ip;
   std::vector<uint16_t> const &ports = cfg.m_ports;
   int                       nservers = ports.size ();


   // -------------------------
   // Set the number of servers
   // -------------------------
   m_nservers     = nservers;
   m_mservers     = ((1 << nservers) - 1);
   m_connected    = 0;


   // ----------------------------------------
   // Set the batch and event sequence numbers
   // ---------------------------------------
   m_batchSeqNumber = 0;
   m_eventSeqNumber = 0;


   // ---------------------
   // Set the logging level
   // ---------------------
   setLevel (cfg.m_level);


   // --------------------------
   // Construct the contributors
   // --------------------------
   for (int idx = 0; idx < nservers; ++idx)
   {
      uint8_t rceAddress = 0x80 + idx;
      m_servers[idx] = RssiContributor::construct (idx,
                                                   rceAddress,
                                                   cfg.m_nGenericEvents,
                                                   ip, 
                                                   cfg.m_ports[idx], 
                                                   nframes);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Rssi Servers destructor

\* ---------------------------------------------------------------------- */
RssiServers::~RssiServers ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Starts/enables all the connections

\* ---------------------------------------------------------------------- */
void RssiServers::start ()
{
   for (int idx = 0; idx < m_nservers; ++idx)
   {
       m_servers[idx]->m_connection.start ();
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Wait for all contributors to connect
  \return  A bit mask of those contributors that failed to connect

\* ---------------------------------------------------------------------- */
uint32_t RssiServers::waitForConnections ()
{
   int timeout = 1000;

   for (int idx = 0; idx < m_nservers; ++idx)
   {
      RssiConnection const &connection = m_servers[idx]->m_connection;
      bool                   connected = waitForConnection (connection, 
                                                               timeout);

      // Check if connection was established
      if (connected) m_connected |= (1 << idx);
   }

   return missingServers ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief    Wait for the RSSI connection to be established
  \return   Boolean indicating whether or not the RSSI connection has 
            been established

  \param[in] connection The target RSSI connection 
  \param[in]   nseconds The number of seconds to wait
                                                                          */
/* ---------------------------------------------------------------------- */
static inline bool waitForConnection (RssiConnection const &connection,
                                      int                     nseconds)
{
   #define POLLTIME 100 * 1000
   uint64_t left =  nseconds * (1000 * 1000LL);

   bool connected = true;

   printf ("DaqHost:Port = %20s:%d ", connection.m_ip, connection.m_port);

   while (!connection.isConnected ())
   {
      fflush (stdout);
      usleep (POLLTIME);
      left -= POLLTIME;
      int skip = left % (1 * (1000 * 1000ll));
      if (skip == 0) putchar ('.');
      if (left < 0) { connected = false; break; }
   }

   puts  (connected ? "Connection established" : "Connection failed");      
   return connected;

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
uint32_t RssiServers::generateEvent (unsigned short int nmultisamples)

{
//   static int Count = 0;
//   if (Count-- <= 0)
//   {
//      RssiContributor::Statistics::print_title ();
//      Count = 9;
//   }


   uint32_t readyCtbs = 0;


   // Generate one event
   for (int idx = 0; idx < m_nservers;  ++idx)
   {
      bool ready = m_servers[idx]->generateEvent (nmultisamples);

      // ---------------------------------------------------------
      // If this SVT batch is ready to ship, sdd to the ready list
      // ---------------------------------------------------------
      if (ready)
      {
         readyCtbs |= (1 << idx);
      }
   }


   // Return a mask of all connected contributors 
   return readyCtbs;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Send the generated SVT batch

  \param[in] ctbSet The set of contributors to send the data
                                                                          */
/* ---------------------------------------------------------------------- */
void RssiServers::sendBatch (uint32_t ctbSet)
{
   while (ctbSet)
   {
      int  idx = __builtin_ctz (ctbSet);
      auto ctb = m_servers[idx];
              

      // Send and reset the batch
      ctb->sendBatch  ();
      ctb->resetBatch ();


      // Remove this contributor from the remaining set
      ctbSet &= ~(1 << idx);
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the rogue::Logging level from an generic 
          ldmx::builder::Logging level
  \return The set rogue::Logging level or -1 if the translation 
          fails

  \param[in] level The builder::Logging level

\*---------------------------------------------------------------------- */
static int setLevel (ldmx::builder::Logging::Level level)
{
   namespace lcl = ldmx::builder;
   int lvl;
   if      (level == lcl::Logging::Level::Debug   ) lvl = rogue::Logging::Debug;
   else if (level == lcl::Logging::Level::Info    ) lvl = rogue::Logging::Info;
   else if (level == lcl::Logging::Level::Warning ) lvl = rogue::Logging::Warning;
   else if (level == lcl::Logging::Level::Error   ) lvl = rogue::Logging::Error;
   else if (level == lcl::Logging::Level::Critical) lvl = rogue::Logging::Critical;
   else    lvl = -1;

   // Set the message logging level
   if (lvl >= 0)
   {
      rogue::Logging::setLevel (lvl);
   }

   return lvl;
}
/* ---------------------------------------------------------------------- */
