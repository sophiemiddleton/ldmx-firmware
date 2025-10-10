/**
 *-----------------------------------------------------------------------------
 * Title      : Rogue Coda Gateway
 * ----------------------------------------------------------------------------
 * File       : RogueCodaGw-Real.cpp
 * Created    : 2019-04-13
 * ----------------------------------------------------------------------------
 * Description:
 * Rogue Coda Gateway Class
 * ----------------------------------------------------------------------------
 * This file is part of the LDMX software platform. It is subject to 
 * the license terms in the LICENSE.txt file found in the top-level directory 
 * of this distribution and at: 
 *    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
 * No part of the LDMX software platform, including this file, may be 
 * copied, modified, propagated, or distributed except according to the terms 
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include "ldmx/builder/client/RogueCodaGw.hh"
#include "Gateway.hh"
#include <rogue/RogueSMemFunctions.h>
#include <rogue/Version.h>
#include <sys/time.h>
#include <stdlib.h>

RogueCodaGw::RogueCodaGw() {

   // Open shared memory
   smemFd_ = rogueSMemControlOpenAndMap(&smem_, "LDMX_GENERIC");
   lastEventNumber_ = 0;
   trigData_ = NULL;
   trigLength_ = 0;
   trigBlkSize_ = 0;
   runType_ = 0;
   calGroup_ = 0;
   calDelay_ = 0;
   calLevel_ = 0;
   writeConfig_ = 0;
   m_gw = new Gateway;
}

RogueCodaGw::~RogueCodaGw() {

   // Close shared memory
   ::close(smemFd_);
   delete m_gw;
}

// Helper function to update coda state
int32_t RogueCodaGw::codaState ( const char *state, const char *arg ) {
   char path[100];
   char result[100];

   sprintf(path,"%s.%s",ROOTNAME,state);

   if ( rogueSMemControlExec(smem_,path,arg,result,100) != 0 ) return -1;

   if (strcmp(result,"OK") != 0 ) {
      printf("Rogue state change failed: %s\n",result);
      return -1;
   }
   else {
      printf("Rogue state change succeeded\n");
      return 0;
   }
}

// Process download sequence
int32_t RogueCodaGw::download(const char *confFile, const char *usrString) {
   char ebFile[1000];

   printf("============ ROGUE Download =================\n");
   printf("Rogue Version: %s\n",rogue::Version::current().c_str());

   if ( codaState("CodaDownload",confFile) != 0 ) return -1;

   if ( rogueSMemControlValue(smem_,ROOTNAME ".SvtEbConfigFile",ebFile,1000) != 0 ) return -1;

   printf("EB Config File: %s\n",ebFile);

   int32_t status = m_gw->download (ebFile, "");
   return  status;
}

// Process prestart sequence
int32_t RogueCodaGw::prestart() {
   char tmpStr[100];

   // Get run type from rogue
   if ( rogueSMemControlValue(smem_,ROOTNAME ".SvtRunType",tmpStr,100) != 0 ) return -1;
   runType_ = atoi(tmpStr);

   // Calibration
   if ( runType_ != 0 ) {
      calGroup_ = 0;
      calDelay_ = 1;
      calLevel_ = 0;

      printf("Setting calibration mode to True\n");
      strcpy(tmpStr,"True");

      if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibMode",tmpStr) != 0 ) return -1;

      printf("Setting calibration group to %i\n",calGroup_);
      sprintf(tmpStr,"%i",calGroup_);

      if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibGroup",tmpStr) != 0 ) return -1;
      calGroup_++;

      printf("Setting calibration level to %i\n",calLevel_);
      sprintf(tmpStr,"%i",calLevel_);
      if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibLevel",tmpStr) != 0 ) return -1;
      calLevel_++;

      if ( runType_ == 2 || runType_ == 3 ) {
         printf("Setting calibration delay to %i\n",calDelay_);
         sprintf(tmpStr,"%i",calDelay_);

         if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibDelay",tmpStr) != 0 ) return -1;
         calDelay_++;
      }
   }

   int32_t status = m_gw->prestart ();
   if (status == 0)
   {
      status = codaState("CodaPrestart",NULL);
   }

   return status;
}


int32_t RogueCodaGw::go() {

   int32_t status = m_gw->go ();
   if (status == 0)
   {
      status = codaState("CodaGo",NULL);
   }

   return status;
}

// Process trigger data, update passed pointer and return size
int32_t RogueCodaGw::trigger(uint32_t *trigData, uint32_t trigLen) {
   uint32_t x;

   // Get trigger record
   trigData_   = trigData;
   trigLength_ = trigLen;

   // Extract block size
   trigBlkSize_ = trigData[1] & 0xFF;

   if (( (trigBlkSize_ * 4) + 3 ) != trigLength_ ) {
      printf("Bad trigger block size = %i, length = %i\n",trigBlkSize_,trigLength_);
      trigBlkSize_ = 0;
   }

   printf("Trigger:\n");
   for (x=0; x < trigLen; x++) printf("Ti %i: 0x%x\n",x,trigData[x]);
   printf("\n");

   // Generate emulation trigger here?

   /// !!! What to do with return value 
   //trigBlkSize_ = m_gw->trigger (trigData, trigLen);

   return trigBlkSize_;
}

// Process trigger data, update passed pointer and return size
int32_t RogueCodaGw::event(uint32_t *eventBuffer, uint32_t index) {
   uint32_t x;

   lastEventNumber_++;

   // Fake data
   if ( 1 ) {

      // First word is trigger number, 32-bits from 2nd word in trigger block
      eventBuffer[0] = trigData_[2+(index*4)+1];

      // Second and third word is trigger time, 48-bits
      eventBuffer[1]  = trigData_[2+(index*4)+2] & 0x00FFFFFF;         // 23:0 = Timestamp 23:0
      eventBuffer[2]  = (trigData_[2+(index*4)+2] >> 24) & 0xFF;       // 7:0  = Timestamp 31:24
      eventBuffer[2] |= (trigData_[2+(index*4)+3] << 8 ) & 0x00FFFF00; // 23:8 = Timestamp 47:32

      // Add a few words for the hell of it
      eventBuffer[3] = 0xAAAAAAAA;
      eventBuffer[4] = 0xBBBBBBBB;
      eventBuffer[5] = 0xCCCCCCCC;
      eventBuffer[6] = 0xDDDDDDDD;

      printf("Event %i:%i:\n",index,lastEventNumber_);
      for (x=0; x < 7; x++) printf("Data %i: 0x%x\n",x,eventBuffer[x]);
      printf("\n");

      return(7);
   }

   // real data
   else {
      int32_t n32 = m_gw->event (eventBuffer, index);
      return  n32;
   }
}

// Convert calibration state to injection value
uint32_t RogueCodaGw::getCalLevel(uint32_t levelIn) {
   uint32_t level;
   switch (levelIn) {
      case 0 :
         level = 29;
         break;
      case 1 :
         level = 48;
         break;
      case 2 :
         level = 80;
         break;
      case 3 :
         level = 120;
         break;
      case 4 :
         level = 192;
         break;
      default:
         printf("this cal_level (%i) should never happen. Exit!\n",levelIn);
         level = 0;
         break;
   }
   return level;
}

// Called per event for book keeping, and calibration updates
// Return |= 0x01 if config should be added
// Return |= 0x10 if calibration run is ended.
// Three calibration modes are supported based upon the
// configuration file name:
//   *injection* = Gain calibration run 
//   *t0Single*  = t0 calibration run, single group
//   *t0*        = t0 calibration run
int32_t RogueCodaGw::update(uint32_t syncFlag) {
   int32_t  ret;
   char tmpStr[1000];

   ret = 0;

   // Do we dump state
   if ( writeConfig_ == 1 || lastEventNumber_ == 1 ||
        ((syncFlag > 0) && ((lastEventNumber_ % 50000) == 0) ) ) {
      ret = 1;
   } 

   // Update calibration
   if ( (runType_ != 0) && ((lastEventNumber_ % 100) == 0) ) {

      // make sure the config gets written on the next event
      writeConfig_ = 1;
          
      printf("Processing cal_group %d at EVENT_NUMBER %d\n",calGroup_,lastEventNumber_);
          
      // only 8 cal groups. Should stop before the 9th	
      // one run_type only uses one group
      if (calGroup_ < 8 && runType_ !=3 ) {
         sprintf(tmpStr, "%i", calGroup_);

         printf("Setting calibration group to %i\n",calGroup_);
         sprintf(tmpStr,"%i",calGroup_);

         if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibGroup",tmpStr) != 0 ) return -1;
         calGroup_++;

      } else {
             
         // start all over again for different delay
         if ( runType_== 3 && calDelay_ < 9 ) {

            printf("Setting calibration delay to %i\n",calDelay_);
            sprintf(tmpStr,"%i",calDelay_);

            if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibDelay",tmpStr) != 0 ) return -1;
            calDelay_++;

            //do not update calgroup for this run type
         } 

         // start all over again for different delay
         else if ( runType_ == 2 && calDelay_ < 9 ) {

            printf("Setting calibration delay to %i\n",calDelay_);
            sprintf(tmpStr,"%i",calDelay_);

            if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibDelay",tmpStr) != 0 ) return -1;
            calDelay_++;

            //reset cal_group
            calGroup_ = 0;
            printf("Setting calibration group to %i\n",calGroup_);
            sprintf(tmpStr,"%i",calGroup_);

            if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibGroup",tmpStr) != 0 ) return -1;
            calGroup_++;
         } 

         // start all over again with a different charge level
         else if ( runType_ == 1 && calLevel_ < 5 ) {

            sprintf(tmpStr,"%i",getCalLevel(calLevel_));
            printf("Setting calibration level to %s\n",tmpStr);

            if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibLevel",tmpStr) != 0 ) return -1;
            calLevel_++;

            //reset cal_group
            calGroup_ = 0;
            printf("Setting calibration group to %i\n",calGroup_);
            sprintf(tmpStr,"%i",calGroup_);

            if ( rogueSMemControlExec(smem_,ROOTNAME ".SetCalibGroup",tmpStr) != 0 ) return -1;
            calGroup_++;

         } else {
            printf("Reached all cal groups. End run.\n");
            ret |= 0x10;
         }
      }
   } else writeConfig_ = 0;

   return ret;
}

// Copy configuration to event buffer and return size
int32_t RogueCodaGw::config(uint32_t *eventBuffer) {
   uint32_t  len;
   char state[256 * 1024];

   printf("Reading rogue state\n");

   if ( rogueSMemControlExec(smem_,ROOTNAME ".GetYamlState","False",state,256*1024) != 0 ) return 0;

   len = strlen(state);
   while ( (len % 4) != 0 ) state[len++] = 0;
   return (len / 4);
}

// Process end sequence
int32_t RogueCodaGw::end() {

   int32_t status = m_gw->end ();
   if (status == 0)
   {
      status = codaState("CodaEnd",NULL);
   }

   return status;
}

// Process close
void RogueCodaGw::close() {

   m_gw->close ();
   return;
}
