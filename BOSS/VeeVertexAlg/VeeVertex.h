#ifndef Reconstruction_VeeVertex_H
#define Reconstruction_VeeVertex_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class VeeVertex : public Algorithm {

public:
  VeeVertex(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:
  StatusCode registerParent(MsgStream& log);
  StatusCode clearEvtRecVeeVertexCol(MsgStream& log);
  void registerEvtRecVeeVertexCol(MsgStream& log);
 
private:
  bool m_recKShort;
  bool m_recLambda;
  bool m_recGC;
  Algorithm* m_KShortReconstruction;
  Algorithm* m_LambdaReconstruction;
  Algorithm* m_gcReconstruction;
};
#endif


