#ifndef EMCSHDIGI_H
#define EMCSHDIGI_H

#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

class EmcShDigi 
{
public:

  // Constructors
  EmcShDigi();
  
  // Destructors
  ~EmcShDigi(); 
  
  //Copy and assignment
  EmcShDigi(const EmcShDigi& aDigi);
  EmcShDigi& operator=(const EmcShDigi& aDigi);
  
  // Operators
  bool operator==(const EmcShDigi & otherDigi) const;
  bool operator!=(const EmcShDigi & otherDigi) const;
  bool operator>=(const EmcShDigi & otherDigi) const;
  bool operator<=(const EmcShDigi & otherDigi) const;
  bool operator> (const EmcShDigi & otherDigi) const;
  bool operator< (const EmcShDigi & otherDigi) const;

  const double& energy() const {return m_energy;}
  const double& theta() const {return m_theta;}
  const double& phi() const {return m_phi;}
  const unsigned int& module() const {return m_module;}
  const unsigned int& thetaIndex() const {return m_thetaIndex;}
  const unsigned int& phiIndex() const {return m_phiIndex;}
  const double& time() const {return m_time;}
  const double& fraction() const {return m_fraction;}
  const HepPoint3D& where() const {return m_where;}
  const double& x() const {return m_x;}
  const double& y() const {return m_y;}
  const double& z() const {return m_z;}

  void setEnergy(double energy) {m_energy = energy;}
  void setTheta(double theta) {m_theta = theta;}
  void setPhi(double phi) {m_phi = phi;}
  void setModule(unsigned int module) {m_module = module;}
  void setThetaIndex(unsigned int thetaIndex) {m_thetaIndex = thetaIndex;}
  void setPhiIndex(unsigned int phiIndex) {m_phiIndex = phiIndex;}
  void setTime(double time ) {m_time = time;}
  void setFraction(double fraction ) {m_fraction = fraction;}
  void setWhere(HepPoint3D where) {m_where = where;}
  void setX(double x) {m_x = x;}
  void setY(double y) {m_y = y;}
  void setZ(double z) {m_z = z;}

 private:

  double m_energy;
  double m_theta;
  double m_phi;
  unsigned int m_module;
  unsigned int m_thetaIndex;
  unsigned int m_phiIndex;
  double m_time;
  double m_fraction;
  HepPoint3D m_where;
  double m_x;
  double m_y;
  double m_z;
};


inline bool EmcShDigi::operator!=(const EmcShDigi& otherDigi) const {
  return !(*this == otherDigi);
}

#endif  // EMCSHDIGI_H

