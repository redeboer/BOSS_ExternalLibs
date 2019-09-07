//*****************************************************************************
//
// Generator/Bhwide/Bhwide.h
//
// Algorithm runs large angle  Bhabha event generator BHWIDE
// and stores output to transient store
//
// Aug 2007 Original BES3 code by Alexey Zhemchugov
//
//*****************************************************************************

#ifndef GENERATORMODULESBHWIDE_H
#define GENERATORMODULESBHWIDE_H

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"

#include <vector>

class IBesRndmGenSvc;

class Bhwide:public Algorithm 
{	
public:
 Bhwide(const std::string& name, ISvcLocator* pSvcLocator);
		
 StatusCode initialize();
 StatusCode execute();
 StatusCode finalize();

private:
 double xpar[100];
 int npar[100];        	

// jobOption params
 double m_cmEnergy,  m_infraredCut, m_Acolli;
 double m_ThMine, m_ThMaxe, m_EnMine;
 double m_ThMinp, m_ThMaxp, m_EnMinp;
 int m_keyPia, m_keyMod, m_keyLib, m_keyEwc;
 
 IBesRndmGenSvc* p_BesRndmGenSvc; 
 
// Initial Seed
 std::vector<int> m_initSeed;
};

#endif
