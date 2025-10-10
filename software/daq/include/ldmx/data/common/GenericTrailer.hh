// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_COMMON_GENERICTRAILER_HH__
#define __LDMX_DATA_COMMON_GENERICTRAILER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/common/GenericTrailer.hh
  \brief  Defines the Generic event trailer structure.
  \author JJRussell - russell@slac.stanford.edu

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
 * 2021.04.20 jjr Adapted from the HPS version
 * 2019.02.22 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>


/* ====================================================================== */
namespace ldmx       {
namespace data       {
namespace common     {
/* ---------------------------------------------------------------------- *//*!

  \brief Defines the bit fields of the 32-bit word that comprises the 
         Generic Event Trailer
                                                                          */
/* ---------------------------------------------------------------------- */
struct GenericTrailer
{

   /* ------------------------------------------------------------------- *\
    |                                                                     |
    | Layout of the trigger header bit fields                             |
    | ---------------------------------------                             |
    |                                                                     |
    |  11: 0  --  12 bits  Number of multisamples                         |
    |  23:12  --  12 bits  Skip Count - # dumped due to FIFO backup       |
    |  25:24  --   2 bits  Undefined                                      |
    |  26:26  --   1 bit   APV buffer address sync error                  |
    |  27:27  --   1 bit   FIFO backup error,skip count should be non-zero|
    |  31:28  --   4 bits  Undefined                                      |
    |                                                                     |
   \* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *\
    | Values of the 'must be' fields'                                     |
   \* ------------------------------------------------------------------- */
   static const uint8_t Undefined0 = 0;
   static const uint8_t Undefined1 = 0;



   /* ------------------------------------------------------------------- *//*!

     \brief  The size, in bits, of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      MultiSampleCount   = 12, /*!< Size of multi-sample count            */
      SkipCount          = 12, /*!< Size of skip count                    */
      Undefined0         =  2, /*!< Size of undefined field (instance #0) */
      ApvSyncError       =  1, /*!< Size of APV address sync error        */
      ApvFifoBackupError =  1, /*!< Size of FIFO backup error             */
      Undefined1         =  4, /*!  Size of undefined field (instance #1) */
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
      MultiSampleCount   =  0, /*!< Offset to multi-sample count          */
      SkipCount          = 12, /*!< Offset to skip count                  */
      Undefined0         = 24, /*!< Offset to undefined field(instance #0)*/
      ApvSyncError       = 26, /*!< Offset to APV address sync error      */
      ApvFifoBackupError = 27, /*!< Offset to FIFO backup error           */
      Undefined1         = 28, /*!  Offset to undefined field(instance #1)*/
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The right justified bit mask of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask
   {
      MultiSampleCount   = 0xfff, /*!< Mask of multi-sample count         */
      SkipCount          = 0xfff, /*!< Mask of skip count                 */
      Undefined0         =   0x3, /*!< Mask of undefined field(instance#0)*/
      ApvSyncError       =   0x1, /*!< Mask of APV address sync error     */
      ApvFifoBackupError =   0x1, /*!< Mask of FIFO backup error          */
      Undefined1         =   0xf, /*!  Mask of undefined field(instance#1)*/
   };
   /* ------------------------------------------------------------------- */

   uint32_t  m_w32;  /*!< The 32-bit word containing all the bit fields   */
   uint32_t  m_pad;  /*!< This is a pad word to round to 64-bit boundary  */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* namespace common                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
