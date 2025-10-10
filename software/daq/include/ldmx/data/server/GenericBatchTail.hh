// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_GENERICBATCHTAIL_HH__
#define __LDMX_DATA_SERVER_GENERICBATCHTAIL_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/GenericBatchTail.hh
  \brief  Defines filling the Generic batch tail word structure.
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
 * 2019.02.22 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "ldmx/data/common/GenericBatchTail.hh"
#include "ldmx/utl/BfInsert.hh"
#include <new>
#include <cinttypes>

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {

   class GenericBatch;

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

  \brief Fill the Generic Batch Tail
                                                                          */
/* ---------------------------------------------------------------------- */
class GenericBatchTail : public ldmx::data::common::GenericBatchTail
{
public:
   GenericBatchTail ();
   GenericBatchTail (uint32_t     nvalid,
                 bool      errorFlag);

   GenericBatchTail (uint32_t     nvalid, 
                 uint8_t       tDest,
                 uint8_t  tUserFirst,
                 uint8_t   tUserLast,
                 uint8_t       width,
                 uint8_t       rsvd);

   void *operator new (std::size_t                  nbytes, 
                       ldmx::data::server::GenericBatch *batch);

public:
   static uint32_t addTDest      (uint8_t      tDest);
   static uint32_t addTUserFirst (uint8_t tUserFirst);
   static uint32_t addErrorFlag  (bool     errorFlag);
   static uint32_t addTUserLast  (uint8_t  tUserLast);
   static uint32_t addWidth      (uint8_t      width);
   static uint32_t addRsvd       (uint8_t       rsvd);

private:
   template   <typename ValueType>
   static uint32_t add (ValueType value, 
                        ldmx::data::common::GenericBatchTail::Mask     mask,
                        ldmx::data::common::GenericBatchTail::Offset offset);
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */





#include "ldmx/data/server/GenericBatch.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Default constructor
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericBatchTail::GenericBatchTail ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  The simplified Generic Batch Cconstructor

  \param[in]     nvalid  The number of valid bytes in the preceding data
                         record.
  \param[in]  errorFlag  FIFO overflow flag
  
  \par
   This constructor defaults the firmware fields which are generally 
   ignored by downstream code.
                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericBatchTail::GenericBatchTail (uint32_t nvalid, 
                                   bool  errorFlag)
{
   m_nvalid = nvalid;
   m_status = addErrorFlag (errorFlag);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Complete, \e i.e. no defaults, Generic batch tail constructor

  \param[in]     nvalid  The number of valid bytes in the preceding dat
                         record
  \param[in]      tDest  Firmware tDest field (unused/ignored)
  \param[in] tUserFirst  Firmware tUserFirst field (error flag = low bit)
  \param[in]  tUserLast  Firmware tUserLast field (unused/ignored)
  \param[in[      width  AXI bit stream width
  \param[in        rsvd  The reserved, normally must be zero field

                                                                          */
/* ---------------------------------------------------------------------- */
inline GenericBatchTail::GenericBatchTail (uint32_t     nvalid, 
                                   uint8_t       tDest,
                                   uint8_t  tUserFirst,
                                   uint8_t   tUserLast,
                                   uint8_t       width,
                                   uint8_t        rsvd)
{
   m_nvalid  = nvalid;
   m_status  = addTDest      (     tDest)
             | addTUserFirst (tUserFirst)
             | addTUserLast  ( tUserLast)
             | addWidth      (     width)
             | addRsvd       (      rsvd);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Custom Generic Batch Tail word Header allocator
  \return Pointer to the allocated class
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *GenericBatchTail::
             operator new (std::size_t                  nbytes, 
                           ldmx::data::server::GenericBatch *batch)
{
   return batch->getTail (nbytes);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Convenience method to customize the general template method
         to add a field to be specific for the Generic Batch Header word.

  \param[in]  value  The value of the field to add
  \param[in]   mask  The right justified bit mask of the bit field
  \param[in] offset  The right justified shift
                                                                         */
/* ---------------------------------------------------------------------- */
template  <typename ValueType>
inline uint32_t GenericBatchTail::add 
                    (ValueType value, 
                     ldmx::data::common::GenericBatchTail::Mask     mask,
                     ldmx::data::common::GenericBatchTail::Offset offset)
{
   uint64_t field = ldmx::utl::bf::
            add<uint32_t, 
                ValueType, 
                ldmx::data::common::GenericBatchTail::Mask, 
                ldmx::data::common::GenericBatchTail::Offset>(value, 
                                                          mask,
                                                          offset);
     return field;
   }
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the \a tDest value
  \return    The correctly masked and justified field

  \param[in] tDest  The 8-bit TDEST value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchTail::addTDest (uint8_t tDest)
{
   uint32_t field = add<uint8_t> (        tDest,
                                  Mask  ::TDest,
                                  Offset::TDest);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the \a TUserFirst value
  \return    The correctly masked and justified field

  \param[in] tUserFirst  The 8-bit TUSER_FIRST value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchTail::addTUserFirst (uint8_t tUserFirst)
{
   uint32_t field = add<uint8_t>(        tUserFirst,
                                 Mask  ::TUserFirst,
                                 Offset::TUserFirst);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the \a ErrorFlag value
  \return    The correctly masked and justified field

  \param[in] errorFlag  The error flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchTail::addErrorFlag (bool errorFlag)
{
   uint32_t field = add<bool>(        errorFlag,
                              Mask  ::TUserLast,
                              Offset::TUserLast);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the \a TUserLast value
  \return    The correctly masked and justified field

  \param[in] tUserLast  The 8-bit TUSER_LAST value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchTail::addTUserLast (uint8_t tUserLast)
{
   uint32_t field = add<uint8_t>(        tUserLast,
                                 Mask  ::TUserLast,
                                 Offset::TUserLast);
   return  field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the AXI bit stream widht
  \return    The correctly masked and justified field
  
  \param[in] width The AXI bit stream width, This should be a value
                   selected from the enum AxiBitStreamWidth in
                   ldmx::common::GenericBatchTail 

  \note
   Since the AXI bit stream width for the Generic is currently fixed at 
   64 bits (width = 2), this method is primarily provided for completeness
   and testing purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchTail::addWidth (uint8_t width)
{
   uint32_t field = add<uint8_t>(        width,
                                 Mask  ::Width,
                                 Offset::Width);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the reserved, must be 0 field
  \return    The correctly masked and justified field
  
  \param[in] rsvd  The value of the must be 0 field

  \note
   Since this field is currently fixed to 0 for the Generic this method is
   primarily provided for completeneess and testing purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t GenericBatchTail::addRsvd (uint8_t rsvd)
{
   uint32_t field = add<uint8_t>(        rsvd,
                                 Mask  ::Rsvd,
                                 Offset::Rsvd);
   return   field;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
