// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_GENERICBATCHTAIL_HH__
#define __LDMX_DATA_CLIENT_GENERICBATCHTAIL_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericBatchTail.hh
  \brief  Defines accessing the Generic batch header structure.
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


#include "ldmx/data/common/GenericBatchTail.hh"
#include "ldmx/utl/BfExtract.hh"
#include <cinttypes>
#include <cstdio>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to access the members of the Generic batch tail
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericBatchTail : public ldmx::data::common::GenericBatchTail
{
public:
   GenericBatchTail () = delete;


public:
   uint32_t  getNBytes     () const;
   uint32_t  getNValid     () const;
   uint8_t   getTDest      () const;
   uint8_t   getTUserFirst () const;
   bool      getErrorFlag  () const;
   uint8_t   getTUserLast  () const;
   uint8_t   getWidth      () const;
   uint8_t   getRsvd       () const;
   

public:
   static void print (GenericBatchTail const *tail);
   void        print () const;


private:
   template<typename ValueType>
   static ValueType get (uint32_t                                    w32,
                         ldmx::data::common::GenericBatchTail::Mask     mask,
                         ldmx::data::common::GenericBatchTail::Offset offset);
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
inline ValueType GenericBatchTail::
                 get (uint32_t                                    w32,
                      ldmx::data::common::GenericBatchTail::Mask     mask,
                      ldmx::data::common::GenericBatchTail::Offset offset)
{
   ValueType value = 
   ldmx::utl::bf::extract<uint32_t,
                          ValueType,
                          ldmx::data::common::GenericBatchTail::Mask,
                          ldmx::data::common::GenericBatchTail::Offset>
                         (w32, mask, offset);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the number of bytes in the the data payload
  \return  The number of bytes in the the data payload

  \note
   This value includes the rounding up to the AXI stream bit width
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchTail::getNValid () const
{
   return  m_nvalid;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Get the number of valid bytes in the the data payload
  \return  The number of valid bytes in the the data payload

                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchTail::getNBytes () const
{
   /// printf ("GenericBatchTail: m_nvalid = %8" PRIx32 "\n", m_nvalid);
   return  (m_nvalid + 7) & ~0x7;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of th tDest field
  \return  The 8-bit value of the tDest field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericBatchTail::getTDest () const
{
   uint8_t value = get<uint8_t>(     m_status, 
                                Mask  ::TDest, 
                                Offset::TDest);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the tUserFirst field
  \return  The 8-bit value of the tUserFirst field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericBatchTail::getTUserFirst () const
{
   uint8_t value = get<uint8_t>(          m_status, 
                                Mask  ::TUserFirst, 
                                Offset::TUserFirst);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the error flag
  \return  The error flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool GenericBatchTail::getErrorFlag () const
{
   uint8_t value = get<uint8_t>(         m_status, 
                                Mask  ::TUserLast, 
                                Offset::TUserLast) & 1;
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the tUserLast field
  \return  The 7-bit value of the tUserLast field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericBatchTail::getTUserLast () const
{
   uint8_t value = get<uint8_t>(         m_status, 
                                Mask  ::TUserLast, 
                                Offset::TUserLast);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the AXI bit stream width
  \return  The AXI bit stream width, this is should be one of the 
           enum ldmx::common::AxiBitStreamWidth values
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericBatchTail::getWidth () const
{
   uint8_t value = get<uint8_t>(      m_status, 
                                Mask  ::Width, 
                                Offset::Width);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the reserved field
  \return  The reserved, must be zero field

  \note
   The method is provided to access the reserved so that it may be checked
   to be 0 or just for completeness
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericBatchTail::getRsvd () const
{
   uint8_t value = get<uint8_t>(    m_status, 
                                Mask  ::Rsvd, 
                                Offset::Rsvd);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the Generic batch tail
                                                                          */
/* ---------------------------------------------------------------------- */
inline void GenericBatchTail::print () const
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
