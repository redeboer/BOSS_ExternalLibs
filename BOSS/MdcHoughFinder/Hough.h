#ifndef  HOUGHSPACE_H
#define  HOUGHSPACE_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"                                        
#include "GaudiKernel/INTupleSvc.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include <string>
#include <vector>
#include <map>
#include "TH2D.h" 
#include "TF1.h" 
#include "TGraph.h" 
#include <fstream>

#include "CLHEP/Alist/AList.h"
#include "MdcGeom/MdcDetector.h"
#include "BField/BField.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcUtilitySvc/MdcUtilitySvc.h"
#include "MdcPrintSvc/MdcPrintSvc.h"
//#include "CLHEP/Matrix/Vector.h"

#include "MdcHoughFinder/HoughHit.h"
#include "MdcHoughFinder/HoughHitList.h"
#include "MdcHoughFinder/HoughGlobal.h"
#include "MdcHoughFinder/HoughMap.h"

#include "MdcRecEvent/RecMdcTrack.h"                                                          
#include "MdcRecEvent/RecMdcHit.h" 
#include "HepPDT/ParticleDataTable.hh"
#include "MdcTrkRecon/MdcTrack.h"
#include "MdcTrkRecon/MdcTrackParams.h"
#include "MdcTrkRecon/MdcTrackList.h"
#include "TrkBase/TrkRecoTrk.h"  
#include "TrkFitter/TrkContextEv.h" 
class IBesTimerSvc;
class BesTimer;

class MdcHoughFinder:public Algorithm {
  public:
    MdcHoughFinder(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    StatusCode beginRun();  
    StatusCode bookTuple();  

  private:
    int GetMcInfo();
    MdcDigiVec prepareDigi();
	void dumpHoughHitList(const HoughHitList&); 
	void dumpHoughMap(const HoughMap& );
	void diffMap(const HoughMap& ,const HoughMap&);
	void Leastfit(vector<double>, vector<double>,double&,double&);
	int storeTracks(RecMdcTrackCol*& a,RecMdcHitCol*& b,vector<RecMdcTrack*>& vec_trackPatTds);
	void clearMem(MdcTrackList& list1,MdcTrackList& list2); 
	void compareHough(MdcTrackList& mdcTrackList);
	int  comparePatTsf(MdcTrackList& tracklist, RecMdcTrackCol* trackList_tds);
	int arbitrateHits(vector<MdcTrack*>& , MdcTrackParams );
	//int judgeHit(MdcTrackList& list ,const vector<HoughTrack>& tracklist);
	int judgeHit(MdcTrackList& list , vector<MdcTrack*>& tracklist);
	int judgeChargeTrack(MdcTrackList& list1 , MdcTrackList& list2);

//declare property
	std::string m_pdtFile;
	std::string m_sigmaFile;
	HepPDT::ParticleDataTable* m_particleTable; 
	std::map< int, const HepVector > g_tkParTruth;
	HepPoint3D g_firstHit;
	double t_d0;
	double t_z0;
	double t_tanl;
	double t_p;
	double t_pt;
	double t_cos;

	int    	m_filter;
	std::string m_evtFile;
	int    	m_debug;
	int    	m_debugMap;
	int     m_debug2D;
	int     m_debug3D;
	int     m_debugTrack;
	int     m_debugStereo;
	int     m_debugZs;
	int     m_debugArbHit;

	bool   	m_hist;
	bool		    m_keepBadTdc;
	bool		    m_dropHot;
	bool		    m_keepUnmatch;
	bool m_combineTracking;
	int  m_removeBadTrack;
	double m_dropTrkDrCut;
	double m_dropTrkDzCut;
	double m_dropTrkPtCut;
	double m_dropTrkChi2Cut;

	int 	m_inputType;
	double m_mapCharge;
	int 	m_useHalf;
	int 	m_mapHit;
	int 	m_nBinTheta;
	int 	m_nBinRho;
	double 	m_rhoRange;
	int     m_npart;
	int     m_n2;
	double     m_dsigma;
	double     m_dsigma2;
	double m_peakWidth;
	double m_peakHigh;
	double m_hitPro;
	
	int m_recpos;
	int m_recneg;
	int m_combine;
	double m_z0Cut_compareHough;


	//data member
	int 	t_eventNum;
	int 	t_runNum;
	BField*	    	m_bfield;
	double       	m_bunchT0;
	TrkContextEv*   m_context;
	const MdcCalibFunSvc*     m_mdcCalibFunSvc; 
	RawDataProviderSvc*       m_rawDataProviderSvc;  
	MdcGeomSvc*  	          m_mdcGeomSvc;
	IMagneticFieldSvc*        m_pIMF;
	const MdcPrintSvc*        m_mdcPrintSvc;
	IBesTimerSvc* m_timersvc;
	BesTimer* m_timer_all;

	//Ntuple
	NTuple::Tuple*         ntuple_evt;
	NTuple::Item<int>      m_eventNum;
	NTuple::Item<int>      m_runNum;
	NTuple::Item<int>      m_nHit;
	NTuple::Item<int>      m_nSig_Axial;
	NTuple::Array<int>     m_layer;
	NTuple::Array<int>     m_cell;
	NTuple::Array<double>  m_x;
	NTuple::Array<double>  m_y;
	NTuple::Array<double>  m_z;
	NTuple::Array<double>  m_u;
	NTuple::Array<double>  m_v;
	NTuple::Array<double>  m_r;
	NTuple::Array<double>  m_uTruth;
	NTuple::Array<double>  m_vTruth;
	NTuple::Array<double>  m_rTruth;
	NTuple::Array<double>  m_xTruth;
	NTuple::Array<double>  m_yTruth;
	NTuple::Array<double>  m_type;

	NTuple::Array<double>  m_deltaD;
	NTuple::Array<double>  m_flt;
	NTuple::Array<double>  m_drift;
	NTuple::Array<double>  m_driftTruth;
	NTuple::Array<int>     m_slant;

	NTuple::Item<int>      m_xybinNum;
	NTuple::Item<double>      m_xybinMax;
	NTuple::Array<double>      m_xybinNL;
	NTuple::Array<double>      m_xybinRL;
	NTuple::Array<double>      m_xybinS;

	NTuple::Item<double>      m_theta_left;
	NTuple::Item<double>      m_theta_right;
	NTuple::Item<double>      m_rho_down;
	NTuple::Item<double>      m_rho_up;
	NTuple::Item<double>      m_rho;
	NTuple::Item<double>      m_theta;
	NTuple::Item<double>      m_rho_line;
	NTuple::Item<double>      m_theta_line;
	NTuple::Item<double>      m_height;
	NTuple::Item<double>      m_aver;
	NTuple::Item<double>      m_sigma;

	NTuple::Item<int>      m_exit_multiturn;

	NTuple::Item<int>      m_nMap1Pk;
	NTuple::Item<int>      m_nMap1Tk;
	NTuple::Item<int>      m_nMap2Pk;
	NTuple::Item<int>      m_nMap2Tk;
	NTuple::Array<double>     m_PkRho1;
	NTuple::Array<double>     m_PkRho2;
	NTuple::Array<double>     m_PkTheta1;
	NTuple::Array<double>     m_PkTheta2;
	NTuple::Array<double>     m_TkRho1;
	NTuple::Array<double>     m_TkRho2;
	NTuple::Array<double>     m_TkTheta1;
	NTuple::Array<double>     m_TkTheta2;

	NTuple::Item<int>         m_nMaxLayerSlant;
	NTuple::Array<double>     m_MaxLayerSlant;
	NTuple::Array<double>     m_MaxLayer;
	NTuple::Item<int>         m_nNoMaxLayerSlant;
	NTuple::Array<double>     m_NoMaxLayerSlant;
	NTuple::Array<int>        m_NoMaxLayerid;

	NTuple::Item<int>      m_MapMax;
	NTuple::Item<int>      m_nMapPk;
	NTuple::Array<int>         m_PeakOrder;
	NTuple::Array<double>      m_PeakRho;
	NTuple::Array<double>      m_PeakTheta;
	NTuple::Array<double>      m_PeakHeight;
	NTuple::Array<double>      m_PeakHit;
	NTuple::Array<double>      m_PeakHitA;
	NTuple::Item<int>      m_nMapTrk;
	NTuple::Array<double>      m_TrackRho;
	NTuple::Array<double>      m_TrackTheta;
	NTuple::Array<double>      m_TrackHitA;
   //rec - charge
	//global 2D 
	NTuple::Item<int>      m_nTrk2D_neg;
	NTuple::Array<double>      m_pt2D_neg;
	NTuple::Array<double>      m_nHit2D_neg;
	NTuple::Array<double>      m_chi2_2D_neg;
	//global 3D 
	NTuple::Item<int>      m_nTrk3D_neg;
	NTuple::Array<double>      m_tanl_neg;
	NTuple::Array<double>      m_tanl3D_neg;
	NTuple::Array<double>      m_z0_neg;
	NTuple::Array<double>      m_z0_3D_neg;
	NTuple::Array<double>      m_pro_neg;
	NTuple::Array<double>      m_pt3D_neg;
	NTuple::Array<double>      m_nHit3D_neg;
	NTuple::Array<double>      m_chi2_3D_neg;


	//mc 
	//NTuple::Item<int> m_nTrkMC;
	NTuple::Item<double> m_pidTruth;
	NTuple::Item<double> m_costaTruth;                                
	NTuple::Item<double> m_ptTruth;                                   
	NTuple::Item<double> m_pzTruth;                                   
	NTuple::Item<double> m_pTruth;                                    
	NTuple::Item<double> m_qTruth;    
	NTuple::Item<double> m_drTruth;                                   
	NTuple::Item<double> m_phi0Truth;                                 
	NTuple::Item<double> m_omegaTruth;                                 
	NTuple::Item<double> m_dzTruth;                                   
	NTuple::Item<double> m_tanl_mc;    
	NTuple::Item<double> m_rho_mc;    
	NTuple::Item<double> m_theta_mc;    

	NTuple::Item<double> m_time;    

	NTuple::Tuple*      ntuplehit;
	NTuple::Item<int>  		m_evt_stereo;    
	NTuple::Item<int>  		m_run_stereo;    
	NTuple::Item<double>  	m_cos_stereo;    
	NTuple::Item<double>  	m_tanlTruth_stereo;    
	NTuple::Item<double>  	m_ncir_stereo;    
	NTuple::Item<double>  	m_npair_stereo;    
	NTuple::Item<double>  	m_tanl_stereo;    
	NTuple::Item<double>  	m_tanl3D_stereo;    
	NTuple::Item<double>  	m_z0_stereo;    
	NTuple::Item<double>  	m_z03D_stereo;    
	NTuple::Item<int>  	    m_nHit_axial;    
	NTuple::Array<int>  	m_axial_layer;    
	NTuple::Array<int>  	m_axial_wire;    
	NTuple::Array<double>   m_axial_deltaD;    
	NTuple::Array<double>   m_axial_flt;    
	NTuple::Item<int>  	    m_nHit_stereo;    
	NTuple::Array<int>  	m_stereo_layer;    
	NTuple::Array<int>  	m_stereo_wire;    
	NTuple::Array<int>  	m_stereo_style;    
	NTuple::Array<double>   m_stereo_z0;    
	NTuple::Array<double>   m_stereo_z1;    
	NTuple::Array<double>   m_stereo_s0;    
	NTuple::Array<double>   m_stereo_s1;    
	NTuple::Array<double>   m_stereo_z;    
	NTuple::Array<double>   m_stereo_s;    
	NTuple::Array<double>   m_stereo_sTruth;    
	NTuple::Array<double>   m_stereo_zTruth;    
	NTuple::Array<double>   m_stereo_deltaZ;    
	NTuple::Array<int>   	m_stereo_nsol;    
	NTuple::Array<double>   m_stereo_disToCir;    
	NTuple::Array<int>   	m_stereo_cirlist;    
	NTuple::Array<int>      m_stereo_ambig;    
	NTuple::Array<int>      m_stereo_ambig_truth;    

};
bool more_pt(const HoughTrack* tracka,const HoughTrack* trackb);
#endif
