// -*-Mode: C++;-*-

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
 * 2019.02.12 jjr Moved to namespace ldmx::builder::client
 * 2019.01.31 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "Fragment.hh"
#include "Contribution.hh"
#include "GenericContributor.hh"

#include <cinttypes>
#include <cstdio>


#define REPORT_CONSTRUCTOR 0
#define REPORT_DESTRUCTOR  0


/* ====================================================================== */



// ----------------------------------------
// Diagnostics for the fragment constructor
// ----------------------------------------
namespace constructor
{
   #if REPORT_CONSTRUCTOR

   inline static void report (uint32_t sequence)
   {
      printf ("Fragment::constructor %8.8" PRIx32 "", sequence);
      return;
   }


   inline static void report (int id, uint32_t emptied)
   {
      printf (" [%d:%8.8" PRIx32 "]", id, emptied);
      return;
   }
   

   inline static void report ()
   {
      putchar ('\n');
      return;
   }

   #else

   inline static void report (uint32_t sequence)        { return; }   
   inline static void report (int id, uint32_t emptied) { return; }
   inline static void report ()                         { return; }

   #endif
}



// ---------------------------------------
// Diagnostics for the fragment destructor
// ---------------------------------------
namespace destructor
{
   #if REPORT_DESTRUCTOR

   inline static void report (uint32_t sequence, uint32_t present)
   {
      printf ("Fragment:: destructor %8.8" PRIx32 ":%8.8" PRIx32 "",
              sequence, present);
      return;
   }


   inline static void reportGeneric (int                     id, 
                                 uint32_t          sequence,
                                 unsigned long int useCount)
   {
      printf (" [%d:%8.8" PRIx32 ": %2ld (Generic)]",
              id, sequence, useCount);
      return;
   }


   inline static void reportTrg (int id, uint32_t sequence)
   {
      printf (" [%d:%8.8" PRIx32 "(trg)]", id, sequence);
   }


   inline static void report ()
   {
      putchar ('\n');
      return;
   }

   #else

   inline static void report    (uint32_t          sequence,
                                 uint32_t           present) { return; }
   inline static void reportGeneric (int                     id, 
                                 uint32_t          sequence,
                                 unsigned long int useCount) { return; }
   inline static void reportTrg (int  id, uint32_t sequence) { return; }
   inline static void report    ()                           { return; }

   #endif

}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructs a new fragment

  \param[in]  expected  A bit mask of the contributions
                                                                          */
/* ---------------------------------------------------------------------- */
Fragment::Fragment (uint64_t     timestamp,
                    uint32_t      sequence,
                    uint32_t      expected,
                    uint32_t          seed,
                    ldmx::utl::List *lists,
                    uint32_t   *emptiedSet) :
   m_timestamp (       timestamp),
   m_sequence  (        sequence),
   m_missing   (expected & ~seed),
   m_present   (            seed),
   m_duplicate (               0)
{
   uint32_t    emptied = 0;
   uint32_t genericPresent = 0;
   uint32_t trgPresent = 0;

   constructor::report (sequence);


   // -----------------------------------------------------------------------
   // !!! KLUDGE: The initialization of the contribution timestamp is kludged
   // -----------------------------------------------------------------------
   while (seed)
   {

      // -----------------------------------------------
      // Warning, do not flip the order of last and node
      // Removing a node changes the definition of last
      // -----------------------------------------------
      int         id = __builtin_ctz (seed);
      ListNode *last = lists[id].last    ();
      ListNode *node = lists[id].remove  ();
      uint32_t  mask = (1 << id);


      // -----------------------------------------
      // If this removal emptied the list, note it
      // -----------------------------------------
      if (node == last)
      {
         emptied |= mask;
      }

      constructor::report (id, emptied);


      // ------------------------------------------
      // Set the masks of generic and TRG contributions
      // ------------------------------------------
      Contribution const *ctb = reinterpret_cast<decltype(ctb)>(node);
      if      (ctb->m_type == Contribution::Type::Generic) genericPresent |= mask;
      else if (ctb->m_type == Contribution::Type::Trigger) trgPresent |= mask;


      // -------------------------------------
      // Add this contribution to the fragment
      // Record when it has added
      // -------------------------------------
      m_ctbs    [id] = reinterpret_cast<Contribution *>(node);
      m_received[id] = timestamp;


      // -------------------------------------------------------
      // Remove this contribution from those still to be handled
      // -------------------------------------------------------
      seed &= ~ mask;
   }

   constructor::report ();

  
   // -----------------------------------------
   // Set the mask of generic & TRG contributions
   // Return the set of lists that were emptied
   // -----------------------------------------
   m_genericPresent = genericPresent;
   m_trgPresent = trgPresent;
   *emptiedSet  = emptied;


   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Fragment destructor
                                                                          */
/* ---------------------------------------------------------------------- */
Fragment::~Fragment ()
{
   destructor::report (m_sequence, m_present);


   // -------------------------------------------------
   // Delete each of the contributions that are present
   // -------------------------------------------------
   uint32_t set = m_present;
   while   (set)
   {

      using Type = Contribution::Type;


      // ----------------------------------
      // Get the id of and its contribution
      // ----------------------------------
      int     id = __builtin_ctz (set);
      auto  *ctb = m_ctbs[id];


      // -----------------------------------------
      // Separate destruction by contribution type
      // -----------------------------------------
      if (ctb->m_type == Type::Generic)
      {
         GenericContributor *generic = reinterpret_cast<decltype (generic)>(ctb);

         destructor::reportGeneric (id, ctb->m_sequence, generic->m_core.use_count ());

         generic->m_core.reset ();
      }
      else
      {
         destructor::reportTrg (id, ctb->m_sequence);
      }


      // ------------------------------
      // Delete the contribution itself
      // ------------------------------
      delete ctb;


      // -------------------------------------------------------
      // Remove this contribution from those still to be handled
      // -------------------------------------------------------
      set &= ~(1 << id);
   }

   destructor::report ();

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Initializes the static information of one fragment in the pool

  \param[in]      pool       The parent memory pool
  \param[in:out]  packet     The paket to initialize
  \param[in]      packetSize The size, in bytes, of the packet
  \param[in]      pcbOffset  The offset, in bytes, of the packet control
                             block

  \par
   The initialization consists of seeding the parent memory pool into this
   packet so that it can be released.
                                                                          */
/* ---------------------------------------------------------------------- */
static void initFragment (void *pool,
                          void *packet,
                          int   packet_size,
                          int   pcb_offset)
{
   Fragment *fragment = reinterpret_cast<Fragment *>(packet);
   fragment->m_pool   = reinterpret_cast<Pool     *>(  pool);
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
   \brief Populate the fragment pool

   \param[in] nfragments  The number of fragments in the pool
                                                                          */
/* ---------------------------------------------------------------------- */
Pool::Pool (int nfragments) :
   ldmx::utl::FixedPacket (nfragments, 
                           sizeof (Fragment),
                           0, 
                           initFragment,
                           this)
{
   return;
}
/* ====================================================================== */
