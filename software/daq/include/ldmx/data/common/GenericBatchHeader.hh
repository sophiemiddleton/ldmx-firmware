// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_COMMON_GENERICBATCHHEADER_HH__
#define __LDMX_DATA_COMMON_GENERICBATCHHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/common/GenericBatchHeader.hh
  \brief  Defines the Generic batch header structure.
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
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.03.07 jjr Replaced the two 'must be' fields with updated definitions
 *                Mbf0 -> AxiWidth - enumerates the AXI bit stream width
 *                Mbf1 -> Rsvd     - reserved, must be 0
 * 2019.02.22 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>


/* ====================================================================== */
namespace ldmx       {
namespace data       {
namespace common     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Trigger/Batch super frame header
                                                                          */
/* ---------------------------------------------------------------------- */
struct GenericBatchHeader
{
   /* ------------------------------------------------------------------- *\
    |                                                                     |
    | Layout of the Generic Batch header                                      |
    | ------------------------------                                      |
    |                                                                     |
    |   31:16 --  16 bits   Reserved, must be zero                        |
    |   15:08 --   8 bits   Sequence number (running, spans each run)     |
    |   07:04 --   4 bits   AXI bit stream width, must be 0x02 => 64 bits |
    |   03:00 --   4 bits   Version number (currently = 1)                |
    |                                                                     |
   \* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!

     \brief Enumerates the AXI stream bit width

     \par
      This value is log2 (AXI stream bit width / 16 bits)

      For the current Generic firmware this is 64-bits
                                                                          */
   /* ------------------------------------------------------------------- */
   enum Width 
   {
      B16  = 0x0,    /*!< AXI stream bit width =  16 bits                 */
      B32  = 0x1,    /*!< AXI stream bit width =  32 bits                 */
      B64  = 0x2,    /*!< AXI stream bit width =  64 bits                 */
      B128 = 0x3,    /*!< AXI stream bit width = 128 bits                 */
      B256 = 0x4,    /*!< AXI stream bit width = 256 bits                 */
      B512 = 0x5     /*!< AXI stream bit width = 128 bits                 */
   };
   /* ------------------------------------------------------------------- */
   


   /* ------------------------------------------------------------------- *\
    | Values of the 'must be' fields'                                     |
   \* ------------------------------------------------------------------- */
   static const uint32_t   Width = Width::B64;
   static const uint32_t    Rsvd =    0x0000;
   static const uint32_t Version =       0x1;



   /* ------------------------------------------------------------------- *//*!

     \brief  The size, in bits, of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      Version        =  4, /*!< Size of the version number                */
      Width          =  4, /*!< Size of must be AXI stream bit width      */
      SequenceNumber =  8, /*!< Size of the sequence number               */
      Rsvd           = 16  /*!< Size of the reserved, must be 0 field     */
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
      Version        =  0,  /*!< Offset to the version number             */
      Width          =  4,  /*!< Offset to the AXI stream bit width       */
      SequenceNumber =  8,  /*!< Offset to the sequence number            */
      Rsvd           = 16   /*!< Offset to the reserved field             */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The right justified bit mask of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask
   {
      Version        =   0xf, /*!< Mask of the version number             */
      Width          =   0xf, /*!< Offset to the AXI stream bit width     */
      SequenceNumber =   0xff, /*!< Mask of the sequence number           */
      Rsvd           = 0xffff  /*!< Mask of the reserved field            */
   };
   /* ------------------------------------------------------------------- */

   uint32_t          m_w32; /*!< The first 32-bit word (seq, version... ) */
   uint32_t         m_rsvd; /*!< Reserved for future use (must be 0)      */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* namespace common                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
