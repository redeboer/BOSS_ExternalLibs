#ifndef COSMICGENERATOR_COSMICGUN_H
#define COSMICGENERATOR_COSMICGUN_H

#include "CLHEP/Vector/LorentzVector.h"
using namespace CLHEP;
class CosmicGun{
public:
  static CosmicGun* GetCosmicGun(void);
  HepLorentzVector GenerateEvent(void);
  HepLorentzVector GenerateVertex(void);
  void SetEnergyRange(float emin, float emax);
  void SetCosCut(float ctcut);
  void PrintLevel(int printevt,int printmod);
  int  GetMuonCharge(void);
  float InitializeGenerator();  // returns flux after all cuts in cm2/s
private:
  CosmicGun(void);
  static CosmicGun* mpointer;

long int   m_event;
long int   m_printevt, m_printmod;
  float m_emin, m_emax;
  float m_coscut;
};


#endif
