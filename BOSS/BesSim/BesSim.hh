#ifndef BesSim_h
#define BesSim_h 1

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IEvtSelector.h"

class IBesRndmGenSvc;
class G4Svc;

/////////////////////////////////////////////////////////////////////////////

class BesSim:public Algorithm {
public:
  BesSim(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 
  StatusCode bookMdcRootFile();
  StatusCode bookTofRootFile();
  StatusCode bookEmcRootFile();
  StatusCode bookMucRootFile();

  StatusCode beginRun();

private:
  int m_nEvent;
  int m_nRun;
  G4Svc* m_G4Svc;
  
  int m_mdc;
  int m_tof;
  int m_emc;
  int m_muc;
  int m_pipeSCM;
  int m_field;
  int m_physicsList;
  int m_formatAR;
  int m_tuning;
  int m_hitOut;
  bool m_TDSFlag;
  double newcut;
  std::vector<std::string> m_tuningFile;
  std::string m_asciiFlag;
  std::string m_asciiFile;
  bool m_rootFlag;
  std::string m_rootFile;



 //caogf for random seed
  IBesRndmGenSvc* p_BesRndmGenSvc;

  int m_logLevel;
};
#endif
