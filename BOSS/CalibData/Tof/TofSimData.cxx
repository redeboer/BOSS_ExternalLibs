// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/Tof/TofSimData.cxx,v 1.2 2010/02/03 00:33:53 huangb Exp $

#include "GaudiKernel/MsgStream.h"
#include "CalibData/Tof/TofSimData.h"

namespace CalibData {

  TofSimData::TofSimData(std::vector<TofSimConstBase>* fTofConst,std::vector<BTofSimBase>* fBTofSim,std::vector<ETofSimBase>* fETofSim) {
    if(fTofConst) {
      m_TofSimConstData = new std::vector<TofSimConstBase>(*fTofConst);
    }
    if(fBTofSim){
      m_BTofSimData = new std::vector<BTofSimBase>(*fBTofSim);
    }
    if(fETofSim){
      m_ETofSimData = new std::vector<ETofSimBase>(*fETofSim);
    }
  }
     
  StatusCode TofSimData::update(CalibBase1& other, MsgStream* log) {
    TofSimData& other1 = dynamic_cast<TofSimData& >(other);
    std::cout<<"========================================"<<"\n"<<"here is the update in the TofSimData"<<"\n"<<"=================================================="<<std::endl;
    CalibBase1::update(other, log);
    delete m_TofSimConstData;
    m_TofSimConstData = new std::vector<TofSimConstBase>(*(other1.m_TofSimConstData));
    delete m_BTofSimData;
    m_BTofSimData = new std::vector<BTofSimBase>(*(other1.m_BTofSimData));
    delete m_ETofSimData;
    m_ETofSimData = new std::vector<ETofSimBase>(*(other1.m_ETofSimData));

    return StatusCode::SUCCESS;  
  }
 
  //get the Simulation Data

  TofSimConstBase TofSimData::getTofConst() const {
    std::vector<TofSimConstBase>::iterator it = m_TofSimConstData->begin();
    return *it;
  }

  BTofSimBase TofSimData::getBTofSim(int cnt) const {
    std::vector<BTofSimBase>::iterator it = m_BTofSimData->begin();
    for(int i=0;i!=cnt;i++,it++) {}
    return *it;
  }

  ETofSimBase TofSimData::getETofSim(int cnt) const {
    std::vector<ETofSimBase>::iterator it = m_ETofSimData->begin();
    for(int i=0;i!=cnt;i++,it++) {}
    return *it;
  }

  double TofSimData::getBarLowThres() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getBarLowThres();
  }

  double TofSimData::getBarHighThres() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getBarHighThres();
  }

  double TofSimData::getEndLowThres() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getEndLowThres();
  }

  double TofSimData::getEndHighThres() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getEndHighThres();
  }

  double TofSimData::getBarPMTGain() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getBarPMTGain();
  }

  double TofSimData::getEndPMTGain() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getEndPMTGain();
  }

 double TofSimData::getBarConstant() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getBarConstant();
  }

  double TofSimData::getEndConstant() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getEndConstant();
  }

  double TofSimData::getEndNoiseSwitch() {
    TofSimConstBase tofConst = getTofConst();
    return tofConst.getEndNoiseSwitch();
  }

  double TofSimData::getBarGain(int cnt) {
    BTofSimBase bTof = getBTofSim(cnt);
    return bTof.getGain();
  }

  double TofSimData::getBarRatio(int cnt) {
    BTofSimBase bTof = getBTofSim(cnt);
    return bTof.getRatio();
  }

  double TofSimData::getBarAttenLength(int cnt) {
    BTofSimBase bTof = getBTofSim(cnt);
    return bTof.getAttenLength();
  }

  double TofSimData::getEndGain(int cnt) {
    ETofSimBase eTof = getETofSim(cnt);
    return eTof.getGain();
  }

  double TofSimData::getEndAttenLength(int cnt) {
    ETofSimBase eTof = getETofSim(cnt);
    return eTof.getAttenLength();
  }

  double TofSimData::getEndNoiseSmear(int cnt) {
    ETofSimBase eTof = getETofSim(cnt);
    return eTof.getNoiseSmear();
  }

}


