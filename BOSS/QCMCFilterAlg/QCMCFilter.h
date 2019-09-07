#ifndef QCMCFilter_h
#define QCMCFilter_h 1

#include <string>
#include "HepPDT/ParticleDataTable.hh"
#include "GaudiKernel/Algorithm.h"

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"

#include "CLHEP/Matrix/SymMatrix.h"
#include <vector>
using namespace std;

class IDataProviderSvc;

class QCMCFilter : public Algorithm {
 public:
  QCMCFilter(const std::string& name, ISvcLocator* pSvcLocator);

  std::vector<double> findD0Decay(int charm);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  // ------------ constants, enums and typedefs --------------
 
 private:
  IDataProviderSvc* m_evtSvc;

  HepPDT::ParticleDataTable* m_particleTable;

  // ------------ data members -------------------------------
  int m_nUnknownEvents ;
  int m_nUnknownDecays ;
  int m_nD0D0barEvents ;
  int m_nD0bar ;
  int m_nDpDmEvents ;
  int m_nD0D0barDiscarded ;
  int m_nDpDmDiscarded ;
  int m_nCPPlus ;
  int m_nCPMinus ;
  int m_nFlavoredCFD0 ;
  int m_nFlavoredCSD0 ;
  int m_nFlavoredDCSD0 ;
  int m_nSL ;
  
  int m_nDalitz ;
  double m_dalitzNumer1 ;
  double m_dalitzNumer2 ;
  double m_dalitzDenom ;
  
  bool m_invertSelection ;
  double m_x ;
  double m_y ;
  double m_rCF ;
  double m_zCF ;
  bool m_wCFSign ; // true = +, false = -
  double m_rCS ;
  double m_zCS ;
  bool m_wCSSign ; // true = +, false = -
  double m_dplusDminusWeight ;
  int m_dalitzModel ; 
  bool m_useNewWeights ;
  
  double m_rwsCF ;
  double m_rwsCS ;
  double m_deltaCF ;
  double m_deltaCS ;
  
  double m_largestWeight ;

};

#endif

