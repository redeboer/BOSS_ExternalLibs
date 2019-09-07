#include "RootFile/RootFileWriter.h"
#include "TFile.h"
#include "TTree.h"
#include "TBufferFile.h"
#include "RootEventData/TJobInfo.h"
#include "IRawFile/RawFileExceptions.h"
#include "DistBossUtil/DistBossCode.h"
#include <stdlib.h>

RootFileWriter::RootFileWriter(WriterArgType& fname)
   : m_fname( fname),
     m_firstRound( true ),
     m_stat(0)
{
   m_file = new TFile(fname.c_str(), "RECREATE");
   m_tree = new TTree("Event","Event");
   m_jtree = new TTree("JobInfoTree", "Job info");
   m_evt  = new TBossFullEvent;
   m_jobInfo = new TJobInfo;
}

RootFileWriter::~RootFileWriter()
{
   if ( m_file->IsOpen() ) {
      this->close();
   }

   delete m_file;
   //delete m_tree;
   //delete m_jtree;
   delete m_evt;
   delete m_jobInfo;
}

int RootFileWriter::writeEvent(void* pevt, int size)
{
   if ( size == 4 ) {
      m_stat = *((int*)pevt);
      if ( m_stat == DistBossCode::StatusFinalize ) {
	 throw ReachEndOfFileList();
      }
      else {
	 throw RawExMessage("[RootFileWriter] Get an invalid DistBossCode!");
      }
   }

   TBufferFile buffer(TBuffer::kRead, size, ((char*)pevt+4), kFALSE);
   TBossFullEvent * evt = (TBossFullEvent*)buffer.ReadObject(TBossFullEvent::Class());

   m_evt->copy(evt);

   /*
   static int ii = 0;
   std::cout << "Total: " << ++ii << "   Run: " << m_evt->m_EvtHeader->getRunId()
             << "  Evt: " << m_evt->m_EvtHeader->getEventId() << std::endl;
	     */

   if ( m_firstRound ) {
      if (m_evt->m_EvtHeader) m_tree->Branch("TEvtHeader","TEvtHeader",&m_evt->m_EvtHeader,3200000,1);
      if (m_evt->m_digiEvt) m_tree->Branch("TDigiEvent","TDigiEvent",&m_evt->m_digiEvt,3200000,1);
      if (m_evt->m_dstEvt) m_tree->Branch("TDstEvent", "TDstEvent", &m_evt->m_dstEvt, 3200000, 1);
      if (m_evt->m_mcEvt) m_tree->Branch("TMcEvent","TMcEvent",&m_evt->m_mcEvt,3200000,1);
      if (m_evt->m_trigEvt) m_tree->Branch("TTrigEvent","TTrigEvent",&m_evt->m_trigEvt,3200000,1);
      if (m_evt->m_hltEvt) m_tree->Branch("THltEvent","THltEvent",&m_evt->m_hltEvt,3200000,1);
      if (m_evt->m_rectrackEvt) m_tree->Branch("TRecEvent", "TRecTrackEvent", &m_evt->m_rectrackEvt, 3200000, 1);
      if (m_evt->m_evtRecObject) m_tree->Branch("TEvtRecObject", "TEvtRecObject", &m_evt->m_evtRecObject, 3200000, 1);

      m_jtree->Branch("JobInfo",&m_jobInfo);

      m_firstRound = false;
   }

   if ( m_file->IsZombie() || (!m_file->IsOpen()) ) {
      std::cout<<"ROOT File: " << m_fname << " bad status in RootFileWriter" << std::endl;
      exit(1);
   }

   int nb = m_tree->Fill();
   if ( nb == -1 ) {
      std::cerr << "RootFileWriter: error in fill tree " << m_tree->GetName() << std::endl;
      exit(1);
   }

   delete evt;

   m_evt->reset();
   m_file = m_tree->GetCurrentFile();

   return nb;
}

int RootFileWriter::close()
{
   m_jobInfo->setBossVer( getenv("BES_RELEASE") );
   m_jobInfo->addJobOptions( this->getJobOptions() );
   m_jobInfo->setDecayOptions( this->getDecayOptions() );
   m_jtree->Fill();

   m_file->Write();
   m_file->Close();

   m_stat = 1;

   return 0;
}

int RootFileWriter::stat()
{
   return m_stat;
}

std::string RootFileWriter::getJobOptions()
{
   string opts("JobOptions Place Holder\n");
   return opts;
}

std::string RootFileWriter::getDecayOptions()
{
   string opts("DecayOptions Place Holder\n");
   return opts;
}
