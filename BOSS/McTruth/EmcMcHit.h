#ifndef EmcMcHit_H
#define EmcMcHit_H 
#include <algorithm>
#include "GaudiKernel/ContainedObject.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include <map>

using namespace EventModel;
extern const CLID& CLID_EmcMcHit;

namespace Event {  // NameSpace

class EmcMcHit : virtual public ContainedObject { 
public:
   virtual const CLID& clID() const 
   {
	return EmcMcHit::classID();
   }
		   
   static const CLID& classID()
    {
	return CLID_EmcMcHit;
    }  
  // Constructor
  EmcMcHit(){};
  EmcMcHit(const Identifier& id, unsigned int trackIndex, double xPosition,
      double yPosition, double zPosition,   double px, double py,
      double pz, double depositEnergy);

//Get
  // Get associated id
  Identifier identify() const;

  // Get the associated track id 
  unsigned int getTrackIndex() const;

  // Whether hit Emc
  int getHitEmc() const;

  // Get PDG code
  int getPDGCode() const;

  // Get PDG charge
  double getPDGCharge() const;
  
  // Get time
  double getTime() const;
  
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
  
  // Get the total deposited energy 
  double getDepositEnergy() const;

//Set
  void setIdentifier(Identifier& id) {m_id = id ;}
  void setHitEmc(int is) { m_hitEmc=is; }
  void setPDGCode(int code) { m_PDGCode=code; }
  void setPDGCharge(double charge) { m_PDGCharge = charge; }
  void setTime(double time) { m_time=time; }
  void setTrackIndex(unsigned int trackIndex){ m_trackIndex = trackIndex;}
  void setPositionX(double positionX) {m_xPosition = positionX;}
  void setPositionY(double positionY) {m_yPosition = positionY;}
  void setPositionZ(double positionZ) {m_zPosition = positionZ;}
  void setPx(double px) {m_px = px;}
  void setPy(double py) {m_py = py;}
  void setPz(double pz) {m_pz = pz;}
  void setDepositEnergy(double depositEnergy) {m_depositEnergy = depositEnergy;}
  
//About hit map
  std::map<Identifier,double>::const_iterator begin() const;
  std::map<Identifier,double>::const_iterator end() const;
  std::map<Identifier,double>::const_iterator find(Identifier&) const;

  unsigned int size() const;
  std::map<Identifier,double> getHitMap() const;
  void setHitMap(std::map<Identifier,double>& hitMap) { m_hitMap=hitMap; }

private:
  EmcMcHit(const EmcMcHit &);

  EmcMcHit &operator=(const EmcMcHit &);

  // const Identifier m_id;  //2005/12/29 wensp modified

  Identifier m_id; 

  unsigned int m_trackIndex;
  //whether hit Emc
  int m_hitEmc;
  //particle ID
  int m_PDGCode;
  //particle charge;
  double m_PDGCharge;
  //hit crystal time or dead time
  double m_time;
  
  double m_xPosition;

  double m_yPosition;

  double m_zPosition;

  double m_px;
  
  double m_py;

  double m_pz;

  double m_depositEnergy;

  std::map<Identifier,double> m_hitMap;
};

typedef ObjectVector<EmcMcHit> EmcMcHitCol;

} // NameSpace Event

#endif 
