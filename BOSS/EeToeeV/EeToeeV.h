//*****************************************************************************
//
// EeToeeV.cxx
//
// Algorithm runs e+e- ->e+e- rho0, rho0->pi+pi-  precess 
//
// July 2016-4-29 Rong-Gang Ping to create package for BES3
// The original fortran code is generated with FDC, consult Prof. Wang Jianxiong
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
class EeToeeV:public Algorithm 
{	
public:
 EeToeeV(const string& name, ISvcLocator* pSvcLocator);
		
 StatusCode initialize();
 StatusCode execute();
 StatusCode finalize();
 StatusCode getMaxEvent();

private:
// jobOption params
 int m_evtMax;
 double m_Ecms;
 std::string m_vect;
 int m_mctruth;
IBesRndmGenSvc* p_BesRndmGenSvc;

};

#endif
