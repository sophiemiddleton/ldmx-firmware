// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericEvent.cc
  \brief  Access the memory buffer containing 1 Generic event.
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


#include "ldmx/data/client/GenericEvent.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Prints the Generic event to the console
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericEvent::print (GenericEvent const *evt)
{
   auto header = evt->getHeader ();
   header->print ();

   int nms = evt->getMultiSampleCount ();
   auto ms = evt->getMultiSample      ();


   ldmx::data::client::GenericMultiSample::printTitle ();
   for (int ims = 0; ims < nms; ims++)
   {
      ms[ims].print (ims);
   }

   auto trailer = evt->getTrailer ();
   trailer->print ();
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */
