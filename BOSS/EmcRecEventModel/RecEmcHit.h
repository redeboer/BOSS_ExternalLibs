//
//  Bes EMC Rec Hit
//
//  Created by Zhe Wang  2003, 10, 1
//
#ifndef REC_EMC_HIT_H
#define REC_EMC_HIT_H

#include <map>

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EmcRecEventModel/RecEmcDataType.h"

using namespace std;

extern const CLID& CLID_RecEmcHit;

class RecEmcHit : virtual public ContainedObject
{
 public:
   
    virtual const CLID& clID()  const { return  RecEmcHit::classID();}
    static  const CLID& classID() { return CLID_RecEmcHit; }

   //Constructors and destructors
   RecEmcHit();
   RecEmcHit(const RecEmcID& CellId,
	     const RecEmcEnergy& Energy,
	     const RecEmcTime& Time);
   ~RecEmcHit();

   //Copy and assignment
   //RecEmcHit(const RecEmcHit& aHit);
   //RecEmcHit& operator=(const RecEmcHit& aHit);

   bool operator < (const RecEmcHit& aHit) const {
     return fEnergy < aHit.fEnergy;
   }
   
   bool operator > (const RecEmcHit& aHit) const {
     return fEnergy > aHit.fEnergy;
   }

   //Other methods
   //Read
   RecEmcID getCellId() const { return fCellId; }
   RecEmcEnergy getEnergy() const { return fEnergy; }
   RecEmcTime getTime() const { return fTime; }
   HepPoint3D getCenter() const;
   HepPoint3D getFrontCenter() const;

   //Write
   RecEmcID CellId(const RecEmcID& CellId);
   RecEmcEnergy Energy(const RecEmcEnergy& Energy);
   RecEmcTime Time(const RecEmcTime& Time);

   //Detector input
   void Assign(const RecEmcID& CellId,
	       const RecEmcEnergy& Energy,
	       const RecEmcTime& Time);

   //Dump out
   void Dump() const;

 protected:
   //Data members
   RecEmcID fCellId;
   RecEmcEnergy fEnergy;
   RecEmcTime fTime;
};

ostream& operator<<(ostream & os, const RecEmcHit& aHit);

typedef map<RecEmcID,RecEmcHit,less<RecEmcID> > RecEmcHitMap;
typedef ObjectVector<RecEmcHit> RecEmcHitCol;

ostream& operator<<(ostream & os, const RecEmcHitMap& aHitMap); 

#endif	/* REC_EMC_HIT_H */
