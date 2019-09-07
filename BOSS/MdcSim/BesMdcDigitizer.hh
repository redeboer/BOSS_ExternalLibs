//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: Oct. 26, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesMdcDigitizer_h
#define BesMdcDigitizer_h 1

class BesMdcDigitizerMessenger;
class TFile;
class TH1F;
class G4Svc;
#include "GaudiKernel/NTuple.h"

#include "G4VDigitizerModule.hh"
#include "BesMdcDigi.hh"
#include "BesMdcGeoParameter.hh"
#include "BesMdcCalTransfer.hh"
#include "MdcTunningSvc/IMdcTunningSvc.h"
#include "MdcTunningSvc/MdcTunningSvc.h"

#include "globals.hh"
#include <vector>

class BesMdcDigitizer : public G4VDigitizerModule
{
public:
  BesMdcDigitizer(G4String modName);
  ~BesMdcDigitizer();
  
  //necessary digi collection object must be constructed and set to 
  //G4DCofThisEvent by StoreDigiCollection protected method.
  virtual void Digitize();
  void SetNoiseFlag(G4int flag){noiseFlag=flag;}  
  void SetNoiseType(G4int type){noiseType=type;}
  void SetNoiseLevel(G4double level){noiseLevel=level;}  

  void SetSmearFlag(G4int flag){smearFlag=flag;}
  void SetMdcDRes(G4double res){mdcDRes=res;}

  void SetEffFlag(G4int flag){effFlag=flag;}
  void SetEff(G4int layer, G4double eff);


private:
  G4double Smear(G4double);
  G4double Smear(G4double, G4double,G4double);
  G4double Smear(G4double, G4double,G4double,G4double,G4double,G4double);
  G4double Smear(G4double, G4double,G4double,G4double,G4double,G4double,G4double,G4double,G4double);//liukai 2012-6-4
  void AddNoise(void);
  void AddNoise2(void);
private:
  G4int noiseFlag; //1: Add noise; 0: do nothing
  G4int noiseType; //Noise level distribution mode. 0: N(r)=N0; 1: N(r)=N0*r0/r; 2: N(r)=N0*r0^2/r^2
  G4double noiseLevel; // Noise level of first layer
  G4double maxNoiseT; //Max drift time of Mdc, maybe change to layer dependence

  G4int smearFlag; //0: do nothing; 1: smear(double gauss); 2: smear(fix sigma)  default is 1
  G4double mdcDRes; //Space resolution of wire, default is 130um by now

  G4int effFlag;//0: from CalFunSvc; 1: from file
  vector<G4double>   layerEff;
  vector<G4double>   mixLevel;

  G4int digiPointer[43][288];//pointer to digisCollection
  BesMdcDigisCollection* digisCollection;         
  BesMdcDigitizerMessenger* digitizerMessenger;
  BesMdcGeoParameter* mdcGeoPointer;
  BesMdcCalTransfer* mdcCalPointer;
  MdcTunningSvc* mdcTunningSvc;
  
  TFile* f;
  TH1F* h1;
  TH1F* h2;
  TH1F* h3; 
   
  G4Svc* m_G4Svc;
  NTuple::Tuple* m_tupleMdc;
  NTuple::Item<long> m_NHits;
  NTuple::Item<long> m_layerId;
  NTuple::Item<long> m_cellId;
  NTuple::Item<double> m_edep;
  NTuple::Item<double> m_driftD; 
 // NTuple::Item<double> m_driftT;
  NTuple::Item<double> m_globalT;
  NTuple::Item<double> m_theta;
  NTuple::Item<double> m_enterAngle;
  NTuple::Item<double> m_driftDNew;
  NTuple::Item<double> m_driftTNew;
 //   NTuple::Item<double> m_adc;
 //   NTuple::Item<double> m_tdc;
};

#endif

