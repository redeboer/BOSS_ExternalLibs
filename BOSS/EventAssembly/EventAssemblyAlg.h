//
//File: RecEventAssemblyAlg.h
//Description: Assemble Rec Events.
//Author: Wang LiangLiang
//History: 2006/3/7 created by Wang Liangliang
//

// Change EventAssemblyAlg.h
// by hekl 2008.03.19

#ifndef Event_EventAssemblyAlg_H
#define Event_EventAssemblyAlg_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"


class EventAssemblyAlg:public Algorithm
{
 public:
  EventAssemblyAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  double thetaCut(double p,double nSigma,int parId);
  double phiCut(double p,double nSigma,int parId);
  
  bool myMsgFlag;
  bool myActiveCutFlag;
  
  double myEmcThetaCut;//Emc matching cut
  double myEmcPhiCut;//Emc matching cut
  
  double myEmcThetaNSigmaCut;
  double myEmcPhiNSigmaCut;
  int    myParticleId;
  int    m_Output; //switch of output RecAssemble.root
    
  NTuple::Tuple*  myNTuple1;
  NTuple::Item<double> myExted;
  NTuple::Item<double> myMatched;
  NTuple::Item<double> myEnergyMatched;
};

#endif

