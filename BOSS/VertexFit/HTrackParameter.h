#ifndef HTrack_Parameter_H
#define HTrack_Parameter_H
//
//
// Track Parameters in Helix format (drho, phi0, kappa, dz, lambda)
// The helix parameters are used in track fit
// Author: K.L He   date: 04/12/2007, created for Kalman vertex fit
//
//

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
using CLHEP::HepVector;
using CLHEP::HepLorentzVector;
using CLHEP::Hep3Vector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif

class WTrackParameter;

class HTrackParameter {

 public:
     HTrackParameter();
     ~HTrackParameter() {;}
     HTrackParameter(const HTrackParameter &htrk);
     HTrackParameter & operator = (const HTrackParameter &htrk);
     HTrackParameter(const HepVector helix, const HepSymMatrix eMatrix, const int trackid, const int partid);
     HTrackParameter(const HepVector helix, const double error[], const int trackid, const int partid);

     //
     // HTrackParameter from WTrackParameter
     //
     HTrackParameter(const WTrackParameter wtrk);
     //
     // measurement equation and derivative matrices
     //
     HTrackParameter(const int charge, const HepVector p, const HepVector x);
     HepMatrix dHdx(const HepVector p, const HepVector x);
     HepMatrix dHdp(const HepVector p, const HepVector x);

     HepVector hel() const {return m_hel;}
     HepVector helix() const {return m_hel;}
     HepSymMatrix eHel() const {return m_eHel;}
     int trackID() const {return m_trackID;}
     int partID() const {return m_partID;}

     // charge
     inline int charge() const;
     // 3-momentum and 3-position
     inline double pxy() const;
     inline HepVector p() const;
     inline HepVector x() const;
     inline Hep3Vector p3() const;
     inline HepPoint3D x3() const;
     inline HepLorentzVector  p(const double mass) const;
     HepPoint3D center() const;
     double radius() const;
     // helix parameter
     double drho() const {return m_hel[0];}
     double phi0() const {return m_hel[1];}
     double kappa() const {return m_hel[2];}
     double dz() const {return m_hel[3];}
     double lambda() const {return m_hel[4];}


     // WTrackParameter
     WTrackParameter wTrack() const;
     WTrackParameter wTrack(const double mass) const;

     // set partID, trackID
     void setTrackID(const int trackID) {m_trackID = trackID;}
     void setPartID(const int partID) { m_partID = partID;}
     // set helix parameter and covariance matrix
     void setHel(const HepVector he) {m_hel = he;}
     void setEHel(const HepSymMatrix eH) {m_eHel = eH;}
     //
     // some utility function  
     //
     // mass 
     double xmass(const int i) const;
     //
     // int section position with helix, plane, cylinder, cone, etc
     //
     HepPoint3D positionTwoHelix(const HTrackParameter) const;    // intersection position in x-y plane
     HepPoint3D positionPlane(const double) const;
     HepPoint3D positionCylinder(const double) const;
     HepPoint3D positionCone() const;
     //
     //  Mininum distance between helix and helix, helix and line
     //
     double minDistanceTwoHelix(const HTrackParameter G, HepPoint3D &pos);
 private:
     int m_trackID;      // for vertex reconstruction
     int m_partID;       // for Kalman track, 0:e, 1:mu, 2:pi, 3:K, 4:p
     HepVector m_hel;   // 5 helix parameter
     HepSymMatrix m_eHel; // 5x5 error matrix
};

inline int HTrackParameter::charge() const {
  return (m_hel[2]>0 ? +1 :-1);
}

inline double HTrackParameter::pxy() const {
  return fabs(1/m_hel[2]);
}

inline HepVector HTrackParameter::p() const {
  HepVector p0(3, 0);
  double pxy = 1./fabs(m_hel[2]);
  p0[0] = 0 - pxy*sin(m_hel[1]);
  p0[1] = pxy*cos(m_hel[1]); 
  p0[2] = pxy * m_hel[4];
  return p0;
}


inline HepVector HTrackParameter::x() const {
  HepVector v0(3, 0);
  v0[0] = m_hel[0]*cos(m_hel[1]); 
  v0[1] = m_hel[0]*sin(m_hel[1]); 
  v0[2] = m_hel[3];
  return v0;
}

inline HepPoint3D HTrackParameter::x3() const {
  return HepPoint3D(m_hel[0]*cos(m_hel[1]), m_hel[0]*sin(m_hel[1]), m_hel[3]);
}

inline Hep3Vector HTrackParameter::p3() const {
  double pxy = 1./fabs(m_hel[2]);
  return Hep3Vector(0-pxy*sin(m_hel[1]), pxy*cos(m_hel[1]), pxy*m_hel[4]);
}

inline HepLorentzVector HTrackParameter::p(const double mass) const{
//   // xum 2007-12-28  
//   //Hep3Vector ptrk = (p3()).rho();
//   Hep3Vector ptrk = (p3()).r();
//   double e = sqrt(ptrk*ptrk+mass*mass);
//   return HepLorentzVector(p3(), e);

  // nefedov 2011-11-17
  Hep3Vector p3tmp = p3();
  double ptrk = p3tmp.r();
  double e = sqrt(ptrk*ptrk+mass*mass);
  return HepLorentzVector(p3tmp, e);
}

#endif
