// -*- C++ -*-
//
// Package:     <package>
// Module:      Lpar
// 
// Description: <one line class summary>
//
// Usage:
//    <usage>
//
// Author:      KATAYAMA Nobuhiko
// Created:     Fri Feb  6 10:21:31 JST 1998
//

#if !defined(PACKAGE_LPAR_H_INCLUDED)
#define PACKAGE_LPAR_H_INCLUDED

// system include files
#include <iosfwd>
#include <cmath>
#include <iostream>

// user include files
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
//#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
//#endif

  using CLHEP::HepVector; 
  using CLHEP::Hep3Vector;
  using CLHEP::HepMatrix;
  using CLHEP::HepSymMatrix;

using namespace CLHEP;

// forward declarations

class Lpar
{
    // friend classes and functions

  public:
    // constants, enums and typedefs

    // Constructors and destructor
    Lpar();

    virtual ~Lpar();

    // assignment operator(s)
    inline const Lpar& operator=( const Lpar& );

    // member functions
    inline void neg();
    void circle(double x1, double y1, double x2, double y2,
		double x3, double y3);

    // const member functions
    double kappa() const { return m_kappa; }
    double radius() const { return 0.5/std::fabs(m_kappa);}
    HepVector center() const;
    double s(double x, double y) const;
    inline double d(double x, double y) const;
    inline double dr(double x, double y) const;
    double s(double r, int dir=0) const;
    double phi(double r, int dir=0) const;
    inline int sd(double r, double x, double y, 
		  double limit, double & s, double & d) const;
    inline HepVector Hpar(const HepPoint3D & pivot) const;

    // static member functions

    // friend functions and classes
    friend class Lpav;
    friend std::ostream & operator<<(std::ostream &o, Lpar &);
    friend int intersect(const Lpar&, const Lpar&, HepVector&, HepVector&);

  protected:
    // protected member functions

    // protected const member functions

  private:
    // Private class cpar
    class Cpar {
    public:
      Cpar(const Lpar&);
      double xi() const{ return 1 + 2 * m_cu * m_da; }
      double sfi() const { return m_sfi; }
      double cfi() const { return m_cfi; }
      double da() const { return m_da; }
      double cu() const { return m_cu; }
      double fi() const { return m_fi; }
    private:
      double m_cu;
      double m_fi;
      double m_da;
      double m_sfi;
      double m_cfi;
    };
  friend class Lpar::Cpar;
    // Constructors and destructor
    inline Lpar( const Lpar& );

    // comparison operators
    bool operator==( const Lpar& ) const;
    bool operator!=( const Lpar& ) const;

    // private member functions
    void scale(double s) { m_kappa /= s; m_gamma *= s; }
    inline void rotate(double c, double s);
    inline void move (double x, double y);

    // private const member functions
    double alpha() const { return m_alpha; }
    double beta() const { return m_beta; }
    double gamma() const { return m_gamma; }
    inline double check() const;
    HepMatrix dldc() const;
    inline double d0(double x, double y) const;
    double kr2g(double r) const { return m_kappa * r * r + m_gamma; }
    double x(double r) const;
    double y(double r) const;
    void xhyh(double x, double y, double&xh, double&yh) const;
    double xi2() const { return 1 + 4 * m_kappa * m_gamma; } 
    bool xy(double, double&, double&, int dir=0) const;
    inline double r_max() const;
    double xc() const { return - m_alpha/2/m_kappa; }
    double yc() const { return - m_beta/2/m_kappa; }
    double da() const {  return 2 * gamma() / (std::sqrt(xi2()) + 1); }
    inline double arcfun(double xh, double yh) const;

    // data members
    double m_alpha;
    double m_beta;
    double m_gamma;
    double m_kappa;


    static const double BELLE_ALPHA;

    // static data members

};

// inline function definitions

// inline Lpar::Lpar(double a, double b, double k, double g) {
//   m_alpha = a; m_beta = b; m_kappa = k; m_gamma = g;
// }

inline Lpar::Lpar() {
  m_alpha = 0;
  m_beta = 1;
  m_gamma = 0;
  m_kappa = 0;
}

inline Lpar::Lpar(const Lpar&l) {
  m_alpha = l.m_alpha;
  m_beta = l.m_beta;
  m_gamma = l.m_gamma;
  m_kappa = l.m_kappa;
}

inline const Lpar&Lpar::operator=(const Lpar&l) {
    if (this != &l) {
	m_alpha = l.m_alpha;
	m_beta = l.m_beta;
	m_gamma = l.m_gamma;
	m_kappa = l.m_kappa;
    }
  return *this;
}

inline void Lpar::rotate(double c, double s) {
  double aLpar =  c * m_alpha + s * m_beta;
  double betar = -s * m_alpha + c * m_beta;
  m_alpha = aLpar;
  m_beta = betar;
}

inline void Lpar::move (double x, double y) {
  m_gamma += m_kappa * ( x * x + y * y ) + m_alpha * x + m_beta * y;
  m_alpha += 2 * m_kappa * x;
  m_beta  += 2 * m_kappa * y; 
}

inline double Lpar::check() const {
  return m_alpha * m_alpha + m_beta * m_beta - 4 * m_kappa * m_gamma - 1;
}

inline void Lpar::neg() {
  m_alpha = -m_alpha;
  m_beta = -m_beta;
  m_gamma = -m_gamma;
  m_kappa = -m_kappa;
}

inline double Lpar::d0(double x, double y) const {
  return m_alpha * x + m_beta * y + m_gamma + m_kappa * ( x * x + y * y);
}

inline double Lpar::d(double x, double y) const {
  double dd = d0(x,y);
  const double approx_limit = 0.2;
  if(std::fabs(m_kappa*dd)>approx_limit) return -1;
  return dd * ( 1 - m_kappa * dd );
}

inline double Lpar::dr(double x, double y) const {
  double dx = xc() - x;
  double dy = yc() - y;
  double r = 0.5/std::fabs(m_kappa);
  return std::fabs(std::sqrt(dx * dx + dy * dy) - r);
}

inline double Lpar::r_max() const {
  if (m_kappa==0) return 100000000.0;
  if (m_gamma==0) return 1/std::fabs(m_kappa);
  return std::fabs(2*m_gamma/(std::sqrt(1+4*m_gamma*m_kappa)-1));
}

inline double Lpar::arcfun(double xh, double yh) const {
  //
  // Duet way of calculating Sperp.
  //
  double r2kap = 2.0 * m_kappa;
  double xi = std::sqrt(xi2());
  double xinv = 1.0 / xi;
  double ar2kap = std::fabs(r2kap);
  double cross = m_alpha * yh - m_beta * xh;
  double a1 = ar2kap * cross * xinv;
  double a2 = r2kap * (m_alpha * xh + m_beta * yh) * xinv + xi;
  if (a1>=0 && a2>0 && a1<0.3) {
    double arg2 = a1*a1;
    return cross * ( 1.0 + arg2 * (1./6. + arg2 * (3./40.))) * xinv;
  } else {
    double at2 = std::atan2(a1,a2);
    if (at2<0) at2 += (2*M_PI);
    return at2/ar2kap;
  }
}

inline int Lpar::sd(double r, double x, double y, 
		    double limit, double & s, double & d) const{
  if ((x*yc()-y*xc())*m_kappa<0) return 0;
  double dd = d0(x,y);
  d = dd * ( 1 - m_kappa * dd );
  double d_cross_limit = d*limit;
  if (d_cross_limit < 0 || d_cross_limit > limit*limit) return 0;
  
  double rc = std::sqrt(m_alpha*m_alpha+m_beta*m_beta)/(2*m_kappa);
  double rho = 1./(-2*m_kappa);
  double cosPhi = (rc*rc + rho*rho - r*r)/(-2*rc*rho);
  double phi = std::acos(cosPhi);
  s = std::fabs(rho)*phi;
  d *= r/(std::fabs(rc)*std::sin(phi));
  if (abs(d) > abs(limit)) return 0;
  d_cross_limit = d*limit;
  if (d_cross_limit > limit*limit) return 0;
  return 1;
}

inline HepVector Lpar::Hpar(const HepPoint3D & pivot) const{
  HepVector a(5);
  double dd = d0(pivot.x(),pivot.y());
  a(1) = dd * ( m_kappa * dd - 1 );
  a(2) = (m_kappa>0) ? std::atan2(yc() - pivot.y(), xc() - pivot.x()) + M_PI 
    : std::atan2(pivot.y() - yc(), pivot.x() - xc()) - M_PI;
  a(3) = -2.0*BELLE_ALPHA*m_kappa;
  a(4) = 0;
  a(5) = 0;
  return a;
}

#endif /* PACKAGE_LPAR_H_INCLUDED */

