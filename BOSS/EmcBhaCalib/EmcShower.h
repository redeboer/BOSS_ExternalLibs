#ifndef EMCSHOWER_H
#define EMCSHOWER_H

#include <list>
#include "EmcBhaCalib/EmcShDigi.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace std;
//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class EmcShower 
{
 public:
  
  // Constructors
  EmcShower();
  
  // Destructors
  ~EmcShower(); 
  
  //Copy and assignment
  EmcShower(const EmcShower& aShower);
  EmcShower& operator=(const EmcShower& aShower);

  // Operators
  bool operator==(const EmcShower & otherShower) const;
  bool operator!=(const EmcShower & otherShower) const;
  bool operator>=(const EmcShower & otherShower) const;
  bool operator<=(const EmcShower & otherShower) const;
  bool operator>(const EmcShower & otherShower) const;
  bool operator<(const EmcShower & otherShower) const;
  
  const double& energy() const {return m_energy;}
  const double& theta() const {return m_theta;}
  const double& phi() const {return m_phi;}
  const unsigned int& module() const {return m_module;}
  const unsigned int& thetaIndex() const {return m_thetaIndex;}
  const unsigned int& phiIndex() const {return m_phiIndex;}
  const long int& numberOfDigis() const {return m_numberOfDigis;}
  const std::list<EmcShDigi> digiList() const {return  m_digiList;} 
  const EmcShDigi maxima() const { return m_maxima;}
  const HepPoint3D& where() const {return m_where;}
  const double& x() const {return m_x;}
  const double& y() const {return m_y;}
  const double& z() const {return m_z;}

  // Modifiers
  void addDigi( EmcShDigi d );  
  
  void setEnergy(double energy) {m_energy = energy;}
  void setTheta(double theta) {m_theta = theta;}
  void setPhi(double phi) {m_phi = phi;}
  void setModule(unsigned int module) {m_module = module;}
  void setThetaIndex(unsigned int thetaIndex) {m_thetaIndex = thetaIndex;}
  void setPhiIndex(unsigned int phiIndex) {m_phiIndex = phiIndex;}
  void setNumberOfDigis(long int numberOfDigis ) {m_numberOfDigis = numberOfDigis;}
  void setDigiList(std::list<EmcShDigi> digiList) {m_digiList = digiList;}
  void setMaxima( EmcShDigi maxima) {m_maxima = maxima;}
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
  long int m_numberOfDigis;
  EmcShDigi m_maxima;
  std::list<EmcShDigi> m_digiList;
  HepPoint3D m_where;
  double m_x;
  double m_y;
  double m_z;  
};


inline bool EmcShower::operator!=(const EmcShower& otherShower) const {
  return !(*this == otherShower);
}

#endif  // EMCSHOWER_H

