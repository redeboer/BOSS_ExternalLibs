#ifndef BESVISALG_H_
#define BESVISALG_H_

// something needed by Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataStoreAgent.h"
#include "GaudiKernel/DataStoreItem.h"

#include "NetDataWriter/NetDataWriter.h"
// something needed by root
#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TDigiEvent.h"
#include "RootEventData/TJobInfo.h"
#include "RootCnvSvc/commonData.h"
#include "RootCnvSvc/RootInterface.h"

#include "RootCnvSvc/RootCnvSvc.h"
#include "TTree.h"
#include "TFile.h"


using namespace std;

class TBossFullEvent;

///////////////////////////////////////////////////////////////////////////////


class EventWriter : public Algorithm {
public:
  EventWriter (const std::string& name, ISvcLocator* pSvcLocator);
  ~EventWriter();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  // some variables needed by semaphore

  // Gaudi service interface
  IDataManagerSvc	*m_pDataManager;
  IDataProviderSvc	*m_pDataProvider;
  IConversionSvc        *m_pConversionSvc;
  IDataStoreAgent 	*m_pAgent;
  DataStoreItem         *m_currentItem;
  RootInterface 	*m_pRootInterface;
  RootCnvSvc*  m_cnvSvc;
  TRecTrackEvent *m_trkEvt;
  TDigiEvent *m_digiEvt;
  TMcEvent  *m_mcEvt;
  THltEvent *m_hltEvt;
  TEvtHeader *m_evtHd;
  virtual std::string getJobOptions(); 
  virtual std::string getDecayOptions(); 

  typedef std::vector<DataStoreItem*> Items;
  typedef std::vector<std::string>    ItemNames;

  commonData m_common;
  TFile*	m_single_outputFiles;
  TTree*	m_single_outputTrees;
  TTree*        m_jobInfoTree;
  int st;
  string m_dofileName;
  string m_dofileName_1;
  Items		m_itemList;
  ItemNames  m_itemNames;
  StatusCode getSvc();
  void addItem(Items& itms, const std::string& descriptor);
  void clearItems(Items& items);
  DataStoreItem* findItem(const std::string& path);
  StatusCode collectObjects(); 
  std::vector<std::string> m_jobOptions; 
  std::string m_bossVer; 
  std::string m_decayOptions; 
  TJobInfo* jobInfo;   

  // for DistBoss
  int              m_mode;  //2-OfflineMode, 3-DistBossMode
  std::string      m_svrName;
  TBossFullEvent  *m_TFullEvt;
  NetDataWriter   *m_writer;
  int              m_bufsize;
  char            *m_cbuf;
};

#endif
