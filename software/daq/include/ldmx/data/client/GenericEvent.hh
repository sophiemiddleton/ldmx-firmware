// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_GENERICEVENT_HH__
#define __LDMX_DATA_CLIENT_GENERICEVENT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericEvent.hh
  \brief  Defines cataloging  Generic events
  \author JJRussell - russell@slac.stanford.edu

  \par
   This various member fields of the GenericEvent.


  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level
   directory of this distribution and at: 

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
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.02.28 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCE                                                      */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {

   class GenericHeader;
   class GenericMultiSample;
   class GenericTrailer;

} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */






/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to catalog the Generic events
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericEvent
{
public:
   GenericEvent () { return; }
   GenericEvent (uint8_t const *ptr, uint32_t nbytes);


public:
   GenericHeader       const *getHeader        () const;
   GenericMultiSample  const *getMultiSample   () const;
   GenericTrailer      const *getTrailer       () const;
   uint32_t               getNBytes           () const;
   uint32_t               getMultiSampleCount () const;

public:
   static void print (GenericEvent const *evt);
   void        print () const;


public:
   GenericHeader  const  *m_header; /*!< Pointer to the Generic event header      */
   GenericTrailer const *m_trailer; /*!< Pointer to the Generic event trailer     */
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */



#include "ldmx/data/client/GenericHeader.hh"
#include "ldmx/data/client/GenericMultiSample.hh"
#include "ldmx/data/client/GenericTrailer.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructors the information to describe one Generic event

  \param[in]    ptr  Pointer to the memory just after end of the Generic event
  \param[in] nbytes  The length, in bytes, of the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericEvent::GenericEvent (uint8_t const *ptr, uint32_t nbytes) :
   m_header  (reinterpret_cast<GenericHeader  const *>(ptr - nbytes)),
   m_trailer (reinterpret_cast<GenericTrailer const *>(ptr 
                       - sizeof(ldmx::data::common::GenericTrailer)))
{
   //// printf ("GenericEvent %4" PRId32 " @ %p\n", nbytes, (void *)ptr);
   //// printf ("   Header: @ %p\n", (void *)m_header);
   //// printf ("  Trailer: @ %p\n", (void *)m_trailer);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a readonly pointer to the Generic event header
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericHeader const *GenericEvent::getHeader () const
{
   return reinterpret_cast<GenericHeader const *>(m_header);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a readonly pointer to the beginning of Generic event
          multisample data
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericMultiSample const *GenericEvent::getMultiSample () const
{
   return reinterpret_cast<GenericMultiSample const *>(m_header + 1);
}
/* ---------------------------------------------------------------------- */   



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a readonly pointer to the Generic event trailer
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericTrailer const *GenericEvent::getTrailer () const
{
   return m_trailer;
}
/* ---------------------------------------------------------------------- */
 


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the length of the Generic event in bytes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericEvent::getNBytes () const
{
   return reinterpret_cast<uint8_t const *>(m_trailer + 1)
        - reinterpret_cast<uint8_t const *>(m_header);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the length of the Generic event in bytes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericEvent::getMultiSampleCount () const
{
   return m_trailer->getMultiSampleCount ();
}
/* ---------------------------------------------------------------------- */


 
/* ---------------------------------------------------------------------- *//*!

  \brief Prints the Generic event to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void GenericEvent::print () const
{
   print (this);
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
