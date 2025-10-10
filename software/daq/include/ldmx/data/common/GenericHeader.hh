// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_COMMON_GENERICHEADER_HH__
#define __LDMX_DATA_COMMON_GENERICHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/common/Generic.hh
  \brief  Defines the Generic evernt header structure.
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
 * 2019.02.22 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>


/* ====================================================================== */
namespace ldmx       {
namespace data       {
namespace common     {
/* ---------------------------------------------------------------------- *//*!

  \brief Defines the bit fields of the 64-bit word that comprises the 
         Trigger Header
                                                                          */
/* ---------------------------------------------------------------------- */
struct GenericHeader
{

   /* ------------------------------------------------------------------- *\
    |                                                                     |
    | Layout of the Generic header                                            |
    | -----------------------------------                                 |
    |                                                                     |
    |   31:00 --  32 bits   Event Counter                                 |
    |   31:08 --  24 bits   Must be zero                                  |
    |   07:00 --   8 bits   RCE Address                                   |
    |                                                                     |
   \* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *\
    | Values of the 'must be' fields'                                     |
   \* ------------------------------------------------------------------- */
   static const uint32_t  Mbf0 = 0x000000;


   /* ------------------------------------------------------------------- *//*!

     \brief  The size, in bits, of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      RceAddress     =  8,  /*!< Size of RCE addresscount                 */
      Mbf0           = 24   /*!< Size of must be field                    */
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
      RceAddress      = 0,  /*!< Offset to RCE address                    */
      Mbf0           =  8,  /*!< Offset to musb be field                  */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The right justified bit mask of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask
   {
      RceAddress     =     0xff, /*!< Mask of RCE address                 */
      Mbf0           = 0xffffff  /*!< Mask of must be field               */
   };
   

   uint32_t m_eventCounter; /*!< The event sequence counter               */
   uint32_t          m_w32; /*!< The RCE address/must be field            */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* namespace common                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
