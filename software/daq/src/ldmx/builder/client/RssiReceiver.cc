// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/RssiReceiver.cc
  \brief  Receives contributions from RSSI contributors
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
 * 2019.02.12 jjr Moved to namespace ldmx::builder::client
 * 2019.02.12 jjr Added documentation header
 *
\* ---------------------------------------------------------------------- */




#include "RssiReceiver.hh"
#include "RssiConnection.hh"
#include "RssiContributor.hh"   

#include "GenericContributor.hh"


#include "ldmx/builder/Logging.hh"
#include "ldmx/builder/client/Configuration.hh"
#include "ldmx/builder/client/CfgContributor.hh"

#include "rogue/Logging.h"
#include <unistd.h>


/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static int            setLevel (ldmx::builder::Logging::Level level);
static  bool waitForConnection (RssiConnection const    &connection,
                                int                        nseconds);
/* ====================================================================== */




/* ---------------------------------------------------------------------- *//*!

   \brief  Main program to establish and accept data from an RSSI stream

   \param[in]    cfg  The RSSI receiver's configuration values
   \param[in]   ctbs  The incoming message queue for all contributions
   \param[in] ctbBeg  Index of the first message queue for the RSSI 
                      contributions

\* ---------------------------------------------------------------------- */
RssiReceiver::RssiReceiver (ldmx::builder::client::Configuration const &cfg,
                            Contributions                             *ctbs,
                            int                                      ctbBeg) :
   m_contributions     (ctbs, ctbBeg, cfg.m_contributors.size ()),
   m_mconnected        (0),
   m_connectionTimeout (cfg.m_contributors[0].m_timeout)
{
   // -------------------------------------------------------
   // Get the count of and contributor connection descriptors
   // -------------------------------------------------------
   std::vector<ldmx::builder::client::CfgContributor> const 
        &cfgCtbs = cfg.m_contributors;
   int  ncfgCtbs = cfgCtbs.size ();


   // ---------------------
   // Set the logging level
   // ---------------------
   setLevel (cfg.m_level);


   // -----------------------------------
   // Construct the Generic data contributors
   // -----------------------------------
   for (int idx = 0; idx < ncfgCtbs; ++idx)
   {
      m_contributors[idx] = RssiContributor::construct (idx + ctbBeg,
                                                        cfgCtbs[idx],
                                                        m_contributions.m_ctbs,
                                                        sizeof (GenericContributor));
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Rssi receiver's destructor

\* ---------------------------------------------------------------------- */
RssiReceiver::~RssiReceiver ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Starts/enables all the connections

\* ---------------------------------------------------------------------- */
void RssiReceiver::start ()
{
   int nctbs = m_contributions.getCnt ();
   for (int idx = 0; idx < nctbs; ++idx)
   {
      m_contributors[idx]->m_connection.start ();
   }
}
/* ---------------------------------------------------------------------- */


#include <iostream>

/* ---------------------------------------------------------------------- *//*!

  \brief   Wait for all contributors to connect
  \return  A bit mask of those contributors that failed to connect

\* ---------------------------------------------------------------------- */
uint32_t RssiReceiver::waitForConnections ()
{
   int ctbBeg  = m_contributions.getBeg ();
   int ctbCnt  = m_contributions.getCnt ();

   for (int idx = 0; idx < ctbCnt; ++idx)
   {

      RssiConnection const &connection = m_contributors[idx]->m_connection;
      std::cout << "Connection Timeout " 
                << connection.m_timeout << " secs" <<std::endl;
      bool                   connected = waitForConnection (connection, 
                                                            connection.m_timeout);

      // Check if connection was established
      if (connected)
      {
         m_mconnected |= ldmx::utl::ListsInterlocked::listMask (ctbBeg+ idx);
      }
   }

   return missingContributors ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief    Wait for the RSSI connection to be established
  \return   Boolean indicating whether or notthe RSSI connection has 
            been established

  \param[in] connection The target RSSI connection 
  \param[in]   nseconds The number of seconds to wait
                                                                          */
/* ---------------------------------------------------------------------- */
static inline bool waitForConnection (RssiConnection const &connection,
                                      int                     nseconds)
{
   #define WAITTIME 100000
   int left = nseconds * 1000 * 1000;

   bool connected = true;

   printf ("DaqHost:Port = %20s:%d ", connection.m_ip, connection.m_port);

   while (!connection.isConnected ())
   {
      putchar ('.');
      fflush (stdout);
      usleep (WAITTIME);
      left -= WAITTIME;
      if (left < 0) { connected = false; break; }
   }

   puts  (connected ? "Connection established" : "Connection failed");      
   return connected;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the rogue::Logging level from an generic 
          ldmx::builder::Logging level
  \return The set rogue::Logging level or -1 if the translation 
          fails

  \param[in] level The eb::Logging level

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
