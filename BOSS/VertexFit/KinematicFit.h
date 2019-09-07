#ifndef VertexFit_KinematicFit_H
#define VertexFit_KinematicFit_H

#include <vector>
#include "VertexFit/WTrackParameter.h"
#include "VertexFit/KinematicConstraints.h"
#include "VertexFit/TrackPool.h"
#include "VertexFit/GammaShape.h"

class KinematicFit : public  TrackPool{

 public:

  // constructor & destructor

  static KinematicFit * instance();
  ~KinematicFit();
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
//  void AddPosition(int number, HepPoint3D xorigin, std::vector<int> tlis_V); 
  
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
  void setKalman (const int kalman) {m_kalman = kalman;}
  void setEspread (const double espread = 0.0009) {m_espread = espread;}
  void setCollideangle (const double collideangle = 11e-3) {m_collideangle = collideangle;}
  void setDynamicerror (const bool dynamicerror = 1) {m_dynamicerror = dynamicerror;}	  
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
  HepLorentzVector pfit1(int n) const {return p4Origin(n);}


  WTrackParameter origin(int n) const {return wTrackOrigin(n);}
  WTrackParameter infit(int n) const {return wTrackInfit(n);}
  
  HepVector pull(int n) ;
 // error Matrix parameters
  double espread() const {return m_espread;}
  double collideangle() const {return m_collideangle;} 
  bool dynamicerror() const {return m_dynamicerror;}
  // cpu time
  HepVector cpu() const {return m_cpu;}

  WTrackParameter  wVirtualTrack(int n) const {return m_virtual_wtrk[n];}
  private:
  //bulid virtual particle
  std::vector<WTrackParameter> m_virtual_wtrk;
  

  void updateConstraints(KinematicConstraints kc);
  void fits();
  void fit();
  //  void covMatrix();
  void upCovmtx();
  void fit(int n);
  void covMatrix(int n);
  void gda();
  private:
  std::vector<KinematicConstraints>    m_kc;
  std::vector<double>   m_chisq;
  double    m_chi;
  //  std::vector<HepMatrix> m_D;
  //  std::vector<HepMatrix> m_DT;
  HepMatrix m_D;
  HepMatrix D(int ic, int itk) const {return m_D.sub(ic+1, ic+1, itk*NTRKPAR+1, (itk+1)*NTRKPAR);}
  void setD(int ic, int itk, const HepMatrix &p) {m_D.sub(ic+1, itk*NTRKPAR+1, p);}
  HepMatrix m_DT;
  HepMatrix DT(int itk, int ic) const {return m_DT.sub(itk*NTRKPAR+1, (itk+1)*NTRKPAR, ic+1, ic+1);}
  void setDT(int itk, int ic, const HepMatrix &p) { m_DT.sub(itk*NTRKPAR+1, ic+1, p);}
  HepVector m_d;
  HepSymMatrix m_VD;
  HepMatrix m_KP;
  //  HepVector m_lambda;
  int m_nc;
  int m_nktrk;
  HepVector m_pOrigin;
  HepVector m_pInfit;
  HepSymMatrix m_covOrigin;
  HepSymMatrix m_covInfit;
  HepVector  m_massvector;    //add to save the mass of each particle
  HepVector pOrigin(int i) const {return m_pOrigin.sub(i*NTRKPAR+1, (i+1)*NTRKPAR);}
  HepLorentzVector p4Origin(int i) const { HepVector p(3, 0); p = pOrigin(i); double E = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2] + origin(i).mass() * origin(i).mass());return HepLorentzVector(p[0], p[1], p[2], E);}  //modified by yanl 2010.7.26 for changing the wtrackparameters
  HepVector pInfit(int i) const {return m_pInfit.sub(i*NTRKPAR+1, (i+1)*NTRKPAR);}
  HepLorentzVector p4Infit(int i) const { HepVector p(3, 0); p = pInfit(i);  double E = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2] + origin(i).mass() * origin(i).mass());  return HepLorentzVector(p[0], p[1], p[2], E); }   //modified by yanl 2010.7.26 for changing the wtrackparameters
  void setPOrigin(int i, const HepVector &p) { m_pOrigin.sub(i*NTRKPAR+1, p);}
  void setPInfit(int i, const HepVector &p) {m_pInfit.sub(i*NTRKPAR+1, p);}
  HepSymMatrix covOrigin(int i) const {return m_covOrigin.sub(i*NTRKPAR+1, (i+1)*NTRKPAR);}
  HepSymMatrix covInfit(int i) const {return m_covInfit.sub(i*NTRKPAR+1, (i+1)*NTRKPAR);}
  void setCovOrigin(int i, const HepSymMatrix &c) {m_covOrigin.sub(i*NTRKPAR+1, c);}
  void setCovInfit(int i, const HepSymMatrix &c) {m_covInfit.sub(i*NTRKPAR+1,c);}
  void setMassvector(int i, const double mass) {m_massvector[i] = mass;}
	private:
  KinematicFit();
  static KinematicFit * m_pointer;
	private:
  int m_niter;
  bool m_flag;
  double m_chicut;
  double m_chiter;
	private:
  int m_kalman;
  double m_espread;
  double m_collideangle;
	private:
  HepVector m_cpu;
	private:
  bool m_dynamicerror; 
	private:
  static const int NTRKPAR;

  static const int Resonance;
  static const int TotalEnergy;
  static const int TotalMomentum;
  static const int ThreeMomentum;
  static const int FourMomentum;
  static const int EqualMass;
  static const int Position;


};
#endif
