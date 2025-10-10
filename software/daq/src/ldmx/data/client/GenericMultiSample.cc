// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericMultiSample.cc
  \brief  Accesses the Generic multi-sample data structure.
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


#include "ldmx/data/client/GenericMultiSample.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- */
void GenericMultiSample::printTitle ()
{
   printf ("Data: RCE FEB APV.chn Hyb Err Tail Head Filter "
           "Smp0 Smp1 Smp2 Smp3 Smp4 Smp5\n");
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the multi-sample data record

  \param[in]   ms  The multi-sample data record
  \param[in]  ims  The instance number of the multi-sample data record
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericMultiSample::print (GenericMultiSample const *ms,
                                   int                  ims)
{
   uint8_t    rceAddress = ms->getRceAddress    ();
   uint8_t    febAddress = ms->getFebAddress    ();
   uint8_t    apvChannel = ms->getApvChannel    ();
   uint8_t     apvNumber = ms->getApvNumber     ();
   uint8_t hybridAddress = ms->getHybridAddress ();
   bool        readError = ms->getReadError     ();
   bool             tail = ms->getTail          ();
   bool             head = ms->getHead          ();
   bool       filterFlag = ms->getFilterFlag    ();

   uint16_t const *samples = ms->locateSamples ();

   printf (" %3x:"
           "  %2.2" PRIx8
           "  %2.2" PRIx8
           " %3"   PRIx8 ".%2.2" PRIx8
           "   %2.2" PRIx8
           "   %1d    %1d    %1d      %1d",
           ims,
           rceAddress, febAddress, apvNumber, apvChannel, 
           hybridAddress, readError, tail, head, filterFlag);


   printf (" %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 ""
           " %4.4" PRIx16 " %4.4" PRIx16 "\n",
           samples[0], samples[1], samples[2],
           samples[3], samples[4], samples[5]);

   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */
