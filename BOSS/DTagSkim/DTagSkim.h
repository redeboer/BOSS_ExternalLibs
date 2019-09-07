#ifndef DTagSkim_H
#define DTagSkim_H 
#include <cstring>

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecDTag.h"

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogramFactory.h"
using AIDA::IHistogram1D;


class DTagSkim : public Algorithm {

public:
  DTagSkim(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(); 

  
private:

  bool m_ifwriterec;
  bool m_ifwritedst;
  Algorithm* m_subalg1; //write dst
  Algorithm* m_subalg2; //write rec 

};

#endif 
