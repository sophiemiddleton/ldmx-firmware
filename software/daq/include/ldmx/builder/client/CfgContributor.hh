// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CFG_CONTRIBUTOR_HH__
#define __LDMX_BUILDER_CLIENT_CFG_CONTRIBUTOR_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/CfgContributor.hh
  \brief  Contains all the information needed to establish either any
          TCP/IP or RSSI connection
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
 * 2019.04.05 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>
#include <string.h>
#include <iostream>
#include <iomanip>


/* ====================================================================== */
namespace ldmx    {
namespace builder {
namespace client  {
/* ---------------------------------------------------------------------- *//*!

  \brief Class to describe one contribution
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgContributor
{
public:
   CfgContributor ();


   enum class TransportType: int8_t
   {
      Unknown = -1,
      None    =  0,
      Pipe    =  1,
      Rssi    =  2,
      TcpIp   =  3
   };

   enum class ContributorType: int8_t
   {
      Unknown = -1,
      None    =  0,
      Trigger =  1,
      Generic     =  2
   };


public:
   CfgContributor (ContributorType ctbType,
                   TransportType   trnType,
                   char const     *trnName,
                   int          trnNameLen,
                   uint16_t           port,
                   int             timeout,
                   int          nbuffering,
                   int             nevents);

public:
   bool isGeneric     () const;
   bool isTrigger () const;

public:
   static void printTitle ();
   void        print      () const;
      
public:
   ContributorType
                 m_ctbType;  /*!< The contributor type: Generic, TRG           */
   TransportType m_trnType;  /*!< The transport    type: RSSI, TCP/IP, PIPE*/
   char       m_trnStr[32];  /*!< The transport  string: IP or PIPE name   */
   uint16_t         m_port;  /*!< The port number                          */
   int32_t       m_timeout;  /*!< The connection timeout                   */
   uint32_t         m_nbuf;  /*!< The buffering parameter                  */
   uint32_t      m_nevents;  /*!< The number of events to queue            */
};
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: client                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace builder {
namespace client  {
/* ---------------------------------------------------------------------- *//*!

  \brief Default constructor
                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgContributor::CfgContributor::CfgContributor () :
      m_ctbType (ContributorType::Unknown),
      m_trnType (TransportType::Unknown),
      m_port    (0),
      m_timeout (0),
      m_nbuf    (0),
      m_nevents (0)
{
   m_trnStr[0]  = 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Complete constructor, filling in all fields

  \param[in]    ctbType The contributor type (Generic, TRG)
  \param[in]    trnType The transport type (TCP/IP, RSSI, PIPE)
  \param[in]    trnName The transport connection name (it's IP, pipe name...)
  \param[in] trnNameLen The number of characters in the transport name
  \param[in]       port The port name (relevant for TCP/IP, RSSI types)
  \param[in]    timeout The connection timeout period (in seconds)
  \param[in] nbuffering Any buffering parameter associated with the 
                        transport type
  \param[in]    nevents The number of events to buffer between the input
                        and event building threads
                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgContributor::CfgContributor (ContributorType  ctbType,
                                       TransportType    trnType,
                                       char const      *trnName,
                                       int           trnNameLen,
                                       uint16_t            port,
                                       int              timeout,
                                       int           nbuffering,
                                       int              nevents) :
   m_ctbType (   ctbType),
   m_trnType (   trnType),
   m_port    (      port),
   m_timeout (   timeout),
   m_nbuf    (nbuffering),
   m_nevents (   nevents)
{
   strncpy (m_trnStr, trnName, trnNameLen);
   m_trnStr[trnNameLen] = 0;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

 \brief  Check  if this is an Generic contributor
 \retval true,  if this is an Generic contributor
 \retval false, if this is not an Generic contributor
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::isGeneric () const
{
   bool   is_Generic = (m_ctbType == ContributorType::GenericContributor);
   return is_Generic;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

 \brief  Check if this is an TRIGGER contributor
 \retval true,  if this is an TRIGGER contributor
 \retval false, if this is not an TRIGGER contributor
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::isTrigger () const
{
   bool   is_trigger = (m_ctbType == ContributorType::Trigger);
   return is_trigger;
}
/* ---------------------------------------------------------------------- */


inline void CfgContributor::printTitle ()
{
   std::cout << 
   "                Type Transport      Name  |  IP:port RssiFrames Events Timeout" 
             << std::endl <<
   "                ---- ---------  -------------------- ---------- ------ -------"
             << std::endl;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Prints the configuration
                                                                          */
/* ---------------------------------------------------------------------- */
inline void CfgContributor::print () const
{
   char const  *ctbName = isGeneric     () ? "Generic"
                        : isTrigger () ? "TRG"
                        : "Unknown";

   char const *trnName  = m_trnType == TransportType::Pipe ? "Pipe"
                        : m_trnType == TransportType::Rssi ? "Rssi"
                        : m_trnType == TransportType::TcpIp ? "TcpIp"
                        : m_trnType == TransportType::None  ? "None"
                        : "Unknown";

   std::cout << "Contributor " 
             << std::setw( 8) << std::right << ctbName   << ' ' 
             << std::setw( 9) << std::right << trnName   << ' '; 


   if (m_trnType == TransportType::Rssi || m_trnType == TransportType::TcpIp)
   {
      std::cout << std::setw(16) << std::right << m_trnStr
                << ':' << std::setw( 5) << std::left  << m_port  << ' ';
   }
   else 
   {
      std::cout << std::setw(21) << std::right << m_trnStr << ' ' << ' ';
   }

   std::cout << std::setw( 9) << std::right << m_nbuf    << ' ' 
             << std::setw( 6) << std::right << m_nevents << ' ' 
             << std::setw (7) << std::right << m_timeout
             << std::endl;

   return;
}
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: client                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */

#endif

