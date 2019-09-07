//
//  Bes EMC Rec Shower
//
//  Created by Zhe Wang 2003, 10, 1
//
#ifndef REC_EMC_SHOWER_H
#define REC_EMC_SHOWER_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "EmcRecEventModel/RecEmcFraction.h"
#include "EmcRecEventModel/RecEmcCluster.h"
#include "EmcRecEventModel/RecEmcDataType.h"
#include "DstEvent/DstEmcShower.h"

using namespace std;

extern const CLID& CLID_RecEmcShower;

class RecEmcShower : public DstEmcShower
{
  public:

    virtual const CLID& clID()  const { return  RecEmcShower::classID();}
    static  const CLID& classID() { return CLID_RecEmcShower; }

    //Constructors and destructors
    RecEmcShower();
    ~RecEmcShower();

    //Copy and assignment
    //RecEmcShower(const RecEmcShower& aShower);
    //RecEmcShower& operator=(const RecEmcShower& aShower);
    RecEmcShower(const DstEmcShower& aShower);
    RecEmcShower& operator=(const DstEmcShower& aShower);

    bool operator < (const RecEmcShower& aShower) const {
      return energy() < aShower.energy();
    }

    bool operator > (const RecEmcShower& aShower) const {
      return energy() > aShower.energy();
    }

    //Other methods
    //Reset all data members to default value
    void Clear();

    //Whether this shower comes from Tof
    inline bool TofShower() const {return fTofShower;}
    inline void TofShower(bool tof) {fTofShower=tof;}

    //Access a Shower
    RecEmcID getShowerId() const { return fShowerId; }
    RecEmcID ShowerId(RecEmcID id);

    //Access a cluster
    RecEmcID getClusterId() const { return fClusterId; }
    void ClusterId(const RecEmcID id);
    
    RecEmcCluster* getCluster() const { return fCluster; }
    void Cluster(RecEmcCluster *pCluster);

    RecEmcIDVector getCellId3x3() const { return fCellId3x3; }
    void CellId3x3(RecEmcIDVector& id3x3);

    RecEmcIDVector getCellId5x5() const { return fCellId5x5; }
    void CellId5x5(RecEmcIDVector& id5x5);

    //Read fraction map of a cluster
    //Only const_iterator is supplied.
    RecEmcFractionMap::const_iterator Begin() const;
    RecEmcFractionMap::const_iterator End() const;
    RecEmcFractionMap::const_iterator Find(const RecEmcID& CellId) const;

    //Append and remove a Fraction
    void Insert(const RecEmcFraction& aFraction);
    void Erase(const RecEmcFraction& aFraction);

    //Dump out
    void Dump() const;

  public:   
    //physics attributes
    unsigned int getSize() const;

    RecEmcFractionMap getFractionMap() const;
    RecEmcFractionMap getFractionMap3x3() const;
    RecEmcFractionMap getFractionMap5x5() const;

    RecEmcEnergy getEAll() const { return fEAll; }
    RecEmcEnergy EAll(RecEmcEnergy e);

    RecEmcEnergy getELepton() const { return fELepton; }
    RecEmcEnergy ELepton(RecEmcEnergy e);

    RecEmcEnergy getETof2x1() const { return fETof2x1; }
    RecEmcEnergy ETof2x1(RecEmcEnergy e);

    RecEmcEnergy getETof2x3() const { return fETof2x3; }
    RecEmcEnergy ETof2x3(RecEmcEnergy e);

    // NearestSeed
    RecEmcID NearestSeed() const;
    RecEmcID NearestSeed(const RecEmcID& ns);

    // ThetaGap
    int ThetaGap() const;
    int ThetaGap(int ThetaGap);
    // PhiGap
    int PhiGap() const;
    int PhiGap(int PhiGap);

  protected:   
    //only from Tof
    bool fTofShower;
    //fShowerId: the max fraction's CellId.
    RecEmcID fShowerId;
    //fCluster: the cluster belongs to
    RecEmcID fClusterId;
    RecEmcCluster *fCluster;
    //3x3 Id vector
    RecEmcIDVector fCellId3x3;
    //5x5 Id vector
    RecEmcIDVector fCellId5x5;
    //fShowerFractionMap: Fraction members of a Shower.
    RecEmcFractionMap fFractionMap;

    //attribute
    //
    // To do: add other physics attributes.
    //        At the same time you should add corresponding statements
    //        in the following method.
    //        Clear(), copy & assignment, read & write interface
    //        Others is optional.
    // 

    // Some usable attribute of a shower
    // Energy with all crystals included
    RecEmcEnergy fEAll;
    // Energy of shower with certain crystals corrding to E3x3
    RecEmcEnergy fELepton;

    // Energy of TOF 2x1 scintillators
    RecEmcEnergy fETof2x1;
    // Energy of TOF 2x3 scintillators
    RecEmcEnergy fETof2x3;

    // Position
    //HepPoint3D fPosition;

    // Nearest neighbor seed in the same cluster
    // If it exists it will be an ID value, else it it an invalid value.
    RecEmcID fNearestSeed;
    //
    // Gap: gap between its seed and its nearest neighbor seed
    //      in the same cluster.
    // 1: **; 2: *o*; 3: *oo*; 4: *ooo*; 5: *oooo*;
    // 6: gap>=6;
    // 9: only one seed is in this cluster.
    int fThetaGap;
    int fPhiGap;

};

ostream& operator<<(ostream & os, const RecEmcShower& aShower); 

typedef map<RecEmcID,RecEmcShower,less<RecEmcID> > RecEmcShowerMap;
typedef vector<RecEmcShower> RecEmcShowerVec;
typedef ObjectVector<RecEmcShower> RecEmcShowerCol;

#endif  // REC_EMC_SHOWER_H
