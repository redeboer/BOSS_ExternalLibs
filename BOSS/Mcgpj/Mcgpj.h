//*****************************************************************************
//
// Generator/Mcgpj/Mcgpj.h
//
// General 2-body generator for e^+e^-, mu^+mu^-, pi^+pi^-, tau^+tau^-, 
// K_S K_L, K^+K^-, gamma gamma with precision better 0.2 %
//
// Mar 2009 Original BES3 code by Alexei Sibidanov
//
//*****************************************************************************

#ifndef GENERATORMODULEMCGPJ_H
#define GENERATORMODULEMCGPJ_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"

class IBesRndmGenSvc;

class Mcgpj:public Algorithm{	
public:
  Mcgpj(const std::string& name, ISvcLocator* pSvcLocator);
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  double cmE;
  double pc,de,nt0,dt,dp,at,td,am,cm,em,ti,al,thm,thp,te,re;
  double spread,phase;
  int proc,NRad,IsHardPhoton,IsNoVacPol,IsFSR,Seed;
  double fM;
  int fpid[2];
  int m_fmode5pi;
  std::string m_datapath;
  std::string m_vpolfname;

  IBesRndmGenSvc* p_BesRndmGenSvc; 

};

#endif
