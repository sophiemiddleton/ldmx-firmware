// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_GENERICBATCH_HH__
#define __LDMX_DATA_CLIENT_GENERICBATCH_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericBatch.hh
  \brief  Defines catalogues the Generic events in a Generic batch
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
 * 2019.02.28 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/client/GenericEvent.hh"
#include <vector>
#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {

   class GenericBatchHeader;

} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                        */
/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to catalogue the Generic events in a Generic batch
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericBatch : public std::vector<GenericEvent>
{
public:
   GenericBatch     (int eventCount);
   int populate (uint8_t const *ptr, uint32_t nbytes);

public:
   GenericBatchHeader const *getHeader () const;

public:
   void        print () const;
   static void print (GenericBatch const *batch);
   
public:
   GenericBatchHeader const *m_header;  /*!< The Generic batch header             */
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/client/GenericBatchTail.hh"
#include "ldmx/data/common/GenericBatchTail.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor the collection of Generic events in one Generic batch

  \param[in] eventCount The initialize number of Generic events to allocate

                                                                          */
/* ---------------------------------------------------------------------- */
   inline GenericBatch::GenericBatch (int eventCount) :
      std::vector<GenericEvent>()
{
   reserve (eventCount);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Populate the Generic events

  \param[in]    ptr Pointer to the batch memory
  \param[in] nbytes The number of bytes in the batch memory
                                                                          */
/* ---------------------------------------------------------------------- */
inline int GenericBatch::populate (uint8_t const *ptr, uint32_t nbytes)
{
   // Locate the batch header
   m_header = reinterpret_cast<decltype(m_header)>(ptr);
   /// printf ("GenericBatch %4" PRId32 " @ %p\n", nbytes, (void *)ptr);


   ptr    += nbytes;
   nbytes -= sizeof (ldmx::data::common::GenericBatchTail);
   ptr    -= sizeof (ldmx::data::common::GenericBatchTail);


   while (nbytes)
   {
      // -----------------------------------------------------------------
      // Locate the batch tail record and retrieve the length of the event
      // -----------------------------------------------------------------
      GenericBatchTail const *tail = 
                     reinterpret_cast<decltype (tail)>(ptr);
      /// printf ("GenericBatch::populate tail @ %p %8.8" PRIx32 "\n",
      ///         (void *)tail, *((uint32_t const *)tail));


      // ----------------------------------------------------------
      // Get the number of bytes in the header + data sections
      // Add on the size of the trailer to get the total event size
      // ----------------------------------------------------------
      uint32_t evtBytes = tail->getNBytes ();
      /// printf ("GenericBatch::populate evtBytes = %4" PRIx32 " @ %p\n",
      ///          evtBytes, ptr);


      // -----------------------------------------------
      // Construct the description of the event in place
      // -----------------------------------------------
      emplace (end(), ptr, evtBytes);


      // ------------------------------------------
      // Reduce the number of bytes left to process
      // ------------------------------------------
      nbytes -= evtBytes + sizeof (ldmx::data::common::GenericBatchTail);
      ptr    -= evtBytes + sizeof (ldmx::data::common::GenericBatchTail);
      /// printf ("GenericBatch:populate %d @ %p\n", (int)nbytes, (void *)ptr);
   }

   int    nevents = size ();
   return nevents;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the GenericBatch header
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericBatchHeader const *GenericBatch::getHeader () const
 {
    return m_header;
 }
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Prints the Generic batch of events to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void GenericBatch::print () const
{
   print (this);
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
