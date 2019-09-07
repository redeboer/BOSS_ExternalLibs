//************************************************************
//
//   KKMC generator to BES3
//  
//  Author: K.L. He
//  Date:  2006/04/21, created
//
//***********************************************************

#ifndef Generator_KKMC_H
#define Generator_KKMC_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "MeasuredEcmsSvc/IMeasuredEcmsSvc.h"//Wu Lianjin
#include <vector>

class IBesRndmGenSvc;
class KKMC: public Algorithm
{
  public:

  KKMC(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  IBesRndmGenSvc* p_BesRndmGenSvc;
	IMeasuredEcmsSvc* ecmsSvc;//Wu Lianjin
  int m_numberEvent;
  int m_numberEventPrint;
  int m_ihvp;

  std::vector<int>  m_kkseed;
  double m_cmsEnergy;
  double m_cmsEnergySpread;
  bool m_generateResonance;
  bool m_generateContinuum;
  bool m_generateDownQuark;
  bool m_generateUpQuark;
  bool m_generateStrangeQuark;
  bool m_generateCharmQuark;
  bool m_generateBottomQuark;
  bool m_generateMuonPair;
  bool m_generateTauPair;
  bool m_generateRho;
  bool m_generateOmega;
  bool m_generatePhi;
  bool m_generateJPsi;
  bool m_generatePsiPrime;
  bool m_generatePsi3770;
  double m_DdbarCutPsi3770;
  bool m_generatePsi4030;
  bool m_generatePsi4160;
  bool m_generatePsi4415;
  bool m_generatePsi4260;
  bool m_isrtag;
  bool m_fsrtag;
//  bool m_ReadEcmsFromDB;
  bool m_RdMeasuredEcms;
  int m_ich;

  std::vector<double> m_paramRho;
  std::vector<double> m_paramRh2;
  std::vector<double> m_paramRh3;
  std::vector<double> m_paramOme;
  std::vector<double> m_paramOm2;
  std::vector<double> m_paramOm3;
  std::vector<double> m_paramPhi;
  std::vector<double> m_paramPh2;
  std::vector<double> m_paramPsi;
  std::vector<double> m_paramPs2;
  std::vector<double> m_paramPs3;
  std::vector<double> m_paramPs4;
  std::vector<double> m_paramPs5;
  std::vector<double> m_paramPs6;
  std::vector<double> m_paramPs7;
  std::vector<double> m_paramPs8;
  std::vector<double> m_paramUps;
  std::vector<double> m_paramUp2;
  std::vector<double> m_paramUp3;
  std::vector<double> m_paramUp4;
  std::vector<double> m_paramUp5;
  std::vector<double> m_paramUp6;
  std::vector<double> m_paramZeta;
  std::vector<double> m_paramW;
  std::vector<std::string> m_pypars;
  std::vector<double> m_beam1PolVec,m_beam2PolVec;

  double m_ps3toNonDDb;
  double m_ps3D0toDp;

  double m_ps4toD0D0b;
  double m_ps4toDpDm;
  double m_ps4toDsDs;
  double m_ps4toD0D0Star;
  double m_ps4toDpDmStar;
  double m_ps4toD0StarD0Star;
  double m_ps4toDpStarDmStar;

  double m_ps5toD0D0b;
  double m_ps5toDpDm;
  double m_ps5toDsDs;
  double m_ps5toD0D0Star;
  double m_ps5toDpDmStar;
  double m_ps5toDsDsStar;
  double m_ps5toD0StarD0Star;
  double m_ps5toDpStarDmStar;

  bool m_evtGenDecay;
  bool m_radiationCorrection;

  static int m_runNo;
  double xwpar[10000];  

};


#endif
