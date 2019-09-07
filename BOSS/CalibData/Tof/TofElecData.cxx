// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/Tof/TofElecData.cxx,v 1.1 2009/09/22 07:28:05 huangb Exp $

/** @class Mdctxpar
 *    Implementation of Mdcspres calibration TCDS class
 */


#include "GaudiKernel/MsgStream.h"
#include "CalibData/Tof/TofElecData.h"
namespace CalibData {

  TofElecData::TofElecData(std::vector<BarrelElec>* bTofCal,std::vector<EndcapElec>* eTofCal){
    if(bTofCal){
      m_bTofElecData = new std::vector<BarrelElec>(*bTofCal);
    }
    if(eTofCal){
      m_eTofElecData = new std::vector<EndcapElec>(*eTofCal);
    }
  }
     
  StatusCode TofElecData::update(CalibBase1& other, MsgStream* log)
  {
    TofElecData& other1 = dynamic_cast<TofElecData& >(other);
    std::cout<<"========================================"<<"\n"<<"here is the update in the TofElecData"<<"\n"<<"=================================================="<<std::endl;
    CalibBase1::update(other, log);
    delete m_bTofElecData;
    m_bTofElecData = new std::vector<BarrelElec>(*(other1.m_bTofElecData));
    delete m_eTofElecData;
    m_eTofElecData = new std::vector<EndcapElec>(*(other1.m_eTofElecData));
    return StatusCode::SUCCESS;  
}	

void TofElecData::sim(TofElecData* other1){
 //  if(!m_bTofElecData->empty()) delete m_bTofElecData;
   if(m_bTofElecData) delete m_bTofElecData;
   m_bTofElecData = new std::vector<BarrelElec>(*(other1->m_bTofElecData));
   if(m_eTofElecData) delete m_eTofElecData;
   m_eTofElecData = new std::vector<EndcapElec>(*(other1->m_eTofElecData));
}

  //get the CalibData

  BarrelElec TofElecData::getBTof(int cnt) const{
    std::vector<BarrelElec>::iterator it = m_bTofElecData->begin();
    for(int i=0;i!=cnt;i++,it++)
      {  
      }
    return *it;
  }


  EndcapElec TofElecData::getETof(int cnt) const{
    std::vector<EndcapElec>::iterator it = m_eTofElecData->begin();
    for(int i=0;i!=cnt;i++,it++)
      {  
      }
    return *it;
  }

 }


