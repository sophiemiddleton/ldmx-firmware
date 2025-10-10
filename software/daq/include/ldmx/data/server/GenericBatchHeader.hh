// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_GENERICBATCHHEADER_HH__
#define __LDMX_DATA_SERVER_GENERICBATCHHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericBatchHeader.hh
  \brief  Defines filling the Generic batch header structure.
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
 * 2019.02.22 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/GenericBatchHeader.hh"
#include "ldmx/utl/BfInsert.hh"
#include <new>
#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {

   class GenericBatch;

} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Fill the Generic Batch Header
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericBatchHeader : public ldmx::data::common::GenericBatchHeader
{
public:
   GenericBatchHeader ();

   GenericBatchHeader (uint32_t sequence);

   GenericBatchHeader (uint32_t  version, 
                   uint32_t    width,
                   uint32_t sequence,
                   uint32_t     rsvd);

public:
   void *operator new (std::size_t                  nbytes, 
                       ldmx::data::server::GenericBatch *batch);

public:
   static uint32_t addVersion   ();
   static uint32_t addVersion   (uint32_t  version);
   static uint32_t addWidth     ();
   static uint32_t addWidth     (uint32_t    width);
   static uint32_t addSequence  (uint32_t  sequence);
   static uint32_t addRsvd      ();
   static uint32_t addRsvd      (uint32_t      rsvd);

private:
   template<typename ValueType>
   static uint32_t add 
                  (ValueType value, 
                   ldmx::data::common::GenericBatchHeader::Mask     mask,
                   ldmx::data::common::GenericBatchHeader::Offset offset);
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/server/GenericBatch.hh"

/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- */
inline GenericBatchHeader::GenericBatchHeader () :
       GenericBatchHeader (Version, Width, 0, Rsvd)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline GenericBatchHeader::GenericBatchHeader (uint32_t sequence) :
       GenericBatchHeader (Version, Width, sequence, Rsvd)
{
   //// printf ("BatchHeader = %8.8" PRIx32 "\n", m_w32);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline GenericBatchHeader::GenericBatchHeader (uint32_t  version, 
                                       uint32_t    width,
                                       uint32_t sequence,
                                       uint32_t     rsvd) 
{
   m_w32  =  addRsvd     (    rsvd)
          |  addSequence (sequence)
          |  addWidth    (   width)
          |  addVersion  ( version);
   m_rsvd = 0;

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Custom Allocator
  \return Pointer to the allocated class

  \param[in] nbytes  The number of bytes to allocate
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *GenericBatchHeader::operator
             new (size_t                       nbytes, 
                  ldmx::data::server::GenericBatch *batch)
{
   return batch->getHeader (nbytes);
}
/* ---------------------------------------------------------------------- */



/* --------------------------------------------------------------------- *//*!

   \brief Convenience method to customize the general template method
         to add a field to be specific for the Generic Batch Header word.

   \param[in]  value  The value of the field to add
   \param[in]   mask  The right justified bit mask of the bit field
   \param[in] offset  The right justified shift
                                                                         */
/* --------------------------------------------------------------------- */
template<typename ValueType>
inline uint32_t GenericBatchHeader::add
                 (ValueType value, 
                  ldmx::data::common::GenericBatchHeader::Mask     mask,
                  ldmx::data::common::GenericBatchHeader::Offset offset)
{
   uint64_t field = ldmx::utl::bf::
                    add<uint32_t, 
                        ValueType, 
                        ldmx::data::common::GenericBatchHeader::Mask, 
                        ldmx::data::common::GenericBatchHeader::Offset>
                       (value, mask, offset);
     return field;
}
/* --------------------------------------------------------------------- */



/* --------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the version number value to its
             default value
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchHeader::addVersion ()
{
   uint32_t field = add<uint8_t>(Version, Mask::Version, Offset::Version);
   return   field;
}
/* ---------------------------------------------------------------------- */

 

/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the version number value to the
             specified value
  \return    The correctly masked and justified field

  \param[in] version The version number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchHeader::addVersion (uint32_t version)
{
   uint32_t field = add<uint8_t>(Version, Mask::Version, Offset::Version);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the AXI bit stream width to its 
             default value
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchHeader::addWidth ()
{
   uint32_t field = add<uint8_t>(        Width, 
                                 Mask  ::Width, 
                                 Offset::Width);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the AXI bit stream width to the
             specified value
  \return    The correctly masked and justified field

  \param[in] width The value of AXI bit stream width type,  This 
                   should be one of the enum Width found in
                   ldmx::data::common

  \note
   Since this field is set to AxiWidth64 for the Genericvalue, this 
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchHeader::addWidth (uint32_t width)
{
   uint32_t field = add<uint8_t>(        width,
                                 Mask  ::Width, 
                                 Offset::Width);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the sequence number
  \return    The correctly masked and justified field

  \param[in] sequenceNumber The value of the sequence number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchHeader::addSequence (uint32_t sequenceNumber)
{
   uint32_t field = add<uint8_t>(        sequenceNumber, 
                                 Mask  ::SequenceNumber, 
                                 Offset::SequenceNumber);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the reserved field to its default
             value
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchHeader::addRsvd ()
{
   uint32_t field = add<uint16_t>(Rsvd, Mask::Rsvd, Offset::Rsvd);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the reserved field to the specified
             value
  \return    The correctly masked and justified field

  \param[in] rsvd The value of the reserved field

  \note
   Since this field is designed to be a fixed, well-known value, this 
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchHeader::addRsvd (uint32_t rsvd)
{
   uint32_t field = add<uint16_t>(rsvd, Mask::Rsvd, Offset::Rsvd);
   return   field;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif



