#ifndef VertexFit_KalmanKinematicFit_H
#define VertexFit_KalmanKinematicFit_H

#include <vector>
#include "VertexFit/WTrackParameter.h"
#include "VertexFit/KinematicConstraints.h"
#include "VertexFit/TrackPool.h"
#include "VertexFit/GammaShape.h"
#include "TGraph2D.h"

class KalmanKinematicFit : public  TrackPool{

 public:
  // constructor & destructor

  static KalmanKinematicFit * instance();
  ~KalmanKinematicFit();
  //
  //  Resonance Constraints 
  //
  void AddResonance(int number, double mres, std::vector<int> tlis);
  void AddResonance(int number, double mres, int n1);
  void AddResonance(int number, double mres, int n1, int n2);
  void AddResonance(int number, double mres, int n1, int n2, int n3);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
		    int n5);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
		    int n5, int n6);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
		    int n5, int n6, int n7);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
		    int n5, int n6, int n7, int n8);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
		    int n5, int n6, int n7, int n8, int n9);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
		    int n5, int n6, int n7, int n8, int n9, 
		    int n10);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
		    int n5, int n6, int n7, int n8, int n9, 
		    int n10, int n11);
  void AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
		    int n5, int n6, int n7, int n8, int n9, 
		    int n10, int n11, int n12);
  //
  //  Total Energy Constraints
  //
  void AddTotalEnergy(int number, double etot, std::vector<int> lis);
  void AddTotalEnergy(int number, double etot, int n1);
  void AddTotalEnergy(int number, double etot, int n1, int n2);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
		      int n5);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
		      int n5, int n6);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
		      int n5, int n6, int n7);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
		      int n5, int n6, int n7, int n8);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
		      int n5, int n6, int n7, int n8, int n9);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
		      int n5, int n6, int n7, int n8, int n9, int n10);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
		      int n5, int n6, int n7, int n8, int n9, int n10, int n11);
  void AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
		      int n5, int n6, int n7, int n8, int n9, 
		      int n10, int n11, int n12);

  //
  //  Total Momentum Constraints
  //
  void AddTotalMomentum(int number, double ptot, std::vector<int> lis);
  void AddTotalMomentum(int number, double ptot, int n1);
  void AddTotalMomentum(int number, double ptot, int n1, int n2);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
			int n5);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
			int n5, int n6);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
			int n5, int n6, int n7);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
			int n5, int n6, int n7, int n8);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
			int n5, int n6, int n7, int n8, int n9);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
			int n5, int n6, int n7, int n8, int n9, 
			int n10);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
			int n5, int n6, int n7, int n8, int n9, 
			int n10, int n11);
  void AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
			int n5, int n6, int n7, int n8, int n9, 
			int n10, int n11, int n12);
  //
  //  Three Momentum Constraints
  //
  void AddThreeMomentum(int number, Hep3Vector p3);
  //
  //  Four Momentum Constraints
  //
  void AddFourMomentum(int number, HepLorentzVector p4);
  void AddFourMomentum(int number, double etot);

  //
  //  Equal Mass Constraints 
  //
  
  void AddEqualMass(int number, std::vector<int> tlis1, std::vector<int> tlis2);
  
  //
  //  Position Constraints
  //
 // void AddPosition(int number, HepPoint3D Xbeam, HepSymMatrix Vbeam,std::vector<int> tlis_V); 
  
  //
  //  Build Virtual Particle
  //
  void BuildVirtualParticle(int number);

  // initialization called before fit

  void init();
  // 
  //  set iteration number and chisq cut
  //
  void setFlag(const bool flag = 1)  {m_flag = flag;}
  void setIterNumber(const int niter = 5) {m_niter = niter;}
  void setChisqCut(const double chicut = 200, const double chiter=0.05) {m_chicut = chicut;m_chiter=chiter;}
   //
   //  set error matrix parameters
   //
  void setEspread (const double espread = 0.0009) {m_espread = espread;}
  void setCollideangle (const double collideangle = 11e-3) {m_collideangle = collideangle;}
  void setDynamicerror (const bool dynamicerror = 1) {m_dynamicerror = dynamicerror;}
  void setTgraph ( TGraph2D* graph2d) {m_graph2d = graph2d;}	  
  //
  // fit function
  //
  bool Fit();
  bool Fit(int n);
  //
  // Fit Results
  //
  //                        chisq of fit
  //
  double chisq() const {return m_chi;}
  double chisq(int n) const {return m_chisq[n];}
  //  updated WTrack parameter after kinematic fit
  //  HepLorentzVector pfit(int n) const {return wTrackInfit(n).p();}
  HepLorentzVector pfit(int n) const {return p4Infit(n);}
 // added by yanl 05.10.07
  //  HepLorentzVector pfit1(int n) const {return wTrackOrigin(n).p();}
  HepLorentzVector pfit1(int n)  {return p4Origin(n);}
  HepVector xfit() {return m_q.sub(1,3);}

  WTrackParameter origin(int n) const {return wTrackOrigin(n);}
  WTrackParameter infit(int n) const {return wTrackInfit(n);}
  
  HepVector pull(int n) ;
//  WTrackParameter wVirtualTrack(int n) const {return m_virtual_wtrk[n];}
 // error Matrix parameters
  double espread() const {return m_espread;}
  double collideangle() const {return m_collideangle;} 
  bool dynamicerror() const {return m_dynamicerror;}
  // cpu time
  HepVector cpu() const {return m_cpu;}
  HepSymMatrix getCOrigin(int i) const;
  HepSymMatrix getCInfit(int i) const;  

  WTrackParameter  wVirtualTrack(int n) const {return m_virtual_wtrk[n];}
    
private:
  //bulid virtual particle
  std::vector<WTrackParameter> m_virtual_wtrk;

 private:
  void updateConstraints(KinematicConstraints kc);
  void fit();
  //  void covMatrix();
  void upCovmtx();
  void upTrkpar();
  void clearDDT();
  void fit(int n);
  void covMatrix(int n);
  void gda();
 private:
  std::vector<KinematicConstraints>    m_kc;
  std::vector<double>   m_chisq;
  double    m_chi;
  HepSymMatrix m_Vm;
  HepMatrix m_A;
  void setA(int ic, int itk, const HepMatrix &p) {m_A.sub(ic+1, itk, p);}
  HepMatrix m_AT;
  void setAT(int itk, int ic, const HepMatrix &p) { m_AT.sub(itk, ic+1, p);}
  HepVector m_G;
  HepSymMatrix m_W;
  HepMatrix m_KP;

   // virtual particle
  HepMatrix m_B;
  void setB(int ic, int itk, const HepMatrix &p) {m_B.sub(ic+1, itk, p);}
  HepMatrix m_BT;
  void setBT(int itk, int ic, const HepMatrix &p) { m_BT.sub(itk, ic+1, p);}
  

  HepMatrix m_KQ;
  int m_nc;
  int m_nktrk;


  HepVector m_p0;
  HepVector m_p;
  HepSymMatrix m_C0;
  HepSymMatrix m_C;
  HepVector pOrigin(int i) const ;
  HepLorentzVector p4Origin(int i) const { HepVector p(4, 0); p = pOrigin(i); return HepLorentzVector(p[0], p[1], p[2], p[3]);}
  HepVector pInfit(int i) const ;
  HepLorentzVector p4Infit(int i) const { HepVector p(4, 0); p = pInfit(i); return HepLorentzVector(p[0], p[1], p[2], p[3]); }


  void setPOrigin(int i, const HepVector &p) { m_p0.sub(i, p);}
  void setPInfit(int i, const HepVector &p) {m_p.sub(i, p);}
  void setCOrigin(int i, const HepSymMatrix &D) {m_C0.sub(i, D);}
  void setCInfit(int i, const HepSymMatrix &D) {m_C.sub(i,D);}
  
  HepVector m_q0;
  HepVector m_q;
  HepSymMatrix m_D0;
  HepSymMatrix m_D;
  HepSymMatrix m_D0inv;
  HepSymMatrix m_Dinv;
 
  void setQOrigin(int i, const HepVector &q) { m_q0.sub(i, q);}
  void setQInfit(int i, const HepVector &q) {m_q.sub(i, q);}
  void setDOrigin(int i, const HepSymMatrix &D) {m_D0.sub(i, D);}
  void setDInfit(int i, const HepSymMatrix &D) {m_D.sub(i,D);}
  void setDOriginInv(int i, const HepSymMatrix &Dinv) {m_D0inv.sub(i,Dinv);}
   


 private:
  KalmanKinematicFit();
  static KalmanKinematicFit * m_pointer;
 private:
  int m_niter;
  bool m_flag;
  double m_chicut;
  double m_chiter;
 private:
  double m_espread;
  double m_collideangle;
 private:
  HepVector m_cpu;
 private:
  bool m_dynamicerror; 
 private:
  static const int NTRKPAR;
  static const int NKFPAR;
  static const int Resonance;
  static const int TotalEnergy;
  static const int TotalMomentum;
  static const int ThreeMomentum;
  static const int FourMomentum;
  static const int EqualMass;
  static const int Position;
 private:
  TGraph2D* m_graph2d;


};
#endif

