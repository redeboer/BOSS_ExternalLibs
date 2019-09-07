//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     /
////
////---------------------------------------------------------------------------/
////
////Description:
////Author:   Caogf
////Created:  Feb, 2006
////Modified:
////Comment:
////
//
#include "Trigger/EmcTCFinder.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "CLHEP/Random/RanecuEngine.h"
#include "CLHEP/Random/RandGauss.h"

#include "Identifier/Identifier.h"
#include "Identifier/EmcID.h"
#include "RawEvent/RawDataUtil.h"
#include "RawEvent/DigiEvent.h"
#include "EmcCalibConstSvc/EmcCalibConstSvc.h"
#include <math.h>
using namespace std;
using namespace CLHEP;

EmcTCFinder* EmcTCFinder::emc_Pointer = 0;

EmcTCFinder* EmcTCFinder::get_Emc(void) {
  if(!emc_Pointer) emc_Pointer = new EmcTCFinder();
  return emc_Pointer;
}

EmcTCFinder::EmcTCFinder()
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IRealizationSvc *tmpReal;
  StatusCode status = svcLocator->service("RealizationSvc",tmpReal);
  if (!status.isSuccess())
  {
     cout << "FATAL:  Could not initialize Realization Service" << endl;
  } else {
    m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
  }

  // Get EmcCalibConstSvc.
  status = svcLocator->service("EmcCalibConstSvc", emcCalibConstSvc);
  if(status != StatusCode::SUCCESS) {
    cout << "EmcRecDigit2Hit Error: Can't get EmcCalibConstSvc." << endl;
  }
}
EmcTCFinder::~EmcTCFinder()
{
}
void EmcTCFinder::setEmcDigi(EmcDigiCol* emcDigiCol)
{
  //double energy;
  double tot_adc = 0.;
  double adc = 0., adc1 = 0., tdc = 0.;
  unsigned int measure;
  //reset tc energy of barrel and endcap        
  for(int i=0;i<TrigConf::TCTHETANO_B;i++)
    for(int j=0;j<TrigConf::TCPHINO_B;j++) {
      BTCEnergy[i][j] = 0;
      BTCEnergy_adc[i][j] = 0;
  }
  for(int i=0;i<TrigConf::TCTHETANO_E;i++)
    for(int j=0;j<TrigConf::TCPHINO_E;j++)
    {
      EETCEnergy[i][j] = 0;
      WETCEnergy[i][j] = 0;
      EETCEnergy_adc[i][j] = 0;
      WETCEnergy_adc[i][j] = 0;
    }
  EmcDigiCol::iterator iter3;
  Identifier id;
  for (iter3=emcDigiCol->begin();iter3!= emcDigiCol->end();iter3++) {
    id=(*iter3)->identify();
     
    unsigned int module;
    unsigned int theta;
    unsigned int phi;
    module = EmcID::barrel_ec(id);
    theta = EmcID::theta_module(id);
    phi = EmcID::phi_module(id);
    adc = double ((*iter3)->getChargeChannel());
    adc1 = double ((*iter3)->getChargeChannel());
    measure = (*iter3)->getMeasure();
    tdc = (*iter3)->getTimeChannel();
    
    int index = emcCalibConstSvc->getIndex(module,theta,phi); 
    //divided by electronics calibration constants and times trigger gain
    //double trgGain = ((50./255.)*(m_RealizationSvc->getTrgGain(index))/60.)*5*0.5;
    double trgGain = m_RealizationSvc->getTrgGain(index);
    std::cout <<"partId, thetaId, phiId, trgGain: " << module << ", " << theta << ", " << phi << ", " << trgGain << std::endl;
    //if((*iter3)->getMeasure()==0) adc = adc*(m_RealizationSvc->getEleCalib(index,7))*(trgGain)*800.*2/65535;
    //else if((*iter3)->getMeasure()==1) adc = adc*(m_RealizationSvc->getEleCalib(index,4))*(trgGain)*800.*2/65535;
    //else adc = adc*(m_RealizationSvc->getEleCalib(index,1))*(trgGain)*800.*2/65535;
    if((*iter3)->getMeasure()==0) adc = adc*2*800.*2/65535.*(trgGain);
    else if((*iter3)->getMeasure()==1) adc = adc*16*800.*2/65535*(trgGain);
    else adc = adc*64*800.*2/65535*(trgGain);

    //double trgConst = m_RealizationSvc->getTrgConst(index);
    //if((*iter3)->getMeasure()==0) adc1 = adc1*2*800.*2/65535.*(trgConst)*0.333;
    //else if((*iter3)->getMeasure()==1) adc1 = adc1*16*800.*2/65535*(trgConst)*0.333;
    //else adc1 = adc1*64*800.*2/65535*(trgConst)*0.333;

    //std::cout << "eCalib: " << m_RealizationSvc->getEleCalib(index,7) << "," << m_RealizationSvc->getEleCalib(index,4) << "," << m_RealizationSvc->getEleCalib(index,1) << " TrigGain: " << m_RealizationSvc->getTrgGain(index) << std::endl;
    //std::cout << " TrgGain: " << trgGain << " adc: " << adc << std::endl; 
    //adc = RawDataUtil::EmcCharge(measure,adc); //MeV
    //energy = RandGauss::shoot(energy,0.1*energy); //add 10% smear in each ADC channel
    
    int TCThetaId = getTCThetaId(module,theta,phi);
    int TCPhiId = getTCPhiId(module,theta,phi);
    //Get energy of each trigger cell 
    if(module==1) BTCEnergy[TCThetaId][TCPhiId]  += adc;
    if(module==0) EETCEnergy[TCThetaId][TCPhiId] += adc;
    if(module==2) WETCEnergy[TCThetaId][TCPhiId] += adc;
    if(module==1) BTCEnergy_adc[TCThetaId][TCPhiId]  += adc;
    if(module==0) EETCEnergy_adc[TCThetaId][TCPhiId] += adc;
    if(module==2) WETCEnergy_adc[TCThetaId][TCPhiId] += adc;
  }
 
  //subtract ped
/*
  for(int i=0;i<TrigConf::TCTHETANO_B;i++)
    for(int j=0;j<TrigConf::TCPHINO_B;j++) {
      if((BTCEnergy[i][j] - 0x0a) < 0) BTCEnergy[i][j] = 0;
      else BTCEnergy[i][j] = BTCEnergy[i][j] - 0x0a;
  }
  for(int i=0;i<TrigConf::TCTHETANO_E;i++)
    for(int j=0;j<TrigConf::TCPHINO_E;j++)
    {
      if((EETCEnergy[i][j] - 0x0a) < 0) EETCEnergy[i][j] = 0;
      else EETCEnergy[i][j] = EETCEnergy[i][j] - 0x0a;
      if((WETCEnergy[i][j] - 0x0a) < 0) WETCEnergy[i][j] = 0;
      else WETCEnergy[i][j] = WETCEnergy[i][j] - 0x0a;
    }*/
}

void EmcTCFinder::setEmcTC(std::vector<uint32_t> vTC) {
  //reset tc energy of barrel and endcap        
  for(int i=0;i<TrigConf::TCTHETANO_B;i++)
    for(int j=0;j<TrigConf::TCPHINO_B;j++) {
      BTC[i][j] = 0;
  }
  for(int i=0;i<TrigConf::TCTHETANO_E;i++)
    for(int j=0;j<TrigConf::TCPHINO_E;j++)
    {
      EETC[i][j] = 0;
      WETC[i][j] = 0;
    }
  for(std::vector<uint32_t>::iterator iter = vTC.begin(); iter != vTC.end(); iter++) {
    int par_TC = (*iter & 0xFF0000) >> 16;
    int the_TC = (*iter & 0xFF00) >> 8;
    int phi_TC = (*iter & 0xFF);
    if(par_TC == 0) EETC[the_TC][phi_TC] = 1; 
    if(par_TC == 1) BTC[the_TC][phi_TC] = 1; 
    if(par_TC == 2) WETC[the_TC][phi_TC] = 1; 
  }
}

void EmcTCFinder::setEmcBE(std::vector<double> vBE) {
  //reset block energy
  for(int i = 0; i < 16; i++) {
    BlkE[i] = 0;
  }
  if(vBE.size() != 0) {
  if(vBE.size() != 16 ) std::cerr << "The number of block is not equal 16, please check it (in EmcTCFinder::setEmcBE() )" << std::endl;
    for(int i = 0; i < vBE.size(); i++) {
      BlkE[i] = vBE[i];
    } 
  }
} 
	
int EmcTCFinder::getTCThetaId(int partId,int ThetaNb,int PhiNb)
{
	//Note: There are 44*120(theta*phi) crystals and they are divided 11*30 trigger cell in 
	//barrel emc. So each trigger cell includes 4*4 crystals  
	if(partId==1)
	  {	
	    //if(ThetaNb<2)
	      //TCThetaNb = 0;
	    //else if(ThetaNb>41)
	     // TCThetaNb = 11;
            //else	    
	      //TCThetaNb = int((ThetaNb+2)/4);
	    TCThetaNb = (int)ThetaNb/4;
	  }
/*	if(partId==0)
	{
		if(ThetaNb<3) TCThetaNb = 0;
		if(ThetaNb>3) TCThetaNb = 1;
		if(ThetaNb==3) 
		{
			int n = PhiNb%10;
			if(n>2&&n<7)
				TCThetaNb = 0;
			else TCThetaNb = 1;
		}
	}
	if(partId==2)
	{
		if(ThetaNb<3) TCThetaNb = 0;
		if(ThetaNb>3) TCThetaNb = 1;
		if(ThetaNb==3)
		{
			int n = PhiNb%10;
			if(n>2&&n<7)
				TCThetaNb = 0;
			else TCThetaNb = 1;
		}
	}
*/
        if(partId==0) TCThetaNb = 0;
        if(partId==2) TCThetaNb = 0;
	return TCThetaNb;
}
int EmcTCFinder::getTCPhiId(int partId,int ThetaNb,int PhiNb)
{
	if(partId==1)
		TCPhiNb = int(PhiNb/4);
/*
        //trigger cell in endcaps theta*phi = 2*16
	if(partId==0)
	{
		if(ThetaNb<2) TCPhiNb = int(PhiNb/4);
		if(ThetaNb>=2&&ThetaNb<4) TCPhiNb = int(PhiNb/5);
		if(ThetaNb>=4) TCPhiNb = int(PhiNb/6);
	}
	if(partId==2)
	{
		if(ThetaNb<2) TCPhiNb = int(PhiNb/4);
		if(ThetaNb>=2&&ThetaNb<4) TCPhiNb = int(PhiNb/5);
		if(ThetaNb>=4) TCPhiNb = int(PhiNb/6);
	}
*/
      // trigger cell in endcaps theta*phi = 1*32
      if(partId==0)
        {
                if(ThetaNb<2) TCPhiNb = int(PhiNb/2);
                if(ThetaNb==2) 
                { 
                  int quot = int(PhiNb/5);
                  int rema = PhiNb%5;
                  if(rema <= 1)  TCPhiNb = 2*quot;
                  if(rema > 1)  TCPhiNb = 2*quot + 1;
                } 
                if(ThetaNb==3)
                {
                  int quot = int(PhiNb/5);
                  int rema = PhiNb%5;
                  if(rema <= 2) TCPhiNb = 2*quot;
                  if(rema > 2)  TCPhiNb = 2*quot + 1;
                }
                if(ThetaNb>=4) TCPhiNb = int(PhiNb/3);
        }
        if(partId==2)
        {
                if(ThetaNb<2) TCPhiNb = int(PhiNb/2);
                if(ThetaNb==2)
                {
                  int quot = int(PhiNb/5);
                  int rema = PhiNb%5;
                  if(rema <= 1)  TCPhiNb = 2*quot;
                  if(rema > 1)  TCPhiNb = 2*quot + 1;
                }
                if(ThetaNb==3)
                {
                  int quot = int(PhiNb/5);
                  int rema = PhiNb%5;
                  if(rema <= 2) TCPhiNb = 2*quot;
                  if(rema > 2)  TCPhiNb = 2*quot + 1;
                }
                if(ThetaNb>=4) TCPhiNb = int(PhiNb/3);
        }

	return TCPhiNb;
}
int EmcTCFinder::getBLKId(int TCTheta,int TCPhi) const
{
  int id,parity;
  parity = (int)TCPhi/5;
  if(parity%2==0)
  {
    if(TCTheta<6) id = parity;
    if(TCTheta>=6) id = parity+6;
  }
  if(parity%2!=0)
  {
    if(TCTheta<5) id = parity;
    if(TCTheta>=5) id = parity+6;
  }

  return id;
}
