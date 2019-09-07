//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     /
//
//---------------------------------------------------------------------------/
//
//Description:
//Author:   Caogf
//Created:  Mar, 2006
//Modified:
//Comment:
//

#include "Trigger/MdcTCount.h"
#include "Trigger/MdcTF.h"
#include <iostream>

#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"

MdcTCount::MdcTCount()
{
  m_MdcTF = new MdcTF();
}
MdcTCount::~MdcTCount()
{
  if(m_MdcTF) delete m_MdcTF;
}
void MdcTCount::GetTCount()
{
  if(m_MdcTF) m_MdcTF->GetTrack();
  //reset values
  strackNo = 0;
  ltrackNo = 0;
  nItrkSL1 = 0;
  nItrkSL2 = 0;
  strackId.clear();
  ltrackId.clear();
  intrkId1.clear();
  intrkId2.clear();

  intrkId1 = m_MdcTF->GetintrkSL1();
  intrkId2 = m_MdcTF->GetintrkSL2();
  nItrkSL1 = intrkId1.size();
  nItrkSL2 = intrkId2.size();

  //Get short and long track information 
  for(int i = 0;i<128;i++)
  {
    int stId = 0, ltId = 0;
    //find short track
    if(m_MdcTF->GetstrackId(i) != 0) { 
      //cout<<"TCount Cell ID "<<i<<endl;
      //for(int j = 1; j < 128; j++) {
      for(int j = 1; j < 17; j++) {
        if((i-j) < 0) stId = m_MdcTF->GetstrackId(128-j+i);
        else stId = m_MdcTF->GetstrackId(i-j);
        //cout<<"stId is "<<stId<<" j is "<<j<<" j%2 is "<<j%2<<endl;
        if(stId == 0) {
          if(j%2 != 0) { strackId.push_back(i); strackNo+=1; }//cout<<"short track count id : "<<i<<endl; }
          break;
        }
      }
    }
    //find long track
    if(m_MdcTF->GetltrackId(i) != 0) { 
      //for(int j = 1; j < 128; j++) {     
      for(int j = 1; j < 17; j++) {     
        if((i-j) < 0) ltId = m_MdcTF->GetltrackId(128-j+i);
        else ltId = m_MdcTF->GetltrackId(i-j); 
        if(ltId == 0) {
          if(j%2 != 0) { ltrackId.push_back(i); ltrackNo+=1; }// cout<<"long track count id : "<<i<<endl; }
          break;
        }
      }
    }
  }
/*
  //No track combination
  for(int i = 0;i<128;i++)
  {
    //find short track
    if(m_MdcTF->GetstrackId(i) != 0) {
      strackId.push_back(i); 
    }
    //find long track
    if(m_MdcTF->GetltrackId(i) != 0) {
      ltrackId.push_back(i); 
    }
  }
*/
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("BesGlobalTrigSvc", m_tmpSvc);
  m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);

  //set short and long track Id in service
  m_pIBGT->setMdcStrkId(strackId);
  m_pIBGT->setMdcLtrkId(ltrackId);
}
