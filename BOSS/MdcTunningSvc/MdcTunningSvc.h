#ifndef MDCTUNNINGSVC_H_
#define MDCTUNNINGSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "BesMdcRes.hh"
#include "MdcTunningSvc/IMdcTunningSvc.h"
//#include <mysql.h>
#include "DatabaseSvc/IDatabaseSvc.h"
#include <map>
#include <vector>

class MdcTunningSvc: public Service, virtual public IMdcTunningSvc, 
  virtual public IIncidentListener{
 public:
  MdcTunningSvc( const std::string& name, ISvcLocator* svcloc );
  ~MdcTunningSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Incident handler
  void handle(const Incident&);
  bool initTuningConst();
  //huangb add 
  bool setMcEff(std::string eff_con);
  bool setMcRes();
  //bool setMcRes2();
  bool setMcRes2(std::string res_con);
  bool setMcRes3(std::string res_con);
  StatusCode getMdcTuningTableInfo();

  BesMdcRes* getMdcRes();
  void setMdcRes(std::string path);
  double NewSig(int layerId, double driftD);
  double DeldriftD(int layerId,double driftD);
  double Delcostta(int layerId,double costta);
  double GetEff(int layerId,int cellId,double driftD,double cosTheta,int posFlag);
  double GetRes(int layerId,int cellId,double driftD,double cosTheta,int posFlag,double entranceAngle,double& mean,double& sigma);
  double GetRes2(int layerId,int cellId,double driftD,double cosTheta,int posFlag,double entranceAngle,double& f,double& mean1,double& sigma1,double& mean2,double& sigma2);
  double GetRes3(int layerId,int cellId,double driftD,double cosTheta,int posFlag,double entranceAngle,double& f,double& mean1,double& sigma1,double& mean2,double& sigma2,double& ResLargest,double& ResSmallest,double& ResRatio);
  double ResvEntr(int layerId,double enterA,int ilr,double driftD); 
  double DelEtr_Sig(int lay,double enterA,int ilr,double driftD);

  double get_docaEff(int i,int j){ return docaEff[i][j];};
  double get_thetaEff(int i,int j){ return thetaEff[i][j];}; 
  double get_cellEff(int i,int j){return cellEff[i][j];}
                         
 private:
			 
  static const int docaNo=60;
  static const int thetaNo=40;
  int cellNo[43];
  double docaEff[43][docaNo];
  double thetaEff[43][thetaNo];
  double cellEff[43][288];
  double docaRes[43][docaNo][2][2];
  double docaF[43][docaNo][2];
  double docaMean1[43][docaNo][2];
  double docaSigma1[43][docaNo][2];
  double docaMean2[43][docaNo][2];
  double docaSigma2[43][docaNo][2];
  //added by liukai 2012-6-4
  //residual range  in every residual distribution for hitOnTrk
  double resLargest[43][docaNo][2];
  double resSmallest[43][docaNo][2];
  double resRatio[43][docaNo][2];
  double resLargest_2[43][docaNo][2];
  double resSmallest_2[43][docaNo][2];
  double resRatio_2[43][docaNo][2];

  double docaEff_2[43][docaNo];
  double thetaEff_2[43][thetaNo];
  double cellEff_2[43][288];
  double docaRes_2[43][docaNo][2][2];
  double docaF_2[43][docaNo][2];
  double docaMean1_2[43][docaNo][2];
  double docaSigma1_2[43][docaNo][2];
  double docaMean2_2[43][docaNo][2];
  double docaSigma2_2[43][docaNo][2];

  std::string m_ParBossVer;//added by liukai
  double dD[18];
  int  m_EndcapTuning;
  bool m_dbFlag;
  std::string m_effFile;
  std::string m_resFile;
  std::string m_effFile_endcap;
  std::string m_resFile_endcap;
  std::string m_path;
  BesMdcRes* m_BesMdcRes;
  //huangb add 
  bool m_fromDB;
  IDataProviderSvc* m_eventSvc;
  //			 MYSQL *conn;
  std::string host;
  std::string dbName;
  std::string table;
  std::string userName;
  std::string password;
  unsigned int serialNo;

  IDatabaseSvc* m_dbsvc; 
};

#endif 
