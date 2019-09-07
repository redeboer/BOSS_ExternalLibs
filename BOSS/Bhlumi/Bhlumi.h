//*****************************************************************************
//
// Generator/Bhlumi/Bhlumi.h
//
// Algorithm runs small angle  Bhabha event generator BHLUMI
// and stores output to transient store
//
// Jan 2006 Original BES3 code by Alexey Zhemchugov
//
//*****************************************************************************

#ifndef GENERATORMODULESEVTDECAY_H
#define GENERATORMODULESEVTDECAY_H

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"

#include <vector>

class IBesRndmGenSvc;

class Bhlumi:public Algorithm 
{	
public:
 Bhlumi(const std::string& name, ISvcLocator* pSvcLocator);
		
 StatusCode initialize();
 StatusCode execute();
 StatusCode finalize();

private:
 double xpar[100];
 int npar[100];        	

 IBesRndmGenSvc* p_BesRndmGenSvc; 

 // angle unit control
 int m_angleMode;
// jobOption params
 double m_cmEnergy, m_minThetaAngle, m_maxThetaAngle, m_infraredCut;
 // Initial Seed
 std::vector<int> m_initSeed;
};

#endif
