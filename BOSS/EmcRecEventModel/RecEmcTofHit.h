//
// Bes Tof Rec Hit
//
// Created by He Miao 2006,9,15
//
#ifndef REC_EMC_TOF_HIT_H
#define REC_EMC_TOF_HIT_H

#include "Identifier/Identifier.h"
#include <map>
#include <CLHEP/Geometry/Point3D.h>
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace std;
using namespace CLHEP;

class RecEmcTofHit
{
  public:
    //Constructors and destructors
    RecEmcTofHit();
    RecEmcTofHit(const Identifier& id, 
                 const double e,
                 const HepPoint3D pos);
    ~RecEmcTofHit();

    //Copy and assignment
    RecEmcTofHit(const RecEmcTofHit& tofHit);
    RecEmcTofHit& operator=(const RecEmcTofHit& tofHit);
    
    bool operator < (const RecEmcTofHit& tofHit) const {
      return m_energy < tofHit.m_energy;
    }

    bool operator > (const RecEmcTofHit& tofHit) const {
      return m_energy > tofHit.m_energy;
    }

    //Read
    int IntCellId() const;
    inline Identifier CellId() const { return m_cellId; }
    inline double Energy() const { return m_energy; }
    inline HepPoint3D Position() const { return m_pos; }
    inline double ZPosition() const { return m_pos.z(); }

    //Write
    void CellId(const Identifier& id);
    void Energy(const double e);
    void Position(const HepPoint3D pos);
    void ZPosition(const double z);

    void Assign(const Identifier& id,
        const double e,
        const HepPoint3D pos);
    void Clear();

    double DistanceTo(const RecEmcTofHit &);

  private:
    Identifier m_cellId;
    double     m_energy;
    HepPoint3D m_pos;
};

typedef map<int, RecEmcTofHit, less<int> > RecEmcTofHitMap;

ostream& operator<<(ostream & os, const RecEmcTofHit& tofHit);

#endif
