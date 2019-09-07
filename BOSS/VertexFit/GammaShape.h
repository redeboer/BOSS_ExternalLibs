#ifndef VertexFit_GammaShape_H
#define VertexFit_GammaShape_H

//
//use for gamma dynamic adjust
//
 
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


  
class GammaShape {

public:
 
  // constructor & deconstructor
  GammaShape();
  ~GammaShape(){;}
  
  GammaShape(const GammaShape &);

  
  GammaShape(const double eorigin, const double epeak);
  GammaShape(const HepLorentzVector p4, const double dphi, const double dthe, const double de);  

  GammaShape & operator = (const GammaShape &);
  double peak(const double fitvalue);
  double mean(const double epeak);
  double tail(const double epeak);
  double width(const double epeak);
  double de(const double eorigin, const double peak);
  double fitm(double x, double pk, double width, double tail, double mean);
  //  Set function
  void settail(const double tail) {m_tail = tail;}
  void setwidth(const double width) {m_width = width;}
  void setpeak(const double peak) {m_peak = peak;}
  void sete(const double e) {m_e = e;}
  void setde(const double de) {m_de = de;}
  void setdphi(const double dphi) {m_dphi = dphi;}
  void setdtheta(const double dtheta) {m_dtheta = dtheta;}

  //  get function
  double gettail() const {return m_tail;}
  double getwidth() const {return m_width;}
  double getpeak() const {return m_peak;}
  double gete() const {return m_e;}
  double getde() const {return m_de;}
  double getdphi() const {return m_dphi;}
  double getdthe() const {return m_dtheta;}
//  double getdlambda() const {return m_dlambda;}
protected:



private:

double  m_tail;    //gamma distribution information      
double  m_width;
double  m_peak;
double  m_e;
double  m_de;
double  m_dphi;
double  m_dtheta;
//double  m_dlambda;
};
#endif

