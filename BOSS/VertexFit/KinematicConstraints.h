#ifndef VertexFit_Kinematic_Constraints_H
#define VertexFit_Kinematic_Constraints_H

// #include <iostream>
#include <vector>

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
//#include "CLHEP/Matrix/DiagMatrix.h"
#include "VertexFit/WTrackParameter.h"
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using CLHEP::HepVector;
using CLHEP::HepMatrix;
//using CLHEP::HepDiagMatrix;


class KinematicConstraints {

 public:
  KinematicConstraints();
  ~KinematicConstraints(){;}
  inline KinematicConstraints(const KinematicConstraints &);
  int  Type() const {return m_type;} 
  void ResonanceConstraints(const double mres, std::vector<int> tlis, HepSymMatrix Vre); 
  void TotalEnergyConstraints(const double etot, std::vector<int> tlis);
  void TotalMomentumConstraints(const double ptot, std::vector<int> tlis);
  void ThreeMomentumConstraints(const Hep3Vector p3, std::vector<int> tlis);
//  void FourMomentumConstraints(const HepLorentzVector p4, std::vector<int> tlis , HepSymMatrix Vme);
  void FourMomentumConstraints(const HepLorentzVector p4, std::vector<int> tlis, HepSymMatrix Vme );
  void EqualMassConstraints(std::vector<int> tlis1, std::vector<int> tlis2, HepSymMatrix Vne);
//Add a Constranint function for the Position constraints  
//  void PositionConstraints(const double phipre,const double lambdapre, std::vector<int> tlis_V, HepSymMatrix Vpe);

//  void UpdateConstraints(std::vector<WTrackParameter> wlis);
//  void UpdateConstraints(std::vector<WTrackParameter> wlis);
  std::vector<HepMatrix> Ec() {return m_Ec;}
  std::vector<HepMatrix> Dc() {return m_Dc;}
  std::vector<HepVector> dc() {return m_dc;}
  std::vector<HepVector> lambda() {return m_lambda;}
  std::vector<HepSymMatrix> Vm() {return m_Vm;}
  std::vector<HepSymMatrix> VD() {return m_VD;}
  std::vector<int> Ltrk() {return m_ltrk;}
  int nc() {return m_nc;}
  void setType(const int type) {m_type = type;}
  void setLambda(HepVector lambda) {m_lambda[0]=lambda;} 
  void setVD(HepSymMatrix vd) {m_VD[0] = vd;}
  inline KinematicConstraints & operator = (const KinematicConstraints &);
 public:
  // type
  int typeRes() const {return Resonance;}
  int typeEtot() const {return TotalEnergy;}
  int typePtot()  const {return TotalMomentum;}
  int typeP3()   const {return ThreeMomentum;}
  int typeP4()  const {return FourMomentum;}
  int typeEqMass()  const {return EqualMass;}
  int typePoint()  const {return Position;}
  // get 
  double mres() const {return m_mres;}
  double etot() const {return m_etot;}
  double ptot() const {return m_ptot;}
  HepPoint3D point() const {return m_point;}
  Hep3Vector p3() const {return m_p3;}
  HepLorentzVector p4() const {return m_p4;}
  double phipre() const {return m_phipre;}
  double lambdapre() const {return m_lambdapre;}
  HepSymMatrix Vre() const {return m_Vre;}
  HepSymMatrix Vme() const {return m_Vme;}
  HepSymMatrix Vne() const {return m_Vne;}
  HepSymMatrix Vpe() const {return m_Vpe;}
  HepSymMatrix Vmeasure() const {return m_Vmeasure;}
  std::vector<int> numEqual() {return m_nequal;}
  // set
  void setMres(const double mres) {m_mres = mres;}
  void setEtot(const double etot) {m_etot = etot;}
  void setPtot(const double ptot) {m_ptot = ptot;}
  void setP3(const Hep3Vector p3) {m_p3 = p3;}
  void setP4(const HepLorentzVector p4) {m_p4 = p4;}
  void setP4(const double etot) {m_p4 = HepLorentzVector(0., 0., 0., etot);}
  void setPoint(const HepPoint3D x) {m_point = x;}
  void setPhipre(const double phipre){m_phipre = phipre;}
  void setLambdapre(const double lambdapre){m_lambdapre = lambdapre;}
  void setVre(const HepSymMatrix Vre) {m_Vre = Vre;}
  void setVme(const HepSymMatrix Vme) {m_Vme = Vme;}
  void setVne(const HepSymMatrix Vne) {m_Vne = Vne;}
  void setVpe(const HepSymMatrix Vpe) {m_Vpe = Vpe;}
  void setVmeasure(const HepSymMatrix Vmeasure) {m_Vmeasure = Vmeasure;}
  void setNumEqual(std::vector<int> nequal) {m_nequal = nequal;}
 private:
  int m_type;
  std::vector<HepMatrix> m_Dc;
  std::vector<HepMatrix> m_Ec;
  std::vector<HepVector> m_dc;
  std::vector<HepSymMatrix> m_Vm;
  std::vector<HepVector> m_lambda;
  std::vector<HepSymMatrix> m_VD;
  std::vector<int> m_ltrk;
  int m_nc;
  double m_mres;
  double m_etot;
  double m_ptot;
  Hep3Vector m_p3;
  HepLorentzVector m_p4;
  double m_phipre;
  double m_lambdapre;
  HepPoint3D m_point;
  HepSymMatrix m_Vmeasure;
  HepSymMatrix m_Vre;
  HepSymMatrix m_Vme;          //FourMomentum measurment error
  HepSymMatrix m_Vne;          //EqualMass measurment error
  HepSymMatrix m_Vpe;          //Position measurment error
  std::vector<int> m_nequal;

  static const int Resonance;
  static const int TotalEnergy;
  static const int TotalMomentum;
  static const int ThreeMomentum;
  static const int FourMomentum;
  static const int EqualMass;
  static const int Position;

};
inline KinematicConstraints::KinematicConstraints(const KinematicConstraints &kc) {
  m_type = kc.m_type;
  m_Ec = kc.m_Ec;
  m_Dc = kc.m_Dc;
  m_dc = kc.m_dc;
  m_Vm = kc.m_Vm;
  m_lambda = kc.m_lambda;
  m_VD = kc.m_VD;
  m_ltrk = kc.m_ltrk;
  m_nc = kc.m_nc;
  m_mres = kc.m_mres;
  m_etot = kc.m_etot;
  m_ptot = kc.m_ptot;
  m_p3 = kc.m_p3;
  m_p4 = kc.m_p4;
  m_phipre = kc.m_phipre;
  m_lambdapre = kc.m_lambdapre;
  m_nequal = kc.m_nequal;
  m_Vre = kc.m_Vre;
  m_Vme = kc.m_Vme;
  m_Vne = kc.m_Vne;
  m_Vpe = kc.m_Vpe;
  m_Vmeasure = kc.m_Vmeasure;
    
}

inline KinematicConstraints & KinematicConstraints::operator = (const KinematicConstraints &kc) {
  m_type = kc.m_type;
  m_Ec = kc.m_Ec;
  m_Dc = kc.m_Dc;
  m_dc = kc.m_dc;
  m_Vm = kc.m_Vm;
  m_lambda = kc.m_lambda;
  m_VD = kc.m_VD;
  m_ltrk = kc.m_ltrk;
  m_nc = kc.m_nc;
  m_mres = kc.m_mres;
  m_etot = kc.m_etot;
  m_ptot = kc.m_ptot;
  m_p3 = kc.m_p3;
  m_p4 = kc.m_p4;
  m_phipre = kc.m_phipre;
  m_lambdapre = kc.m_lambdapre;
  m_nequal = kc.m_nequal;
  m_Vre = kc.m_Vre;
  m_Vme = kc.m_Vme;
  m_Vne = kc.m_Vne;
  m_Vpe = kc.m_Vpe;
  m_Vmeasure = kc.m_Vmeasure;
  return (*this);
}
#endif

