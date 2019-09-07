//*****************************************************************************
//
// Generator/Babayaga/Babayaga.h
//
// Algorithm runs small angle  Bhabha event generator BABAYAGA
// and stores output to transient store
//
// Aug. 2007 Original BES3 code by Rong-Gang Ping
//
//*****************************************************************************

#ifndef GENERATORMODULESEVTDECAY_H
#define GENERATORMODULESEVTDECAY_H

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"

#include <vector>

using namespace std;

class IBesRndmGenSvc;
class Babayaga:public Algorithm 
{	
public:
 Babayaga(const string& name, ISvcLocator* pSvcLocator);
		
 StatusCode initialize();
 StatusCode execute();
 StatusCode finalize();
 StatusCode getMaxEvent();

private:
// jobOption params
 int m_Int,m_Ich,m_Iarun,m_Ires,m_on,m_evtMax,HN;
 int m_INTUPLE,m_PHCUT,m_CUTG; 
 double m_Ebeam,m_Thmin, m_Thmax,m_Emin,m_Zmax,m_Egmin,m_Thgmin,m_Thgmax; 
 double m_CutNgam,m_CutEgam;

IBesRndmGenSvc* p_BesRndmGenSvc;

// Initial Seed
// std::vector<int> m_initSeed;
};

#endif
