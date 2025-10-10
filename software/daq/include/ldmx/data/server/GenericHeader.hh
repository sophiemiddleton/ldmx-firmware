// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_GENERICHEADER_HH__
#define __LDMX_DATA_SERVER_GENERICHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericHeader.hh
  \brief  Defines filling the Generic header structure.
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


#include "ldmx/data/common/GenericHeader.hh"
#include "ldmx/utl/BfInsert.hh"
#include <cinttypes>



/* ====================================================================== */
/* FORWARD REFERENCE                                                      */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
  
   class GenericEvent;

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

  \brief  Fill the Generic header
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericHeader : public ldmx::data::common::GenericHeader
{
public:
   GenericHeader () = delete;
   GenericHeader (uint32_t eventCounter, uint8_t rceAddress);
   GenericHeader (uint32_t eventCounter, uint8_t rceAddress, uint32_t mbf0);


public:
   void *operator new (std::size_t                  nbytes, 
                       ldmx::data::server::GenericEvent *event);

public:
   static uint32_t addRceAddress (uint32_t  rceAddress);
   static uint32_t addMbf0       ();
   static uint32_t addMbf0       (uint32_t        mbf0);


private:
   template<typename ValueType>
   static uint32_t add (ValueType value, 
                        ldmx::data::common::GenericHeader::Mask     mask,
                        ldmx::data::common::GenericHeader::Offset offset);
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */



#include "ldmx/data/server/GenericEvent.hh"

/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief      Minimal Generic header constructor

  \param[in] eventCounter  The 32-bit event counter
  \param[in]   rceAddress  An 8-bit value giving the RCE address/id
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericHeader::GenericHeader (uint32_t eventCounter,
                             uint8_t    rceAddress)
{
   m_eventCounter = eventCounter;
   m_w32          = addRceAddress (rceAddress) | addMbf0 ();

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Complete Generic header constructor

  \param[in] eventCounter  The 32-bit event counter
  \param[in]   rceAddress  An 8-bit value giving the RCE address/id

                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericHeader::GenericHeader (uint32_t eventCounter, 
                             uint8_t    rceAddress,
                             uint32_t         mbf0)
{
   m_eventCounter = eventCounter;
   m_w32          = addRceAddress (rceAddress) | addMbf0 (mbf0);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Custom Generic Event Header allocator
  \return Pointer to the allocated class
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *GenericHeader::operator new (std::size_t                  nbytes, 
                                      ldmx::data::server::GenericEvent *event)
{
   return event->getHeader (nbytes);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Convenience method to customize the general template method
         to add a field to be specific for the Generic header word.

  \param[in]  value  The value of the field to add
  \param[in]   mask  The right justified bit mask of the bit field
  \param[in] offset  The right justified shift
                                                                          */
/* ---------------------------------------------------------------------- */
template   <typename ValueType>
inline uint32_t GenericHeader::
                add (ValueType                              value, 
                     ldmx::data::common::GenericHeader::Mask     mask,
                     ldmx::data::common::GenericHeader::Offset offset)
{
   uint32_t field = ldmx::utl::bf::
                         add<uint32_t, 
                              ValueType, 
                              ldmx::data::common::GenericHeader::Mask, 
                              ldmx::data::common::GenericHeader::Offset>
                             (value, mask, offset);
   return   field;
}

/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the RCE address field
  \return    The correctly masked and justified field

  \param[in] rceAddress The RCE address
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericHeader::addRceAddress (uint32_t  rceAddress)
{
   uint32_t field = add<uint8_t>(       rceAddress, 
                                 Mask  ::RceAddress,
                                 Offset::RceAddress);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief    Creates the field to set the must be field to its default
             value
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericHeader::addMbf0 ()
{
   uint32_t field = add<uint32_t>(        Mbf0,
                                  Mask  ::Mbf0,
                                  Offset::Mbf0);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!


   \brief    Creates the field to set the must be field to the specified
             value.
  \return    The correctly masked and justified field

  \param[in] mbf0  The must be field value
   
  \note
   Since this field is designed to be a fixed, well-known value, this 
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericHeader::addMbf0 (uint32_t  mbf0)
{
   uint32_t field  = add<uint32_t>(        mbf0,
                                   Mask  ::Mbf0,
                                   Offset::Mbf0);
   return   field;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif






