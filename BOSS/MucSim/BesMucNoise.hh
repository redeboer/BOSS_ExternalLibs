//
//
//
//


#ifndef BesMucNoise_h
#define BesMucNoise_h 1

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "BesMucHit.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4LogicalVolume.hh"

#include "TMath.h"

#include "MucCalibAlg/MucStructConst.h"
#include "MucCalibAlg/MucIdTransform.h"
#include "MucCalibAlg/MucMark.h"
#include "MucCalibConstSvc/IMucCalibConstSvc.h"
#include "MucCalibConstSvc/MucCalibConstSvc.h"


using namespace std;

class BesMucNoise
{
public:
  BesMucNoise();
  ~BesMucNoise();

  static BesMucNoise* Instance(void);  
  void Initialize(G4String filename,G4LogicalVolume* logicalMuc);
  void Initialize(G4String filename,G4LogicalVolume* logicalMuc,G4String temp);

  void CheckCalibSvc();	  
  G4int AddNoise( int model, BesMucHitsCollection* MucHitCollection,BesMucHitsCollection* MucHitList);
  G4int NoiseByCnt(BesMucHitsCollection* MucHitCollection,BesMucHitsCollection* MucHitList);
  G4int NoiseByNosRatio(BesMucHitsCollection* MucHitCollection,BesMucHitsCollection* MucHitList);
  G4int NoiseSampling(int level, int prt, int seg, int lay, int strip);
  G4int GetStripNo(G4int,G4int,G4int);
  G4int IsNearestStrip(G4int,G4int,G4int,G4int,G4float);
  bool IsExist(BesMucHit* aNoiseHit, BesMucHitsCollection* aMucHitList);  
  G4float Factorial(G4int i);
  void InitProb();

private:

  static const int m_kPart            =3;
  static const int m_kSegment[m_kPart]  ;
  static const int m_kAbsorber[m_kPart] ;
  static const int m_kGap[m_kPart]      ;
  static const int m_kPanel[m_kPart]    ;
  static const int m_kGasChamber      =2;
    
  G4double m_noise[3][8][9];
  G4double m_noise_strip[3][8][9][112];
  G4float area[3][8][9][112];
  G4float strip_area[3][8][9][112];
  G4float box_area[3][8][9];
  double m_HitMean;
  double m_Prob[20][2];
  static BesMucNoise * fPointer;

  int m_noiseLevel; //2: box;  3: strip 
  IMucCalibConstSvc*  m_ptrCalibSvc;   // pointer of calibration constants service
  MucIdTransform*     m_ptrIdTr;       // pointer of muc id transform 

};
#endif
