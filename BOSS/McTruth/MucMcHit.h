#ifndef MucMcHit_H
#define MucMcHit_H 
#include <algorithm>
#include "GaudiKernel/ContainedObject.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_MucMcHit;

namespace Event {  // NameSpace

class MucMcHit : virtual public ContainedObject { 
public:
   virtual const CLID& clID() const 
   {
	return MucMcHit::classID();
   }
		   
   static const CLID& classID()
    {
	return CLID_MucMcHit;
    } 
  // Constructor
  MucMcHit(){};
  MucMcHit(const Identifier& id, unsigned int trackIndex, double xPosition,
           double yPosition, double zPosition,   double px, double py, double pz);

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

//Set
  void setIdentifier(unsigned id) {m_id = id;}
  void setTrackIndex(unsigned int trackIndex){ m_trackIndex = trackIndex;}
  void setPositionX(double positionX) {m_xPosition = positionX;}
  void setPositionY(double positionY) {m_yPosition = positionY;}
  void setPositionZ(double positionZ) {m_zPosition = positionZ;}
  void setPx(double px) {m_px = px;}
  void setPy(double py) {m_py = py;}
  void setPz(double pz) {m_pz = pz;}

    
private:
  MucMcHit(const MucMcHit &);

  MucMcHit &operator=(const MucMcHit &);

  // const Identifier m_id;  //2005/12/29 wensp modified

  Identifier m_id;
  
  unsigned int m_trackIndex;

  double m_xPosition;

  double m_yPosition;

  double m_zPosition;

  double m_px;
  
  double m_py;

  double m_pz;
};

typedef ObjectVector<MucMcHit> MucMcHitCol;

} // NameSpace Event
#endif 
