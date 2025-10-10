// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericHeader.cc
  \brief  Accesses the Generic header structure.
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


#include "ldmx/data/client/GenericHeader.hh"
#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Prints the Generic event header to the console

  \param[in] hdr  The Generic event header to print
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericHeader::print (ldmx::data::client::GenericHeader const *hdr)
{
   uint32_t eventCounter = hdr->getEventCounter ();
   uint8_t    rceAddress = hdr->getRceAddress   ();
   uint8_t          mbf0 = hdr->getMbf0         ();

   printf ("Generic  Header: eventCounter: %8.8" PRIx32 " "
                        "rceAddress: %2.2" PRIx8 " "
                        "mbf0: %2.2" PRIx8 "\n",
           eventCounter, rceAddress, mbf0);
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


