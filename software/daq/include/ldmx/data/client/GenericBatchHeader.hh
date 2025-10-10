// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_GENERICBATCHHEADER_HH__
#define __LDMX_DATA_CLIENT_GENERICBATCHHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericBatchHeader.hh
  \brief  Defines accessing the GENERIC batch header structure.
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
 * 2019.02.26 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/GenericBatchHeader.hh"
#include "ldmx/utl/BfExtract.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Class to access the members of the GENERIC batch header
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericBatchHeader : public ldmx::data::common::GenericBatchHeader
{
public:
   GenericBatchHeader () = delete;

public:
   uint8_t  getVersion        () const;
   uint8_t  getWidth          () const;
   uint8_t  getSequenceNumber () const;
   uint16_t getRsvd           () const;

public:
   static void print (GenericBatchHeader const *hdr);
   void        print () const;

private:
   template<typename ValueType>
   static ValueType get (uint32_t                                      w32,
                         ldmx::data::common::GenericBatchHeader::Mask     mask,
                         ldmx::data::common::GenericBatchHeader::Offset offset);
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- */
template<typename ValueType>
inline ValueType GenericBatchHeader::
                 get (uint32_t                                      w32,
                      ldmx::data::common::GenericBatchHeader::Mask     mask,
                      ldmx::data::common::GenericBatchHeader::Offset offset)
{
   ValueType value = 
   ldmx::utl::bf::extract<uint32_t,
                          ValueType,
                          ldmx::data::common::GenericBatchHeader::Mask,
                          ldmx::data::common::GenericBatchHeader::Offset>
                          (w32, mask, offset);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the version number
  \return  The 4-bit version number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericBatchHeader::getVersion () const
{
   uint8_t value = get<uint8_t>(m_w32, Mask::Version, Offset::Version);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the AXI stream bit width
  \return  This 4-bit value is one of the ldmx::data::common enum Width
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericBatchHeader::getWidth () const
{
   uint8_t value = get<uint8_t>(m_w32, 
                                Mask  ::Width, 
                                Offset::Width);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the sequence number
  \return  The 8-bit sequence number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericBatchHeader::getSequenceNumber () const
{
   uint8_t value = get<uint8_t>(m_w32, 
                                Mask  ::SequenceNumber, 
                                Offset::SequenceNumber);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the reserved field
  \return  The 16-bit value of the \e must \e be field

  \par 
   By default this is a must be zero field, but this allows this
   assumption to be checked.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t GenericBatchHeader::getRsvd () const
{
   uint8_t value = get<uint8_t>(m_w32, Mask::Rsvd, Offset::Rsvd);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the GENERIC batch header to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void GenericBatchHeader::print () const
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
