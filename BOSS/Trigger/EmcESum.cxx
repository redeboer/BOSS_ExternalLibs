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
#include "Trigger/EmcESum.h"
#include "Trigger/TrigPara.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Random/RandGauss.h"
#include <iostream>

#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"

using namespace std;
using namespace TrigConf;
using namespace CLHEP;

EmcESum::EmcESum()
{
  m_EmcTCFinder = EmcTCFinder::get_Emc();
}
EmcESum::~EmcESum()
{
}
void EmcESum::getESum()
{
  //reset values
  _TotE = 0;
  _RTotE = 0;
  _LTotE = 0;
  _BTotE = 0;
  _ETotE = 0;
  _LBTotE = 0;
  _RBTotE = 0;
  _WETotE = 0;
  _EETotE = 0;
/*
  double L1TC_GATE = double (m_pIBGT->getL1TC_GATE());
  
  for(int i=0;i<12;i++) BLK[i] = 0;
  for(int i = 0; i < 2; i++) { WEBLK[i] = 0; EEBLK[i] = 0; }
/////////////////////////////////////////////////////////////////////////////////////////////////
  //Get total energy ,energy of right part and left part divided by z=0.The energy threshold of each trigger cell is 30*MeV. 
  for(int i=0;i<TrigConf::TCTHETANO_B;i++)
    for(int j=0;j<TrigConf::TCPHINO_B;j++)
    {
      double BEnergy = m_EmcTCFinder->getBTC_ADC(i,j);
   
      //set each barrel trigger cell energy to service
      m_pIBGT->setBTCEnergy(i,j,BEnergy);
   
      int blkId = m_EmcTCFinder->getBLKId(i,j);
      //low energy threshold of trigger cell
      if(m_EmcTCFinder->getBTCEnergy(i,j) < L1TC_GATE) BEnergy = 0.;

      BLK[blkId] +=BEnergy;//Get each block energy for babar trigger 	
    }

  for(int i =0;i<TrigConf::TCTHETANO_E;i++)
    for(int j =0;j<TrigConf::TCPHINO_E;j++)
    {
      double EEnergy = m_EmcTCFinder->getEETC_ADC(i,j);
      //set each barrel trigger cell energy to service
      m_pIBGT->setEETCEnergy(i,j,EEnergy);
      //low energy threshold of trigger cell
      if(m_EmcTCFinder->getEETCEnergy(i,j) < L1TC_GATE) EEnergy = 0.;

      if(j >= 0 && j < TrigConf::TCPHINO_E/2) EEBLK[0] += EEnergy; // north energy block
      else EEBLK[1] += EEnergy; // south energy block

      double WEnergy = m_EmcTCFinder->getWETC_ADC(i,j);
      //set each barrel trigger cell energy to service
      m_pIBGT->setWETCEnergy(i,j,WEnergy);    
      //low energy threshold of trigger cell
      if(m_EmcTCFinder->getWETCEnergy(i,j) < L1TC_GATE) WEnergy = 0.;

      if(j >= 0 && j < TrigConf::TCPHINO_E/2) WEBLK[0] += WEnergy; // north energy block
      else WEBLK[1] += WEnergy;
    }

    std::vector<double> BlockE;
    BlockE.clear();

    double bmean[12]  = {8.02,10.1,12.3,7.43,14.8,13.0,12.5,13.2,10.9,12.3,14.7,15.7};
    double bsigma[12] = {0.88,0.52,0.9,0.72,0.7,0.82,0.64,0.78,0.72,0.76,0.54,0.64};
    double bratio[12] = {3.28,3.15,3.01,3.12,3.09,3.22,2.90,2.89,2.97,3.00,3.02,3.15};//run10584

    for(int i = 0; i < 12; i++) {
      //double block_E = (BLK[i]/bratio[i] - 0xa + RandGauss::shoot(bmean[i],bsigma[i]))*m_pIBGT->getEnergy_Ratio(); //mv convert to adc
      double block_E = (BLK[i] - 0xa); //mv convert to adc
      if(block_E < 0) block_E = 0;
      _TotE += block_E; //Total energy of emc including barrel and endcaps
      _BTotE += block_E;//Total energy of barrel emc

      if(i >= 6)
      {
        _LTotE += block_E;//Total energy of left part emc(left barrel and west endcap)
        _LBTotE += block_E;//Total energy of left part emc (only barrel)
      }
      else
      {
        _RTotE += block_E;//Total energy of right part emc(right barrel and east endcap)
        _RBTotE += block_E;//Total energy of right part emc (only barrel)
      }
      //BlockE.push_back(block_E);
      BlockE.push_back(BLK[i]);
    }

    double emean[2]  = {11.8,12.4};
    double esigma[2] = {0.77,0.43};
    double eratio[2] = {3.19,3.19};

    double wmean[2]  = {19.9,8.89};
    double wsigma[2] = {0.63,0.51};
    double wratio[2] = {3.19,3.19};

    for(int i = 0; i < 2; i++) {
      //double wblock_E = (WEBLK[i]/wratio[i] - 0xa + RandGauss::shoot(wmean[i],wsigma[i]))*m_pIBGT->getEnergy_Ratio();
      double wblock_E = (WEBLK[i] - 0xa);
      //double wblock_E = WEBLK[i]*0.51 - 14.4 - 0xa + RandGauss::shoot(wmean[i],wsigma[i]);
      double eblock_E = (EEBLK[i] - 0xa);
      //double eblock_E = (EEBLK[i]/eratio[i] - 0xa + RandGauss::shoot(emean[i],esigma[i]))*m_pIBGT->getEnergy_Ratio();
      //double eblock_E = EEBLK[i]*0.51 - 14.4 - 0xa + RandGauss::shoot(emean[i],esigma[i]);
      if(wblock_E < 0) wblock_E = 0;
      if(eblock_E < 0) eblock_E = 0;

      _TotE   += eblock_E;//Total energy of emc including barrel and endcaps
      _RTotE  += eblock_E;//Total energy of right part emc(right barrel and east endcap)
      _ETotE  += eblock_E;//Total energy of west and east endcaps
      _EETotE += eblock_E;//Total energy of east endcap
      //_EETotE += EEBLK[i];//Total energy of east endcap

      _TotE   += wblock_E;//Total energy of emc including barrel and endcaps
      _LTotE  += wblock_E;//Total energy of left part emc(left barrel and west endcap)
      _ETotE  += wblock_E;//Total energy of west and east endcaps
      _WETotE += wblock_E;//Total energy of west endcap
      //_WETotE += WEBLK[i];//Total energy of west endcap
    }
*/

  for(int i=0;i<12;i++) BLK[i] = 0;
  for(int i = 0; i < 2; i++) { WEBLK[i] = 0; EEBLK[i] = 0; }
  /////////////////////////////////////////////////////////////////////////////////////////////////
  //Get total energy ,energy of right part and left part divided by z=0.The energy threshold of each trigger cell is 30*MeV. 
  for(int i = 0; i < 12; i++) {
    BLK[i] = m_EmcTCFinder->getBLKE(i+2);
  }

  for(int i = 0; i < 2; i++) {
    EEBLK[i] = m_EmcTCFinder->getBLKE(i);
    WEBLK[i] = m_EmcTCFinder->getBLKE(i+14);
  }

  std::vector<double> BlockE;
  BlockE.clear();

  double bmean[12]  = {8.02,10.1,12.3,7.43,14.8,13.0,12.5,13.2,10.9,12.3,14.7,15.7};
  double bsigma[12] = {0.88,0.52,0.9,0.72,0.7,0.82,0.64,0.78,0.72,0.76,0.54,0.64};

  for(int i = 0; i < 12; i++) {
    double block_E = (BLK[i]*0.333 - 0xa + RandGauss::shoot(bmean[i],bsigma[i])); //mv convert to adc
    if(block_E < 0) block_E = 0;
    _TotE += block_E; //Total energy of emc including barrel and endcaps
    _BTotE += block_E;//Total energy of barrel emc

    if(i >= 6)
    {
      _LTotE += block_E;//Total energy of left part emc(left barrel and west endcap)
      _LBTotE += block_E;//Total energy of left part emc (only barrel)
    }
    else
    {
      _RTotE += block_E;//Total energy of right part emc(right barrel and east endcap)
      _RBTotE += block_E;//Total energy of right part emc (only barrel)
    }
    BlockE.push_back(block_E);
  }

  double emean[2]  = {11.8,12.4};
  double esigma[2] = {0.77,0.43};

  double wmean[2]  = {19.9,8.89};
  double wsigma[2] = {0.63,0.51};

  for(int i = 0; i < 2; i++) {
    double wblock_E = (WEBLK[i]*0.333 - 0xa + RandGauss::shoot(wmean[i],wsigma[i]));
    double eblock_E = (EEBLK[i]*0.333 - 0xa + RandGauss::shoot(emean[i],esigma[i]));
    if(wblock_E < 0) wblock_E = 0;
    if(eblock_E < 0) eblock_E = 0;

    _TotE   += eblock_E;//Total energy of emc including barrel and endcaps
    _RTotE  += eblock_E;//Total energy of right part emc(right barrel and east endcap)
    _ETotE  += eblock_E;//Total energy of west and east endcaps
    _EETotE += eblock_E;//Total energy of east endcap

    _TotE   += wblock_E;//Total energy of emc including barrel and endcaps
    _LTotE  += wblock_E;//Total energy of left part emc(left barrel and west endcap)
    _ETotE  += wblock_E;//Total energy of west and east endcaps
    _WETotE += wblock_E;//Total energy of west endcap
  }

  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("BesGlobalTrigSvc", m_tmpSvc);
  m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);

  //set energy information related to emc in service
  m_pIBGT->setEmcTotE(_TotE);
  m_pIBGT->setEmcLTotE(_LTotE);
  m_pIBGT->setEmcRTotE(_RTotE);
  m_pIBGT->setEmcBTotE(_BTotE);
  m_pIBGT->setEmcETotE(_ETotE); 
  m_pIBGT->setEmcLBTotE(_LBTotE);
  m_pIBGT->setEmcRBTotE(_RBTotE);
  m_pIBGT->setEmcWETotE(_WETotE);
  m_pIBGT->setEmcEETotE(_EETotE);
  m_pIBGT->setEmcBlockE(BlockE);
}
