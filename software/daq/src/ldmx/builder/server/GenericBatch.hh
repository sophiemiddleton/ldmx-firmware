// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_GENERICBATCH_HH__
#define __LDMX_BUILDER_SERVER_GENERICBATCH_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/GenericBatch.hh
  \brief  Composes and sends Generic event batches for the builder server
          emulator
  \author S. Middleton

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the LDMX software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the terms 
   ontained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.21 jjr Adapted from the HPS version
 * 2019.03.19 jjr Created
 * 2025 Sophie Middleton further adapting for LDMX
\* ---------------------------------------------------------------------- */


class GenericGenerator  : public  ldmx::data::server::GenericBatch
{
public:
   GenericGenerator (uint8_t rceAddress);

   static const size_t NBytes = 32 * 1024;

   void addHeader  (ldmx::data::server::GenericBatch &batch,);
   int  addEvent   (ldmx::data::server::GenericBatch &batch,
                    int                   nmultisamples);
   
public:
   uint32_t           m_batchSeqNumber;
   uint32_t           m_eventSeqNumber;
   uint32_t                m_curEvents;
   uint8_t                m_rceAddress;

};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Generic generator
                                                                          */
/* ---------------------------------------------------------------------- */
GenericGenerator::GenericGenerator (uint8_t rceAddress) :
   m_batchSeqNumber (0),
   m_eventSeqNumber (0),
   m_curEvents      (0),
   m_rceAddress     (rceAddress) 
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add the batch header
                                                                          */
/* ---------------------------------------------------------------------- */
void GenericGenerator::addHeader  (ldmx::data::server::GenericBatch &batch)
{
   using namespace ldmx::data::server;
   
   // Clear the old batch out
   batch.reset ();

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

  \param[in] nmultisamples  The number of multi-sample data records
                                                                          */
/* ---------------------------------------------------------------------- */
int GenericGenerator::addEvent (ldmx::data::server::GenericBatch   &batch,
                            int                     nmultisamples)
{
   int febAddress    = 0x55;
   int hybridAddress = 0x44;


   // -----------------------------------
   // Create a new event for this batcher
   // -----------------------------------
   GenericEvent    evt (*batch);


   // ------------------------
   // Add the Generic event header
   // ------------------------
   GenericHeader *eHdr __attribute__ ((unused))
      = new (&evt) GenericHeader (m_eventSeqNumber, rceAddress);
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
         samples[isample] = (ievt << 8) | (ims << 4) | (isample);
      } 


      // ----------------------------------------
      // Create the Generic multi-sample summary word
      // ----------------------------------------
      uint8_t apvChannel = ims;
      uint8_t apvNumber  = ievt;
      bool     readError = false;
      bool          tail = false;
      bool          head = false;
      bool    filterFlag =  true;
      uint32_t   summary = GenericMultiSample::composeSummary (rceAddress,
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

   /// printf ("Allocated trailer %2d @ %p\n", ievt, (void *)eTlr);


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


   return m_curEvents += 1;
}
/* ---------------------------------------------------------------------- */
   
#endif   


