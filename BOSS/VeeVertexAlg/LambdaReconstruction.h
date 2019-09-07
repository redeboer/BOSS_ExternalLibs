#ifndef Reconstruction_LambdaReconstruction_H
#define Reconstruction_LambdaReconstruction_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "EvtRecEvent/EvtRecVeeVertex.h"

class LambdaReconstruction : public Algorithm {

 public:
  LambdaReconstruction(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

 private:
   StatusCode registerEvtRecVeeVertexCol(
      EvtRecVeeVertexCol* veeVertexCol, MsgStream& log);

 private:
  // charge track selection
  double m_vzCut;
  double m_cosThetaCut;
  double m_ChisqCut;
  // particle ID Cut
  bool m_useDedx;
  bool m_useTof1;
  bool m_useTof2;
  bool m_useTofE;
  bool m_useTofQ;
  bool m_useEmc;
  double m_PidProbCut;
  // mass cut
  double m_massRangeLower;
  double m_massRangeHigher;
  // SecondVertex Cut
  double m_chisqCut;
};
#endif
