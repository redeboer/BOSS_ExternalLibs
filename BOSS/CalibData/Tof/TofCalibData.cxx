#include "GaudiKernel/MsgStream.h"
#include "CalibData/Tof/bTofCalibBase.h"
#include "CalibData/Tof/bTofCommonCalibBase.h"
#include "CalibData/Tof/eTofCalibBase.h"
#include "CalibData/Tof/etfCalibBase.h"
#include "CalibData/Tof/TofCalibData.h"

namespace CalibData {

  TofCalibData::TofCalibData(std::vector<bTofCalibBase>* bTofCal,std::vector<bTofCommonCalibBase>* bTofComCal,std::vector<eTofCalibBase>* eTofCal,std::vector<etfCalibBase>* etfCal,std::vector<etfBunchCalibBase>* etfBunchCal,std::vector<tofCalibInfoBase>* tofInfoCal){
    if(bTofCal){
      m_bTofCalibData = new std::vector<bTofCalibBase>(*bTofCal);
    }
    if(bTofComCal){
      m_bTofCommonCalibData = new std::vector<bTofCommonCalibBase>(*bTofComCal);
    }
    if(eTofCal){
      m_eTofCalibData = new std::vector<eTofCalibBase>(*eTofCal);
    }

    m_hasEtf = false;
    m_etfCalibData = NULL;
    if( etfCal->size() > 0  ) {
      m_hasEtf = true;
      m_etfCalibData = new std::vector<etfCalibBase>(*etfCal);
    }
    m_hasEtfBunch = false;
    m_etfBunchCalibData = NULL;
    if( etfBunchCal->size() > 0  ) {
      m_hasEtfBunch = true;
      m_etfBunchCalibData = new std::vector<etfBunchCalibBase>(*etfBunchCal);
    }

    if(tofInfoCal){
      m_tofCalibInfoData = new std::vector<tofCalibInfoBase>(*tofInfoCal);
    }
  }
     
  TofCalibData::TofCalibData(std::vector<bTofCalibBase>* bTofCal,std::vector<bTofCommonCalibBase>* bTofComCal,std::vector<eTofCalibBase>* eTofCal,std::vector<etfCalibBase>* etfCal,std::vector<tofCalibInfoBase>* tofInfoCal){
    if(bTofCal){
      m_bTofCalibData = new std::vector<bTofCalibBase>(*bTofCal);
    }
    if(bTofComCal){
      m_bTofCommonCalibData = new std::vector<bTofCommonCalibBase>(*bTofComCal);
    }
    if(eTofCal){
      m_eTofCalibData = new std::vector<eTofCalibBase>(*eTofCal);
    }

    m_hasEtf = false;
    m_etfCalibData = NULL;
    if( etfCal->size() > 0  ) {
      m_hasEtf = true;
      m_etfCalibData = new std::vector<etfCalibBase>(*etfCal);
    }
    m_hasEtfBunch = false;
    m_etfBunchCalibData = NULL;

    if(tofInfoCal){
      m_tofCalibInfoData = new std::vector<tofCalibInfoBase>(*tofInfoCal);
    }
  }
     
  TofCalibData::TofCalibData(std::vector<bTofCalibBase>* bTofCal,std::vector<bTofCommonCalibBase>* bTofComCal,std::vector<eTofCalibBase>* eTofCal,std::vector<tofCalibInfoBase>* tofInfoCal){
    if(bTofCal){
      m_bTofCalibData = new std::vector<bTofCalibBase>(*bTofCal);
    }
    if(bTofComCal){
      m_bTofCommonCalibData = new std::vector<bTofCommonCalibBase>(*bTofComCal);
    }
    if(eTofCal){
      m_eTofCalibData = new std::vector<eTofCalibBase>(*eTofCal);
    }
    if(tofInfoCal){
      m_tofCalibInfoData = new std::vector<tofCalibInfoBase>(*tofInfoCal);
    }
    m_hasEtf            = false;
    m_etfCalibData      = NULL;
    m_hasEtfBunch       = false;
    m_etfBunchCalibData = NULL;
  }

  StatusCode TofCalibData::update(CalibBase1& other, MsgStream* log)
  {
    TofCalibData& other1 = dynamic_cast<TofCalibData& >(other);
    std::cout<<"========================================"<<"\n"<<"here is the update in the TofCalibData"<<"\n"<<"========================================"<<std::endl;
    CalibBase1::update(other, log);
    delete m_bTofCalibData;
    m_bTofCalibData = new std::vector<bTofCalibBase>(*(other1.m_bTofCalibData));
    delete m_bTofCommonCalibData;
    m_bTofCommonCalibData = new std::vector<bTofCommonCalibBase>(*(other1.m_bTofCommonCalibData));
    delete m_eTofCalibData;
    m_eTofCalibData = new std::vector<eTofCalibBase>(*(other1.m_eTofCalibData));
    delete m_tofCalibInfoData;
    m_tofCalibInfoData = new std::vector<tofCalibInfoBase>(*(other1.m_tofCalibInfoData));

    if( m_etfCalibData ) { delete m_etfCalibData; }
    if( other1.m_etfCalibData ) {
      if( (other1.m_etfCalibData)->size() > 0 ) {
	m_etfCalibData = new std::vector<etfCalibBase>(*(other1.m_etfCalibData));
	m_hasEtf = true;
      }
    }
    else {
      m_etfCalibData = NULL;
      m_hasEtf = false;
    }

    if( m_etfBunchCalibData ) { delete m_etfBunchCalibData; }
    if( other1.m_etfBunchCalibData ) {
      if( (other1.m_etfBunchCalibData)->size() > 0 ) {
	m_etfBunchCalibData = new std::vector<etfBunchCalibBase>(*(other1.m_etfBunchCalibData));
	m_hasEtfBunch = true;
      }
    }
    else {
      m_etfBunchCalibData = NULL;
      m_hasEtfBunch = false;
    }

    return StatusCode::SUCCESS;  
  }	
 
  //get the CalibData

  bTofCalibBase TofCalibData::getBTof(int cnt) const{
    std::vector<bTofCalibBase>::iterator it = m_bTofCalibData->begin();
    for(int i=0;i!=cnt;i++,it++) {  
    }
    return *it;
  }

  bTofCommonCalibBase TofCalibData::getBTofCom() const{
    std::vector<bTofCommonCalibBase>::iterator it = m_bTofCommonCalibData->begin();
    return *it;
  }

  eTofCalibBase TofCalibData::getETof(int cnt) const{
    std::vector<eTofCalibBase>::iterator it = m_eTofCalibData->begin();
    for(int i=0;i!=cnt;i++,it++) {  
    }
    return *it;
  }

  etfCalibBase TofCalibData::getEtf(int cnt,int strip) const{
    std::vector<etfCalibBase>::iterator it = m_etfCalibData->begin();
    for(int i=0;i!=cnt;i++) {  
      for( int j=0;j!=12;j++,it++) {
      }
    }
    for( int j=0;j!=strip;j++,it++) {
    }
    return *it;
  }

  etfBunchCalibBase TofCalibData::getEtfBunch() const{
    std::vector<etfBunchCalibBase>::iterator it = m_etfBunchCalibData->begin();
    return *it;
  }

  tofCalibInfoBase TofCalibData::getTofInfo() const{
    std::vector<tofCalibInfoBase>::iterator it = m_tofCalibInfoData->begin();
    return *it;
  }

  double TofCalibData::getBTofAtten(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return  bTof.getAtten(pardex);
  }

  double TofCalibData::getBTofSpeed(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return  bTof.getSpeed(pardex);
  }
 
  double TofCalibData::getBTofPleft(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getP1(pardex);
  }

  double TofCalibData::getBTofPright(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getP2(pardex);
  }

  double TofCalibData::getBTofPoffleft_bunch0(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getPoff1_bunch0(pardex);
  }

  double TofCalibData::getBTofPoffright_bunch0(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getPoff2_bunch0(pardex);
  }

  double TofCalibData::getBTofPoffleft_bunch1(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getPoff1_bunch1(pardex);
  }
  
  double TofCalibData::getBTofPoffright_bunch1(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getPoff2_bunch1(pardex);
  } 

  double TofCalibData::getBTofPoffleft_bunch2(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getPoff1_bunch2(pardex);
  }
  
  double TofCalibData::getBTofPoffright_bunch2(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getPoff2_bunch2(pardex);
  }

  double TofCalibData::getBTofPoffleft_bunch3(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getPoff1_bunch3(pardex);
  }

  double TofCalibData::getBTofPoffright_bunch3(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getPoff2_bunch3(pardex);
  }

  double TofCalibData::getBTofFleft(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getFleft(pardex);
  }

  double TofCalibData::getBTofFright(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getFright(pardex);
  }

  double TofCalibData::getBTofFcounter(int index,int pardex){
    bTofCalibBase bTof = getBTof(index);
    return bTof.getFcounter(pardex);
  }

  double TofCalibData::getBTofOffset(int pardex){
    bTofCommonCalibBase bTofCom = getBTofCom();
    return bTofCom.getOffset(pardex);
  }

  double TofCalibData::getBTofSigmaCorr(int pardex){
    bTofCommonCalibBase bTofCom = getBTofCom();
    return bTofCom.getSigmaCorr(pardex);
  }

  double TofCalibData::getETofAtten(int index,int pardex){
    eTofCalibBase eTof = getETof(index);
    return  eTof.getAtten(pardex);
  }

  double TofCalibData::getETofSpeed(int index,int pardex){
    eTofCalibBase eTof = getETof(index);
    return  eTof.getSpeed(pardex);
  }

  double TofCalibData::getETofP(int index,int pardex){
    eTofCalibBase eTof = getETof(index);
    return eTof.getP(pardex);
  }

  double TofCalibData::getETofFP(int index,int pardex){
    eTofCalibBase eTof = getETof(index);
    return eTof.getFP(pardex);
  }

  double TofCalibData::getEtfSpeed(int index,int strip,int pardex){
    if( !m_hasEtf ) { return 1.0; }
    etfCalibBase etf = getEtf(index,strip);
    return  etf.getSpeed(pardex);
  }
 
  double TofCalibData::getEtfPcombine(int index,int strip,int pardex){
    if( !m_hasEtf ) { return 1.0; }
    etfCalibBase etf = getEtf(index,strip);
    return etf.getP(pardex);
  }

  double TofCalibData::getEtfPleft(int index,int strip,int pardex){
    if( !m_hasEtf ) { return 1.0; }
    etfCalibBase etf = getEtf(index,strip);
    return etf.getP1(pardex);
  }

  double TofCalibData::getEtfPright(int index,int strip,int pardex){
    if( !m_hasEtf ) { return 1.0; }
    etfCalibBase etf = getEtf(index,strip);
    return etf.getP2(pardex);
  }

  double TofCalibData::getEtfPBunch(int pardex){
    if( !m_hasEtfBunch ) { return 0.0; }
    etfBunchCalibBase etfBunch = getEtfBunch();
    return etfBunch.getPBunch(pardex);
  }

  int TofCalibData::getRunBegin(){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getRunBegin();
  }

  int TofCalibData::getRunEnd(){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getRunEnd();
  }

  int TofCalibData::getVersion(){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getVersion();
  }

  int TofCalibData::getQCorr(){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getQCorr();
  }

  int TofCalibData::getQElec(){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getQElec();
  }

  int TofCalibData::getMisLable(){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getMisLable();
  }

  int TofCalibData::getBrEast(int index){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getBrEast(index);
  }

  int TofCalibData::getBrWest(int index){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getBrWest(index);
  }

  int TofCalibData::getEndcap(int index){
    tofCalibInfoBase tofInfo = getTofInfo();
    return tofInfo.getEndcap(index);
  }

}


