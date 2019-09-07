#ifndef RootRawEvtReader_h
#define RootRawEvtReader_h

#include "GaudiKernel/Algorithm.h"
#include <string>

#include "GaudiKernel/IDataProviderSvc.h"

class TTree;

class RootRawEvtReader:public Algorithm {
public:
  RootRawEvtReader(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RootRawEvtReader();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  StatusCode readMdc(int entry);
  StatusCode readTof(int entry);
  StatusCode readEmc(int entry);
  StatusCode readMuc(int entry);
private:

  IDataProviderSvc* m_evtSvc;


  int m_mdcDigi;
  int m_tofDigi;
  int m_emcDigi;
  int m_mucDigi;

  TTree* m_tree;
  int m_totalEvents;
  int m_fileNumber;
  int m_tempNumber;
  int m_currentEntry;
  int m_currentEntries;

  std::vector<std::string> m_inputFiles;
};

#endif
