//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     /
////
////---------------------------------------------------------------------------/
////
////Description:
////Author:   Caogf
////Created:  Sep, 2008
////Modified:
////Comment:
////
//
#include "Trigger/BesTMTrig.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include <map>

BesTMTrig::BesTMTrig() {
  char line[255];
  std::string filename = std::string(getenv( "TRIGGERROOT" ));
  filename += std::string("/data/tm/TM_BARREL.pat");
  std::ifstream infile( filename.c_str() );
  if(infile) {
    while(infile) {
      infile.getline(line,255);
      if(line[0] == '#') continue;
      std::string mdcId, tofId_l, tofId_h, emcId_l, emcId_h;
      char* token = strtok( line, " " );
      if ( token ) { mdcId   = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { tofId_l = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { tofId_h = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { emcId_l = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { emcId_h = token; token = strtok( NULL, " " );} else continue;
      if ( token != NULL ) continue;
      //Convert to int
      //int i_mdcId   = atoi( mdcId.c_str() );
      int i_tofId_l = atoi( tofId_l.c_str() );
      int i_tofId_h = atoi( tofId_h.c_str() );
      int i_emcId_l = atoi( emcId_l.c_str() );
      int i_emcId_h = atoi( emcId_h.c_str() );
      //std::cout << "MdcId TofId_L TofId_H EmcId_L EmcId_H: " << i_mdcId << ", " << i_tofId_l <<", " << i_tofId_h << ", " << i_emcId_l << ", " << i_emcId_h << std::endl;
      std::vector<int> tof_tmp;
      std::vector<int> emc_tmp;
      //for tof
      if(i_tofId_l > i_tofId_h) {
        for(int i = i_tofId_l; i <= (i_tofId_h + 88); i++) {
          if(i >= 88) tof_tmp.push_back(i - 88);
          else tof_tmp.push_back(i);
        }
      }
      else {
        for(int i = i_tofId_l; i <= i_tofId_h; i++) {
          tof_tmp.push_back(i);
        }
      }
      //for emc
      if(i_emcId_l > i_emcId_h) {
        for(int i = i_emcId_l; i <= (i_emcId_h + 30); i++) {
          if(i >= 30) emc_tmp.push_back(i - 30);
          else emc_tmp.push_back(i);
        }
      }
      else {
        for(int i = i_emcId_l; i <= i_emcId_h; i++) {
          emc_tmp.push_back(i);
        }
      }
      m_tm_barrel.push_back(tof_tmp);
      m_tm_barrel.push_back(emc_tmp);
    }
  }
  else {
    std::cout << "Can not open file: " << filename << " in BesTMTrig. " << std::endl;
  }
  //read track match east endcap
  filename = std::string(getenv( "TRIGGERROOT" ));
  filename += std::string("/data/tm/TM_EAST_ENDCAP.pat");
  std::ifstream infile1( filename.c_str() );
  if(infile1) {
    while(infile1) {
      infile1.getline(line,255);
      if(line[0] == '#') continue;
      std::string mdcId, tofId_l, tofId_h, emcId_l, emcId_h;
      char* token = strtok( line, " " );
      if ( token ) { mdcId   = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { tofId_l = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { tofId_h = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { emcId_l = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { emcId_h = token; token = strtok( NULL, " " );} else continue;
      if ( token != NULL ) continue;
      //Convert to int
      //int i_mdcId   = atoi( mdcId.c_str() );
      int i_tofId_l = atoi( tofId_l.c_str() );
      int i_tofId_h = atoi( tofId_h.c_str() );
      int i_emcId_l = atoi( emcId_l.c_str() );
      int i_emcId_h = atoi( emcId_h.c_str() );
      //std::cout << "MdcId TofId_L TofId_H EmcId_L EmcId_H: " << i_mdcId << ", " << i_tofId_l <<", " << i_tofId_h << ", " << i_emcId_l << ", " << i_emcId_h << std::endl;
      std::vector<int> tof_tmp;
      std::vector<int> emc_tmp;
      //for tof
      if(i_tofId_l > i_tofId_h) {
        for(int i = i_tofId_l; i <= (i_tofId_h + 24); i++) {
          if(i >= 24) tof_tmp.push_back(i - 24);
          else tof_tmp.push_back(i);
        }
      }
      else {
        for(int i = i_tofId_l; i <= i_tofId_h; i++) {
          tof_tmp.push_back(i);
        }
      } 
      //for emc
      if(i_emcId_l > i_emcId_h) {
        for(int i = i_emcId_l; i <= (i_emcId_h + 16); i++) {
          if(i >= 16) emc_tmp.push_back(i - 16);
          else emc_tmp.push_back(i);
        }
      }
      else {
        for(int i = i_emcId_l; i <= i_emcId_h; i++) {
          emc_tmp.push_back(i);
        }
      }
      m_tm_ecap.push_back(tof_tmp);
      m_tm_ecap.push_back(emc_tmp);
    } 
  } 
  else {
    std::cout << "Can not open file: " << filename << " in BesTMTrig. " << std::endl;
  }

  //read track match west endcap
  filename = std::string(getenv( "TRIGGERROOT" ));
  filename += std::string("/data/tm/TM_WEST_ENDCAP.pat");
  std::ifstream infile2( filename.c_str() );
  if(infile2) {
    while(infile2) {
      infile2.getline(line,255);
      if(line[0] == '#') continue;
      std::string mdcId, tofId_l, tofId_h, emcId_l, emcId_h;
      char* token = strtok( line, " " );
      if ( token ) { mdcId   = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { tofId_l = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { tofId_h = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { emcId_l = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { emcId_h = token; token = strtok( NULL, " " );} else continue;
      if ( token != NULL ) continue;
      //Convert to int
      //int i_mdcId   = atoi( mdcId.c_str() );
      int i_tofId_l = atoi( tofId_l.c_str() );
      int i_tofId_h = atoi( tofId_h.c_str() );
      int i_emcId_l = atoi( emcId_l.c_str() );
      int i_emcId_h = atoi( emcId_h.c_str() );
      //std::cout << "MdcId TofId_L TofId_H EmcId_L EmcId_H: " << i_mdcId << ", " << i_tofId_l <<", " << i_tofId_h << ", " << i_emcId_l << ", " << i_emcId_h << std::endl;
      std::vector<int> tof_tmp;
      std::vector<int> emc_tmp;
      //for tof
      if(i_tofId_l > i_tofId_h) {
        for(int i = i_tofId_l; i <= (i_tofId_h + 24); i++) {
          if(i >= 24) tof_tmp.push_back(i - 24);
          else tof_tmp.push_back(i);
        }
      }
      else {
        for(int i = i_tofId_l; i <= i_tofId_h; i++) {
          tof_tmp.push_back(i);
        }
      }
      //for emc
      if(i_emcId_l > i_emcId_h) {
        for(int i = i_emcId_l; i <= (i_emcId_h + 16); i++) {
          if(i >= 16) emc_tmp.push_back(i - 16);
          else emc_tmp.push_back(i);
        }
      }
      else {
        for(int i = i_emcId_l; i <= i_emcId_h; i++) {
          emc_tmp.push_back(i);
        }
      }
      m_tm_wcap.push_back(tof_tmp);
      m_tm_wcap.push_back(emc_tmp);
    } 
  } 
  else {
    std::cout << "Can not open file: " << filename << " in BesTMTrig. " << std::endl;
  }
  infile.close();
  infile1.close();
  infile2.close();
  //dump();
}

BesTMTrig::~BesTMTrig() {
}

void BesTMTrig::dump() {
  std::cout << "The size of barrel vector: " << m_tm_barrel.size() << std::endl;
  std::cout << "The size of east endcap vector: " << m_tm_ecap.size() << std::endl;
  std::cout << "The size of west endcap vector: " << m_tm_wcap.size() << std::endl;
  std::cout << "--------------------------- BARREL -----------------------------" << std::endl;
  for(unsigned int i = 0; i < m_tm_barrel.size(); i+=2) {
    std::cout << "MdcId is " << i/2 << " TofId size " << m_tm_barrel[i].size() <<
                 " Id " << m_tm_barrel[i][0] << " " << m_tm_barrel[i][m_tm_barrel[i].size() - 1] << std::endl;
    std::cout << "MdcId is " << i/2 << " EmcId size " << m_tm_barrel[i+1].size() <<
                 " Id " << m_tm_barrel[i+1][0] << " " << m_tm_barrel[i+1][m_tm_barrel[i+1].size() - 1] << std::endl;
  }
  std::cout << "--------------------------- EAST ENDCAP -----------------------------" << std::endl;
  for(unsigned int i = 0; i < m_tm_ecap.size(); i+=2) {
    std::cout << "MdcId is " << i/2 << " TofId size " << m_tm_ecap[i].size() <<
                 " Id " << m_tm_ecap[i][0] << " " << m_tm_ecap[i][m_tm_ecap[i].size() - 1] << std::endl;
    std::cout << "MdcId is " << i/2 << " EmcId size " << m_tm_ecap[i+1].size() <<
                 " Id " << m_tm_ecap[i+1][0] << " " << m_tm_ecap[i+1][m_tm_ecap[i+1].size() - 1] << std::endl;
  }
  std::cout << "--------------------------- WEST ENDCAP -----------------------------" << std::endl;
  for(unsigned int i = 0; i < m_tm_wcap.size(); i+=2) {
    std::cout << "MdcId is " << i/2 << " TofId size " << m_tm_wcap[i].size() <<
                 " Id " << m_tm_wcap[i][0] << " " << m_tm_wcap[i][m_tm_wcap[i].size() - 1] << std::endl;
    std::cout << "MdcId is " << i/2 << " EmcId size " << m_tm_wcap[i+1].size() <<
                 " Id " << m_tm_wcap[i+1][0] << " " << m_tm_wcap[i+1][m_tm_wcap[i+1].size() - 1] << std::endl;
  }
}

void BesTMTrig::startTMTrig() {
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("BesGlobalTrigSvc", m_tmpSvc);
  m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);

  //find mdc track id used in track match
  std::vector<int> tm_mdcstrkId;
  std::vector<int> tm_mdcltrkId;
  std::vector<int> mdcstrkId = m_pIBGT->getMdcStrkId();
  std::vector<int> mdcltrkId = m_pIBGT->getMdcLtrkId();
  for(unsigned int i = 0; i < mdcstrkId.size(); i++)
  { 
    int trackId = int (mdcstrkId[i]/2);
    if(find(tm_mdcstrkId.begin(),tm_mdcstrkId.end(),trackId) == tm_mdcstrkId.end())  tm_mdcstrkId.push_back(trackId);
  }
  for(unsigned int i = 0; i < mdcltrkId.size(); i++)
  { 
    int trackId = int (mdcltrkId[i]/2);
    if(find(tm_mdcltrkId.begin(),tm_mdcltrkId.end(),trackId) == tm_mdcltrkId.end())  tm_mdcltrkId.push_back(trackId);
  }
  //find tof track id used in track match
  std::map<int,std::vector<int>,greater<int> > tofHit;
  std::vector<int> tm_btofhitId;
  std::vector<int> tm_etofhitId;
  std::vector<int> tm_wtofhitId;
  tofHit = m_pIBGT->getTofHitPos();
  for(std::map<int,std::vector<int>,greater<int> >::iterator iter = tofHit.begin(); iter != tofHit.end(); iter++) {
    if(iter->first == 0) {
      for(unsigned int i = 0; i < iter->second.size(); i++){
        tm_etofhitId.push_back(iter->second[i]);
      }
    }
    if(iter->first == 1) {
      for(unsigned int i = 0; i < iter->second.size(); i++){
        tm_btofhitId.push_back(iter->second[i]);
      }
    }
    if(iter->first == 2) {
      for(unsigned int i = 0; i < iter->second.size(); i++){
        tm_wtofhitId.push_back(iter->second[i]);
      }
    }
  }
  //find emc track id used in track match
  std::map<int,std::vector<complex<int> >, greater<int> > emcHit;
  std::vector<int> tm_bemcClusId;
  std::vector<int> tm_eemcClusId;
  std::vector<int> tm_wemcClusId;
  emcHit = m_pIBGT->getEmcClusId();
  for(std::map<int,std::vector<complex<int> >, greater<int> >::iterator iter = emcHit.begin(); iter != emcHit.end(); iter++) {
    if(iter->first == 1) {
      for(unsigned int i = 0; i < iter->second.size(); i++)
      {
        // cout<<"barrel theta is "<<(iter->second[i]).real()<<" phi is "<<(iter->second[i]).imag()<<endl;
        int crystalId = iter->second[i].imag();
        if(find(tm_bemcClusId.begin(),tm_bemcClusId.end(),crystalId) == tm_bemcClusId.end())  tm_bemcClusId.push_back(crystalId);
      }
    }
    if(iter->first == 0) {
      for(unsigned int i = 0; i < iter->second.size(); i++)
      {
        // cout<<"east theta is "<<(iter->second[i]).real()<<" phi is "<<(iter->second[i]).imag()<<endl;
        int crystalId = int ((iter->second[i].imag())/2);
        if(find(tm_eemcClusId.begin(),tm_eemcClusId.end(),crystalId) == tm_eemcClusId.end())  tm_eemcClusId.push_back(crystalId);
      }
    }
    if(iter->first == 2) {
      for(unsigned int i = 0; i < iter->second.size(); i++)
      {
        //cout<<"west theta is "<<(iter->second[i]).real()<<" phi is "<<(iter->second[i]).imag()<<endl;
        int crystalId = int ((iter->second[i].imag())/2);
        if(find(tm_wemcClusId.begin(),tm_wemcClusId.end(),crystalId) == tm_wemcClusId.end()) tm_wemcClusId.push_back(crystalId);
      }
    }
  }
  //start track match
  int BTOF_MATCH[64];
  int ETOF_MATCH[64];
  int WTOF_MATCH[64];
  int BEMC_MATCH[64];
  int EEMC_MATCH[64];
  int WEMC_MATCH[64];
  int BTOF_EMC_MATCH[64];
  int ETOF_EMC_MATCH[64];
  int WTOF_EMC_MATCH[64];
  for(int i = 0; i < 64; i++) {
    BTOF_MATCH[i] = 0;
    ETOF_MATCH[i] = 0;
    WTOF_MATCH[i] = 0;
    BEMC_MATCH[i] = 0;
    EEMC_MATCH[i] = 0;
    WEMC_MATCH[i] = 0;
    BTOF_EMC_MATCH[i] = 0;
    ETOF_EMC_MATCH[i] = 0;
    WTOF_EMC_MATCH[i] = 0;
  }
  for(unsigned int i = 0; i < tm_mdcltrkId.size(); i++) {
    int cellId = tm_mdcltrkId[i];
    for(unsigned int j = 0; j < (m_tm_barrel[2*cellId]).size(); j++) {
      if(find(tm_btofhitId.begin(),tm_btofhitId.end(),m_tm_barrel[2*cellId][j]) != tm_btofhitId.end()) { BTOF_MATCH[cellId] = 1;  break; }
    }
    for(unsigned int j = 0; j < (m_tm_barrel[2*cellId+1]).size(); j++) {
      if(find(tm_bemcClusId.begin(),tm_bemcClusId.end(),m_tm_barrel[2*cellId+1][j]) != tm_bemcClusId.end()) { BEMC_MATCH[cellId] = 1;  break; }
    }
  }
  for(unsigned int i = 0; i < tm_mdcstrkId.size(); i++) {
    int cellId = tm_mdcstrkId[i];
    for(unsigned int j = 0; j < (m_tm_ecap[2*cellId]).size(); j++) {
      if(find(tm_etofhitId.begin(),tm_etofhitId.end(),m_tm_ecap[2*cellId][j]) != tm_etofhitId.end()) { ETOF_MATCH[cellId] = 1;  break; }
    }
    for(unsigned int j = 0; j < (m_tm_ecap[2*cellId+1]).size(); j++) {
      if(find(tm_eemcClusId.begin(),tm_eemcClusId.end(),m_tm_ecap[2*cellId+1][j]) != tm_eemcClusId.end()) { EEMC_MATCH[cellId] = 1;  break; }
    }
    for(unsigned int j = 0; j < (m_tm_wcap[2*cellId]).size(); j++) {
      if(find(tm_wtofhitId.begin(),tm_wtofhitId.end(),m_tm_wcap[2*cellId][j]) != tm_wtofhitId.end()) { WTOF_MATCH[cellId] = 1;  break; }
    }   
    for(unsigned int j = 0; j < (m_tm_wcap[2*cellId+1]).size(); j++) {
      if(find(tm_wemcClusId.begin(),tm_wemcClusId.end(),m_tm_wcap[2*cellId+1][j]) != tm_wemcClusId.end()) { WEMC_MATCH[cellId] = 1;  break; }
    }
  }
  //count track number
  int NATrk = 0;
  int NBTrk = 0;
  int NCTrk = 0;
  for(int i = 0; i < 64; i++) {
    if(BTOF_MATCH[i] == 1) NATrk++; 
    if((BTOF_MATCH[i] == 1) && (BEMC_MATCH[i] == 1)) { BTOF_EMC_MATCH[i] = 1;  NBTrk++; }
    if((ETOF_MATCH[i] == 1) && (EEMC_MATCH[i] == 1)) ETOF_EMC_MATCH[i] = 1;
    if((WTOF_MATCH[i] == 1) && (WEMC_MATCH[i] == 1)) WTOF_EMC_MATCH[i] = 1;
    if(m_pIBGT->getEndCapMode() == 0) {
      if(ETOF_MATCH[i] == 1) NCTrk++;
      if(WTOF_MATCH[i] == 1) NCTrk++;
    }
    else {
      if(ETOF_EMC_MATCH[i] == 1) NCTrk++;
      if(WTOF_EMC_MATCH[i] == 1) NCTrk++;
    }
  } 
  //set trigger conditions
  bool NATrk1 = false;
  bool NATrk2 = false;
  bool ATrk_BB = false;
  bool NBTrk1 = false;
  bool NBTrk2 = false;
  bool BTrk_BB = false;
  bool NCTrk1 = false;
  bool NCTrk2 = false;
  bool CTrk_BB = false;

  if(NATrk >= 1) NATrk1 = true;
  if(NATrk >= 2) NATrk2 = true;
  if(NBTrk >= 1) NBTrk1 = true;
  if(NBTrk >= 2) NBTrk2 = true;
  if(NCTrk >= 1) NCTrk1 = true;
  if(NCTrk >= 2) NCTrk2 = true;

  for(int i = 0; i < 64; i++) {
    if(BTOF_MATCH[i] == 1) {
      for(int j=0; j < 7; j++)
      {
        int id;
        if((i+j+29) > 63)
        {
          id = i+j+29-64;
        }
        else
        {
          id = i+j+29;
        }
        if(BTOF_MATCH[id] == 1) ATrk_BB = true;
      }
    }
    if(BTOF_EMC_MATCH[i] == 1) {
      for(int j=0; j < 7; j++)
      {
        int id;
        if((i+j+29) > 63)
        {
          id = i+j+29-64;
        }
        else
        {
          id = i+j+29;
        }
        if(BTOF_EMC_MATCH[id] == 1) BTrk_BB = true;
      } 
    }
    if(m_pIBGT->getEndCapMode() == 0) {
      if((ETOF_MATCH[i] == 1) || (WTOF_MATCH[i] == 1)) {
        for(int j=0; j < 7; j++)
        {
          int id;
          if((i+j+29) > 63)
          {
            id = i+j+29-64;
          }
          else
          {
            id = i+j+29;
          }
          if((ETOF_MATCH[id] == 1) || (WTOF_MATCH[id] == 1)) CTrk_BB = true;
        }
      }
    }
    else {
      if((ETOF_EMC_MATCH[i] == 1) || (WTOF_EMC_MATCH[i] == 1)) {
        for(int j=0; j < 7; j++)
        {
          int id;
          if((i+j+29) > 63)
          {
            id = i+j+29-64;
          }
          else
          {
            id = i+j+29;
          }
          if((ETOF_EMC_MATCH[id] == 1) || (WTOF_EMC_MATCH[id] == 1)) CTrk_BB = true;
        }
      }
    }
  }

  m_pIBGT->setTMNATrk1(NATrk1);
  m_pIBGT->setTMNATrk2(NATrk2);
  m_pIBGT->setTMATrkBB(ATrk_BB);
  m_pIBGT->setTMNBTrk1(NBTrk1);
  m_pIBGT->setTMNBTrk2(NBTrk2);
  m_pIBGT->setTMBTrkBB(BTrk_BB);
  m_pIBGT->setTMNCTrk1(NCTrk1);
  m_pIBGT->setTMNCTrk2(NCTrk2);
  m_pIBGT->setTMCTrkBB(CTrk_BB);
}
