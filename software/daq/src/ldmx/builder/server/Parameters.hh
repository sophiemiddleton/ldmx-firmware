// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_PARAMETERS_HH__
#define __LDMX_BUILDER_SERVER_PARAMETERS_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/Parameters.hh
  \brief  Class to contain the command line parameters for the LDMX builder 
          server emulator
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
 * 2019.02.12 jjr Moved to namespace hps::builder;:server
 * 2019.11.21 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/Logging.hh"
#include <cinttypes>

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace builder {
namespace server  {
/* ---------------------------------------------------------------------- */

class Configuration;

}  /* NAMESPACE: server                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */



/* ---------------------------------------------------------------------- *//*!

  \class Parameters
  \brief Captures the command line parameters in a digestable form

\* ---------------------------------------------------------------------- */
class Parameters
{
public:
   Parameters (int argc, char *const argv[]);

public:
   enum class ConnectionType
   {
      None  = 0,
      TcpIp = 1,
      Rssi  = 2
   };

public:
   void report ();
   int configure      (ldmx::builder::server::Configuration *cfg) const;
   int extract_ports  (ldmx::builder::server::Configuration *cfg) const;

public:
   ConnectionType            m_type; /*!< Connection type                 */
   char const             *m_server; /*!< IP addresse                     */
   char const              *m_ports; /*!< IP ports, comma separated list  */
   char const            *m_trgPipe; /*!< Name of pipe for the trigger    */
   int                     m_period; /*!< The trigger period (usecs)      */
   unsigned short int  m_ntrgEvents; /*!< # trigger events/batch          */
   unsigned short int  m_nGenericEvents; /*!< Number of events in an Generic batch*/
   unsigned short int    m_nGenericMsdr;  /*!< Number of multisample data 
                                           records in an Generic event        */
   ldmx::builder::Logging::Level 
                           m_level;  /*!< The logging level               */
};
/* ---------------------------------------------------------------------- */

#endif
