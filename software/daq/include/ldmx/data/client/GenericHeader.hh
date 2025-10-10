// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_GENERICHEADER_HH__
#define __LDMX_DATA_CLIENT_GENERICHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericHeader.hh
  \brief  Defines accessing the Generic header structure.
  \author JJRussell - russell@slac.stanford.edu

  \par
   This various member fields of the GenericEvent.


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
 * 2019.02.27 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/GenericHeader.hh"
#include "ldmx/utl/BfExtract.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to access the members of the Generic header
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericHeader : public ldmx::data::common::GenericHeader
{
public:
   GenericHeader () = delete;


public:
   uint32_t getEventCounter  () const;
   uint8_t  getRceAddress    () const;
   uint8_t  getMbf0          () const;

public:
   static void print (ldmx::data::client::GenericHeader const *hdr);
   void print () const;


private:
   template<typename ValueType>
   static ValueType get (uint32_t                                 w32,
                         ldmx::data::common::GenericHeader::Mask     mask,
                         ldmx::data::common::GenericHeader::Offset offset);
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
/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to the 8 samples
  \return A pointer to the 8 samples
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericHeader::getEventCounter () const
{
   return m_eventCounter;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
template<typename ValueType>
inline ValueType GenericHeader::
                 get (uint32_t                                 w32,
                      ldmx::data::common::GenericHeader::Mask     mask,
                      ldmx::data::common::GenericHeader::Offset offset)
{
   ValueType value = 
   ldmx::utl::bf::extract<uint32_t,
                          ValueType,
                          ldmx::data::common::GenericHeader::Mask,
                          ldmx::data::common::GenericHeader::Offset>
                         (w32, mask, offset);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the RCE address/identifier
  \return  The 8-bit value of the RCE address/identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericHeader::getRceAddress () const
{
   uint8_t value = get<uint8_t>(             m_w32,
                                Mask  ::RceAddress,
                                Offset::RceAddress);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the FEB address/identifier
  \return  The 8-bit value of the FEB address/identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericHeader::getMbf0 () const
{
   uint8_t value = get<uint8_t>(       m_w32,
                                Mask  ::Mbf0, 
                                Offset::Mbf0);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the Generic event header to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void GenericHeader::print () const
{
   print (this);
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                        */
/* ====================================================================== */

#endif
