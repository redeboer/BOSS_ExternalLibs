#ifndef WTrack_Parameter_H
#define WTrack_Parameter_H

//--------------------------------------------------------------------------------
//
// Track Parameters Format:  (Px, Py, Pz, E, x, y, z)
//
// Author   Date         Comments
// K.L He   10/31/2005   original version
// Martin   09/02/2010   remove dependency of Class Helix(from Belle), add some 
//                       kindly comments 
// 
//--------------------------------------------------------------------------------
#include <cmath>
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

class WTrackParameter 
{
public:

	WTrackParameter();
	~WTrackParameter(){;}
	WTrackParameter(const WTrackParameter &wtrk);
	WTrackParameter& operator =(const WTrackParameter &wtrk);
	
	// For charged tracks
	WTrackParameter(const int charge, const HepLorentzVector &p, const HepPoint3D &x, const double err[]);
	WTrackParameter(const int charge, const HepLorentzVector &p, const HepPoint3D &x, const HepSymMatrix &err);
	WTrackParameter(const double mass, const HepVector &helix, const double err[]);
	WTrackParameter(const double mass, const HepVector &helix, const HepSymMatrix &err);
	
	// For Neutral tracks
	WTrackParameter(const HepPoint3D &x, const HepLorentzVector &p, const double dphi, const double dtheta, const double dE);
	WTrackParameter(const HepLorentzVector &p, const double dphi, const double dtheta, const double dE);
	
	// Set methods
	void              setW(const HepVector &w) {m_w = w; m_mass = sqrt(w[3]*w[3] - w[2]*w[2] - w[1]*w[1] - w[0]*w[0]);}
	void              setW(const int n, const double w) {m_w[n] = w;}
	void              setEw(const HepSymMatrix &Ew) {m_Ew = Ew;}
	void              setCharge(const int charge) {m_charge = charge;}
	void              setMass(const double mass) {m_mass = mass;}
	void              setType(const int type) {m_type = type;}
	void              setVplm(const HepSymMatrix &Vplm) {m_Vplm = Vplm;}
	void              setPlmp(const HepVector &plmp) {m_plmp = plmp;}
	
	// Get methods
	int               type() const {return m_type;}
	int               charge() const {return m_charge;}
	double            mass() const {return m_mass;} 
	bool              IsInvariableMass() const {return m_massInvariable;}
	double            phi() const {return atan(w()[1]/(w()[0]));}
	double            Lambda() const {return w()[2]/sqrt(w()[0]*w()[0] + w()[1]*w()[1]);}
	HepVector         w() const {return m_w;}
	HepSymMatrix      Ew() const {return m_Ew;}
	HepLorentzVector  p() const {return HepLorentzVector(m_w[0], m_w[1], m_w[2], m_w[3]);}
	HepSymMatrix      Ep() const {return m_Ew.sub(1, 4); }
	HepPoint3D        x() const {return HepPoint3D(m_w[4], m_w[5], m_w[6]);}
	HepVector         X() const {return m_w.sub(5, 7); }
	HepSymMatrix      Ex() const {return m_Ew.sub(5, 7); }
	HepVector         philambdamass();
	HepSymMatrix      Vplm() const {return m_Vplm;}
	HepVector         plmp() const {return m_plmp;}

private:
	
	int               m_charge;           // charge of particle
	HepVector         m_w;                // HepVector(px, py, pz, E, x, y, z)
	HepSymMatrix      m_Ew;               // error matrix
	int               m_type;             // 1-charged track; 2-neutral track
	HepSymMatrix      m_Vplm;             // 
	HepVector         m_plmp;             // 
	double            m_mass;             // mass of particle  added by yanl 2010.7.23
	bool              m_massInvariable;   // tag of mass invariable  added by yanl 2010.8.12

	HepMatrix         GetCvtMatrix(const double mass, const HepVector &helix);  // conversion matrix of 5x1 into 7x1
	HepVector         CvtH2W(const double mass, const HepVector &helix);        // conversion of Helix into WTrackParameter
};

#endif  // WTrack_Parameter_H

