//------------------------------------------------------------------------
// Description : 
// Main file of the module KalFit in charge of :
// 1/ Refit of the Mdc tracks using Kalman filter
// 2/ Backward filter (smoothing)
// 3/ and also several mass hypothesis, Non unif mag field treatment...
//
//------------------------------------------------------------------------

#ifndef _DEFINE_KALFITALG_H_
#define _DEFINE_KALFITALG_H_
#ifndef DBL_MAX
#define DBL_MAX 9999
#endif

class KalFitTrack;
class KalFitHitMdc;
class KalFitHelixSeg;
class Bfield;
//class Helix;

#include "GaudiKernel/Algorithm.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"
#include "KalFitAlg/KalFitWire.h"
#include "KalFitAlg/KalFitHelixSeg.h"
#include "KalFitAlg/KalFitCylinder.h"
#include "KalFitAlg/KalFitLayer_Mdc.h"
#include "KalFitAlg/KalFitSuper_Mdc.h"
#include "KalFitAlg/KalFitMaterial.h"
#include "MdcTables/MdcTables.h"
#include "GaudiKernel/NTuple.h"
#include "AIDA/IHistogram1D.h" 
#include "AIDA/IHistogramFactory.h" 
#include "GaudiKernel/IHistogramSvc.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcKalHelixSeg.h"
#include "HepPDT/ParticleDataTable.hh"
#include <vector>
//added header files for 6.0.0 
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "G4Material.hh"
#include "G4Tubs.hh"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D <double > HepPoint3D;
#endif

using namespace std;
using namespace CLHEP;


class KalFitAlg : public Algorithm {

	public:
		/**@name Main member functions*/
		//@{ 
		/// constructor
		KalFitAlg(const std::string& name, ISvcLocator* pSvcLocator);
		/// destructor
		~KalFitAlg(void);
		/// initialize
		StatusCode initialize();
		/// event function
		StatusCode execute();
		StatusCode finalize();  
		StatusCode beginRun();
		/// hist definition
		void hist_def ( void );
		/// delete C++ objects, necessary to clean before begin_run or inside
		/// destructor
		void clean(void);
		//@}
		/**@name Set up the description of the Mdc */
		//@{ 
		/// Set up the wires, layers and superlayers...
		void set_Mdc(void);
		/// Initialize the material for Mdc
		void setMaterial_Mdc(void);
		/// Initialize the cylinders (walls and cathodes) for Mdc
		void setCylinder_Mdc(void);
		///
		void setDchisqCut(void);
		/// initialize for the services
		void setCalibSvc_init(void);
		void setGeomSvc_init(void);
		void setMagneticFieldSvc_init(void);
		///
		void getEventStarTime(void);

		/**@name Kalman filter method related member functions*/
		//@{
		/// Kalman filter (forward) in Mdc
		void filter_fwd_anal(KalFitTrack& trk, int l_mass, int way, HepSymMatrix& Eakal);
		void filter_fwd_calib(KalFitTrack& trk, int l_mass, int way, HepSymMatrix& Eakal);

		void init_matrix(MdcRec_trk& trk, HepSymMatrix& Ea);
		void init_matrix(int k, MdcRec_trk& trk, HepSymMatrix& Ea);

		void start_seed(KalFitTrack& track, int lead_, int way, MdcRec_trk& trk);

		/// Kalman filter (smoothing or backward part)
		void smoother_anal(KalFitTrack& trk, int way);
		void smoother_calib(KalFitTrack& trk, int way);

		/// Take the inner walls (eloss and mult scat) into account
		void innerwall(KalFitTrack& trk, int l_mass, int way);     
		//@{
		/// with results got at the inner Mdc hit 
		void fillTds(MdcRec_trk& TrasanTRK, KalFitTrack& track, 
				RecMdcKalTrack* trk,int l_mass);
		void fillTds_lead(MdcRec_trk& TrasanTRK, KalFitTrack& track, 
				RecMdcKalTrack* trk, int l_mass);
		///  with results got at the outer Mdc hit
		void fillTds_back(KalFitTrack& track, RecMdcKalTrack* trk,
				MdcRec_trk& TrasanTRK,int l_mass );

		void fillTds_back(KalFitTrack& track, RecMdcKalTrack* trk, 
				MdcRec_trk& TrasanTRK,int l_mass,RecMdcKalHelixSegCol*  segcol);

		///for smoother process
		void fillTds_back(KalFitTrack& track, RecMdcKalTrack* trk, 
				MdcRec_trk& TrasanTRK,int l_mass,RecMdcKalHelixSegCol*  segcol, int smoothflag);
		///  with results got at (0,0,0) 
		void fillTds_ip(MdcRec_trk& TrasanTRK, KalFitTrack& track, 
				RecMdcKalTrack* trk, int l_mass);

		/// complete the RecMdcKalTrackCol
		void sameas(RecMdcKalTrack* trk, int l_mass, int imain);

		void complete_track(MdcRec_trk& TrasanTRK,
				MdcRec_trk_add& TrasanTRK_add,
				KalFitTrack& track_lead,
				RecMdcKalTrack*  kaltrk,
				RecMdcKalTrackCol* kalcol,RecMdcKalHelixSegCol* segcol,int flagsmooth);

		void complete_track(MdcRec_trk& TrasanTRK, 
				MdcRec_trk_add& TrasanTRK_add, 
				KalFitTrack& track_lead,
				RecMdcKalTrack*  kaltrk,
				RecMdcKalTrackCol* kalcol,RecMdcKalHelixSegCol* segcol);

		// Careful refit
		void kalman_fitting_anal(void);
		void kalman_fitting_calib(void);
		void kalman_fitting_csmalign(void);
		void kalman_fitting_MdcxReco_Csmc_Sew(void);

		// clear tables by wangdy
		void clearTables( ); 

		///
		int getWallMdcNumber(const HepPoint3D& point);

		///
		void extToAnyPoint(KalFitTrack& trk, const HepPoint3D& point);

		///
		void setBesFromGdml(void);

		/// this usage is used to control the usage of this algorithm ,to be 
		/// analysis  or calibration. 
		int usage_;
		///
		int choice_;
		//// flag to calculate path length in each Mdc laye :
		int pathl_;
		/// flag to take account the wire sag into account
		int wsag_;
		/// flag to perform smoothing
		int back_; 

		int eventno;

		int Tds_back_no;
		/// value of the pT cut for backward filter
		double pT_;
		/// leading mass assumption 
		int lead_;
		///
		int mhyp_;
		/// value of the momentum cut to decide refit 
		double pe_cut_, pmu_cut_, ppi_cut_, pk_cut_, pp_cut_;
		double pt_cut_,theta_cut_;


		/// Flag account to multiple scattering and energy loss,
		/// where lr flag from and whether use active hits only  
		int muls_, loss_, lr_, activeonly_, tofflag_, tof_hyp_;
		/// flag to enhance the error matrix at the inner hit of Mdc (cosmic)
		int enhance_;
		double fac_h1_, fac_h2_, fac_h3_, fac_h4_, fac_h5_, matrixg_;
		///
		double  gain1_,gain2_,gain3_,gain4_,gain5_;
		int steplev_;
		int numfcor_;
		int numf_;
		int inner_steps_;
		int outer_steps_;
		int numf_in_;
		int numf_out_;
		int fitnocut_;
		///
		int drifttime_choice_;
		/// mass assumption for backward filter (if <0 means use leading mass)
		int i_back_;
		int i_front_;
		/// Debug flag for the track finder part 
		int debug_ , debug_kft_;
		/// Fill ntuples of KalFit
		int ntuple_;
		// dir of files
		string matfile_, cylfile_;
		//cut for delta_chi2 
		double dchi2cutf_, dchi2cuts_;

		double  dchi2cut_mid1_,dchi2cut_mid2_,dchi2cut_inner_, dchi2cut_outer_,dchi2cut_layid2_,dchi2cut_layid3_;

		/// factor of energy loss straggling for electron
		double fstrag_;
		//wire resoltion flag
		int resolution_;
		int iqual_front_[5],iqual_back_;

		/// propagation correction
		int tprop_;
		int eventNo;
		int m_usevtxdb; 
		int m_csmflag;    //cosmic events flag; for cosmic events tof(y>0) should be minus
		double m_dangcut, m_dphicut;  //for cosmic events cut

	private:
		std::vector<KalFitCylinder> _BesKalmanFitWalls;
		std::vector<KalFitMaterial> _BesKalmanFitMaterials;
		//std::vector<G4Tubs>         _BesKalmanFitTubs;
		
		// --- statistics
		int nTotalTrks;
		int nFailedTrks[5];


		KalFitWire * _wire;
		KalFitLayer_Mdc * _layer;
		KalFitSuper_Mdc * _superLayer;
		HepPDT::ParticleDataTable* m_particleTable;
		static const double RIW;

		const MdcCalibFunSvc* m_mdcCalibFunSvc_;
		const IMagneticFieldSvc* m_MFSvc_;
		static IMdcGeomSvc* imdcGeomSvc_;

		//sort the rec hits by layer
		static bool  order_rechits(const SmartRef<RecMdcHit>& m1, const SmartRef<RecMdcHit>& m2);

		//ntuples
		NTuple::Tuple* m_nt1;  // KalFit track params 
		NTuple::Tuple* m_nt2;  // KalFit 2-prong comparison
		NTuple::Tuple* m_nt3;  // PatRec track params
		NTuple::Tuple* m_nt4;  // PatRec 2-prong comparison
		NTuple::Tuple* m_nt5;  // for hit checking and cut
		NTuple::Tuple* m_nt6;  // for helix seg of calib

		//for nt1
		NTuple::Item<long>  m_trackid,m_evtid;
		NTuple::Item<double>  m_chi2direct,m_prob;
		NTuple::Matrix<double> m_ndf,m_chisq,m_stat;
		NTuple::Array<double> m_length,m_tof,m_nhits;
		NTuple::Item<double> m_zptot,m_zptote,m_zptotmu,m_zptotk,m_zptotp;
		NTuple::Item<double> m_zpt,m_zpte,m_zptmu,m_zptk,m_zptp;
		NTuple::Item<double> m_fptot,m_fptote,m_fptotmu,m_fptotk,m_fptotp;
		NTuple::Item<double> m_fpt,m_fpte,m_fptmu,m_fptk,m_fptp;
		NTuple::Item<double> m_lptot,m_lptote,m_lptotmu,m_lptotk,m_lptotp;
		NTuple::Item<double> m_lpt,m_lpte,m_lptmu,m_lptk,m_lptp; 
		NTuple::Item<double> m_zsigp,m_zsigpe,m_zsigpmu,m_zsigpk,m_zsigpp;
		NTuple::Array<double> m_zhelix,m_zhelixe,m_zhelixmu,m_zhelixk,m_zhelixp;
		NTuple::Array<double> m_fhelix,m_fhelixe,m_fhelixmu,m_fhelixk,m_fhelixp;
		NTuple::Array<double> m_lhelix,m_lhelixe,m_lhelixmu,m_lhelixk,m_lhelixp;
		NTuple::Array<double> m_zerror,m_zerrore,m_zerrormu,m_zerrork,m_zerrorp;
		NTuple::Array<double> m_ferror,m_ferrore,m_ferrormu,m_ferrork,m_ferrorp;
		NTuple::Array<double> m_lerror,m_lerrore,m_lerrormu,m_lerrork,m_lerrorp;
		//for nt1 single track MCTruth
		NTuple::Array<double> m_mchelix;   
		NTuple::Item<double> m_mcptot;   
		NTuple::Item<long> m_mcpid;   
		//for nt3
		NTuple::Array<double> m_trkhelix, m_trkerror;   
		NTuple::Item<double> m_trkndf, m_trkchisq, m_trkptot, m_trksigp;   
		//for nt2
		NTuple::Item<double> m_delx,m_dely,m_delz,m_delthe,m_delphi,m_delp;
		NTuple::Item<double> m_delpx,m_delpy,m_delpz;

		//for nt4
		NTuple::Item<double> m_trkdelx,m_trkdely,m_trkdelz;
		NTuple::Item<double> m_trkdelthe,m_trkdelphi,m_trkdelp;
		//for nt5
		NTuple::Item<double>  m_dchi2,m_orichi2,m_fitchi2,m_residest, m_residnew,m_anal_dr, m_anal_phi0, m_anal_kappa, m_anal_dz, m_anal_tanl, m_anal_ea_dr, m_anal_ea_phi0, m_anal_ea_kappa, m_anal_ea_dz, m_anal_ea_tanl;
		NTuple::Item<long> m_masshyp, m_layer; 
		//for nt6
		NTuple::Item<double> m_docaInc,m_docaExc, m_tdrift;
		NTuple::Item<long>  m_layerid,m_eventNo;
		NTuple::Item<double> m_residualInc, m_residualExc, m_lr, m_yposition, m_dd;

		NTuple::Item<double>  m_dchisq0,m_dchisq1,m_dchisq2,m_dchisq3,m_dchisq4,m_dchisq5,m_dchisq6,m_dchisq7,m_dchisq8,m_dchisq9,m_dchisq10,m_dchisq11,m_dchisq12,m_dchisq13,m_dchisq14,m_dchisq15,m_dchisq16,m_dchisq17,m_dchisq18,m_dchisq19,m_dchisq20,m_dchisq21,m_dchisq22,m_dchisq23,m_dchisq24,m_dchisq25,m_dchisq26,m_dchisq27,m_dchisq28,m_dchisq29,m_dchisq30,m_dchisq31,m_dchisq32,m_dchisq33,m_dchisq34,m_dchisq35,m_dchisq36,m_dchisq37,m_dchisq38,m_dchisq39,m_dchisq40,m_dchisq41,m_dchisq42;
		NTuple::Item<double>  m_dtrack0,m_dtrack1,m_dtrack2,m_dtrack3,m_dtrack4,m_dtrack5,m_dtrack6,m_dtrack7,m_dtrack8,m_dtrack9,m_dtrack10,m_dtrack11,m_dtrack12,m_dtrack13,m_dtrack14,m_dtrack15,m_dtrack16,m_dtrack17,m_dtrack18,m_dtrack19,m_dtrack20,m_dtrack21,m_dtrack22,m_dtrack23,m_dtrack24,m_dtrack25,m_dtrack26,m_dtrack27,m_dtrack28,m_dtrack29,m_dtrack30,m_dtrack31,m_dtrack32,m_dtrack33,m_dtrack34,m_dtrack35,m_dtrack36,m_dtrack37,m_dtrack38,m_dtrack39,m_dtrack40,m_dtrack41,m_dtrack42;
		NTuple::Item<double>  m_dtdc0,m_dtdc1,m_dtdc2,m_dtdc3,m_dtdc4,m_dtdc5,m_dtdc6,m_dtdc7,m_dtdc8,m_dtdc9,m_dtdc10,m_dtdc11,m_dtdc12,m_dtdc13,m_dtdc14,m_dtdc15,m_dtdc16,m_dtdc17,m_dtdc18,m_dtdc19,m_dtdc20,m_dtdc21,m_dtdc22,m_dtdc23,m_dtdc24,m_dtdc25,m_dtdc26,m_dtdc27,m_dtdc28,m_dtdc29,m_dtdc30,m_dtdc31,m_dtdc32,m_dtdc33,m_dtdc34,m_dtdc35,m_dtdc36,m_dtdc37,m_dtdc38,m_dtdc39,m_dtdc40,m_dtdc41,m_dtdc42;

};
#endif

