// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_GENERICRAILER_HH__
#define __LDMX_DATA_CLIENT_GENERICRAILER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/GenericTrailer.hh
  \brief  Defines accessing the Generic trailer structure.
  \author JJRussell - russell@slac.stanford.edu

  \par
   This various member fields of the GenericTrailer.


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


#include "ldmx/data/common/GenericTrailer.hh"
#include "ldmx/utl/BfExtract.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to access the members of the Generic trailer
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericTrailer : public ldmx::data::common::GenericTrailer
{
public:
   GenericTrailer () = delete;


public:
   uint16_t getMultiSampleCount   () const;
   uint16_t getSkipCount          () const;
   uint8_t  getUndefined0         () const;
   bool     getApvSyncError       () const;
   bool     getApvFifoBackupError () const;
   uint8_t  getUndefined1         () const;

public:
   static void print (GenericTrailer const *trailer);
   void print () const;

private:
   template<typename ValueType>
   static ValueType get (uint32_t                                  w32,
                         ldmx::data::common::GenericTrailer::Mask     mask,
                         ldmx::data::common::GenericTrailer::Offset offset);
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
inline ValueType GenericTrailer::
                 get (uint32_t                                  w32,
                      ldmx::data::common::GenericTrailer::Mask     mask,
                      ldmx::data::common::GenericTrailer::Offset offset)
{
   ValueType value = 
   ldmx::utl::bf::extract<uint32_t,
                          ValueType,
                          ldmx::data::common::GenericTrailer::Mask,
                          ldmx::data::common::GenericTrailer::Offset>
                         (w32, mask, offset);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the number of multi-sample records
  \return  The number of multi-sample records
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t GenericTrailer::getMultiSampleCount () const
{
   uint8_t value = get<uint16_t>(                   m_w32,
                                 Mask  ::MultiSampleCount,
                                 Offset::MultiSampleCount);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the skip count
  \return  The 12-bit skip count

  \par
   This is number of multi-sample records that were lost due to FIFO
   overflow.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t GenericTrailer::getSkipCount () const
{
   uint8_t value = get<uint16_t>(            m_w32,
                                 Mask  ::SkipCount, 
                                 Offset::SkipCount);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get value of the first undefined field
  \return  The 3-bit undefined field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericTrailer::getUndefined0 () const
{
   uint8_t value = get<uint16_t>(             m_w32,
                                 Mask  ::Undefined0, 
                                 Offset::Undefined0);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get APV sync error flag
  \return  The APV sync error flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool GenericTrailer::getApvSyncError () const
{
   uint8_t value = get<bool>(               m_w32,
                             Mask  ::ApvSyncError, 
                             Offset::ApvSyncError);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get APV FIFO backup error flag
  \return  The APV FIFO backup error flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool GenericTrailer::getApvFifoBackupError () const
{
   uint8_t value = get<bool>(                     m_w32,
                             Mask  ::ApvFifoBackupError, 
                             Offset::ApvFifoBackupError);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get value of the second undefined field
  \return  The 4-bit undefined field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t GenericTrailer::getUndefined1 () const
{
   uint8_t value = get<uint16_t>(             m_w32,
                                 Mask  ::Undefined1, 
                                 Offset::Undefined1);
   return  value;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Prints the Generic event trailer to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void GenericTrailer::print () const
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
