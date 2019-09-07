//
//
//
//


#ifndef BesMucEfficiency_h
#define BesMucEfficiency_h 1

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "BesMucHit.hh"
#include "BesMucDigit.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

#include "MucCalibConstSvc/IMucCalibConstSvc.h"
#include "MucCalibConstSvc/MucCalibConstSvc.h"

#define part_Max (3)
//#define segOnPart[3] ({4,8,4})
//#define gapOnPart[3] ({8,9,8})
#define seg_Max (8)
#define gap_Max (9)
#define strip_Max (96)
#define pad_Max (100)

using namespace std;

class BesMucEfficiency
{
public:
  BesMucEfficiency();
  ~BesMucEfficiency();

  void Initialize(G4String filename);

  void CheckCalibSvc();
  void SetHit(BesMucHit* hit);
  void GetPosLengthWidth(G4VPhysicalVolume* pvStrip);

  G4int GetPad();
  G4double GetEfficiency();
  static BesMucEfficiency * Instance(void);

public:
  IMessageSvc* 		msgSvc;

private:
  BesMucHit*  m_pHit;
  G4double    m_Pos_Hit;
  G4double    m_Pos_Strip;  
  G4double    m_Length;
  G4double    m_Width;
  G4int       m_Strip;
  G4double    m_effi[part_Max][seg_Max][gap_Max][strip_Max][pad_Max];
  static BesMucEfficiency * fPointer;
  G4double    IsAddEffi;

  IMucCalibConstSvc*  m_ptrCalibSvc;   // pointer of calibration constants service
};

#endif
