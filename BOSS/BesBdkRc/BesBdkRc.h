//*****************************************************************************
//
// 
// 
// 
//
// 
//
//*****************************************************************************

#ifndef GENERATORMODULESEVTDECAY_H
#define GENERATORMODULESEVTDECAY_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"

#include <string>

class IBesRndmGenSvc;

class BesBdkRc:public Algorithm 
{	
public:
 BesBdkRc(const std::string& name, ISvcLocator* pSvcLocator);
		
 StatusCode initialize();
 StatusCode execute();
 StatusCode finalize();

private:
 double toRad;//=pi/180.0;
 double toDeg;//=180.0/pi;
 float m_w2min;
 // float m_ebeam;
 float m_CMEnergy;
 float m_ewe;
 float m_kzero;
 float qmass[6];

 int m_maxNTry;
 int m_ifinal;
 int m_iseed;

 float m_tcmin;
 float m_pcmin; 
 long int m_numberEvent;

 IBesRndmGenSvc* p_BesRndmGenSvc; 
};

#endif
