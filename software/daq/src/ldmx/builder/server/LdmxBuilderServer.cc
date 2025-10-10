// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/HpsBuilderServer.cc
  \brief  LDMX builder command line parameter extraction for the HPS
          build server emulator
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
 * 2019.01.11 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "Parameters.hh"
#include "ldmx/builder/server/Configuration.hh"
#include "TriggerServer.hh"
#include "RssiServers.hh"
#include "ldmx/utl/Timestamp.hh"

#include <cstdio>
#include <unistd.h>


/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static  int launch_tcp_servers  (ldmx::builder::server::Configuration const &cfg);
static  int launch_rssi_servers (ldmx::builder::server::Configuration const &cfg);
static  inline uint64_t waitTill(uint64_t tgtTime, int nsecs);
/* ====================================================================== */




/* ---------------------------------------------------------------------- *//*!

  \brief LDMX server emulation

  \param[in] argc  The number of command line parameters
  \param[in] argv  The vector of command line parameters

\* ---------------------------------------------------------------------- */
int main (int argc, char **argv) 
{
   // -----------------------------------------------------------------
   // Extract the command line parameters and fill in the configuration
   // -----------------------------------------------------------------
   Parameters prms (argc, argv);
   ldmx::builder::server::Configuration cfg (static_cast<int>(prms.m_type));
   prms.configure  (&cfg);

   
   // -----------------------------------------
   // Dispatch to the proper transport protocal
   // -----------------------------------------
   if (prms.m_type == Parameters::ConnectionType::TcpIp)
   {
      int    status = launch_tcp_servers (cfg);
      return status;
   }

   else if (prms.m_type == Parameters::ConnectionType::Rssi)
   {
      int    status = launch_rssi_servers (cfg);
      return status;
   }

   else
   {
      fprintf (stderr, 
               "LdmxBuilderServer: unrecognized transport protocol %d\n",
               static_cast<int>(prms.m_type));
      return -1;
   }

}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructs the TcpIp version of the event builder

  \param[in] prms The configuration parameters

\* ---------------------------------------------------------------------- */
static int launch_tcp_servers (ldmx::builder::server::Configuration const &cfg)
{
   return -1;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static int launch_rssi_servers (ldmx::builder::server::Configuration const &cfg)
{

   RssiServers   servers   (cfg);
   TriggerServer trigger   (cfg);

   servers.start              ();
   servers.waitForConnections ();

   useconds_t      usecs = cfg.m_period;
   uint64_t curTimestamp = ldmx::utl::Timestamp::current ();
   uint64_t nxtTimestamp = curTimestamp;
   int            nsecs = usecs * 1000;

   
   int d[20];
   for (int idx = 0; idx < 20; idx++)
   {
      uint64_t t = ldmx::utl::Timestamp::current ();
      d[idx] = t - curTimestamp;
      curTimestamp = t;
   }


   int sum = 0;
   for (int idx = 0; idx < 20; idx++)
   {
      sum += d[idx];
      printf ("d[%2d] = %d\n", idx, d[idx]);
   }

   int avg = sum/20;
   printf ("Overhead = %d nsecs\n", avg);

   nsecs -= avg;
   ///nsecs -= 100*1000; //avg;
   printf ("Period = %d nsecs\n", nsecs);


   while (1)
   {
      // --------------------
      // Generate one trigger
      // --------------------
      uint64_t trgTimestamp = ldmx::utl::Timestamp::current ();
      bool sendTriggerBatch = trigger.addEvent (trgTimestamp);


      // ------------------
      // Generate one event
      // ------------------
      uint32_t         ctbs = servers.generateEvent (cfg.m_nGenericMsdr);


      nxtTimestamp = waitTill (nxtTimestamp, nsecs);



      // ----------------------------------------------
      // Send this batch of trigger if it is ready to go
      // ----------------------------------------------
      if (sendTriggerBatch)
      {
         printf ("Sending trigger batch  ---> ");
         int err = trigger.sendBatch ();
         if (err)
         {
            trigger.reconnect ();
         }

         printf ("Sent\n");
      }


      // ----------------------------------------------------------
      // Send the batch of events from contributors that are ready
      // ---------------------------------------------------------
      if (ctbs) 
      {
         servers.sendBatch (ctbs); 
         ctbs = 0;
      }
   }

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Wait till the next timestamp
  \return The next absolute time to wait

  \param[in] tgtTime The target time (absolute)
  \param[in]   nsecs The update time (relative)

  \note
   This is an attempt to make the actual rate match the requested trigger
   rate by accounting for overheads.  It works better than just waiting 
   'nsecs' between triggers, but, given this is UNIX and not an RTOS,
   there are limits to how well this can work.
                                                                          */
/* ---------------------------------------------------------------------- */
static inline uint64_t waitTill (uint64_t tgtTime, int nsecs)
{
   // Calculate how long to the target time
   uint64_t curTime   = ldmx::utl::Timestamp::current ();
   int32_t  over      = (curTime - tgtTime);
   int32_t  sleepTime = (over > 0 ? (nsecs - over) : nsecs) / 1000;
      
   ///printf ("SleepTime = %d:%d\n", (int)sleepTime, (int)over);


   if (sleepTime > 0) 
   {
      // Positve time to sleep wait and set the next trigger time
      usleep (sleepTime);
      tgtTime  += nsecs;
   }
   else
   {
      // If got too far behind, reset wait to cur + period
      tgtTime = curTime + nsecs;
   }
   
   return tgtTime;
}
/* ---------------------------------------------------------------------- */
