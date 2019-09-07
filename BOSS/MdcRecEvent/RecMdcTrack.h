/*
 *
 *  this class models "Track" used in Mdc Reconstruction
 * 
 * ********************************************************/

#ifndef RECMDCTRACK_H
#define RECMDCTRACK_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "DstEvent/DstMdcTrack.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "RecMdcHit.h"

 extern const CLID& CLID_RecMdcTrack;  
 typedef SmartRefVector<RecMdcHit>  HitRefVec;
 class RecMdcTrack : public DstMdcTrack {
 public:
   virtual const CLID& clID() const;
   static const CLID& classID();

   //ctor and dector
   RecMdcTrack();
   RecMdcTrack(const RecMdcTrack&);
   RecMdcTrack(const DstMdcTrack&);
   RecMdcTrack& operator=(const RecMdcTrack&);
   RecMdcTrack& operator=(const DstMdcTrack&);
   ~RecMdcTrack();

   //extractors
   //const int    getTrkId()          const { return m_trackId;    }
   //const double getDr()             const { return m_helix[0];   }
   //const double getFi0()            const { return m_helix[1];   }
   //const double getCpa()            const { return m_helix[2];   }
   //const double getDz()             const { return m_helix[3];   }
   //const double getTanl()           const { return m_helix[4];   }
   const double getVX0()            const { return m_pivot[0];   }
   const double getVY0()            const { return m_pivot[1];   }
   const double getVZ0()            const { return m_pivot[2];   }
   //const double getX0()             const { return m_poca[0];    }
   //const double getY0()             const { return m_poca[1];    }
   //const double getZ0()             const { return m_poca[2];    }
   const int    getNhits()          const { return m_nhits;      }
   //const double getChisq()          const { return m_chi2;       }
   //const int    getNdf()            const { return m_ndof;       }
   const double getFiTerm()         const { return m_fiTerm;     }
   //const int    getNster()          const { return m_nster;      }
   //const int    getStat()           const { return m_stat;       }
   //const int    getCharge()         const { return m_charge;     }
   const HepPoint3D& getPivot()     const { return m_pivot;      }
   //const HepPoint3D& getPoca()      const { return m_poca3D;     }
   //const HepVector& getHelix()      const { return m_a;          }
   //const HepSymMatrix& getError()   const { return m_Ea;         }
   const HitRefVec getVecHits(void) const { return m_vechits;    }

   //modifiers
   void setVX0(double x0)                 { m_pivot[0] = x0;     }
   void setVY0(double y0)                 { m_pivot[1] = y0;     }
   void setVZ0(double z0)                 { m_pivot[2] = z0;     }
   void setFiTerm(double fiterm)          { m_fiTerm = fiterm;   }
   void setNhits(int nhits)               { m_nhits = nhits;     }
   void setPivot(const HepPoint3D& pivot) { m_pivot = pivot;     }
   void setVecHits(HitRefVec vechits)     { m_vechits = vechits; }
   
 private:
   HepPoint3D m_pivot;    // pivot of the track
   double m_fiTerm;       // phi angle value at track terminated point
   int m_nhits;           // number of  total hits contained
   HitRefVec  m_vechits;   // hit list of the track
   //vector<int> m_vecHits; // hit list of the track wensp 
};

typedef ObjectVector<RecMdcTrack> RecMdcTrackCol;

#endif
   
