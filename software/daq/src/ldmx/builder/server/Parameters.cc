// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/Parameters.cc
  \brief  LDMX builder command line parameter extraction for the LDMX
          event build server emulator
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
 * 2019.03.18 jjr Added # of events/batch, # multisample records/event
 * 2019.01.11 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "Parameters.hh"
#include "ldmx/builder/server/Configuration.hh"


#include <getopt.h>
#include <cstdio>
#include <iostream>
#include <string.h>
#include <cinttypes>


/* ---------------------------------------------------------------------- *//*!

  \brief Extracts the command line parameters

\* ---------------------------------------------------------------------- */
Parameters::Parameters (int argc, char *const argv[])
{
   static struct option options[] = 
   {
      { "none"    , no_argument, 0, 0x80 | static_cast<int>(ConnectionType::None) },
      { "rssi"    , no_argument, 0, 0x80 | static_cast<int>(ConnectionType::Rssi) },
      { "tcpip"   , no_argument, 0, 0x80 | static_cast<int>(ConnectionType::TcpIp)},
      { "GenericEvents"    , required_argument, 0, 'e' },
      { "triggerEvents", required_argument, 0, 't' },
      { "msrecord"     , required_argument, 0, 'm' },
      { "rate"         , required_argument, 0, 'r' },
      { "ports"        , required_argument, 0, 'p' },
      { "server"       , required_argument, 0, 's' },
      { "triggerPipe"  , required_argument, 0, 'P' },
      { 0, 0, 0, 0}
   };

   int                 type = static_cast<int>(ConnectionType::None);
   char const       *server = NULL;
   char const        *ports = "8192";
   int                 rate =  1;

   unsigned short int nGenericEvents  = 16;  /* # Generic events/batch            */
   unsigned short int nGenericMsdr    =  4;  /* # Multisample records/event   */


   char const           *trgPipe  = "/tmp/ldmxTriggerPipe";
   unsigned short int  ntrgEvents = 16;  /* # trigger events/batch        */


   while (1)
   {
      int option_index = 0;
      int c = getopt_long (argc, argv, "P:t:r:e:m:p:s", options, &option_index);

      if (c == -1)
      {
         break;
      }

      if (c & 0x80) type = (c & ~0x80);
      else
      {
         switch (c)
         {
         case 0:   {                                       break; }
         case 'P': {  trgPipe     = optarg;                break; }
         case 't': {  ntrgEvents  = strtol (optarg, 0, 0); break; }
         case 'r': {  rate        = strtol (optarg, 0, 0); break; }

         case 'e': {  nGenericEvents  = strtol (optarg, 0, 0); break; }
         case 'm': {  nGenericMsdr    = strtol (optarg, 0, 0); break; }
         case 'p': {  ports       = optarg;                break; }
         case 's': {  server      = optarg;                break; }
         }
      }
   }


   m_type          = static_cast<decltype(m_type)>(type);
   m_server        = server;
   m_ports         = ports;
   m_trgPipe       = trgPipe;
   m_ntrgEvents    = ntrgEvents;
   m_nGenericEvents    = nGenericEvents;
   m_nGenericMsdr      = nGenericMsdr;
   m_level         = ldmx::builder::Logging::Level::Error;
   m_period        = rate ? 1000*1000/rate : 1000*1000;


   printf ("Type         = %d\n", type);
   printf ("Srcs         = %s\n", server);
   printf ("Port         = %s\n", ports);

   printf ("TriggerPipe  = %s\n",  m_trgPipe);
   printf ("TriggerBatch = %4u\n", m_ntrgEvents);
   printf ("GenericBatch     = %4u Generic events/batch)\n",              m_nGenericEvents);
   printf ("GenericNms       = %4u Multisample data records/event\n", m_nGenericMsdr);


   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Construct the LDMX Event Builder server emulator configuration

  \param[out] cfg  The LDMX Event Builder server emulator configuration 
                   to complete

\* ---------------------------------------------------------------------- */
int Parameters::configure (ldmx::builder::server::Configuration *cfg) const
{
   cfg->setServerIp (m_server);
   extract_ports    (cfg);
   cfg->m_trgPipe       = m_trgPipe;
   cfg->m_ntrgEvents    = m_ntrgEvents;
   cfg->m_level         = m_level;
   cfg->m_period        = m_period;
   cfg->m_nGenericEvents    = m_nGenericEvents;
   cfg->m_nGenericMsdr      = m_nGenericMsdr;

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts the ports from a comma separated list
  \return The number of extracted ports
  
  \param[out] cfg  The LDMX Event Builder server emulator configuration 
                   to complete
                                                                          */
/* ---------------------------------------------------------------------- */
int Parameters::extract_ports (ldmx::builder::server::Configuration *cfg) const
{
   const char *cur = m_ports;

   while (1)
   {
      const char *end = strchr (cur, ',');
      int         len = (end == NULL) ? strlen (cur) : end - cur;

      if (len)
      {
         uint16_t port = strtoul (cur, NULL, 0);
         cfg->addPort (port);
      }

      if (end == NULL) break;
      cur = end + 1;
   }


   return cfg->m_ports.size ();
}
/* ---------------------------------------------------------------------- */


