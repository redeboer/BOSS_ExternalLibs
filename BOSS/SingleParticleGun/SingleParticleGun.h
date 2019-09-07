// --------------------------------------------------
// 
// File:  SingleParticleGun/SingleParticleGun.h
// Description:
//    This code is used to generate a single particle or a poissonly
//    distributed set of particles with pt, theta and phi selected as
//    specified by the JobOptionsService.  Three modes of generation are
//    possible:
//           FixedMode:  Generation with a fixed value
//           GaussianMode: Generation with a gaussian of specified mean and
//                         sigma
//           FlatMode: Generation of a flat distribution between a specified
//                     minimum and maximum value
//    The mode can be separately selected for pt, theta and phi.
// or for  E, theta and phi
// in PtMode user specifies Pt theta and phi
// in EMode user specifies E, theta and phi 
//    The particle species can also be specified
//
//    The output will be stored in the transient event store so it can be
//    passed to the simulation.


#ifndef GENERATORMODULESSINGLEPARTICLEGUN_H
#define GENERATORMODULESSINGLEPARTICLEGUN_H

#include "GeneratorModule/GenModule.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Vector/LorentzVector.h"

class IBesRndmGenSvc;

struct SingleParticleGunGenMode {
  enum {FixedMode = 1, GaussMode = 2,FlatMode  = 3};
};
struct SingleEnergyMode {
  enum {EMode = 1, PtMode = 2};
};

class SingleParticleGun:public GenModule {
public:
  SingleParticleGun(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SingleParticleGun();
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(GenEvent* evt);

private:
  // Setable Properties:-
  double m_requestedPt;
  double m_requestedPhi;
  double m_requestedE;
  double m_requestedTheta;
  double m_requestedX;
  double m_requestedY;
  double m_requestedZ;
  double m_requestedT;
  double m_minPt;
  double m_minE;
  double m_minTheta;
  double m_minPhi;
  double m_maxE;
  double m_maxPt;
  double m_maxTheta;
  double m_maxPhi;
  double m_sigmaPt;
  double m_sigmaE;
  double m_sigmaTheta;
  double m_sigmaPhi;
  int m_PtGenMode;
  int m_EGenMode;
  int m_ThetaGenMode;
  int m_PhiGenMode;
  int m_pdgCode;
  int m_Emode;
  // event counter
  int m_events ;

  // Local Member Data:-
  IBesRndmGenSvc* p_BesRndmGenSvc;
  std::vector<long int> m_seeds;

  double m_mass;
  CLHEP::HepLorentzVector m_fourMom;  
  CLHEP::HepLorentzVector m_fourPos;  
      

  // Private Methods:=
  double generateValue(int mode,double val,double sigma,double min,
		       double max);

};

#endif
