// -*-Mode: C++;-*-


#ifndef __LDMX_BUILDER_CLIENT_CFGCONTRIBUTOR_HH__
#define __LDMX_BUILDER_CLIENT_CFGCONTRIBUTOR_HH__


/* ---------------------------------------------------------------------- *//*!

  \file   CfgContributor.hh
  \brief  The parsed contributor from a configuration file
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the LDMX software platform, including this file, may be 
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
 * 2019.04.11 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "CfgObject.hh"
#include "ldmx/builder/client/CfgContributor.hh"


/* ---------------------------------------------------------------------- *//*!

  \brief Class to parse and fill in a CfgContribution
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgContributor :  public CfgObject,
                        public ldmx::builder::client::CfgContributor
{
public:
   using ContributorType = ldmx::builder::client::CfgContributor::ContributorType;

public:
   CfgContributor (char const *name, CfgLine *cfgLine);
  ~CfgContributor ();


public:
   bool set (char          const        **str, 
             CfgParameters const  *parameters, 
             CfgLine const           *cfgLine);


   bool parse            (char                      **str,
                          CfgParameters const &parameters);

   bool defineGeneric        (char                      **str,
                          CfgParameters const &parameters);

   bool defineTrigger    (char **str,
                          CfgParameters const &parameters);

   bool setGeneric           (char **str);
   bool setTrigger       (char **str);

   bool set              (char const           *fieldName,
                          CfgParameters const &parameters,
                          char                     **str);

   bool setGeneric           (char const           *fieldName,
                          CfgParameters const &parameters,
                          char                      **str);

   bool setTrigger       (char const           *fieldName,
                          CfgParameters const &parameters,
                          char                      **str);

   
   
   bool setTransportType (char const   *trnTypeToken);
   bool setTransportStr  (char const    *trnStrToken); 
   bool setBuffering     (char const *bufferingToken);
   bool setNevents       (char const   *neventsToken);
   bool setTimeout       (char const   *timeoutToken);

   void print (std::string const &name, int instance) const;
};
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Basic constructor for a parsed configuration contributor 

  \param[in]    name The name of this contributor
  \param[in] cfgLine The description of the line in the configuration file
                     where this contributor is defined
                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgContributor::CfgContributor (char const *name, CfgLine *cfgLine) :
   CfgObject (CfgObject::Type::Contributor, name, cfgLine)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Destructor for a parsed configuration contributor
                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgContributor::~CfgContributor ()
{
   /// std::cout << "Destructor::contributor" << std::endl;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Dummy set
  \return true on failure

  \param[in:out]        str  The string to parse
  \param    [in] parameters  Source of symbolic parameters
  \param    [in]    cfgLine  The originating configuration line
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::set (char          const         **str, 
                                 CfgParameters const *m_parameters, 
                                 CfgLine const               *line)
{
   return true;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Parse a contributor 
  \return true on failer

  \param[out]  dsc The contributor description
  \param[ in[  str The ASCII string to parse
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::parse (char                      **str,
                                   CfgParameters const &parameters)
{
   // Get the contributor's type
   char const *ctbType = getToken (str);

   
   if (compare (ctbType,  "Generic"))
   {
      ///std::cout << "Contributor type = Generic" << std::endl;
      m_ctbType = CfgContributor::ContributorType::GenericContributor;
      bool   err = defineGeneric (str, parameters);

      /// std::cerr << "CfgContribution::parse  Generic status" << err << std::endl;
      return err;
   }


   else if (compare (ctbType, "TRIGGER"))
   {
      ///std::cout << "Contributor type = TRIGGER" << std::endl;
      m_ctbType = CfgContributor::ContributorType::Trigger;
      bool   err = defineTrigger (str, parameters);
      return err;
   }

   else
   {
      std::cerr << "CfgParser::error Unrecognized contributor type <" << ctbType
                << '>'
                << ", must be one of Generic, TRG" << std::endl;
      return false;
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Report file and line number of potential error
                                                                          */
/* ---------------------------------------------------------------------- */
inline void CfgParser::report (int         lineNum, 
                               char const *orgLine)
{
   std::cerr << m_filename << ";" << lineNum << std::endl
             << " -> " << orgLine << std::endl;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Fills in the descriptor for an Generic contributor
  \return true on failure

  \param[in]        str  The remaining part of the line to parse
  \param[in] parameters  Source of symbolic parameters
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::defineGeneric (char                      **str, 
                                       CfgParameters const &parameters)
{
   bool err;

   while (1)
   {
      char const *fieldName = getToken (str);
      if (fieldName == nullptr) break;

      err = setGeneric (fieldName, parameters, str);
      if (err) return true;
   }

   return false;
}
/* ---------------------------------------------------------------------- */



inline bool CfgContributor::set (char const          *fieldName, 
                                 CfgParameters const &parameters,
                                 char                      **str)
{
   return false;
}



/* ---------------------------------------------------------------------- *//*!

  \brief Sets the specified field
  \return true on failure

  \param[in]  fieldName  The name of the field to set
  \param[in] parameters  Source of symbolic parameters
  \param[in]        str  The remaining part of the line to parse
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setGeneric (char const          *fieldName, 
                                    CfgParameters const &parameters,
                                    char                      **str)
{
   bool err;

   /// std::cout << "Generic fieldname <" << fieldName << '>' << std::endl;
   if (compare (fieldName, "TRANSPORT"))
   {
      char const *trnTypeToken = getValue (str, parameters);
      err                      = setTransportType (trnTypeToken);
      if (err) return true;
   }

   else if (compare (fieldName, "IP"))
   {
      // Get the IP/PIPE name
      char const *trnStrToken = getValue (str, parameters);
      /// std::cout << "Got transport <" << trnStrToken << '>' << std::endl;
      err                     = setTransportStr (trnStrToken);
      if (err) return true;
   }

   else if (compare (fieldName, "PORT"))
   {
      /// std::cout << "Got port" << std::endl;
         
      char const *port = getValue (str, parameters);
      m_port = strtoul (port, nullptr, 0);
   }

   else if (compare (fieldName, "RSSIFRAMES"))
   {
      /// std::cout << "Got buffering" << std::endl;
      
      // Get the Generic buffering (basically either RSSI frames or TCP/IP buffer
      char const *bufferingToken = getValue (str, parameters);
      err = setBuffering (bufferingToken);
      if (err) return true;
   }

   else if (compare (fieldName, "EVENTS"))
   {

      char const *neventsToken = getValue (str, parameters);
      err = setNevents (neventsToken);
      if (err) return true;
      }

   else if (compare (fieldName, "TIMEOUT"))
   {
      char const *timeout = getValue (str, parameters);
      err = setTimeout (timeout);
      if (err) return true;
   }

   else
   {
      // Not found
      return true;
   }

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Fills in the descriptor for an Generic contributor
  \return true on error

  \param[in]  str  The remaining part of the line to parse
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setGeneric (char **str)
{
   return false;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Fills in the descriptor for an TRIGGER contributor
  \return true on error

  \param[in]  str  The remaining part of the line to parse
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setTrigger (char **str)
{
   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Fills in the descriptor for an Generic contributor
  \return true on error

  \param[in]        str  The remaining part of the line to parse
  \param[in] parameters  Source of symbolic parameters
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::defineTrigger (char                      **str,
                                           CfgParameters const &parameters)
{
   bool err;

   while (1)
   {
      char const *fieldName = getToken (str);
      if (fieldName == nullptr) break;

      err = setTrigger (fieldName, parameters, str);
      if (err) return true;
   }

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Sets the specified TRIGGER field
  \return true on failure

  \param[in]  fieldName  The name of the field to set
  \param[in] parameters  Source of symbolic parameters
  \param[in]        str  The remaining part of the line to parse
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setTrigger (char const          *fieldName, 
                                        CfgParameters const &parameters,
                                        char                      **str)
{
   bool err;

   /// std::cout << "TRIGGER fieldname <" << fieldName << '>' << std::endl;
   if (compare (fieldName, "TRANSPORT"))
   {
      char const *trnTypeToken = getValue (str, parameters);
      err                      = setTransportType (trnTypeToken);
      if (err || static_cast<int>(m_trnType) <= 0) return true;
   }

   else if (compare (fieldName, "IP") || compare (fieldName, "NAME") )
   {
      // Get the IP/PIPE name
      char const *trnStrToken = getValue (str, parameters);
      /// std::cout << "Got transport <" << trnStrToken << '>' << std::endl;
      err                     = setTransportStr (trnStrToken);
      if (err) return true;
   }


   else if (compare (fieldName, "PORT"))
   {
      /// std::cout << "Got port" << std::endl;
         
      char const *port = getValue (str, parameters);
      m_port = strtoul (port, nullptr, 0);
   }

   else if (compare (fieldName, "RSSIFRAMES"))
   {
      /// std::cout << "Got buffering" << std::endl;
      
      // Get the Generic buffering (basically either RSSI frames or TCP/IP buffer
      char const *bufferingToken = getValue (str, parameters);
      err = setBuffering (bufferingToken);
      if (err) return true;
   }


   else if (compare (fieldName, "EVENTS"))
   {

      char const *neventsToken = getValue (str, parameters);
      err = setNevents (neventsToken);
      if (err) return true;
   }

   else if (compare (fieldName, "TIMEOUT"))
   {
      char const *timeout = getValue (str, parameters);
      err = setTimeout (timeout);
      if (err) return true;
   }

   else
   {
      // Not found
      return true;
   }


   
   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Gets the amount of buffering 
  \return true on error

  \param[in] token  The string to translate to a numerical buffering value
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setBuffering (char const *token)
{   
 
  // Get the buffering
  int nbuf = compare (token, "DEFAULT") 
           ? 0
           : strtoul (token, nullptr, 0); 
  /// std::cout << "NBUF = " << m_nbuf << std::endl;

   m_nbuf = nbuf;

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the transport type
  \return true on error

  \param[in] token The string to convert to a transport type
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setTransportType (char const *token)
{
   // Get the transport type

   if   (strcasecmp (token,  "RSSI") == 0)
   {
      /// std::cout << "Transport type = RSSI" << std::endl;
      m_trnType = CfgContributor::TransportType::Rssi;
   }

   else if (strcasecmp (token, "TCP/IP") == 0)
   {
      /// std::cout << "Transport type = TCP/IP" << std::endl;
      m_trnType = CfgContributor::TransportType::TcpIp;
   }

   else if (strcasecmp (token, "PIPE") == 0)
   {
      /// std::cout << "Transport type = PIPE" << std::endl;
      m_trnType = CfgContributor::TransportType::Pipe;
   }

   else
   {
      return true;
   }

   return false;

}            
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Gets the transport type string. 
  \return true on error

  \param[in] token  The transport string 

  \par
  For RSSI and TCP/IP this is the IP.  For PIPEs this is the pipe's name.
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setTransportStr (char const *trnStrToken)
{
   if ( (m_trnType == CfgContributor::TransportType::Pipe) &&
        (strcasecmp (trnStrToken, "DEFAULT") == 0) )
   {
      // use the default name for the trigger pipe
      /// std::cout  << "Transport Name = " << trnStrToken << std::endl;
      m_trnStr[0] = 0;
   }

   else
   {
      strcpy (m_trnStr, trnStrToken);
      /// std::cout << "Transport Name = " << m_trnStr << std::endl;
   }

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Gets the amount of event buffering 
  \return true on error

  \param[in] token  The string to translate to a numerical buffering value
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setNevents (char const *token)
{   
 
  // Get the buffering
  int nbuf = compare (token, "DEFAULT") 
           ? 0
           : strtoul (token, nullptr, 0); 
  /// std::cout << "NBUF = " << m_nbuf << std::endl;

   m_nevents = nbuf;

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Gets the amount of buffering 
  \return true on error

  \param[in] token  The string to translate to a numerical buffering value
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool CfgContributor::setTimeout (char const *token)
{   
 
  // Get the timeout (in seconds)
  int timeout = compare (token, "DEFAULT") 
              ? 0
              : strtoul (token, nullptr, 0); 
  /// std::cout << "NBUF = " << m_nbuf << std::endl;

  m_timeout = timeout;

   return false;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline void CfgContributor::print (std::string const &name, int instance) const
{
   static const char CtbTypes[4][8] = 
   {
      "Unknown",
      "None",
      "Trigger",
      "Generic"
   };

   static const char TrnTypes[5][8] =
   {
      "Unkown",
      "None",
      "Pipe",
      "Rssi",
      "TcpIp"
   };

   int         ctbIdx = static_cast<int>(m_ctbType) 
                      - static_cast<int>(ContributorType::Unknown);
   char const *ctbNam = CtbTypes[ctbIdx];


   int         trnIdx = static_cast<int>(m_ctbType) 
                      - static_cast<int>(TransportType::Unknown);
   char const *trnNam = TrnTypes[trnIdx];

   char const *trnStr = m_trnStr[0] == 0
                      ? "default"
                      : m_trnStr;


   std::cout << "Contributor  [" << std::hex   << instance   << "] " 
             << std::setw ( 8) << std::right << name      << ' '
             << std::setw ( 8) << std::right << ctbNam    << ' '
             << std::setw (16) << std::right << trnNam    << ' '
             << std::setw (16) << std::right << trnStr    << ' ';

   if (m_trnType != TransportType::Pipe)
   {
      std::cout << std::dec << std::setw (4) << m_port 
                << std::dec << std::setw (4) << m_nbuf
                << std::dec << std::setw (8) << m_nevents
                << std::dec << std::setw (3) << m_timeout;
      
   }
   

   std::cout << std::endl;
   return;
}
/* ====================================================================== */


#endif
