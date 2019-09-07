//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     /
//
//---------------------------------------------------------------------------/
//
//Description:
//Author:   Caogf
//Created:  Feb, 2006
//Modified:
//Comment:

#include "Trigger/BesMdcTrig.h"
#include "Trigger/MdcTCount.h"
#include "Trigger/TrigPara.h"
#include <iostream>
#include <vector>
#include <algorithm>

#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"

using namespace TrigConf;

BesMdcTrig::BesMdcTrig()
{
  m_MdcTCount = new MdcTCount();
}
BesMdcTrig::~BesMdcTrig()
{
  if(m_MdcTCount) delete m_MdcTCount;
}
void BesMdcTrig::startMdcTrig()
{
  StatusCode sc = Gaudi::svcLocator()->service("BesGlobalTrigSvc", m_tmpSvc);
  m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);

  NLtrk1 = false;
  NLtrk2 = false;
  NLtrkN = false;
  LTrk_BB = false;
  NStrk1 = false;
  NStrk2 = false;
  NStrkN = false;
  STrk_BB = false;
  NItrk1 = false;
  NItrk2 = false;

  if(!m_MdcTCount) std::cerr<<"can not initialize mdcTCount"<<std::endl;
  m_MdcTCount->GetTCount();
  int nItrk1 = m_MdcTCount->GetnItrkSL1();
  int nItrk2 = m_MdcTCount->GetnItrkSL2();

//  std::cout<<"track number is "<<s<<" "<<l<<std::endl;
  //trigger condition of short and long track in outer room
  int MAXMDCTRACK = m_pIBGT->getMAXMDCTRACK();
  if((nItrk1 >= 1) || (nItrk2 >= 1)) NItrk1 = true;
  if((nItrk1 >= 1) && (nItrk2 >= 1)) NItrk2 = true;
   
  //trigger condition of babar using short track information
  std::vector<int> stmp;
  std::vector<int>::iterator siter;
  stmp.clear();
  stmp = m_MdcTCount->GetstrackId();
  int nstrk = stmp.size();
  if(nstrk>=1) NStrk1 = true;
  if(nstrk>=2) NStrk2 = true;
  if(nstrk>=MAXMDCTRACK) NStrkN = true;
  
 // if(stmp.empty()) std::cout<<"have no short track id infor in vector tmp"<<std::endl;
  
  for(siter=stmp.begin();siter!=stmp.end();siter++)
  {
    for(int i=0;i<15;i++)
    {
      if(((*siter)+i+57)>127) 
      {
        int id;
        id = (*siter)+i+57-128;
        if(find(stmp.begin(),stmp.end(),id)!=stmp.end()) { STrk_BB = true; break; }
      }
      else
      {
        int id;
        id = (*siter)+i+57;
        if(find(stmp.begin(),stmp.end(),id)!=stmp.end()) { STrk_BB = true; break; }
      }
    }
    
    if(STrk_BB == true) break;
  }
  //trigger condition of babar using long track information
  std::vector<int> ltmp;
  std::vector<int>::iterator liter;
  ltmp.clear();
  ltmp = m_MdcTCount->GetltrackId();
  int nltrk = ltmp.size();
  if(nltrk>=1) NLtrk1 = true;
  if(nltrk>=2) NLtrk2 = true;
  if(nltrk>=MAXMDCTRACK) NLtrkN = true;
//  if(ltmp.empty()) std::cout<<"have no long track id infor in vector tmp"<<std::endl;
  for(liter=ltmp.begin();liter!=ltmp.end();liter++)
  {
    for(int i=0;i<15;i++)
    {
      if(((*liter)+i+57)>127)
      {
        int id;
        id = (*liter)+i+57-128;
        if(find(ltmp.begin(),ltmp.end(),id)!=ltmp.end()) { LTrk_BB = true; break; }
      }
      else
      {
        int id;
        id = (*liter)+i+57;
        if(find(ltmp.begin(),ltmp.end(),id)!=ltmp.end()) { LTrk_BB = true; break; }
      }
    }
//    std::cout<<"LTrk_BB is "<<LTrk_BB<<std::endl;
    if(LTrk_BB == true) break;
  }	    

  //set trigger conditions in service
  m_pIBGT->setSTrk_BB(STrk_BB);
  m_pIBGT->setNStrkN(NStrkN);
  m_pIBGT->setNStrk2(NStrk2);
  m_pIBGT->setNStrk1(NStrk1);
  m_pIBGT->setLTrk_BB(LTrk_BB);
  m_pIBGT->setNLtrkN(NLtrkN);
  m_pIBGT->setNLtrk2(NLtrk2);
  m_pIBGT->setNLtrk1(NLtrk1);
  m_pIBGT->setNItrk2(NItrk2);
  m_pIBGT->setNItrk1(NItrk1);
}
