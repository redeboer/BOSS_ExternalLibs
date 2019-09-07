#ifndef RootEventData_TMdcMc_H
#define RootEventData_TMdcMc_H 1

#include "TObject.h"
//#include <vector>
//using namespace std;

class TMdcMc : public TObject {

public:
    
   TMdcMc();
   ~TMdcMc();
//Get
   // Get associated id
   UInt_t getId() const {return m_id;}

  // Get the associated track id 
  UInt_t getTrackIndex() const {return m_trackIndex; }

  // Get the position x 
  Double_t getPositionX() const {return m_xPosition;}

  // Get the position y 
  Double_t getPositionY() const {return m_yPosition;}

  // Get the position z 
  Double_t getPositionZ() const {return m_zPosition;}

  // Get Drift Distance 
  Double_t getDriftDistance() const {return m_driftDistance;}
  
  // Get the total deposited energy 
  Double_t getDepositEnergy() const {return m_depositEnergy;}

    // Get the position flag
  Int_t getPositionFlag() const {return m_posFlag; }




//Set
  void setId(UInt_t id) {m_id = id ;}
  void setTrackIndex(UInt_t trackIndex){ m_trackIndex = trackIndex;}
  void setPositionX(Double_t positionX) {m_xPosition = positionX;}
  void setPositionY(Double_t positionY) {m_yPosition = positionY;}
  void setPositionZ(Double_t positionZ) {m_zPosition = positionZ;}
  void setDriftDistance(Double_t driftDistance){m_driftDistance = driftDistance;}
  void setDepositEnergy(Double_t depositEnergy) {m_depositEnergy = depositEnergy;}
  void setPositionFlag(Int_t posFlag) { m_posFlag = posFlag; }

private:

  UInt_t m_id; 

  UInt_t m_trackIndex;

  Double_t m_xPosition;

  Double_t m_yPosition;

  Double_t m_zPosition;

  Double_t m_driftDistance ;

  Double_t m_depositEnergy;

  Int_t m_posFlag;

    ClassDef(TMdcMc,1)
};


#endif //TrackRootData_TMdcMc_H

