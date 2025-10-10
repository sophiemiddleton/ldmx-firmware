// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericGenerator.cc
  \brief  Generates a simulated Generic batch of events
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
 * 2019.03.05 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "ldmx/data/server/GenericGenerator.hh"
#include "ldmx/data/server/GenericBatch.hh"
#include "ldmx/data/server/GenericBatchHeader.hh"
#include "ldmx/data/server/GenericEvent.hh"
#include "ldmx/data/server/GenericHeader.hh"
#include "ldmx/data/server/GenericMultiSample.hh"
#include "ldmx/data/server/GenericTrailer.hh"
#include "ldmx/data/server/GenericBatchTail.hh"


#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Generic generator

  \param[in] rceAddress The sourcing RCE
                                                                          */
/* ---------------------------------------------------------------------- */
GenericGenerator::GenericGenerator (uint8_t rceAddress) :
   m_batchSeqNumber (0),
   m_eventSeqNumber (0),
   m_rceAddress     (rceAddress) 
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Resets the batcher's and any other internal context

  \param[in:out] batch Pointer to the destinaion Generic batch 

  \note 
   It is the responsibility of the user to clear the previous batch
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericGenerator::reset (ldmx::data::server::GenericBatch *batch)
{
   batch->reset ();
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add the batch header

  \param[in:out] batch Pointer to the destinaion Generic batch 
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericGenerator::addHeader (ldmx::data::server::GenericBatch *batch)
{
   using namespace ldmx::data::server;
   

   GenericBatchHeader *bHdr __attribute__ ((unused))
                 = new (batch) GenericBatchHeader (m_batchSeqNumber);

   /// printf ("Allocate batch %2d @ %p\n", m_batchSequence, (void *)bHdr);


   m_batchSeqNumber += 1;

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add an event with \a nmultisamples data records
  \return The number of events in the current batch

  \param[in:out]     batch Pointer to the destinaion Generic batch 
  \param[in] nmultisamples The number of multi-sample data records
                                                                          */
/* ---------------------------------------------------------------------- */
int GenericGenerator::addEvent (ldmx::data::server::GenericBatch   *batch,
                            int                     nmultisamples)
{
   using namespace ldmx::data::server;

   int    febAddress    = 0x55;
   int    hybridAddress = 0x44;
   uint32_t eventNumber = batch->getNevents ();


   // -----------------------------------
   // Create a new event for this batcher
   // -----------------------------------
   GenericEvent    evt (batch);


   // ------------------------
   // Add the Generic event header
   // ------------------------
   GenericHeader *eHdr __attribute__ ((unused))
      = new (&evt) GenericHeader (m_eventSeqNumber, m_rceAddress);
   /// printf ("Allocated event %2d @ %p\n", m_eventSeqNumber, (void *)eHdr);
   m_eventSeqNumber += 1;


   // --------------------------------
   // Create the Generic multi-sample data
   // --------------------------------
   uint16_t samples[6];
   for (int ims = 0; ims < nmultisamples; ++ims)
   {
      // -------------------------------------
      // Makeup some fake data for the samples
      // -------------------------------------
      for (int isample = 0; isample < 6; ++isample)
      {
         samples[isample] = (eventNumber << 8) | (ims << 4) | (isample);
      } 


      // ----------------------------------------
      // Create the Generic multi-sample summary word
      // ----------------------------------------
      uint8_t apvChannel = ims;
      uint8_t apvNumber  = eventNumber;
      bool     readError = false;
      bool          tail = false;
      bool          head = false;
      bool    filterFlag =  true;
      uint32_t   summary = GenericMultiSample::composeSummary (m_rceAddress,
                                                           febAddress,
                                                           apvChannel,
                                                           apvNumber,
                                                           hybridAddress,
                                                           readError,
                                                           tail,
                                                           head,
                                                           filterFlag);

      // -------------------------------------
      // Add this Generic multi-sample data record
      // -------------------------------------
      GenericMultiSample *msData __attribute__ ((unused)) 
         = new (&evt) GenericMultiSample (samples, summary);

      /// printf (
      ///  "Allocate multisample data %2d @ %p  summary: %8.8" PRIx32 "\n", 
      /// ims, (void *)msData, summary);
   }


   // -----------------------------------
   // Add the Generic event trailer
   // This will add any necessary padding
   // -----------------------------------
   GenericTrailer *eTlr __attribute__ ((unused))
      = new (&evt) GenericTrailer (nmultisamples, 0);

   /// printf ("Allocated trailer %2d @ %p\n", eventNumber, (void *)eTlr);


   // ------------------------------------------------------
   // Add the batch trailer
   // ---------------------
   // The byte size is specified to be number of valid bytes
   // in the event. This count excludes any padding needed
   // to reach the AXI stream bit size boundary
   // ------------------------------------------------------
   uint32_t nvalid = evt.getNValid ();
   /// printf ("NValid = %8" PRIx32 "\n", nvalid);

   GenericBatchTail *bTail __attribute__ ((unused))
                 = new (batch) GenericBatchTail (nvalid, false);
   /// printf ("BatchTail = %p\n", bTail);      

   auto nevents = batch->getNevents ();

   return nevents;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Quick, no-frills Generic batch generator

  \param[in:out]    batch Pointer to the destinaion Generic batch 
  \param[in]      nevents Number events in the Generic batch
  \param[in] nmultisample Number of multi-sample data records in each
                          event
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericGenerator::generate (ldmx::data::server::GenericBatch *batch,
                             int                         nevents,
                             int                   nmultisamples)
{
   using namespace ldmx::data::server;


   addHeader (batch);

   for (int ievt = 0; ievt < nevents; ++ievt)
   {
      addEvent (batch, nmultisamples);
   }

   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */
