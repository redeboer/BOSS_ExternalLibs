#ifndef REALIZATIONSVC_H
#define REALIZATIONSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include <vector>
#include "RealizationSvc/IRealizationSvc.h"
#include "RealizationSvc/ConnectionProvider.h"

class IDataProviderSvc;

class RealizationSvc: public Service, virtual public IRealizationSvc,
			  virtual public IIncidentListener{

public:
  RealizationSvc( const std::string& name, ISvcLocator* svcloc );
  ~RealizationSvc(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  // Incident handler
  void handle(const Incident&); 

  // interface
  void readDB(int runNo);
  void updateRealData();

  //set or get initial event ID
  int getInitEvtID() { return m_initEvtID; }
  void setInitEvtID(int i) { m_initEvtID = i; }

  void setLuminosity(float i) { m_lumi = i; }
  float getLuminosity() { return m_lumi; }
  float getLuminosity(int runNo);

  void setBunchPosX(double i) { m_bunch_x = i; }
  void setBunchPosY(double i) { m_bunch_y = i; }
  void setBunchPosZ(double i) { m_bunch_z = i; }
  void setBunchSizeX(double i) { m_sigma_x = i; }
  void setBunchSizeY(double i) { m_sigma_y = i; }
  void setBunchSizeZ(double i) { m_sigma_z = i; }
  double getBunchPosX() { return m_bunch_x; }
  double getBunchPosY() { return m_bunch_y; }
  double getBunchPosZ() { return m_bunch_z; }
  double getBunchSizeX() { return m_sigma_x; }
  double getBunchSizeY() { return m_sigma_y; }
  double getBunchSizeZ() { return m_sigma_z; }

  inline double getTrgGain(int cry_id) { return m_trgGain[cry_id]; }

  //set event number in a run
  void setRunEvtNum(int i) { m_RunEvtNum = i; }
  //set event number in a random trigger file
  //void setRanTrgEvtNum(std::vector<int> i) { m_RanTrgEvtNum = i; }

  //get event number in a run
  int getRunEvtNum() { return m_RunEvtNum; }
  //get event number in a random trigger file
  //std::vector<int> getRanTrgEvtNum() { return m_RanTrgEvtNum; }

 // std::vector<int> getRunEvtNum(std::vector<int> inrunList, int EvtMax); 

  // trigger table
  void setTrgTable(std::vector<uint32_t> i) { m_trgTable = i; }
  std::vector<uint32_t> getTrgTable() { return m_trgTable; }

  // trigger config infor.
  int getEtotDataSteps() { return m_EtotDataSteps; }
  int getVthBEtotH() { return m_VthBEtotH; }
  int getVthEEtotH() { return m_VthEEtotH; }
  int getVthEtotL() { return m_VthEtotL; }
  int getVthEtotM() { return m_VthEtotM; }
  int getVthBLZ() { return m_VthBLZ; }
  int getVthDiffB() { return m_VthDiffB; }
  int getVthDiffE() { return m_VthDiffE; }
  int getVthBalBLK() { return m_VthBalBLK; }
  int getVthBalEEMC() { return m_VthBalEEMC; }
  int getVthDiffMin() { return m_VthDiffMin; }

  void setBgFileName(std::vector<std::string> i) { m_bgfilename = i; }
  std::vector<std::string> getBgFileName() { return m_bgfilename; }
  std::vector<std::string> getBgFileName(std::string query);

  //luminosity curve parameters
  float getRunTotalTime() { return m_runTotalTime; }
  float getTauValue() { return m_tauValue; }

  //run infor.
  std::vector<double> getRunInfo() {return m_runInfo; }

  //TFEE
  std::vector<std::string> getTofThreshold() { return m_tfee; }

  //related run id
  std::vector<int> getRunList() { return m_runIdList; }
  int getRunId() { return m_runID; }
  void setRunId(int i) { m_runID = i; }
  
  bool UseDBFlag() { return m_useDB; }

  bool ifReadBunch() { return m_ifReadBunch; }
  bool ifReadTrg() { return m_ifReadTrg; }
  bool ifReadRandTrg() { return m_ifReadRandTrg; }
  bool ifReadTFEE() { return m_ifReadTFEE; }

private:
  int m_CaliConst;

  std::string m_bossRelease;
  std::string m_SftVer;
  std::string m_ParVer;

  float m_lumi;
  double m_bunch_x;
  double m_bunch_y;
  double m_bunch_z;

  double m_sigma_x;
  double m_sigma_y;
  double m_sigma_z;

  int m_initEvtID;

  int m_RunEvtNum;
  //std::vector<int> m_RanTrgEvtNum;

  //run id list property
  std::vector<int> m_runNoList;
  std::vector<int> m_runIdList;
  int m_runID;
  
  bool m_useDB;

  //luminosity curve parameters
  float m_runTotalTime;
  float m_tauValue;

  bool m_ifReadBunch;
  bool m_ifReadTrg;
  bool m_ifReadRandTrg;
  bool m_ifReadTFEE;
  bool m_ifReadRunInfo;

  std::vector<uint32_t> m_trgTable;
  IDataProviderSvc* m_eventSvc;

  //background
  std::vector<std::string> m_bgfilename;
  //TFEE
  std::vector<std::string> m_tfee;

  //run infor.
  std::vector<double> m_runInfo;

  double m_trgGain[6240];

  //trigger config infor.
  int m_EtotDataSteps;
  int m_VthBEtotH;
  int m_VthEEtotH;
  int m_VthEtotL;
  int m_VthEtotM;
  int m_VthBLZ;
  int m_VthDiffB;
  int m_VthDiffE;
  int m_VthBalBLK;
  int m_VthBalEEMC;
  int m_VthDiffMin;

  RealDBUtil::ConnectionProvider* m_connect;
};

#endif
