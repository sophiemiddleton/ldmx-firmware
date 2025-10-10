// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_GENERICBATCH_HH__
#define __LDMX_DATA_SERVER_GENERICBATCH_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericBatch.hh
  \brief  Manages the context and memory containing 1 batch of Generic events.
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


#include "ldmx/data/server/Allocator.hh"
#include <cinttypes>
#include <cstddef>

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {

   class GenericBatchHeader;
   class GenericEvent;
   class GenericBatchTail;

/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Create an Generic Batch
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericBatch
{
public:
   GenericBatch (void *buffer, size_t nbytes);

public:
   GenericBatchHeader *getHeader(std::size_t nbytes);
   GenericEvent       *getEvent ();
   GenericBatchTail   *getTail  (std::size_t nbytes);

public:
   // Return a pointer to the buffer and the number of bytes left
   void  const  *getBuffer () const;
   uint32_t      getNbytes () const;
   uint32_t           left () const;
   uint32_t     getNevents () const;
   void              reset ();

public:
   Allocator  m_alloc; /*!< The allocator                                 */
   uint32_t m_nevents; /*!< The number of events in the batch so far      */
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/server/Allocator.hh"

/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Construct a Generic Event Batch

  \param[in] allocator The memory allocator 
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericBatch::GenericBatch (void *buffer, size_t nbytes) :
   m_alloc   (buffer, nbytes),
   m_nevents (             0)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get a pointer to the start of the buffer memory
  \return The number of bytes left
                                                                          */
/* ---------------------------------------------------------------------- */
inline void const *GenericBatch::getBuffer () const
{
   return m_alloc.buffer ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Get the current byte index, 
          essentially the number of bytes allocated
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t  GenericBatch::getNbytes () const
{
   return m_alloc.used ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of bytes left
  \return The number of bytes left
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatch::left () const
{
   return m_alloc.left ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Clears/resets this batch of events
                                                                          */
/* ---------------------------------------------------------------------- */
inline void GenericBatch::reset ()
{
   m_nevents   = 0;
   m_alloc.reset ();
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Allocates the memory for and returns a pointer to where in the
          batch's buffer to place the GenericBatchHeader
  \return A pointer to where in the batch's buffer to place the
          GenericBatchHeader
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericBatchHeader *GenericBatch::getHeader (std::size_t nbytes)
{
   return reinterpret_cast<GenericBatchHeader *>(m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to where in the buffer to place the GenericEvent
  \return A pointer to where in the buffer to place the GenericEvent

  \note
   Because the size of an event is generally unknown until it is built,
   no memory is allocated, this simply locates where the event will be
   placed.  
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericEvent *GenericBatch::getEvent ()
{
   return reinterpret_cast<GenericEvent *>(m_alloc.next ());
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Allocates the memory for and returns a pointer to where in the 
          batch's buffer to place the GenericBatchTail.
  \return A pointer to where in the buffer to place the GenericBatchTail
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericBatchTail *GenericBatch::getTail (std::size_t nbytes)
{
   return reinterpret_cast<GenericBatchTail *>(m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the number of events in the Generic batch so far
  \return The number of events in the Generic batch so far
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatch::getNevents () const
{
   return m_nevents;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
