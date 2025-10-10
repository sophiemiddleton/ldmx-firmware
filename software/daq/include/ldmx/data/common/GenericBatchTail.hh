// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_COMMON_GENERICBATCHTAIL_HH__
#define __LDMX_DATA_COMMON_GENERICBATCHTAIL_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/common/GenericBatchTail.hh
  \brief  Defines the Generic batch tail structure.
  \author JJRussell - russell@slac.stanford.edu

  \par
   The BatchTail acts as a separator between the batched GenericEvents.
   It is \e not a trailer, in the traditional meaning of trailer, that
   only goes at the end of the batched GenericEvents.

  \par
   This file defines the only the data layout, not how to write or
   access this data and is meant to be the base class for derived
   classes that do the accessing and writing.  No methods are defined,
   so that they are POD (plain old data), free of any C++ decorations.

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


#include <cinttypes>


/* ====================================================================== */
namespace ldmx       {
namespace data       {
namespace common     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Trigger/Batch tail word (64-bits)
                                                                          */
/* ---------------------------------------------------------------------- */
struct GenericBatchTail
{
   /* ------------------------------------------------------------------- *//*!

     \brief  The size, in bits, of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      TDest      = 8,  /*!< Size of the TDEST field (unused)              */
      TUserFirst = 8,  /*!< Size of the TUSER_FIRST field (unused)        */
      TUserLast  = 8,  /*!< Size of the TUSER_LAST  field (unused)        */
      Width      = 4,  /*!< Size of the AXI bit stream width field        */
      Rsvd       = 4,  /*!< Size of the reserved must be 0 field          */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The offset, in bits, to the various bit fields

     \note
      This is the value needed to right justify the field
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Offset
   {
      TDest      =  0, /*!< Offset to the TDEST field (unused)            */
      TUserFirst =  8, /*!< Offset to the TUSER_FIRST field (unused)      */
      TUserLast  = 16, /*!< Offset to the TUSER_LAST  field (unused)      */
      Width      = 24, /*!< Offset to the AXI bit stream width field      */
      Rsvd       = 28  /*!< Offset to the reserved, must be 0 field       */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The right justified bit mask of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask
   {
      TDest      = 0xff, /*!< Mask of the TDEST field (unused)            */
      TUserFirst = 0xff, /*!< Mask of the TUSER_FIRST field (unused)      */
      TUserLast  = 0xff, /*!< Mask of the TUSER_LAST  field (unused)      */
      Width      =  0xf, /*!< Mask of the AXI bit stream width field      */
      Rsvd       =  0xf  /*!< Mask of the reserved, must be 0 field       */
   };
   /* ------------------------------------------------------------------- */

   uint32_t m_nvalid;  /*!< Number of valid bytes in data                 */
   uint32_t m_status;  /*!< The status word                               */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* namespace common                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
