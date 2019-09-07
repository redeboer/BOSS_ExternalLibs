#include "GaudiKernel/Algorithm.h"
#include "McTruth/McParticle.h"

class EventNavigator;

enum EvtNavigatorState {SIMU, RECO, ANA};

class BesNavigatorInit:public Algorithm {
public:
  BesNavigatorInit (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void fillMdcInfo();
  void fillEmcInfo();
  void fillTofInfo();
  void fillMucInfo();

private:
  EventNavigator* m_navigator;
  EvtNavigatorState state;

  bool b_fillMdc;
  bool b_fillTof;
  bool b_fillEmc;
  bool b_fillMuc;

  bool b_fillHitMaps;

  int m_mdcCut;
};


