#ifndef DSTMDCTRACK_H
#define DSTMDCTRACK_H
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "EventModel/EventModel.h"
#include <vector>
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using namespace CLHEP;

using namespace std;
using namespace EventModel;
extern const  CLID &CLID_DstMdcTrack;


class DstMdcTrack  : virtual public ContainedObject 
{
 public: 
  DstMdcTrack();
  DstMdcTrack(const DstMdcTrack&);
  DstMdcTrack& operator=(const DstMdcTrack&);
  virtual ~DstMdcTrack() {}
  virtual const CLID& clID()  const { return  DstMdcTrack::classID();}
  static  const CLID& classID()     { return CLID_DstMdcTrack; }
  
  /// ......

  const HepVector helix()  const;
  const HepSymMatrix err() const;
  const HepLorentzVector p4(const double mass) const;
 
  const HepPoint3D poca() const{     
    return m_poca3D;
  }
  const Hep3Vector p3()  const{
    return Hep3Vector(m_px,m_py,m_pz);
  }
  const HepPoint3D x3()  const{
    return HepPoint3D(m_x,m_y,m_z);
  } 
  
  const double  helix(const int i) const { return  m_helix[i]; } 
  const double  err(const int i)   const { return  m_err[i];   }
  const int     trackId()    const { return  m_trackId;  }
  const int     charge()     const { return  m_charge;   }
  const double  pxy()        const { return  m_pxy;  }
  const double  px()         const { return  m_px;   }
  const double  py()         const { return  m_py;   }
  const double  pz()         const { return  m_pz;   }
  const double  p()          const { return  m_p;    }
  const double  theta()      const { return  m_theta;}
  const double  phi()        const { return  m_phi;  }
  const double  x()          const { return  m_x;    }
  const double  y()          const { return  m_y;    }
  const double  z()          const { return  m_z;    }
  const double  r()          const { return  m_r;    }
  const int     stat()       const { return  m_stat; }
  const double  chi2()       const { return  m_chi2; }
  const int     ndof()       const { return  m_ndof; }
  const int     nster()      const { return  m_nster;}
  const int     firstLayer() const { return  m_firstLayer;}
  const int     lastLayer()  const { return  m_lastLayer; }
  const int     nlayer()     const { return  m_nlayer; }
  

  // set functions
  void  setHelix(double helix[5]);
  void  setPoca(double poca[3]);
  void  setError( double err[15]);
  void  setHelix(const HepVector& a);
  void  setPoca(const HepPoint3D& poca);
  void  setError(const HepSymMatrix& Ea);
  
  void     setTrackId(const int trackId )      { m_trackId = trackId; }
  void     setCharge(const int  charge )       { m_charge = charge;} 
  void     setPxy(const double pxy)            { m_pxy  = pxy;    }     
  void     setPx (const double px)             { m_px   = px;     }
  void     setPy(const double py)              { m_py   = py;     }
  void     setPz(const double pz)              { m_pz   = pz;     }
  void     setP(const double p )               { m_p    = p;      }
  void     setTheta(const double theta)        { m_theta = theta; }
  void     setPhi(const double phi )           { m_phi  = phi;    }
  void     setX(const double x)                { m_x    = x;      }
  void     setY(const double y)                { m_y    = y;      }
  void     setZ(const double z)                { m_z = z; }
  void     setR(const double r)                { m_r = r; }
  void     setStat(const int stat)     { m_stat = stat;}
  void     setChi2(const double chi)   { m_chi2 = chi;}
  void     setNdof(const int ndof)     { m_ndof = ndof;}
  void     setNster(const int ns)      { m_nster = ns;}      
  void     setFirstLayer(const int id) { m_firstLayer = id;}
  void     setLastLayer(const int id)  { m_lastLayer = id;}
  void     setNlayer(const int nlayer) { m_nlayer = nlayer;}

  
 protected: 
  int    m_trackId;    // Track ID wensp Add 2005-10-19        
  int    m_charge;     // charge of the track
  double m_helix[5];   // 5 track parameters 
  double m_err[15];    // Error  Matrix
  double m_poca[3];    // position of closest approach to origin 
  HepPoint3D m_poca3D;
  HepVector  m_a;
  HepSymMatrix m_Ea;
  int    m_stat;       // Track Fit Quality status flag
  double m_chi2;       // chisq of track fit
  int    m_ndof;       // degree of freedom    
  int    m_nster;      // number of  stereo hits contained
  int    m_firstLayer; // layer id of first hit in PR hits collection   
  int    m_lastLayer;  // layer id of last hit in PR hits collection
  int    m_nlayer;     // number of layer that track passed  
  double m_pxy;        // Track Momentum(GeV)
  double m_px;
  double m_py;
  double m_pz;
  double m_p;
  double m_theta;      // The Angles
  double m_phi;        // Need to config
  double m_x;          // The Coordinate of Track origin(m)
  double m_y;      
  double m_z;          
  double m_r;
};

typedef ObjectVector<DstMdcTrack> DstMdcTrackCol;

#endif //DST_MDCTRACK_H

