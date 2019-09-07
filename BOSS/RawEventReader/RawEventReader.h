#ifndef MixerAlg_h
#define MixerAlg_h

#include "GaudiKernel/Algorithm.h"
#include <string>

#include "GaudiKernel/IDataProviderSvc.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h"

class MdcConverter;
class TofConverter;
class EmcConverter;
class MucConverter;

class RawEventReader:public Algorithm {
public:
  RawEventReader(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RawEventReader();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  StatusCode readEvent();
private:

  IDataProviderSvc* m_evtSvc;

  MdcConverter* m_mdcCnv;
  TofConverter* m_tofCnv;
  EmcConverter* m_emcCnv;
  MucConverter* m_mucCnv;

  int m_mdcdigi;
  int m_tofdigi;
  int m_emcdigi;
  int m_mucdigi;


  RAWEVENT* m_raw_event;
  std::vector<std::string> m_inputFiles;
};

#endif
