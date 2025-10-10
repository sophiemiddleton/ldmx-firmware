// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/ptd/LdmxBatchTest.cc
  \brief  Program to test composing a batch super-frame

  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

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
 * 2021.04.21 jjr Added help to command line interface with some 
 *                explanation (report_usage) as to what this does. This was
 *                mainly to refresh my memory after being away from this
 *                for around 2 years.
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.01.11 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <iostream>
#include <iomanip>

/* 
    DESCRIPTION

    APV25   services                  128 channels
    Hybrid  services 5 APV25          640 channels  600 channels
    FEB     services 4 Hybrids       2560 channels 2400 channels
    RCE     services 2-4 FEBs  5120-10240 channels 4800 - 9600

    Hybrids = 18 for TKR
               2 for muon

    Gunther's slides gives a total of 500 APV25's = 64K channels
    500 APV's -> 25 FEBs
    Given total of 14 RCEs, this seems to small (25/14 < 1 FEB/RCE)

    
    Data volume estimate, one event

    For N FEB = N * 2560 channels x 128  bits channel x 5% occupancy
              = N * 2KBytes / event

    So for N = 2-4 = 4-8 KBytes/event

    For all 25 this is 50KBytes/event 
    For 50KHz trigger rate = 2.5 GBytes/sec 
    Splitting between 2 COB -> 1.25 GBytes = 10 GBits/sec

    This is right at the limit.

*/


#include "ldmx/data/server/GenericGenerator.hh"
#include "ldmx/data/server/GenericBatch.hh"
#include "ldmx/data/server/TriggerHeader.hh"
#include "ldmx/data/server/TriggerEvent.hh"

#include "ldmx/data/client/GenericBatch.hh"
#include "ldmx/data/client/TriggerHeader.hh"
#include "ldmx/data/client/TriggerEvent.hh"


#include <cstdio>
#include <getopt.h>
#include <stdlib.h>

/* ---------------------------------------------------------------------- *//*!

  \brief  Class to parse and manage the command line parameters
                                                                          */
/* ---------------------------------------------------------------------- */
class Parameters
{
public:
   Parameters (int argc, char *const argv[]);

private:
   void report_usage ();
   
public:
   int m_nbatches;  /*!< The number of batches                            */
   int  m_nevents;  /*!< The number of events in a batch                  */
   int    m_ndata;  /*!< The number of multi-sample data records          */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Configuration for the LDMX simulated data generator for 1 RCE
                                                                          */
/* ---------------------------------------------------------------------- */
class Configuration
{
public:
   Configuration (int rceAddress, int nFebs);

   class Apv
   {
   public:
      uint8_t     m_address;  /*!< The address of this APV                */
   };


   class Hybrid
   {
   public:
      uint8_t     m_address;  /*!< Address of this hybrid                 */
      Apv         m_apvs[5];  /*!< The 5 APV serviced by this hybrid      */
   };


   class Feb
   {
   public:
      uint8_t     m_address;  /*!< Address of this FEB                    */
      Hybrid   m_hybrids[4];  /*!< The 4 hybrids serviced by this FEB     */
   };


   class Rce
   {
   public:
      uint8_t     m_address;  /*!< Address of this RCE                    */
      uint8_t      m_febCnt;  /*!< Number  of   FEBs serviced by this RCE */
      Feb         m_febs[4];  /*!< Maximum of 4 FEBS serviced by this RCE */
   };

   Rce                m_rce;  /*!< The RCE's hardware constituents        */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Configures the RCE's knowledge of the hardware

  \param[in] rceAddress  The RCE identifying address
  \param[in]     febCnt  The number of FEBs this RCE services (2-4)
                                                                          */
/* ---------------------------------------------------------------------- */
Configuration::Configuration (int rceAddress, int febCnt)
{
   m_rce.m_address = rceAddress;
   m_rce.m_febCnt  = febCnt;

   Feb *feb = m_rce.m_febs;
   for (int ifeb = 0; ifeb < febCnt; ifeb++, feb++)
   {
      Hybrid *hybrid = feb->m_hybrids;
      for (int ihybrid; ihybrid < 4; ihybrid++, hybrid++)
      {
         hybrid->m_address = ((rceAddress << 4) | ihybrid);

         Apv *apv = hybrid->m_apvs;

         for (int iapv; iapv < 5; iapv++, apv++)
         {
            apv->m_address = ((ihybrid << 4) | iapv);
         }
      }
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
Parameters::Parameters (int argc, char *const argv[])
{
   static struct option options[] = 
   {
      { "nbatches", required_argument, 0, 'b'},
      { "nevents" , required_argument, 0, 'e'},
      { "ndata"   , required_argument, 0, 'd'},
      { "help"    ,       no_argument, 0, 'h'},
      { 0, 0, 0, 0}
   };

   int nbatches = 1;
   int nevents  = 1;
   int ndata    = 1;

   while (1)
   {
      int option_index = 0;
      int c = getopt_long (argc, argv, "hb:d:e:", options, &option_index);

      if (c == -1)
      {
         break;
      }

      switch (c)
      {
      case 0:   {                                  break; }
      case 'b': {  nbatches = strtol (optarg, 0, 0); break; }
      case 'd': {  ndata    = strtol (optarg, 0, 0); break; }
      case 'e': {  nevents  = strtol (optarg, 0, 0); break; }
      case 'h': {  report_usage ();                         }
      }
   }

   m_nbatches = nbatches;
   m_nevents  = nevents;
   m_ndata    = ndata;

   //// printf ("Nbatches:Nevents:Ndata  = %d:%d:%d\n",
   ////         m_nbatches, m_nevents, m_ndata);

   return;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief Report the command line usage
                                                                          */
/* ---------------------------------------------------------------------- */
void Parameters::report_usage ()
{
   static const char Bold[]   = { 0x1b, '[', '1', 'm', 0};
   static const char Normal[] = { 0x1b, '[', 'm',  0    };
   
   std::cout << "> LdmxBatchTest -<bde>\n"
             << "  where:\n"
             << "       " << Bold << "-b --nbatches" << Normal 
             << " = Number of batches to produce\n"
      
             << "       " << Bold << "-d --ndata   " << Normal
             << " = Number of data points/event\n"
      
             << "       " << Bold << "-e --nevents " << Normal
             << " = Number of events/batch to generate"
      
             << std::endl;

   std::cout << "\n\n"
      "  LdmxBatchTest generates fake Generic data.  It is meant to check the fake"
      "  event generation.\n"
      "  There are 3 levels: Data, Event, Batch and Number of Batches\n\n"
      
    << Bold << "  Data:   " << Normal <<
      "  Each data point consists of (by definition) 5 16-bit values\n" \
      "            The -d (--ndata) sets the number of data points in an event\n"
      "\n"
    << Bold << "  Events: " << Normal <<
      "  Each batch consists of a number events\n"
      "            The -e (--nevents) sets the number of events in a batch of events\n"   "\n"
    << Bold << "  Batches:" << Normal <<
      "  The -b (--nbatches) sets the number of batches to produce"

    << std::endl;
      
   std::cout << "\n Example:\n\n" <<
      "> LdmxBatchTest -b 2 -e 4 -d 3\n"
      "     This produces 2 batches of 4 events, with each event having 3 data values\n"
      << std::endl;
    
   exit (1);
}
/* ---------------------------------------------------------------------- */


static void dump     (uint8_t const              *buffer, 
                      int                        nbytes);

int main (int argc, char *const argv[])
{
   using namespace ldmx::data::server;

   Parameters prms (argc, argv);

   uint32_t buffer[10000];
   GenericBatch batch (buffer, sizeof(buffer));


   int nbatches = prms.m_nbatches;
   int nevents  = prms.m_nevents;
   int ndata    = prms.m_ndata;

   int offset         = 0x80;
   uint8_t RceAddress = 0x77;

   GenericGenerator generator (RceAddress);
   
   for (int ibatch = 0; ibatch < nbatches; ++ibatch)
   {
      generator.reset             (&batch);
      generator.setBatchSeqNumber (ibatch + offset);
      generator.generate          (&batch, nevents, ndata);
      dump     (reinterpret_cast<uint8_t const *>(batch.getBuffer()),
                batch.getNbytes ());
   }

   return 0;
}
/* ---------------------------------------------------------------------- */


static void print (uint8_t const *buffer, int nbytes);

static void dump (uint8_t const *buffer, int nbytes)
{
   using namespace ldmx::data::client;

   int initNEvents = 32;
   print (buffer, nbytes);
   putchar ('\n');


   GenericBatch batch (initNEvents);
   batch.populate (buffer, nbytes);
   batch.print    ();
   printf ("Done\n");
   return;
}


static void print (uint8_t const *buffer, int nbytes)
{
   uint32_t const *buf = reinterpret_cast<decltype(buf)>(buffer);
   int           nwrds = nbytes / sizeof (*buf);

   for (int idx = 0; idx < nwrds;  ++idx)
   {
      if ( (idx % 8) == 0) printf ("b[%3x]", idx);
      printf (" %8.8" PRIx32 , buf[idx]);
      if ( (idx % 8) == 7) putchar ('\n');
   }

   if (nwrds % 8) putchar ('\n');

   return;
}





