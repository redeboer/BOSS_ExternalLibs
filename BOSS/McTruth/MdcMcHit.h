#ifndef MdcMcHit_H
#define MdcMcHit_H 
#include <algorithm>
#include "GaudiKernel/ContainedObject.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_MdcMcHit;

namespace Event {  // NameSpace

class MdcMcHit : virtual public ContainedObject { 
public:
   virtual const CLID& clID() const 
   {
	return MdcMcHit::classID();
   }
		   
   static const CLID& classID()
    {
	return CLID_MdcMcHit;
    } 
  // Constructor
  MdcMcHit(){};
  MdcMcHit(const Identifier& id, unsigned int trackIndex, double xPosition,
           double yPosition, double zPosition, double driftDistance,
           double depositEnergy, int posFlag = -1);

  // Get associated id
  Identifier identify() const;

  // Get the associated track id 
  unsigned int getTrackIndex() const;

  // Get the position x 
  double getPositionX() const;

  // Get the position y 
  double getPositionY() const;

  // Get the position z 
  double getPositionZ() const;
  
  // Get Drift Distance 
  double getDriftDistance() const;

  // Get deposited energy 
  double getDepositEnergy() const;

  // Get the position flag
  int getPositionFlag() const;

//Set
  void setIdentifier(unsigned int id) {m_id = id ;}
  void setTrackIndex(unsigned int trackIndex) {m_trackIndex = trackIndex;}
  void setPositionX(double positionX) {m_xPosition = positionX;}
  void setPositionY(double positionY) {m_yPosition = positionY;}
  void setPositionZ(double positionZ) {m_zPosition = positionZ;}
  void setDriftDistance(double driftDistance){m_driftDistance = driftDistance;}
  void setDepositEnergy(double depositEnergy) {m_depositEnergy = depositEnergy;}
  void setPositionFlag(int posFlag) { m_posFlag = posFlag; }
   
private:
  MdcMcHit(const MdcMcHit &);

  MdcMcHit &operator=(const MdcMcHit &);

  //  const Identifier m_id;
  Identifier m_id;
  
  unsigned int m_trackIndex;

  double m_xPosition;

  double m_yPosition;

  double m_zPosition;
 
  double m_driftDistance;
  
  double m_depositEnergy;

  int    m_posFlag;
};

typedef ObjectVector<MdcMcHit> MdcMcHitCol;

} // NameSpace Event

#endif 
