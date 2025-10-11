// -*-Mode: C++;-*-

#ifndef _LDMX_BUILDER_SERVER_CONFIGURATION_HH_
#define _LDMX_BUILDER_SERVER_CONFIGURATION_HH_

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/Configuration.hh
  \brief  LDMX event builder server emulator configuration
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the rogue software platform, including this file, may be 
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
 * 2019.03.18 jjr Added # of events/batch, # multisample records/event
 * 2019.01.11 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/Logging.hh"
#include <cinttypes>
#include <vector>


/* ====================================================================== */
namespace ldmx    {
namespace builder {
namespace server  {
/* ---------------------------------------------------------------------- *//*!

  \class  Configuration
  \brief  The server configuration values

\* ---------------------------------------------------------------------- */
class Configuration
{
public:
   Configuration (int type);

   static const int NServers = 8;

public:
   void setServerIp   (const char *serverIp);
   void addPort       (uint16_t        port);

public:
   int                            m_type;
   int                          m_period;
   const char                      *m_ip;
   std::vector<uint16_t>         m_ports;
   const char                 *m_trgPipe;
   unsigned short int       m_ntrgEvents;
   ldmx::builder::Logging::Level m_level;
   unsigned short int       m_nGenericEvents; /*!< # of events/genric batch      */
   unsigned short int         m_nGenericMsdr; /*!< # of multisample recs/event*/       

};
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: server                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */

#endif
