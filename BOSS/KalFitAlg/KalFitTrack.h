#ifndef _DEFINE_KALFITTRACK_H_
#define _DEFINE_KALFITTRACK_H_
#ifndef DBL_MAX 
#define DBL_MAX 9999
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Transform3D  HepTransform3D;
#endif

//#include "TrackUtil/Helix.h"
#include "KalFitAlg/helix/Helix.h"
#include "KalFitAlg/coil/Bfield.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "KalFitAlg/KalFitHitMdc.h"
#include "KalFitAlg/KalFitHelixSeg.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"
#include "MdcRawEvent/MdcDigi.h"
#include "EventModel/Event.h"

//class HepTransform3D;
class KalFitMaterial;


using namespace KalmanFit;
using namespace CLHEP;


/// Description of a track class (<- Helix.cc)
class KalFitTrack : public Helix		 
{
	int type_; // track with  hits in pattern recognition (=1)
	double mass_;
	int trasan_id_;
	int l_mass_;
	int insist_;
	double chiSq_; 
	unsigned int nchits_, nster_, ncath_, ndf_back_;
	double chiSq_back_, pathip_, path_rd_, 
	       path_ab_, tof_, dchi2_max_, r_max_;
	double tof_kaon_, tof_proton_;
	double p_kaon_, p_proton_;
	double r0_;
	double PathL_[43];
	double pathSM_;
	double fiTerm_;
	double tof2_;

	CLHEP::Hep3Vector mom_[43];
	int pat1_, pat2_;
	int layer_prec_;
	int nhit_r_, nhit_z_;

	// Information at the last hit included :
	HepPoint3D pivot_last_;
	CLHEP::HepVector a_last_;
	CLHEP::HepSymMatrix Ea_last_;
	HepPoint3D point_last_;


	// Information at the inned hit of Mdc :
	HepPoint3D pivot_forMdc_;
	CLHEP::HepVector a_forMdc_;
	CLHEP::HepSymMatrix Ea_forMdc_;
	vector<KalFitHitMdc> HitsMdc_;
	//it is used to for record the intermediate info. of fwd-filter and bwd-filter 
	vector<KalFitHelixSeg> HelixSegs_;

	enum { NMASS = 5 };
	static const double MASS[NMASS];

	/// Flags
	static int lead_, back_;
	// the used svcs for this track
	static const MdcCalibFunSvc* CalibFunSvc_;
	static const IMagneticFieldSvc* MFSvc_;
	static IMdcGeomSvc* iGeomSvc_;
	static double EventT0_;
	static HepSymMatrix initMatrix_;
	static MdcDigiCol* mdcDigiCol_ ;
public:
	/// constructor
	KalFitTrack(const HepPoint3D& pivot,
			const CLHEP::HepVector& a,
			const CLHEP::HepSymMatrix& Ea,
			unsigned int m,
			double chiSq,
			unsigned int nhits);

	/// destructor
	~KalFitTrack(void);

	/// Intersection with different geometry
	double intersect_cylinder(double r) const;
	double intersect_yz_plane(const HepTransform3D& plane,
			double x) const;
	double intersect_zx_plane(const HepTransform3D& plane,
			double y) const;
	double intersect_xy_plane(double z) const;

	/// Calculate multiple scattering angle
	void msgasmdc(double path, int index);
	void ms(double path,
			const KalFitMaterial& m, int index);

	/// Calculate total energy lost in material
	void eloss(double path,
			const KalFitMaterial& m, int index);

	/// Kalman smoother for Mdc
	double smoother_Mdc(KalFitHelixSeg& seg, CLHEP::Hep3Vector& meas, int& flg, int csmflag);
	double smoother_Mdc_csmalign(KalFitHelixSeg& seg, CLHEP::Hep3Vector& meas, int& flg, int csmflag );
	double smoother_Mdc(KalFitHitMdc& HitMdc, CLHEP::Hep3Vector& meas, KalFitHelixSeg& seg, double& dchi2, int csmflag);

	/// Include the Mdc wire hits

	double update_hits(KalFitHitMdc& HitMdc, int inext, CLHEP::Hep3Vector& meas, int way, double& dchi2, double& dtrack, double& dtracknew, double& dtdc, int csmflag);

	double update_hits(KalFitHelixSeg& HelixSeg, int inext, CLHEP::Hep3Vector& meas, int way, double& dchi2, int csmflag);
	double update_hits_csmalign(KalFitHelixSeg& HelixSeg, int inext, CLHEP::Hep3Vector& meas, int way, double& dchi2,int csmflag );
	double chi2_next(Helix& H, KalFitHitMdc & HitMdc, int csmflag);
	double chi2_next(Helix& H, KalFitHitMdc & HitMdc);



	///
	//double approach(KalFitHitMdc& hit, bool doSagCorrection) const;

	//double approach(HepPoint3D pfwd, HepPoint3D pbwd,  bool doSagCorrection) const;


	/// Record the current parameters as ..._last information :
	void update_last(void);
	void update_forMdc(void);

	/// set and give out the last point of the track
	void point_last(const HepPoint3D& point) { point_last_ = point;} 
	const HepPoint3D& point_last(void)   { return point_last_;} 


	/// returns helix parameters
	const HepPoint3D & pivot_last(void)         const{ return pivot_last_;  }
	const CLHEP::HepVector & a_last(void)       const{ return a_last_;      }
	const CLHEP::HepSymMatrix & Ea_last(void)   const{ return Ea_last_;     }

	const HepPoint3D & pivot_forMdc(void)       const{ return pivot_forMdc_;}
	const CLHEP::HepVector & a_forMdc(void)     const{ return a_forMdc_;    }
	const CLHEP::HepSymMatrix & Ea_forMdc(void) const{ return Ea_forMdc_;   }

	/// Update the path length estimation
	void path_add(double path);

	void addPathSM(double path);
	double getPathSM(void) { return pathSM_;}

	void addTofSM(double time);
	double getTofSM(void) { return tof2_;}

	void fiTerm(double fi);
	double getFiTerm(void) { return fiTerm_;}

	/// Update the tof estimation
	void tof(double path);
	/// 
	double filter(double v_m, const CLHEP::HepVector& m_H,
			double v_d, double m_V);

	/// Correct the error according the current tanl value :
	double cor_tanldep(double* p, double er);

	void update_bit(int i);

	/// Extractor :
	int insist(void) const { return insist_; }
	int type(void) const { return type_; }
	int trasan_id(void) const { return trasan_id_; }
	double r0(void) const { return r0_; }
	double mass(void) const { return mass_; }
	double chiSq(void) const { return chiSq_; }
	double chiSq_back(void) const { return chiSq_back_; }
	int ndf_back(void) const { return ndf_back_; }
	double pathip(void) const { return pathip_; }
	double path_rd(void) const { return path_rd_; }
	double path_ab(void) const { return path_ab_; }
	double* pathl(void) { return PathL_; }
	CLHEP::Hep3Vector* mom(void) { return mom_; }
	double tof(void) const { return tof_; }
	double tof_kaon(void) const { return tof_kaon_; }
	double tof_proton(void) const { return tof_proton_; }
	double p_kaon(void) const { return p_kaon_; }
	double p_proton(void) const { return p_proton_; }
	double dchi2_max(void) const { return dchi2_max_; }
	double r_max(void) const { return r_max_; }
	unsigned int nchits(void) const { return nchits_; }
	unsigned int nster(void) const { return nster_; }
	unsigned int ncath(void) const { return ncath_; }
	int pat1(void) const { return pat1_;  }
	int pat2(void) const { return pat2_;  }
	int nhit_r(void) const { return nhit_r_;  }
	int nhit_z(void) const { return nhit_z_;  }

	/// Reinitialize (modificator)
	void type(int t){ type_ = t;}
	void trasan_id(int t){ trasan_id_ = t;}
	void insist(int t){ insist_ = t;}

	void pathip(double pl){ pathip_ = pl;}
	void p_kaon(double pl){ p_kaon_ = pl;}
	void p_proton(double pl){ p_proton_ = pl;}
	void chiSq(double c){ chiSq_ = c; }
	void chiSq_back(double c){ chiSq_back_ = c; }
	void ndf_back(int n){ ndf_back_ = n; }
	void nchits(int n){ nchits_ = n; }
	void nster(int n){ nster_ = n; }

	void add_nhit_r(void) { nhit_r_++;}
	void add_nhit_z(void) { nhit_z_++;}

	/// Function to calculate the path length in the layer
	double PathL(int layer);

	/// Functions for Mdc hits list
	void appendHitsMdc(KalFitHitMdc h);
	void HitsMdc(vector<KalFitHitMdc>& lh);
	vector<KalFitHitMdc>& HitsMdc(void) { return HitsMdc_;}
	KalFitHitMdc& HitMdc(int i) { return HitsMdc_[i];}

	void appendHelixSegs(KalFitHelixSeg s);
	void HelixSegs(vector<KalFitHelixSeg>& vs);
	vector<KalFitHelixSeg>& HelixSegs(void) { return HelixSegs_;}
	KalFitHelixSeg& HelixSeg(int i) { return HelixSegs_[i];}

	/// Modifier
	/// Order the wire hits for mdc track
	void order_wirhit(int index);
	void order_hits(void);
	void number_wirhit(void);

	/// Sets pivot position in a given mag field
	const HepPoint3D & pivot_numf(const HepPoint3D & newPivot);
	const HepPoint3D & pivot_numf(const HepPoint3D & newPivot, double & pathl);

	/// Estimation of the radius in a given mag field
	double radius_numf(void) const;

	//temporily for wire resolution, use service in future
	double getSigma(int layerId, double driftDist ) const;

	//  call from calibration service 
	double  getSigma(KalFitHitMdc& hitmdc, double tanlam, int lr, double dist) const;
	double  getDriftDist(KalFitHitMdc& hitmdc, double drifttime, int lr) const;

	double  getDriftTime(KalFitHitMdc& hitmdc , double toftime) const;

	// event start time
	static void  setT0(double t0);
	double getT0(void) const;

	static void setInitMatrix(HepSymMatrix m);
	HepSymMatrix getInitMatrix(void) const;

	double  getDigi() const;

	//for get service 
	static void setMdcCalibFunSvc(const MdcCalibFunSvc* calibsvc);
	static void setMagneticFieldSvc(IMagneticFieldSvc*);
	static void setIMdcGeomSvc( IMdcGeomSvc* igeomsvc);
	static void setMdcDigiCol(MdcDigiCol*  digicol);

	static double  mdcGasRadlen_;

	/// for signal propagation correction
	static int tprop_;

	/// for debug  
	static int debug_;  
	/// Cut chi2 for each hit
	static double chi2_hitf_, chi2_hits_;  
	/// Flag for treatment of non-uniform mag field
	static int numf_;   
	static int inner_steps_;
	static int outer_steps_;

	static double dchi2cutf_anal[43];

	static double dchi2cuts_anal[43];

	static double dchi2cutf_calib[43];

	static double dchi2cuts_calib[43];

	/// NUMF treatment improved
	static int numfcor_;   
	// Nominal mag field 
	static double Bznom_;
	/// Level of precision (= 1 : 5steps for all tracks; = 2: 5 steps for
	/// very non uniform part)
	static int steplev_;   

	/// Flag for TOF correction
	static int Tof_correc_;  

	/// Flag to take account of energy loss straggling :
	static int strag_;
	/// factor of energy loss straggling for electron
	static double factor_strag_;
	/// Cut chi2 for each hit
	static int nmdc_hit2_;
	static double chi2mdc_hit2_; 
	static int tofall_;
	/// wire resoltion flag
	static int resolflag_;

	/// Use L/R decision from MdcRecHit information :
	static int LR_;
	/// the drifttime choice 
	static int drifttime_choice_;
	static int nmass(void);
	static double mass(int i);
	void chgmass(int i);
	static void LR(int x);
	/// Magnetic field map
	static int lead(void);
	static void lead(int i);
	static int back(void);
	static void back(int i);
	static int resol(void);
	static void resol(int i);
	static int numf(void);
	static void numf(int i);

	int nLayerUsed() 
	{
		int n=0;
		for(int i=0; i<43; i++) n+=myLayerUsed[i];
		return n;
	}

	void resetLayerUsed() {
		for(int i=0; i<43; i++) myLayerUsed[i]=0;
	}

	void useLayer(int iLay) {
		if(iLay>=0 && iLay<=43) myLayerUsed[iLay]=1;
	}


private:
	int myLayerUsed[43];


};

#endif

