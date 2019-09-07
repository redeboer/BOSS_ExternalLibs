#include "RealizationSvc/RealizationSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include <stdint.h>
#include "CLHEP/Random/RandGauss.h"
#include <vector>

#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"

using namespace std;


RealizationSvc::RealizationSvc( const string& name, ISvcLocator* svcloc) :
    Service (name, svcloc) {
  m_CaliConst = 100;
  declareProperty("RunIDList", m_runNoList);
  declareProperty("IfUseDB", m_useDB = true);
  declareProperty("IfReadBunch", m_ifReadBunch = true);
  declareProperty("IfUseTrg", m_ifReadTrg = false);
  declareProperty("IfReadRanTrg", m_ifReadRandTrg = false);
  declareProperty("IfReadTFEE", m_ifReadTFEE = false);
  declareProperty("IfReadRunInfo", m_ifReadRunInfo = false);
  declareProperty("BossRelease", m_bossRelease="default");
  declareProperty("SftVer", m_SftVer = "default");
  declareProperty("ParVer", m_ParVer = "default");
  declareProperty("InitEvtID", m_initEvtID = 0);
}

StatusCode RealizationSvc::initialize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "RealizationSvc::initialize()" << endreq;

     StatusCode sc = Service::initialize();
     if( sc.isFailure() ) return sc;
 
     if(m_SftVer == "NONE") m_SftVer = getenv("BES_RELEASE");

     for(unsigned int i = 0; i < m_runNoList.size(); i++) {
       if(m_runNoList[i] != 0) {
         std::cout <<"run id : " << m_runNoList[i] << std::endl;
         if(find(m_runIdList.begin(),m_runIdList.end(),m_runNoList[i]) == m_runIdList.end()) m_runIdList.push_back(m_runNoList[i]);
         else log << MSG::ALWAYS <<"RunNo "<<m_runNoList[i]<<" repeated in run list"<<endreq;
       }
       else {
         if(i == 0 || (i+1) >= m_runNoList.size()) log << MSG::ERROR << "Please check your run id list, it is wrong" << endreq;
         if(m_runNoList[i+1] < m_runNoList[i-1]) {
           for(int j = m_runNoList[i+1]+1; j < m_runNoList[i-1]; j++) {
             if(find(m_runIdList.begin(),m_runIdList.end(),j) == m_runIdList.end()) m_runIdList.push_back(j);
             else log << MSG::ALWAYS <<"RunNo "<<j<<" repeated in run list"<<endreq;
           }
         }
         else {
           for(int j = m_runNoList[i-1]+1; j < m_runNoList[i+1]; j++) {
             if(find(m_runIdList.begin(),m_runIdList.end(),j) == m_runIdList.end()) m_runIdList.push_back(j);
             else log << MSG::ALWAYS <<"RunNo "<<j<<" repeated in run list"<<endreq;
           }
         }
       }
     }
     m_runID = m_runIdList[0];

     m_connect = new RealDBUtil::ConnectionProvider();
     if (!m_connect) {
       log << MSG::ERROR << "Could not open connection to database" << endreq;
     }

     IIncidentSvc* incsvc;
     sc = service("IncidentSvc", incsvc);
     int priority = 100;
     if( sc.isSuccess() ){
       incsvc -> addListener(this, "NewRun", priority);
     }

     sc = serviceLocator()->service("EventDataSvc", m_eventSvc, true);
     if (sc.isFailure() ) {
       log << MSG::ERROR << "Unable to find EventDataSvc " << endreq;
       return sc;
     }

     return StatusCode::SUCCESS;
}

StatusCode RealizationSvc::finalize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "RealizationSvc::finalize()" << endreq;

     return StatusCode::SUCCESS;
}

StatusCode RealizationSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IRealizationSvc::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IRealizationSvc*)this;
  }else{
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void RealizationSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;
  if ( inc.type() == "NewRun" ){
    log << MSG::DEBUG << "Begin New Run" << endreq;
    if(m_useDB == true) {
      int runNo;
        SmartDataPtr<Event::EventHeader> evt(m_eventSvc,"/Event/EventHeader");
        if( evt ){
          runNo = evt -> runNumber();
          log << MSG::INFO <<"The runNumber of current event is  "<<runNo<<endreq;
        }
        else {
          log << MSG::ERROR <<"Can not get EventHeader!"<<endreq;
        }
      readDB(std::abs(runNo)); 
    }
  } 
}

float RealizationSvc::getLuminosity(int runNo) {
  using RealDBUtil::ConnectionProvider;

  MsgStream log( messageService(), name() );
  std::string sLum;
  ConnectionProvider::eRet e = m_connect->getReadLumInfo(sLum,runNo, m_SftVer, m_ParVer, m_bossRelease);
  if(e != 0 ) {
    log << MSG::ERROR << "Could not find Luminosity infor., exit." << endreq;
    exit(1);
  }
  m_lumi  = std::atof(sLum.c_str());
  log << MSG::INFO << "Luminosity is " << m_lumi << " in Run " << runNo << endreq;
  return m_lumi;
}

std::vector<std::string> RealizationSvc::getBgFileName(std::string query) {
  MsgStream log( messageService(), name() );
  using RealDBUtil::ConnectionProvider;

  std::vector<std::string> Bgfilename;
  Bgfilename.clear();

  std::vector<std::string> fileInfor;
  ConnectionProvider::eRet e = m_connect->getReadBackgroundInfo(fileInfor,query);
  if(e != 0 ) {
    log << MSG::ERROR << "Could not find background infor., exit." << endreq;
    exit(1);
  }

  for(unsigned int i = 0; i < fileInfor.size(); i+=2)
  {
    Bgfilename.push_back(fileInfor[i]+"/"+fileInfor[i+1]);
  }
  fileInfor.clear();
  
  for(unsigned int i = 0; i < Bgfilename.size(); i++) {
     log << MSG::INFO << "Background file name: " << Bgfilename[i] << endreq;
  }

  return Bgfilename;
}
/*
std::vector<int> RealizationSvc::getRunEvtNum(std::vector<int> inrunList, int EvtMax) {
  std::vector<int> outrunList;
  std::vector<int> vRunEvtNum;
  outrunList.clear();
  vRunEvtNum.clear();

  for(int i = 0; i < inrunList.size(); i++) {
    if(inrunList[i] != 0) {
      std::cout <<"run id : " << inrunList[i] << std::endl;
      if(find(outrunList.begin(),outrunList.end(),inrunList[i]) == outrunList.end()) outrunList.push_back(inrunList[i]);
      else std::cout << "RunNo "<<inrunList[i]<<" repeated in run list"<< std::endl;
    }
    else {
      if(i == 0 || (i+1) >= inrunList.size())  std::cerr << "Please check your run id list, it is wrong" << std::endl;
      if(inrunList[i+1] < inrunList[i-1]) {
        for(int j = inrunList[i+1]+1; j < inrunList[i-1]; j++) {
          if(find(outrunList.begin(),outrunList.end(),j) == outrunList.end()) outrunList.push_back(j);
          else std::cout <<"RunNo "<<j<<" repeated in run list"<< std::endl;
        }
      }
      else {
        for(int j = inrunList[i-1]+1; j < inrunList[i+1]; j++) {
          if(find(outrunList.begin(),outrunList.end(),j) == outrunList.end()) outrunList.push_back(j);
          else std::cout <<"RunNo "<<j<<" repeated in run list"<< std::endl;
        }
      }
    }
  }

  std::vector<float> lumi;
  lumi.clear();
  float totLumi = 0;
  for(unsigned int i = 0; i < outrunList.size(); i++)
  {
    float lumi_value = getLuminosity(outrunList[i]);
    lumi.push_back(lumi_value);
    totLumi += lumi_value;
  }

  int totSimEvt = 0;
  int evtSubNo = 0;
  if(totLumi != 0) {
    for(unsigned int i = 0; i < (lumi.size() - 1); i++) {
      //if(i == 0) m_evtNoList.push_back(0);
      double ratio = lumi[i]/totLumi*EvtMax;
      evtSubNo = int (ratio);
      if((ratio-evtSubNo) >= 0.5) evtSubNo = evtSubNo + 1;
      totSimEvt += evtSubNo;
      if(evtSubNo == 0) {
        std::cout << "The run " <<outrunList[i]<<" is not simulated, due to the luminosity is too small!" << std::endl;
      }
      else {
        vRunEvtNum.push_back(outrunList[i]);
        vRunEvtNum.push_back(evtSubNo);
      }
      std::cout <<"Total "<< evtSubNo <<" events need to be simulated in run " <<outrunList[i]<< std::endl;
    }
    vRunEvtNum.push_back(outrunList[lumi.size() - 1]); //set the last run id
    vRunEvtNum.push_back(EvtMax - totSimEvt);
    std::cout <<"Total "<< EvtMax - totSimEvt <<" events need to be simulated in run " <<outrunList[lumi.size() - 1]<< std::endl;
  }
  else {
    std::cerr << "ERORR: " << "Total luminosity is ZERO!!! Please check your run list. " << std::endl;
    std::abort();
  }
  
  return vRunEvtNum;
}
*/
void RealizationSvc::readDB(int runNo) {
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "In getCaliConst" << endreq;

  using RealDBUtil::ConnectionProvider;

  StatusCode sc;

  //Get bunch infor
  if(m_ifReadBunch == true) {
    std::vector<std::string> sbunch;
    std::vector<double> bunchInfo;
    ConnectionProvider::eRet e = m_connect->getReadBunchInfo(sbunch,runNo,m_SftVer,m_ParVer,m_bossRelease);
    if(e == 7 ) {
      bool last = true;
      for(int id = 1; true; id++) {
        e = m_connect->getReadBunchInfo(sbunch,runNo+id,m_SftVer,m_ParVer,m_bossRelease);
        last = true;
        if(e == 7) {
          if((runNo-id) > 0) {
            e = m_connect->getReadBunchInfo(sbunch,runNo-id,m_SftVer,m_ParVer,m_bossRelease);
            last = false;
          }
        }
        if(e == 0) {
          if(last == true) log << MSG::INFO << "Use Bunch infor. of run " << runNo+id << " instead of run " << runNo << endreq;
          if(last == false) log << MSG::INFO << "Use Bunch infor. of run " << runNo-id << " instead of run " << runNo << endreq;
          break;
        }
      }
    }
    if(e != 0 && e != 7) {
      log << MSG::ERROR << "Could not find Bunch infor., exit." << endreq;
      exit(1);
    }
    for(unsigned int i = 0; i < sbunch.size(); i++)
    {
      bunchInfo.push_back(atof(sbunch[i].c_str()));
    }
    sbunch.clear();

    if(bunchInfo.size() == 6)
    {
      //convert from cm to mm
      m_bunch_x = 10*bunchInfo[0];
      m_bunch_y = 10*bunchInfo[1];
      m_bunch_z = 10*bunchInfo[2];
      m_sigma_x = 10*bunchInfo[3];
      m_sigma_y = 10*bunchInfo[4];
      m_sigma_z = 10*bunchInfo[5];
      log << MSG::INFO << "BunchPosX: " << m_bunch_x << " BunchPosY: " << m_bunch_y << " BunchPosZ: " << m_bunch_z << " in Run " << runNo << endreq;
      log << MSG::INFO << "BunchSigmaX: " << m_sigma_x << " BunchSigmaY: " << m_sigma_y << " BunchSigmaZ: " << m_sigma_z << " in Run " << runNo << endreq;
    }
    else {
      log << MSG::ERROR << "Please check the bunch information, the size is " << bunchInfo.size() << endreq;
    }
    bunchInfo.clear();
  }
  
  //get trigger table
  if(m_ifReadTrg == true) {
    m_trgTable.clear();
    std::vector<std::string> strgTable;
    ConnectionProvider::eRet e = m_connect->getReadTrgTableInfo(strgTable,runNo);
    if(e != 0 ) {
      log << MSG::ERROR << "Could not find TrgTable infor., exit." << endreq;
      exit(1);
    }
    for(unsigned int i = 0; i < strgTable.size(); i++)
    {
      long long value = 0;
      for (unsigned int j = 0; j < (strgTable[i]).length(); j++)
      {
        value = value*10 + (strgTable[i][j] - '0');
      }
      m_trgTable.push_back(uint32_t(value));
    }
    strgTable.clear();
    if(m_trgTable.size() != 65) log << MSG::ERROR << "Please check the TrgTable information, the size is " << m_trgTable.size() << endreq;

    std::vector<double> vtrgGain;
    e = m_connect->getEmcGain(vtrgGain,runNo);
    if(e != 0 ) {
      log << MSG::ERROR << "Could not find emc gain infor., exit." << endreq;
      exit(1);
    }
    for(unsigned int i = 0; i < vtrgGain.size(); i++) {
      m_trgGain[i] = vtrgGain[i];
    }

    //get trigger config information
    e = m_connect->getTrgConfigInfo(runNo);
    if(e != 0 ) {
      log << MSG::ERROR << "Could not find Trigger config infor., exit." << endreq;
      exit(1);
    }
    m_EtotDataSteps = m_connect->getEtotDataSteps();
    m_VthBEtotH = m_connect->getVthBEtotH();
    m_VthEEtotH = m_connect->getVthEEtotH();
    m_VthEtotL = m_connect->getVthEtotL();
    m_VthEtotM = m_connect->getVthEtotM();
    m_VthBLZ = m_connect->getVthBLZ();
    m_VthDiffB = m_connect->getVthDiffB();
    m_VthDiffE = m_connect->getVthDiffE();
    m_VthBalBLK = m_connect->getVthBalBLK();
    m_VthBalEEMC = m_connect->getVthBalEEMC();
    m_VthDiffMin = m_connect->getVthDiffMin();
  }

  //get background file
  if(m_ifReadRandTrg == true) {
    std::vector<std::string> fileInfor;
    ConnectionProvider::eRet e = m_connect->getReadBackgroundInfo(fileInfor,runNo);
    if(e == 7 ) {
      bool last = true;
      for(int id = 1; true; id++) {
        e = m_connect->getReadBackgroundInfo(fileInfor,runNo+id);
        last = true;
        if(e == 7) {
          if((runNo-id) > 0) {
            e = m_connect->getReadBackgroundInfo(fileInfor,runNo-id);
            last = false;
          }
        }
        if(e == 0) {
          if(last == true) log << MSG::INFO << "Use Bg files of run " << runNo+id << " instead of run " << runNo << endreq;
          if(last == false) log << MSG::INFO << "Use Bg files of run " << runNo-id << " instead of run " << runNo << endreq;
          break;
        }
      }
    }
    if(e != 0 && e != 7) {
      log << MSG::ERROR << "Could not find background infor., exit." << endreq;
      exit(1);
    }
    m_bgfilename.clear();
    for(unsigned int i = 0; i < fileInfor.size(); i+=2)
    {
      m_bgfilename.push_back(fileInfor[i]+"/"+fileInfor[i+1]);
    }
    fileInfor.clear();
    for(unsigned int i = 0; i < m_bgfilename.size(); i++) {
       log << MSG::INFO << "Background file name: " << m_bgfilename[i] << " in run " << runNo << endreq;
    }

    //get luminosity curve parameters
    std::string srunTime;
    std::string stau_value;
    e = m_connect->getLumCurvePar(srunTime, stau_value, runNo, m_SftVer, m_ParVer, m_bossRelease);
    if(e != 0 ) {
      log << MSG::ERROR << "Could not find Luminosity curve parameters, exit." << endreq;
      exit(1);
    }
    m_runTotalTime  = std::atof(srunTime.c_str());
    m_tauValue  = std::atof(stau_value.c_str());
    log << MSG::INFO << "Total time is " << m_runTotalTime << ", tau is " << m_tauValue << " in run " << runNo << ". " << endreq;
  }

  //get tof threshold in table TFEE
  if(m_ifReadTFEE == true) {
    m_tfee.clear();
    ConnectionProvider::eRet e = m_connect->getReadTofThreshInfo(m_tfee,runNo);
    if(e != 0 ) {
      log << MSG::ERROR << "Could not find TFEE infor., exit." << endreq;
      exit(1);
    }
    
    for(unsigned int i = 0; i < m_tfee.size(); i++) {
      log << MSG::INFO << "TFEE ----> " << m_tfee[i] << endreq; 
    }
  }

  //get run infor.
  if(m_ifReadRunInfo == true) {
    m_runInfo.clear();
    std::vector<std::string> srunInfo;
    ConnectionProvider::eRet e = m_connect->getRunInfo(srunInfo,runNo);
    if(e != 0 ) {
      log << MSG::ERROR << "Could not find run infor., exit." << endreq;
      exit(1);
    }
    for(unsigned int i = 0; i < srunInfo.size(); i++)
    {
      m_runInfo.push_back(atof(srunInfo[i].c_str()));
    }
    srunInfo.clear();
    
    for(unsigned int i = 0; i < m_runInfo.size(); i++) {
      log << MSG::INFO << "runInfo ----> " << m_runInfo[i] << endreq; 
    }
  }
  
}

void RealizationSvc::updateRealData() {
}
