// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_GENERICEVENT_HH__
#define __LDMX_DATA_SERVER_GENERICEVENT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericEvent.hh
  \brief  Manages the memory containing 1 Generic event.
  \author JJRussell - russell@slac.stanford.edu

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
 * 2019.02.21 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <cinttypes>
#include <cstdio>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {

   class GenericBatch;
   class GenericHeader;
   class GenericMultiSample;
   class GenericTrailer;

/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace datat                                                      */
} /* namespace ldmx                                                       */
/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Create a Generic Event record
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericEvent
{
public:
   GenericEvent (GenericBatch *batch);

public:
   static const int NPadBytes = sizeof (uint32_t);

public:
   GenericHeader      *getHeader      (std::size_t nbytes);
   GenericMultiSample *getMultiSample (std::size_t nbytes);
   GenericTrailer     *getTrailer     (std::size_t nbytes);

   void            reset          ();
   uint32_t        getSize        () const;
   uint32_t        getNValid      () const;

public:
   GenericBatch *m_batch; /*!< The controlling batcher                        */
   uint32_t  m_nrecs; /*!< The number of multi-sample data records        */
   uint32_t m_nbytes; /*!< The number of bytes in this event              */
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/server/GenericBatch.hh"


/* ====================================================================== */
/* IMPLEMENTAITION                                                        */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Construct a Generic Event Record

  \param[in]  buffer The buffer to use
  \param[in] nbuffer The number of 32-bit words in the buffer
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericEvent::GenericEvent (GenericBatch *batch) :
   m_batch  (batch),
   m_nrecs  (    0),
   m_nbytes (    0)
{
   batch->m_nevents += 1;
   return;
}
/* ---------------------------------------------------------------------- */
   


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to where in the buffer to place the GenericHeader
  \return A pointer to where in the buffer to place the GenericHeader

  \param[in] The number of bytes to allocate for the header
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericHeader *GenericEvent::getHeader (std::size_t nbytes)
{
   m_nbytes += nbytes;
   return reinterpret_cast<GenericHeader *>
         (m_batch->m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to where in the buffer to place the 
          GenericMultiSample data.
  \return A pointer to where in the buffer to place the GenericMultiSample data

  \param[in] nbytes The number of bytes to allocate for the sample
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericMultiSample *GenericEvent::getMultiSample (std::size_t nbytes)
{
   m_nbytes += nbytes;
   return reinterpret_cast<GenericMultiSample *>
         (m_batch->m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to where in the buffer to place the GenericTrailer
  \return A pointer to where in the buffer to place the GenericTailer

  \param[in] nbytes The number of bytes in the trailer.  This count
                    includes any pad bytes needed to reach the AXI
                    stream bit boundary.
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericTrailer *GenericEvent::getTrailer (std::size_t nbytes)
{
   m_nbytes += nbytes;
   return reinterpret_cast<GenericTrailer *>
         (m_batch->m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of bytes in the event
  \return The number of bytes in the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericEvent::getSize () const
{
   return m_nbytes;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of valid bytes in the event
  \return The number of valid bytes in the event. 

  \note 
   This does not include the number of pad bytes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericEvent::getNValid () const
{
   return m_nbytes - NPadBytes;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Resets the event context, \e .e.g the number of records and
          number of bytes in this event are reset to 0.
                                                                          */
/* ---------------------------------------------------------------------- */
inline void GenericEvent::reset ()
{
   m_nbytes = 0;
   m_nrecs  = 0;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
