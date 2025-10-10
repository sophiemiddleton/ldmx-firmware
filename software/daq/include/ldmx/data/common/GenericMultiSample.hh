// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_COMMON_GENERICMULTISAMPLE_HH__
#define __LDMX_DATA_COMMON_GENERICMULTISAMPLE_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/common/Generic.hh
  \brief  Defines the Generic event multi- sample data structure.
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
 * 2021.04.21 jjr Removed the 'packed' attribute.  It was done mainly for
 *                being explicit that this structure maps directly on to
 *                a binary data section. But, the current versions of the
 *                compiler gross about accessing this since that access 
 *                could be unaligned.  In this case, the layout of the 
 *                structure is the same whether it is packed or unpacked.
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

  \brief Defines the bit fields and the optional 32-bit words that
         comprise the Generic Multi Sample Data
                                                                          */
/* ---------------------------------------------------------------------- */
struct GenericMultiSample
{
   /* ------------------------------------------------------------------- *\
    |                                                                     |
    |  Word 0 - 2:                                                        |
    |    31:16 | 15: 00 -- Sample 1 | Sample 0                            |
    |    31:16 | 15: 00 -- Sample 3 | Sample 2                            |
    |    31:16 | 15: 00 -- Sample 5 | Sample 4                            |
    |                                                                     |
    |  Word 3:                                                            |
    |    07:00  -- 8 bits  RCE Address                                    |
    |    15:08  -- 8 bits  FEB Address                                    |
    |    22:16  -- 7 bits  APV Channel                                    |
    |    25:23  -- 3 bits  APV #                                          |
    |    27:26  -- 2 bits  Hybrid Address                                 |
    |    28:28  -- 1 bit   Read Error                                     |
`   |    29:29  -- 1 bit   Tail (always 0)                                |
    |    30:30  -- 1 bit   Head (always 0)                                |
    |    31:31  -- 1 bit   Filter Flag                                    |
    |                                                                     |
   \* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------ *\
    | Values of the 'must be' fields'                                    |
   \* ------------------------------------------------------------------ */
   static const bool Tail = false;
   static const bool Head = false;


   /* ------------------------------------------------------------------- *//*!

     \brief  The size, in bits, of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      RceAddress     =  8,  /*!< Size of RCE address                      */
      FebAddress     =  8,  /*!< Size of FEB address                      */
      ApvChannel     =  7,  /*!< Size of APV channel number               */
      ApvNumber      =  3,  /*!< Size of APV number                       */
      HybridAddress  =  2,  /*!< Size of Hybrid address                   */
      ReadError      =  1,  /*!< Size of read error flag                  */
      Tail           =  1,  /*!< Size of tail flag                        */
      Head           =  1,  /*!< Size of head flag                        */
      FilterFlag     =  1,  /*!< Size of filter flag                      */
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
      RceAddress     =  0,  /*!< Offset to RCE address                    */
      FebAddress     =  8,  /*!< Offset to FEB address                    */
      ApvChannel     = 16,  /*!< Offset to APV channel number             */
      ApvNumber      = 23,  /*!< Offset to APV number                     */
      HybridAddress  = 26,  /*!< Offset to Hybrid address                 */
      ReadError      = 28,  /*!< Offset to read error flag                */
      Tail           = 29,  /*!< Offset to tail flag                      */
      Head           = 30,  /*!< Offset to head flag                      */
      FilterFlag     = 31,  /*!< Offset to filter flag                    */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The right justified bit mask of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask
   {
      RceAddress     = 0xff, /*!< Mask of RCE address                     */
      FebAddress     = 0xff, /*!< Mask of FEB address                     */
      ApvChannel     = 0x7f, /*!< Mask of APV channel number              */
      ApvNumber      = 0x07, /*!< Mask of APV number                      */
      HybridAddress  = 0x03, /*!< Mask of Hybrid address                  */
      ReadError      = 0x01, /*!< Mask of read error flag                 */
      Tail           = 0x01, /*!< Mask of tail flag                       */
      Head           = 0x01, /*!< Mask of head flag                       */
      FilterFlag     = 0x01, /*!< Mask of filter flag                     */
   };
   /* ------------------------------------------------------------------- */


   uint32_t m_samples[3];  /*!< The 6 packed samples                      */
   uint32_t        m_w32;  /*!< The 32-bit pack bit field word            */
};  /// Removed 2021.04.21 to suppress superflous compiler warnings
    /// __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* namespace common                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif

