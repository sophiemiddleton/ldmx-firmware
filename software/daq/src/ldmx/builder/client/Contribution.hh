// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CONTRIBUTION_CONTRIBUTION_HH__
#define __LDMX_BUILDER_CLIENT_CONTRIBUTION_CONTRIBUTION_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Contribution.hh
  \brief  The base class for all contribution types to an LDMX Event 
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level
   directory of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the rogue software platform, including this file, may be 
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
 * 2018.01.31 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/utl/ListsInterlocked.hh"
#include "ldmx/utl/FixedPacket.hh"
#include "ldmx/utl/Timestamp.hh"
#include <rogue/interfaces/stream/Frame.h>
#include <rogue/interfaces/stream/FrameIterator.h>
#include <memory>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ====================================================================== */
namespace ldmx { namespace utl { class FixedPacket; }}
/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief The contribution base class

  \par
   All LDMX contributions inherent from this class
                                                                          */
/* ---------------------------------------------------------------------- */
class Contribution : public ldmx::utl::ListsInterlocked::Node
{
public:
   Contribution () { return; }
   Contribution (uint32_t   nbytes, 
                 uint32_t sequence,
                 uint64_t  rcvTime);
  ~Contribution ();

public:
   void *operator new    (size_t nbytes, ldmx::utl::FixedPacket &fpa);
   void  operator delete (void *p);

public:
   void print () const;

   enum class Type
   {
      Trigger = 0,                 /*!< Trigger contribution              */
      Generic     = 1                  /*!< Generic     contribution              */
   };

public:
   uint32_t             m_nbytes; /*!< The size in bytes                  */
   uint32_t           m_sequence; /*!< The sequence number                */
   uint64_t            m_rcvTime; /*!< Event builder receive time         */
   ldmx::utl::FixedPacket *m_fpa; /*!< Where to return the memory         */
   uint32_t                 m_id; /*!< Server identification              */
   enum Type              m_type; /*!< Contribution type                  */
};
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor to the contribution

  \param[in]   nbytes  The size, int bytes of the contribution
  \param[in] sequence  The sequence number of the contribution
  \param[in] rcvtTime  The absolute time, in nanoseconds the contribution 
                       was received
                                                                          */
/* ---------------------------------------------------------------------- */
inline Contribution::Contribution (uint32_t   nbytes,
                                   uint32_t sequence,
                                   uint64_t  rcvTime) :
   m_nbytes   (  nbytes),
   m_sequence (sequence),
   m_rcvTime  ( rcvTime)
{
   return;
}
/* ---------------------------------------------------------------------- */


   


/* ---------------------------------------------------------------------- *//*!

  \brief Custom allocator

  \param[in] nbytes  The number of bytes to allocate, effectively ignored,
                     This allocates from a fixed sized pool
  \param[in]    fpa  The fixed packet pool to allocate from

  \note
   This is blocking alllocation.  If no packet is available, the method
   waits until such time that there is one.
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *Contribution::operator 
       new (size_t nbytes, ldmx::utl::FixedPacket &fpa)
{
   return fpa.getW ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Contribution destructor
                                                                          */
/* ---------------------------------------------------------------------- */
inline Contribution::~Contribution ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \bref Custom deallocator

  \param[in] p  Pointer to the memory/packet to return to the pool
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Contribution::operator delete (void *p)
{
   Contribution *ctb = reinterpret_cast<Contribution *>(p);
   ctb->m_fpa->free (ctb);
}
/* ====================================================================== */


#endif
