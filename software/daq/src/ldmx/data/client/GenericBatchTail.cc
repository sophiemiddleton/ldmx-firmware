// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericBatchTail.cc
  \brief  Accesses the Generic batch tail word structure.
  \author JJRussell - russell@slac.stanford.edu

  \par
   This 64-bit value acts as a separator between each GenericEvent.
   It is not a trailer in the the tradtional meaning, \e i.e. is does 
   not just appear as the last word in the batched events.

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


#include "ldmx/data/client/GenericBatchTail.hh"
#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Prints the Generic batch tail to the console
                                                                          */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- */
void GenericBatchTail::print (GenericBatchTail const *tail)
{
   uint32_t    nvalid = tail->getNValid     ();
   uint8_t      tDest = tail->getTDest      ();
   uint8_t tUserFirst = tail->getTUserFirst ();
   bool     errorFlag = tail->getErrorFlag  ();
   uint8_t  tUserLast = tail->getTUserLast  ();
   uint8_t      width = tail->getWidth      ();
   uint8_t       bits = 1 << (width + 4);
   uint8_t       rsvd = tail->getRsvd       ();


   printf ("Batch Tail : nvalid %8" PRIx32 " "
                        "tDest: %2.2" PRIx8 " "
                        "tUserFirst:Last:Err:%2.2" PRIx8 ":%2.2" PRIx8 ":%d "
                        "width:%1" PRIx8 "(%3d bits) "
                        "rsvd:%1" PRIx8 "\n",
           nvalid, 
           tDest, 
           tUserFirst, tUserLast,
           errorFlag,
           width, bits,
           rsvd);

   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */
