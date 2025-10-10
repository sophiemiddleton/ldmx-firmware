// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_FRAGMENT_FRAGMENT_HH__
#define __LDMX_BUILDER_CLIENT_FRAGMENT_FRAGMENT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Fragment.hh
  \brief  The aggregation of data contributions (\e e.g. Trigger and generic)
          into a coherent class.  The aggregation is called an Fragment.
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
 * 2019.02.12 jjr Moved to namespace ldmx::builder::client
 * 2019.01.31 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/utl/List.hh"
#include "ldmx/utl/FixedPacket.hh"
#include <new>
#include <cstddef>
#include <cstdint>



/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

   class Contribution;

/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

   \brief The pool to allocate new fragments from
                                                                          */
/* ---------------------------------------------------------------------- */
class Pool : public ldmx::utl::FixedPacket
{
public:
   Pool ();
   Pool (int nfragments);

public:
};
/* ----------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Collects all the contributions into a coherent object
                                                                          */
/* ---------------------------------------------------------------------- */
class Fragment : public ldmx::utl::ListNode
{
public:
   Fragment (uint64_t         timestamp,
             uint32_t          sequence,
             uint32_t          expected,
             uint32_t              seed,
             ldmx::utl::List     *lists,
             uint32_t      *emptiedSet);

  ~Fragment ();


public:
   void *operator new    (size_t nbytes, Pool *pool);
   void  operator delete (void *p);

public:
   Pool             *m_pool;   /*!< Pool to return the fragment to        */
   uint64_t     m_timestamp;   /*!< Timestamp of latest contribution
                                    Used to time the contribution out     */
   uint32_t      m_sequence;   /*!< The sequence number                   */
   uint32_t       m_missing;   /*!< Bit mask of missing contributors,
                                    these are contributors that never
                                    arrrived                              */
   uint32_t       m_present;   /*!< Bit mask of all present contributors  */
   uint32_t    m_genericPresent;   /*!< Bit mask of generic contributors          */
   uint32_t    m_trgPresent;   /*!< Bit mask of TRG contributor           */
   uint32_t     m_duplicate;   /*!< Duplicate contributors                */
   uint64_t  m_received[32];   /*!< The time the contribution was received*/
   Contribution *m_ctbs[32];   /*!< The contributions                     */
};
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy construction
                                                                          */
/* ---------------------------------------------------------------------- */
inline Pool::Pool ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline void *Fragment::operator
       new (size_t nbytes, 
            Pool    *pool)
{
   void *ptr = pool->getW ();
   return ptr;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline void Fragment::operator delete (void *p)
{
   Fragment *fragment = reinterpret_cast<Fragment *>(p);
   fragment->m_pool->free (p);
   return;
}
/* ====================================================================== */


#endif
