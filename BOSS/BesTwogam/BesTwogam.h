//*****************************************************************************
//
// Algorithm runs gamma-gamma event generator Twogam 
// (A. Olchevski, S.Nova, T.Todorov, DELPHI note 90-35)
// and stores output to transient store
//
// Nov 2007 Original BES3 code by Dima Dedovich
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

class BesTwogam:public Algorithm 
{	
public:
 BesTwogam(const string& name, ISvcLocator* pSvcLocator);
		
 StatusCode initialize();
 StatusCode execute();
 StatusCode finalize();
private:
 int    m_numberEvent;
 int    m_seed;
 int    m_unw;
 float  m_cmEnergy;
 float  m_m2min;
 std::string m_fcard;
 std::vector<std::string> m_pypars;
// double xpar[100];
// int npar[100];        	
// jobOption params
// Initial Seed
// std::vector<int> m_initSeed;
};

#endif
