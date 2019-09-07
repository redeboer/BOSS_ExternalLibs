#ifndef TofMcHit_H
#define TofMcHit_H 
#include <algorithm>
#include "GaudiKernel/ContainedObject.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_TofMcHit;

namespace Event {  // NameSpace
class TofMcHit : virtual public ContainedObject { 
public:
   virtual const CLID& clID() const 
   {
	return TofMcHit::classID();
   }
		   
   static const CLID& classID()
    {
	return CLID_TofMcHit;
    } 
  // Constructor
  TofMcHit(){};
  TofMcHit(const Identifier& id, unsigned int trackIndex, double xPosition,
           double yPosition, double zPosition,   double px, double py,
	   double pz, double trackLength, double flightTime);

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

  // Get momentum Px 
  double getPx() const;

  // Get momentum Py  
  double getPy() const;

  // Get momentum Pz  
  double getPz() const;
  
  // Get the trajectory length 
  double getTrackLength() const;

  // Get the flight time 
  double getFlightTime() const;
//Set
  void setIdentifier(unsigned int id) {m_id = id;}
  void setTrackIndex(unsigned int trackIndex){ m_trackIndex = trackIndex;}
  void setPositionX(double positionX) {m_xPosition = positionX;}
  void setPositionY(double positionY) {m_yPosition = positionY;}
  void setPositionZ(double positionZ) {m_zPosition = positionZ;}
  void setPx(double px) {m_px = px;}
  void setPy(double py) {m_py = py;}
  void setPz(double pz) {m_pz = pz;}
  void setTrackLength(double trackLength) {m_trackLength = trackLength; }
  void setFlightTime(double flightTime ) {m_flightTime = flightTime; }

   
private:
  TofMcHit(const TofMcHit &);

  TofMcHit &operator=(const TofMcHit &);

  // const Identifier m_id;  //2005/12/29 wensp modified

  Identifier m_id;
  
  unsigned int m_trackIndex;

  double m_xPosition;

  double m_yPosition;

  double m_zPosition;

  double m_px;
  
  double m_py;

  double m_pz;

  double m_trackLength;

  double m_flightTime; 
};

typedef ObjectVector<TofMcHit> TofMcHitCol;

} // NameSpace Event
#endif 
