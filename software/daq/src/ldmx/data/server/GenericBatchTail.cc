// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericBatchTail.cc
  \brief  Defines filling the Generic batch tail word structure.
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

   No part of the LDMX platform, including this file, may be 
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


#include "ldmx/data/server/GenericBatchTail.hh"
