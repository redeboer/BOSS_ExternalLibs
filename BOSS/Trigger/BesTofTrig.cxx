//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     /
////
////---------------------------------------------------------------------------/
////
////Description:
////Author:   Caogf
////Created:  Feb, 2006
////Modified:
////Comment:
////
//
#include "Trigger/BesTofTrig.h"
#include <algorithm>
#include <iostream>

#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include <map>
using namespace std;

BesTofTrig::BesTofTrig()
{
  m_TofHitCount = TofHitCount::get_Tof();
}
BesTofTrig::~BesTofTrig()
{
}
void BesTofTrig::startTofTrig()
{

  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("BesGlobalTrigSvc", m_tmpSvc);
  m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);

  //reset hit counter
  int barHitCount = 0;
  int endHitCount = 0;
  
  NBTOF1 = false;
  NBTOF2 = false;
  NETOF1 = false;
  NETOF2 = false;
  NTOF1 = false;
 
  TBB = false;
  ETBB = false;
  
  bHitpos.clear();
  ecapHitpos.clear();
  wcapHitpos.clear();
  
  if(!m_TofHitCount) std::cerr<<"can not get TofHitCount pointer"<<std::endl;
  
  std::vector<int> barHitmap;
  std::vector<int> endHitmap;
  std::vector<int> barHitId1;
  std::vector<int> barHitId2;
  std::vector<int> ecapHitId;
  std::vector<int> wcapHitId;
  std::vector<int>::iterator iter;
  
  barHitmap.clear();
  endHitmap.clear();
  barHitId1.clear();
  barHitId2.clear();
  ecapHitId.clear();
  wcapHitId.clear();

  barHitId1 = m_TofHitCount->GetbarrelHit1();
  barHitId2 = m_TofHitCount->GetbarrelHit2();
  ecapHitId = m_TofHitCount->GetecapHit();
  wcapHitId = m_TofHitCount->GetwcapHit();
  
  for(unsigned int btofId = 0; btofId < barHitId1.size(); btofId++) barHitmap.push_back(barHitId1[btofId]);
  for(unsigned int btofId = 0; btofId < barHitId2.size(); btofId++) barHitmap.push_back(88+barHitId2[btofId]);
  for(unsigned int etofId = 0; etofId < ecapHitId.size(); etofId++) endHitmap.push_back(ecapHitId[etofId]);
  for(unsigned int etofId = 0; etofId < wcapHitId.size(); etofId++) endHitmap.push_back(48+wcapHitId[etofId]);
  //two layer tof in barrel is "or" logic
  // -------------------------
  // ... |   |   |j+1| j | ...    outer layer
  // -------------------------
  // ---------------------------
  //   ... |   |   | i |   | ...  inner layer
  // ---------------------------
  // M_i = M_i || M_j || M_j+1
  int scinNo;
  for(int btofId = 0; btofId < 88; btofId++) {
    if(m_pIBGT->getTofLayerControl() == 1) {
      if((find(barHitId1.begin(),barHitId1.end(),btofId)!=barHitId1.end()) ) { 
         bHitpos.push_back(btofId);
      }
    }
    if(m_pIBGT->getTofLayerControl() == 2) {
      if(btofId != 87) {
        if((find(barHitId1.begin(),barHitId1.end(),btofId)!=barHitId1.end()) || 
           (find(barHitId2.begin(),barHitId2.end(),btofId)!=barHitId2.end()) ||
           (find(barHitId2.begin(),barHitId2.end(),btofId+1)!=barHitId2.end()) ) {
           bHitpos.push_back(btofId);
        }
      }
      if(btofId == 87) {
        if((find(barHitId1.begin(),barHitId1.end(),btofId)!=barHitId1.end()) || 
           (find(barHitId2.begin(),barHitId2.end(),btofId)!=barHitId2.end()) ||
           (find(barHitId2.begin(),barHitId2.end(),0)!=barHitId2.end()) ) {
           bHitpos.push_back(btofId);
        }
      }
    }
  }
  //hit position in end caps
  for(iter=ecapHitId.begin();iter!=ecapHitId.end();iter++)
  {
    scinNo = (int) (*iter)/2;
    if(find(ecapHitpos.begin(),ecapHitpos.end(),scinNo)==ecapHitpos.end())
    {
      ecapHitpos.push_back(scinNo);
    }
  }

  for(iter=wcapHitId.begin();iter!=wcapHitId.end();iter++)
  {
    scinNo = (int)*iter/2;
    if(find(wcapHitpos.begin(),wcapHitpos.end(),scinNo)==wcapHitpos.end())
    {
      wcapHitpos.push_back(scinNo);
    }
  }

  //Counting the hit number only for NBTOF1, NBTOF2, NETOF1, NETOF1 trigger conditions. Combine contiguous hits as one hit
  //barrel
  for(int btofId = 0; btofId < 88; btofId++) {
    if(btofId == 0) {
      if((find(bHitpos.begin(),bHitpos.end(),btofId) != bHitpos.end()) &&
         (find(bHitpos.begin(),bHitpos.end(),87) == bHitpos.end())) {
        barHitCount++;
      }
    }
    else {
      if((find(bHitpos.begin(),bHitpos.end(),btofId) != bHitpos.end()) &&
         (find(bHitpos.begin(),bHitpos.end(),btofId-1) == bHitpos.end())) {
        barHitCount++;
      }
    }
  }
  //endcaps
  for(int etofId = 0; etofId < 48; etofId++) {
    if(etofId == 0) {
      if((find(wcapHitId.begin(),wcapHitId.end(),etofId) != wcapHitId.end()) &&
         (find(wcapHitId.begin(),wcapHitId.end(),47) == wcapHitId.end())) {
        endHitCount++;
      }
      if((find(ecapHitId.begin(),ecapHitId.end(),etofId) != ecapHitId.end()) &&
         (find(ecapHitId.begin(),ecapHitId.end(),47) == ecapHitId.end())) {
        endHitCount++;
      }
    }
    else {
      if((find(wcapHitId.begin(),wcapHitId.end(),etofId) != wcapHitId.end()) &&
         (find(wcapHitId.begin(),wcapHitId.end(),etofId-1) == wcapHitId.end())) {
        endHitCount++;
      }
      if((find(ecapHitId.begin(),ecapHitId.end(),etofId) != ecapHitId.end()) &&
         (find(ecapHitId.begin(),ecapHitId.end(),etofId-1) == ecapHitId.end())) {
        endHitCount++;
      }
    }
  }
  
  if(barHitCount>=1) NBTOF1 = true;
  if(barHitCount>=2) NBTOF2 = true;
  if(endHitCount>=1) NETOF1 = true;
  if(endHitCount>=2) NETOF2 = true;
  if((NBTOF1 == true) || (NETOF1 == true)) NTOF1 = true;
  
  for(iter=bHitpos.begin();iter!=bHitpos.end();iter++)
  {
    scinNo = *iter;
    for(int i=0;i<13;i++)
    {
      if(scinNo+38+i<88)
      {
        if(find(bHitpos.begin(),bHitpos.end(),scinNo+38+i)!=bHitpos.end()) TBB = true;
      }
      if(scinNo+38+i>=88)
      {
	if(find(bHitpos.begin(),bHitpos.end(),scinNo+38+i-88)!=bHitpos.end()) TBB = true;
      }
    }
  }
  for(iter=wcapHitId.begin();iter!=wcapHitId.end();iter++)
  {
    scinNo = *iter;
    for(int i=0;i<9;i++)
    {
      if(scinNo+20+i<48)
      {
        if(find(ecapHitId.begin(),ecapHitId.end(),scinNo+20+i)!=ecapHitId.end()) ETBB = true;
      }
      if(scinNo+20+i>=48)
      {
        if(find(ecapHitId.begin(),ecapHitId.end(),scinNo+20+i-48)!=ecapHitId.end()) ETBB = true;
      }
    }
  }
  map<int,vector<int>,greater<int> > mHitId;
  mHitId.clear();
  typedef pair<int, vector<int> > vpair;
/*
  std::vector<int> bhitmap;
  bhitmap.clear();
  for(int id = 0; id < barHitId1.size(); id++) {
    bhitmap.push_back(barHitId1[id]);
  }
  for(int id = 0; id < barHitId2.size(); id++) {
    bhitmap.push_back(88+barHitId2[id]);
  }
*/
  mHitId.insert(vpair(0,ecapHitpos));
  mHitId.insert(vpair(1,bHitpos));
  mHitId.insert(vpair(2,wcapHitpos));

  m_pIBGT->setNBTof1(NBTOF1);
  m_pIBGT->setNBTof2(NBTOF2);
  m_pIBGT->setNETof1(NETOF1);
  m_pIBGT->setNETof2(NETOF2);
  m_pIBGT->setNTof1(NTOF1);
  m_pIBGT->setBTofBB(TBB);
  m_pIBGT->setETofBB(ETBB);
  m_pIBGT->setTofHitPos(mHitId);
  m_pIBGT->setBTofHitMap(barHitmap);
  m_pIBGT->setETofHitMap(endHitmap);
}
