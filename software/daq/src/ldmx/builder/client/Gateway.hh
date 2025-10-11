// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_GATEWAY_HH__
#define __LDMX_BUILDER_CLIENT_GATEWAY_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   Gateway.hh
  \brief  Gateway to the LDMX event/fragment builder -- implementation
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the rogue software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the terms 
   ontained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- -------------------------------------------------------
 * 2021.04.27 jjr Reference CfgParser.hh from public include directory
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.04.13 jjr Created, isolated from old RogueCodaGw.cc
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/client/Configuration.hh"
#include "ldmx/builder/client/CfgParser.hh"
#include "Builder.hh"
#include "Parameters.hh"
#include "Contributions.hh"
#include "RssiReceiver.hh"
#include "TriggerReceiver.hh"
#include "GenericContributor.hh"

#include "ldmx/utl/List.hh"
#include "ldmx/utl/Timeout.hh"

#include <rogue/protocols/batcher/CoreV1.h>
#include <rogue/protocols/batcher/Data.h>

#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief The Rogue Gateway implementation class
                                                                          */
/* ---------------------------------------------------------------------- */
class Gateway 
{
public:
   Gateway ();
  ~Gateway ();

public:
   int32_t download (char const *cfgFile,      char const *usrString);
   int32_t download (ldmx::builder::client::Configuration const &cfg);

   int32_t prestart ();
   int32_t go       ();

   int32_t trigger  (uint32_t    *trigData, uint32_t trigLength);
   int32_t event    (uint32_t *eventBuffer, uint32_t      index);
   int32_t update   (uint32_t     syncFlag);
   int32_t config   (uint32_t *eventBuffer);
   int32_t end      ();
   void    close    ();

   static int assembler (Builder  *builder);


public:
   Contributions          m_ctbs;
   RssiReceiver    m_genericReceiver;
   TriggerReceiver m_trgReceiver;
   Builder             m_builder;
};
/* ====================================================================== */





/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static int32_t  copyFragment (uint32_t *dst32, Fragment const *fragment);
static uint32_t      copyGeneric (uint8_t    *dst, GenericContributor      const      *GenericContributor);
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION: Gateway                                                */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the LDMX Rogue/Code gateway implementation
                                                                          */
/* ---------------------------------------------------------------------- */
inline Gateway::Gateway ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Destructor for the LDMX Rogue/Code gateway implementation
                                                                          */
/* ---------------------------------------------------------------------- */
inline Gateway::~Gateway ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline int32_t Gateway::download (char const *cfgFile, char const *usrString)
{
   CfgParser parser;
   
   bool err;

   err = parser.open (cfgFile);
   if (err)
   {
      std::cout << "Failed to open: " << cfgFile << std::endl;
      return -1;
   }


   {
      ldmx::builder::client::Configuration cfg;
      err = parser.parse (&cfg);
      if (err)
      {
         std::cout << "Failed to parse: " << cfgFile << std::endl;
         return -1;
      }
      else
      {
         cfg.print ();
         int32_t status = download (cfg);
         return  status;
      }
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  This is the configuration callback
  \return Status

  \param[in] cfg A fully completed binary configuration
                                                                          */
/* ---------------------------------------------------------------------- */
inline int32_t Gateway::download(ldmx::builder::client::Configuration const &cfg)
{
   // Total up the number of contribution sources
   int ncontributions = 1                             //  Control contribution
                      + cfg.m_contributors.size (); //  Generic     contributions
                      

   // Construct the synchonization class for incoming messages and data
   new (&m_ctbs) Contributions (ncontributions);


   // Construct the Generic receivers at contributions = [1 , 1 + nGeneric receivers]
   new (&m_genericReceiver) RssiReceiver (cfg, &m_ctbs, 1);


   uint32_t allContributors = m_genericReceiver.getContributors ();
//                          | m_trgReceiver.getContributors ();


   // Configure the builder for the set of all possible contributors
   new (&m_builder) Builder (allContributors, cfg.m_nfragments, &m_ctbs);


   // Launch the builder in a wait state, it is enabled in the go stage
   m_builder.launch ();


   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Prestart control function
  \return Status

                                                                          */
/* ---------------------------------------------------------------------- */
inline int32_t Gateway::prestart ()
{
   // Start/Enable the trigger receiver
   //m_gw->m_trgReceiver.start ();
   //printf ("Trigger started\n");


   // Start/Enable the Generic contributor's connections
   m_genericReceiver.start  ();


   // Check that all connections started
   auto missing = m_genericReceiver.waitForConnections ();


   // Report any missing contributors and abort if any
   if (missing)
   {
      fprintf (stderr, 
               "Aborting::missing contributors = %8.8" PRIx32 "\n", 
               missing);
      return false;
   }

   return true;
}
/* ---------------------------------------------------------------------- */






/* ---------------------------------------------------------------------- *//*!

  \brief    Go control function, basically start accepting data
  \return   Status
                                                                          */
/* ---------------------------------------------------------------------- */
inline int32_t Gateway::go ()
{
   bool   err = m_builder.enable ();
   return err ? -1 : 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Callback when trigger is received
  \return  The event count

  \param[in]  trigData   The trigger data
  \paramiin]  trigLength The length of the trigger data, in units of 32-bits
                                                                          */
/* ---------------------------------------------------------------------- */
inline int32_t Gateway::trigger(uint32_t *trigData, uint32_t trigLength)
{
   int32_t eventCount = 0;
   return  eventCount;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Fill in the event
  \return The length, in 32-bit words, of the event

  \param[out] eventBuffer  Pointer to the event buffer to fill in
  \param[ in]       index  ????
                                                                          */
/* ---------------------------------------------------------------------- */
inline int32_t Gateway::event (uint32_t *eventBuffer, uint32_t index)
{
   // Grab the next completed fragment
   Fragment const *fragment = reinterpret_cast<decltype (fragment)>
                                              (m_builder.m_postList.removeW ());

   // Copy to the output buffer
   int32_t n32 = copyFragment (eventBuffer, fragment);


   // Dispose of this fragment
   delete fragment;


   // Return the number of 32-bit words committed to the event buffer
   return n32;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Fill in the sync event
  \retval |= 0x01, if config should be added
  \retval |= 0x10, if calibrationrun is ended

  \param[in] eventBuffer  Pointer to the event buffer to fill in
                                                                          */
/* ---------------------------------------------------------------------- */
inline int32_t Gateway::update (uint32_t syncFlag)
{
   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Copy the configuration into the event buffer
  \return The number of 32 bit words copied into the buffer

  \param[in] eventBuffer The destintination buffer
                                                                          */
/* ---------------------------------------------------------------------- */
inline int32_t Gateway::config (uint32_t *eventBuffer)
{
   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  End control function
  \return Status
                                                                          */
/* ---------------------------------------------------------------------- */
inline int32_t Gateway::end()
{
   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Close control function

                                                                          */
/* ---------------------------------------------------------------------- */
inline void Gateway::close ()
{
   return;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief  Copies the fragment's data into an output buffer
  \return The size, in 32-bit words, of the copied data

  \param[ in] fragment The target fragment
  \param[out]    dst32 The output buffer
                                                                          */
/* ---------------------------------------------------------------------- */
inline static int32_t copyFragment (uint32_t *dst32, Fragment const *fragment)
{
   uint32_t              genericPresent = fragment->m_genericPresent;
   Contribution const * const *ctbs = fragment->m_ctbs;
   uint8_t                     *dst = reinterpret_cast<decltype(dst)>(dst32);

   //printf ("Gateway::copyFragment Got event fragment ctbs = %8.8" PRIx32 "\n",
   //        present);
   

   while (genericPresent)
   {
      int         id = __builtin_ctz (genericPresent);
      GenericContributor const *generic = reinterpret_cast<decltype(generic)>(ctbs[id]);
      dst           += copyGeneric (dst, generic);
      genericPresent    &= ~(1 << id);
   }


   int32_t n32 = reinterpret_cast<decltype(dst32)>(dst) - dst32;
   return  n32;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Copies the Generic's data into the output buffer
  \return The number of bytes copied

  \param[out] dst The destination buffer
  \param[ in] generic The generic data to copy
                                                                          */
/* ---------------------------------------------------------------------- */
inline static uint32_t copyGeneric (uint8_t *dst,  GenericContributor const *generic)
{ 
   int               ievt = generic->m_evtIdx;
   std::shared_ptr<rogue::protocols::batcher::Data> 
                    &data = generic->m_core->record (ievt);


   auto size = data->size  ();
   auto iter = data->begin ();
   
   
   printf ("Generic %2x %2x %8.8" PRIx32 " %8.8" PRIx32 "\n", 
           generic->m_id, ievt, generic->m_sequence, size);


   fromFrame (iter, size, reinterpret_cast<void *>(dst));
   return size;
}
/* ---------------------------------------------------------------------- */


#endif
