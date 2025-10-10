// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericBatchHeader.cc
  \brief  Accesses the Generic batch header structure.
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


#include "ldmx/data/client/GenericBatchHeader.hh"
#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Print the Generic batch header to the console

  \param[in] hdr  The Generic batch header
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericBatchHeader::print (GenericBatchHeader const *hdr)
{
   uint8_t  version = hdr->getVersion        ();
   uint8_t    width = hdr->getWidth          ();
   uint8_t sequence = hdr->getSequenceNumber ();
   uint8_t     rsvd = hdr->getRsvd           ();

   printf ("Batch Header:  Rsvd: %1" PRIx8 " "
                          "SequenceNumber: %2.2" PRIx8 " "
                          "AxiWidth: %1" PRIx8 "(%3d bits) "
                          "Version: %2.2" PRIx8 "\n",
           rsvd, sequence, width, (1 << (width+4)), version);
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */
