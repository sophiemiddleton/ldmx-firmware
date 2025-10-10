// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericBatch.cc
  \brief  Accesses 1 batch of Generic events.
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
 * 2019.03.02 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/client/GenericBatch.hh"
#include "ldmx/data/client/GenericBatchHeader.hh"
#include "ldmx/data/client/GenericEvent.hh"
#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!
  \brief Prints the Generic batch of events to the console

  \param[in] batch The Generic batch of events to print
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericBatch::print (GenericBatch const *batch)
{
   GenericBatchHeader::print (batch->getHeader ());

   auto rbeg = batch->rbegin ();
   auto rend = batch->rend   ();

   // ----------------
   // Print the events
   // ----------------
   for (auto event = rbeg;  event != rend; event++)
   {
      putchar ('\n');
      event->print ();

      GenericTrailer   const *trailer = event->getTrailer ();
      GenericBatchTail const    *tail = reinterpret_cast<decltype(tail)>
                                                         (trailer+1);

      tail->print  ();
   }

   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

