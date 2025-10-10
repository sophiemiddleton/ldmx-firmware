// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Configuration.cc
  \brief  ldmx event builder configuration
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
 * 2019.02.12 jjr Moved into hps/builder/client namespace
 * 2019.01.08 jjr Moved into hps/eb/client namespace
 * 2018.12.05 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/client/Configuration.hh"


/* ====================================================================== */
namespace ldmx    {
namespace builder {
namespace client  {
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy Constructor for the LDMX Event Builder

\* ---------------------------------------------------------------------- */
Configuration::Configuration ()
{
   return;
}
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the LDMX Event Builder

\* ---------------------------------------------------------------------- */
Configuration::Configuration (int type, int nfragments) :
   m_type       (type),
   m_nfragments (nfragments)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Adds the connection descriptor for one RSSI contributor

  \param[in] name The IP name of the contributor, not necessarily 
                  NUL-termimnated
  \param[in]  len The length of the IP name of the contributor
  \param[in] port The port number

\* ---------------------------------------------------------------------- */
void Configuration::addContributor (char const *name, 
                                    int          len,
                                    uint16_t    port,
                                    int      timeout,
                                    int   nbuffering,
                                    int      nevents)
{
   ldmx::builder::client::CfgContributor 
      ctb (CfgContributor::ContributorType::GenericContributer,
           CfgContributor::TransportType::Rssi,
           name,
           len,
           port,
           timeout,
           nbuffering,
           nevents);
           

   m_contributors.push_back (std::move (ctb));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the configuration using the stored name
                                                                          */
/* ---------------------------------------------------------------------- */
void Configuration::print () const
{
   print (m_name);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the contfiguration
                                                                          */
/* ---------------------------------------------------------------------- */
void Configuration::print (std::string const &name) const
{
   std::cout << "Configuration ---> " << name << " <----" << std::endl;

   char const Logging[static_cast<int>(ldmx::builder::Logging::Level::Max)+1][12] = 
   {
      "None",
      "Debug",
      "Info",
      "Warning",
      "Error",
      "Critical"
   };


   unsigned int    level = static_cast<unsigned int>(m_level);
   char const *levelName = (level < sizeof (Logging) / sizeof (Logging[0]))
                         ? Logging[level]
                         : "Unknown";
      

   CfgContributor::printTitle ();
   for (auto & ctb : m_contributors)
   {
      ctb.print ();
   }

   std::cout << "Nfragments         : " << m_nfragments << '\n'
             << " Level             : " << levelName    << '\n'
             << " Connection Timeout: " << m_connectionTimeout << '\n'
             << " Trigger Pipe      : " << m_triggerPipe       << '\n'
//             << " Triggers          : " << m_ntrgEventsPerBatch << " * "
//                                        << m_ntrgBatches << " = "
//                                        << m_ntrgEvents  << '\n'
//             << " Events            : " << m_ngenricEvents  << '\n'
//             << " Rssi Frames       : " << m_nrssiFrames
             << std::endl;

   return;
}
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: client                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */
