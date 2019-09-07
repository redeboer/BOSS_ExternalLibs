#include <vector>
// #include <iostream.h> 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "VertexFit/IVertexDbSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"

using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/IVertexDbSvc.h"
#include "ParticleID/ParticleID.h"


#include "DQASelBhabha/DQASelBhabha.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRecGeoSvc/EmcRecBarrelGeo.h"
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"



#include "Identifier/TofID.h"

#include "EvTimeEvent/RecEsTime.h"


using CLHEP::HepLorentzVector;
const double mpsi2s=3.68609;
const double mjpsi =3.09691;
const double mpi = 0.13957;
const double mk = 0.493677;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
//declare one counter
static int counter[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};
static  int nbhabha=0;
 
/////////////////////////////////////////////////////////////////////////////

DQASelBhabha::DQASelBhabha(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties 
  declareProperty("writentuple",m_writentuple = true );
  declareProperty("useVertexDB", m_useVertexDB = false );
  declareProperty("ecms",m_ecms = 3.097);
  declareProperty("beamangle",m_beamangle = 0.022); 
  declareProperty("Vr0cut", m_vr0cut=1.0);
  declareProperty("Vz0cut", m_vz0cut=10.0);
  declareProperty("Coscut", m_coscut=0.93);

  declareProperty("EnergyThreshold", m_energyThreshold=0.05);
  declareProperty("GammaPhiCut", m_gammaPhiCut=20.0);
  declareProperty("GammaThetaCut", m_gammaThetaCut=20.0);
  declareProperty("GammaTrkCut", m_gammaTrkCut=20.0);
  declareProperty("GammaTLCut", m_gammatlCut=0);
  declareProperty("GammaTHCut", m_gammathCut=14);
 

  declareProperty ("acoll_e_cut",  m_acoll_e_cut=6.);
  declareProperty ("acopl_e_cut",  m_acopl_e_cut=6.);
  declareProperty ("poeb_e_cut",  m_poeb_e_cut=0.5);  
  declareProperty ("dtof_e_cut",  m_dtof_e_cut=4.);
  declareProperty ("eoeb_e_cut",  m_eoeb_e_cut=0.4);
  declareProperty ("etotal_e_cut",  m_etotal_e_cut=0.8);
  declareProperty ("tpoeb_e_cut",  m_tpoeb_e_cut=0.95); 
  declareProperty ("tptotal_e_cut",  m_tptotal_e_cut=0.16);
  declareProperty ("tetotal_e_cut",  m_tetotal_e_cut=0.65);

        
  //normally, MDC+EMC, otherwise EMC only
  declareProperty ("m_useEMConly",  m_useEMConly= false);
  declareProperty ("m_usePID",  m_usePID= false);// sub-system is under study
  declareProperty ("m_useMDC",  m_useMDC= true);
  declareProperty ("m_useDEDX",  m_useDEDX= false);// not used
  declareProperty ("m_useTOF",  m_useTOF= false);//sub-system is under study
  declareProperty ("m_useEMC",  m_useEMC= true);
  declareProperty ("m_useMUC",  m_useMUC= false);// efficiency    

  declareProperty ("m_use_acolle", m_use_acolle = false);
  declareProperty ("m_use_acople",m_use_acople = false);
  declareProperty ("m_use_eoeb",m_use_eoeb  = false);
  declareProperty ("m_use_deltatof", m_use_deltatof = false);
  declareProperty ("m_use_poeb", m_use_poeb= false);
  declareProperty ("m_use_mucinfo",m_use_mucinfo= false);
  declareProperty ("m_use_ne",m_use_ne = false); 
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQASelBhabha::initialize(){

  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;
  status = service("THistSvc", m_thistsvc);
  if(status.isFailure() ){
    log << MSG::INFO << "Unable to retrieve pointer to THistSvc" << endreq;
    return status;
  }


  e_z0 = new TH1F("e_z0","e_z0",100,0,50);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/e_z0", e_z0);
  e_r0 = new TH1F("e_r0","e_r0",100,0,10);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/e_r0", e_r0);
  
  m_ee_mass = new TH1F( "ee_mass", "ee_mass", 500, m_ecms-0.5, m_ecms+0.5 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_mass", m_ee_mass);
  m_ee_acoll = new TH1F( "ee_acoll", "ee_acoll", 60, 0, 6 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_acoll", m_ee_acoll);
  m_ee_eop_ep = new TH1F( "ee_eop_ep", "ee_eop_ep", 100,0.4,1.4 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_eop_ep", m_ee_eop_ep);
  m_ee_eop_em = new TH1F( "ee_eop_em", "ee_eop_em", 100,0.4,1.4 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_eop_em", m_ee_eop_em);
  m_ee_costheta_ep = new TH1F( "ee_costheta_ep", "ee_costheta_ep", 100,-1,1 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_costheta_ep", m_ee_costheta_ep);
  m_ee_costheta_em = new TH1F( "ee_costheta_em", "ee_costheta_em", 100,-1,1 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_costheta_em", m_ee_costheta_em);

  m_ee_phi_ep = new TH1F( "ee_phi_ep", "ee_phi_ep", 120,-3.2,3.2 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_phi_ep", m_ee_phi_ep);
  m_ee_phi_em = new TH1F( "ee_phi_em", "ee_phi_em", 120,-3.2,3.2 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_phi_em", m_ee_phi_em);

  m_ee_nneu = new TH1I( "ee_nneu", "ee_nneu", 5,0,5 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_nneu", m_ee_nneu);



  m_ee_eemc_ep=new TH1F("ee_eemc_ep","ee_eemc_ep",100,m_ecms/2.-0.8,m_ecms/2.+0.3);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_eemc_ep", m_ee_eemc_ep);
  m_ee_eemc_em=new TH1F("ee_eemc_em","ee_eemc_em",100,m_ecms/2.-0.8,m_ecms/2.+0.3);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_eemc_em", m_ee_eemc_em);
  m_ee_x_ep=new TH1F("ee_x_ep","ee_x_ep",100,-1.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_x_ep", m_ee_x_ep);
  m_ee_y_ep=new TH1F("ee_y_ep","ee_y_ep",100,-1.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_y_ep", m_ee_y_ep);
  m_ee_z_ep=new TH1F("ee_z_ep","ee_z_ep",100,-10.0,10.0);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_z_ep", m_ee_z_ep);
  m_ee_x_em=new TH1F("ee_x_em","ee_x_em",100,-1.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_x_em", m_ee_x_em);
  m_ee_y_em=new TH1F("ee_y_em","ee_y_em",100,-1.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_y_em", m_ee_y_em);
  m_ee_z_em=new TH1F("ee_z_em","ee_z_em",100,-10.0,10.0);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_z_em", m_ee_z_em);

  m_ee_px_ep=new TH1F("ee_px_ep","ee_px_ep",200,-2.2,2.2);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_px_ep", m_ee_px_ep);
  m_ee_py_ep=new TH1F("ee_py_ep","ee_py_ep",200,-2.2,2.2);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_py_ep", m_ee_py_ep);
  m_ee_pz_ep=new TH1F("ee_pz_ep","ee_pz_ep",200,-2.,2.5);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_pz_ep", m_ee_pz_ep);
  m_ee_p_ep=new TH1F("ee_p_ep","ee_p_ep",100, m_ecms/2.-0.8 , m_ecms/2.+0.5 );
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_p_ep", m_ee_p_ep);
  m_ee_px_em=new TH1F("ee_px_em","ee_px_em",100,-2.2,2.2);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_px_em", m_ee_px_em);
  m_ee_py_em=new TH1F("ee_py_em","ee_py_em",100,-2.2,2.2);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_py_em", m_ee_py_em);
  m_ee_pz_em=new TH1F("ee_pz_em","ee_pz_em",100,-2.5,2.);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_pz_em", m_ee_pz_em);
  m_ee_p_em=new TH1F("ee_p_em","ee_p_em",100,m_ecms/2.-0.8,m_ecms/2.+0.5);  
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_p_em", m_ee_p_em);
  m_ee_deltatof=new TH1F("ee_deltatof","ee_deltatof",50,0.0,10.0);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_deltatof", m_ee_deltatof);

  m_ee_pidchidedx_ep=new TH1F("ee_pidchidedx_ep","ee_pidchidedx_ep",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_pidchidedx_ep", m_ee_pidchidedx_ep);
  m_ee_pidchidedx_em=new TH1F("ee_pidchidedx_em","ee_pidchidedx_em",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_pidchidedx_em", m_ee_pidchidedx_em);
  m_ee_pidchitof1_ep=new TH1F("ee_pidchitof1_ep","ee_pidchitof1_ep",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_pidchitof1_ep", m_ee_pidchitof1_ep);
  m_ee_pidchitof1_em=new TH1F("ee_pidchitof1_em","ee_pidchitof1_em",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_pidchitof1_em", m_ee_pidchitof1_em);
  m_ee_pidchitof2_ep=new TH1F("ee_pidchitof2_ep","ee_pidchitof2_ep",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_pidchitof2_ep", m_ee_pidchitof2_ep);
  m_ee_pidchitof2_em=new TH1F("ee_pidchitof2_em","ee_pidchitof2_em",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Bhabha/ee_pidchitof2_em", m_ee_pidchitof2_em);

 
  
  NTuplePtr nt1(ntupleSvc(), "DQAFILE/Bhabha");
  if ( nt1 ) m_tuple1 = nt1;
  else {
    m_tuple1 = ntupleSvc()->book ("DQAFILE/Bhabha", CLID_ColumnWiseTuple, "N-Tuple");
    if ( m_tuple1 )    {
      status = m_tuple1->addItem ("run",   m_run);
      status = m_tuple1->addItem ("rec", m_rec);
      status = m_tuple1->addItem ("Nchrg",  m_ncharg);
      status = m_tuple1->addItem ("Nneu",   m_nneu,0,40);
      status = m_tuple1->addItem ("NGch",   m_ngch, 0, 40);
      status = m_tuple1->addItem ("NGam",   m_nGam);
      
	 
      status = m_tuple1->addItem ("bhabhatag",   m_bhabhatag);

      status = m_tuple1->addItem ("acoll",  m_acoll);
      status = m_tuple1->addItem ("acopl",  m_acopl);
      status = m_tuple1->addItem ("deltatof",  m_deltatof);
      status = m_tuple1->addItem ("eop1",  m_eop1);
      status = m_tuple1->addItem ("eop2",  m_eop2);
      status = m_tuple1->addItem ("eoeb1",  m_eoeb1);
      status = m_tuple1->addItem ("eoeb2",  m_eoeb2);
      status = m_tuple1->addItem ("poeb1",  m_poeb1);
      status = m_tuple1->addItem ("poeb2",  m_poeb2);
      status = m_tuple1->addItem ("etoeb1",  m_etoeb1);
      status = m_tuple1->addItem ("etoeb2",  m_etoeb2);
      status = m_tuple1->addItem ("mucinfo1",  m_mucinfo1);
      status = m_tuple1->addItem ("mucinfo2",  m_mucinfo2);



      status = m_tuple1->addIndexedItem ("delang",m_nneu,  m_delang);
      status = m_tuple1->addIndexedItem ("delphi",m_nneu,  m_delphi);
      status = m_tuple1->addIndexedItem ("delthe",m_nneu,  m_delthe);
      status = m_tuple1->addIndexedItem ("npart",m_nneu,   m_npart);
      status = m_tuple1->addIndexedItem ("nemchits",m_nneu,   m_nemchits);
      status = m_tuple1->addIndexedItem ("module",m_nneu,   m_module);
      status = m_tuple1->addIndexedItem ("x",m_nneu,   m_x);
      status = m_tuple1->addIndexedItem ("y",m_nneu,   m_y);   
      status = m_tuple1->addIndexedItem ("z",m_nneu,   m_z);
      status = m_tuple1->addIndexedItem ("px",m_nneu,   m_px);
      status = m_tuple1->addIndexedItem ("py",m_nneu,   m_py);   
      status = m_tuple1->addIndexedItem ("pz",m_nneu,   m_pz);
      status = m_tuple1->addIndexedItem ("theta",m_nneu,   m_theta);
      status = m_tuple1->addIndexedItem ("phi",m_nneu,   m_phi);
      status = m_tuple1->addIndexedItem ("dx",m_nneu,   m_dx);
      status = m_tuple1->addIndexedItem ("dy",m_nneu,   m_dy);
      status = m_tuple1->addIndexedItem ("dz",m_nneu,   m_dz);
      status = m_tuple1->addIndexedItem ("dtheta",m_nneu,   m_dtheta);
      status = m_tuple1->addIndexedItem ("dphi",m_nneu,   m_dphi);
      status = m_tuple1->addIndexedItem ("energy",m_nneu,   m_energy);
      status = m_tuple1->addIndexedItem ("dE",m_nneu,   m_dE);
      status = m_tuple1->addIndexedItem ("eSeed",m_nneu,   m_eSeed);
      status = m_tuple1->addIndexedItem ("nSeed",m_nneu,   m_nSeed);
      status = m_tuple1->addIndexedItem ("e3x3",m_nneu,   m_e3x3);   
      status = m_tuple1->addIndexedItem ("e5x5",m_nneu,   m_e5x5);
      status = m_tuple1->addIndexedItem ("secondMoment",m_nneu,   m_secondMoment); 
      status = m_tuple1->addIndexedItem ("latMoment",m_nneu,   m_latMoment);
      status = m_tuple1->addIndexedItem ("a20Moment",m_nneu,   m_a20Moment);
      status = m_tuple1->addIndexedItem ("a42Moment",m_nneu,   m_a42Moment);
      status = m_tuple1->addIndexedItem ("getTime",m_nneu,   m_getTime);   
      status = m_tuple1->addIndexedItem ("getEAll",m_nneu,   m_getEAll);

   

      status = m_tuple1->addIndexedItem("charge", m_ngch, m_charge);
      status = m_tuple1->addIndexedItem ("vx",    m_ngch, m_vx0);
      status = m_tuple1->addIndexedItem ("vy",    m_ngch, m_vy0);
      status = m_tuple1->addIndexedItem ("vz",    m_ngch, m_vz0);
      status = m_tuple1->addIndexedItem ("r0",    m_ngch, m_vr0);

      status = m_tuple1->addIndexedItem ("px",    m_ngch, m_px) ;
      status = m_tuple1->addIndexedItem ("py",    m_ngch, m_py) ;
      status = m_tuple1->addIndexedItem ("pz",    m_ngch, m_pz) ;
      status = m_tuple1->addIndexedItem ("p",     m_ngch, m_p)  ;



      status = m_tuple1->addIndexedItem ("kal_vx",    m_ngch, m_kal_vx0);
      status = m_tuple1->addIndexedItem ("kal_vy",    m_ngch, m_kal_vy0);
      status = m_tuple1->addIndexedItem ("kal_vz",    m_ngch, m_kal_vz0);


      status = m_tuple1->addIndexedItem ("kal_px",    m_ngch, m_kal_px) ;
      status = m_tuple1->addIndexedItem ("kal_py",    m_ngch, m_kal_py) ;
      status = m_tuple1->addIndexedItem ("kal_pz",    m_ngch, m_kal_pz) ;
      status = m_tuple1->addIndexedItem ("kal_p",     m_ngch, m_kal_p)  ;


      status = m_tuple1->addIndexedItem ("probPH" , m_ngch, m_probPH) ;
      status = m_tuple1->addIndexedItem ("normPH" , m_ngch, m_normPH) ;
      status = m_tuple1->addIndexedItem ("chie"   , m_ngch, m_chie)   ;
      status = m_tuple1->addIndexedItem ("chimu"  , m_ngch, m_chimu)  ;
      status = m_tuple1->addIndexedItem ("chipi"  , m_ngch, m_chipi)  ;
      status = m_tuple1->addIndexedItem ("chik"   , m_ngch, m_chik)   ;
      status = m_tuple1->addIndexedItem ("chip"   , m_ngch, m_chip)   ;
      status = m_tuple1->addIndexedItem ("ghit"   , m_ngch, m_ghit)   ;
      status = m_tuple1->addIndexedItem ("thit"   , m_ngch, m_thit)   ;

      status = m_tuple1->addIndexedItem ("e_emc"   , m_ngch, m_e_emc)   ;
      status = m_tuple1->addIndexedItem ("phi_emc"   , m_ngch, m_phi_emc)   ;
      status = m_tuple1->addIndexedItem ("theta_emc"   , m_ngch, m_theta_emc)   ;

      status = m_tuple1->addIndexedItem ("nhit_muc"   , m_ngch, m_nhit_muc)   ;
      status = m_tuple1->addIndexedItem ("nlay_muc"   , m_ngch, m_nlay_muc)   ;      
      status = m_tuple1->addIndexedItem ("t_btof" , m_ngch,   m_t_btof  );
      status = m_tuple1->addIndexedItem ("t_etof" , m_ngch,   m_t_etof  );
      status = m_tuple1->addIndexedItem ("qual_etof" , m_ngch,   m_qual_etof  );
      status = m_tuple1->addIndexedItem ("tof_etof"  , m_ngch,   m_tof_etof   );
      status = m_tuple1->addIndexedItem ("te_etof"   , m_ngch,   m_te_etof    );
      status = m_tuple1->addIndexedItem ("tmu_etof"  , m_ngch,   m_tmu_etof   );
      status = m_tuple1->addIndexedItem ("tpi_etof"  , m_ngch,   m_tpi_etof   );
      status = m_tuple1->addIndexedItem ("tk_etof"   , m_ngch,   m_tk_etof    );
      status = m_tuple1->addIndexedItem ("tp_etof"   , m_ngch,   m_tp_etof    );

      status = m_tuple1->addIndexedItem ("qual_btof1", m_ngch,   m_qual_btof1 );
      status = m_tuple1->addIndexedItem ("tof_btof1" , m_ngch,   m_tof_btof1  );
      status = m_tuple1->addIndexedItem ("te_btof1"  , m_ngch,   m_te_btof1   );
      status = m_tuple1->addIndexedItem ("tmu_btof1" , m_ngch,   m_tmu_btof1  );
      status = m_tuple1->addIndexedItem ("tpi_btof1" , m_ngch,   m_tpi_btof1  );
      status = m_tuple1->addIndexedItem ("tk_btof1"  , m_ngch,   m_tk_btof1   );
      status = m_tuple1->addIndexedItem ("tp_btof1"  , m_ngch,   m_tp_btof1   );

      status = m_tuple1->addIndexedItem ("qual_btof2", m_ngch,   m_qual_btof2 );
      status = m_tuple1->addIndexedItem ("tof_btof2" , m_ngch,   m_tof_btof2  );
      status = m_tuple1->addIndexedItem ("te_btof2"  , m_ngch,   m_te_btof2   );
      status = m_tuple1->addIndexedItem ("tmu_btof2" , m_ngch,   m_tmu_btof2  );
      status = m_tuple1->addIndexedItem ("tpi_btof2" , m_ngch,   m_tpi_btof2  );
      status = m_tuple1->addIndexedItem ("tk_btof2"  , m_ngch,   m_tk_btof2   );
      status = m_tuple1->addIndexedItem ("tp_btof2"  , m_ngch,   m_tp_btof2   );
      status = m_tuple1->addIndexedItem ("pidcode"  , m_ngch,       m_pidcode);
      status = m_tuple1->addIndexedItem ("pidprob"  , m_ngch,       m_pidprob);
      status = m_tuple1->addIndexedItem ("pidchiDedx"  , m_ngch,       m_pidchiDedx);
      status = m_tuple1->addIndexedItem ("pidchiTof1"  , m_ngch,       m_pidchiTof1);
      status = m_tuple1->addIndexedItem ("pidchiTof2"  , m_ngch,       m_pidchiTof2);
      

      status = m_tuple1->addItem ("dedx_GoodHits_ep"  ,    m_dedx_goodhits_ep);
      status = m_tuple1->addItem ("dedx_chi_ep"  ,     m_dedx_chiep);     
      status = m_tuple1->addItem ("dedx_GoodHits_em"  , m_dedx_goodhits_em);
      status = m_tuple1->addItem ("dedx_chi_em"  ,      m_dedx_chiem);
      
      status = m_tuple1->addItem ("px_cms_ep",   m_px_cms_ep); //momentum of electron+
      status = m_tuple1->addItem ("py_cms_ep",   m_py_cms_ep); //momentum of electron+
      status = m_tuple1->addItem ("pz_cms_ep",   m_pz_cms_ep); //momentum of electron+
      status = m_tuple1->addItem ("e_cms_ep",   m_e_cms_ep); //momentum of electron+
      status = m_tuple1->addItem ("p_cms_ep",   m_p_cms_ep); //momentum of electron+

      status = m_tuple1->addItem ("cos_ep",   m_cos_ep); //momentum of electron+
      status = m_tuple1->addItem ("kal_p_ep",   m_kal_p_ep); //momentum of electron+  
      status = m_tuple1->addItem ("kal_px_ep",   m_kal_px_ep); //momentum of electron+  
      status = m_tuple1->addItem ("kal_py_ep",   m_kal_py_ep); //momentum of electron+  
      status = m_tuple1->addItem ("kal_pz_ep",   m_kal_pz_ep); //momentum of electron+  
    
      status = m_tuple1->addItem ("px_cms_em",   m_px_cms_em); //momentum of electron-
      status = m_tuple1->addItem ("py_cms_em",   m_py_cms_em); //momentum of electron-
      status = m_tuple1->addItem ("pz_cms_em",   m_pz_cms_em); //momentum of electron-
      status = m_tuple1->addItem ("e_cms_em",   m_e_cms_em); //momentum of electron-
      status = m_tuple1->addItem ("p_cms_em",   m_p_cms_em); //momentum of electron-
    
      status = m_tuple1->addItem ("cos_em",   m_cos_em); //momentum of electron-
      status = m_tuple1->addItem ("kal_p_em",   m_kal_p_em); //momentum of electron-
      status = m_tuple1->addItem ("kal_px_em",   m_kal_px_em); //momentum of electron-
      status = m_tuple1->addItem ("kal_py_em",   m_kal_py_em); //momentum of electron-
      status = m_tuple1->addItem ("kal_pz_em",   m_kal_pz_em); //momentum of electron-
      
      status = m_tuple1->addItem ("mass_ee",   m_mass_ee); // 
      status = m_tuple1->addItem ("px_ee",   m_px_ee); // 
      status = m_tuple1->addItem ("py_ee",   m_py_ee); // 
      status = m_tuple1->addItem ("pz_ee",   m_pz_ee); // 
      status = m_tuple1->addItem ("e_ee",   m_e_ee); // 
      status = m_tuple1->addItem ("p_ee",   m_p_ee); //

      status = m_tuple1->addItem ( "nep", m_nep );
      status = m_tuple1->addItem ( "nem", m_nem );


    }
    else    { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  //
  //--------end of book--------
  //

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQASelBhabha::execute() {
  const double beamEnergy = m_ecms/2.;
  const HepLorentzVector p_cms(m_ecms*sin(m_beamangle*0.5),0.0,0.0,m_ecms);
  const Hep3Vector u_cms = -p_cms.boostVector(); 
  counter[0]++;  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  setFilterPassed(false);
  
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) 
    {
      log << MSG::FATAL << "Could not find Event Header" << endreq;
      return StatusCode::SUCCESS;
    }

  m_run = eventHeader->runNumber();
  m_rec = eventHeader->eventNumber();
 


  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  if (!evtRecEvent) 
    {
      log << MSG::FATAL << "Could not find EvtRecEvent" << endreq;
      return StatusCode::SUCCESS;
    }
  log << MSG::INFO <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;

  m_ncharg  = evtRecEvent->totalCharged();
  m_nneu = evtRecEvent->totalNeutral();


  HepPoint3D vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  if (m_useVertexDB) {
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
   if(vtxsvc->isVertexValid()){
    double* dbv = vtxsvc->PrimaryVertex(); 
    double*  vv = vtxsvc->SigmaPrimaryVertex(); 
    vx.setX(dbv[0]);
    vx.setY(dbv[1]);
    vx.setZ(dbv[2]);
    Evx[0][0]=vv[0]*vv[0];
    Evx[0][1]=vv[0]*vv[1];
    Evx[1][1]=vv[1]*vv[1];
    Evx[1][2]=vv[1]*vv[2];
    Evx[2][2]=vv[2]*vv[2];
  }
  }

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
  if (!evtRecTrkCol) 
    {
      log << MSG::FATAL << "Could not find EvtRecTrackCol" << endreq;
      return StatusCode::SUCCESS;
    }
  Vint iGood;
  iGood.clear();
  
  int nCharge = 0;

  for(int i = 0; i < evtRecEvent->totalCharged(); i++){
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    if(!(*itTrk)->isMdcKalTrackValid()) continue;

    RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
    double pch=mdcTrk->p();
    double x0=mdcTrk->x();
    double y0=mdcTrk->y();
    double z0=mdcTrk->z();
    double phi0=mdcTrk->helix(1);
    double xv=vx.x();
    double yv=vx.y();
    double zv=vx.z();
    double Rxy=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);
    double m_vx0 = x0;
    double m_vy0 = y0;
    double m_vz0 = z0;
    double m_vr0 = Rxy;

    e_z0->Fill(z0);
    if(fabs(z0) >= m_vz0cut) continue;
    e_r0->Fill(Rxy);    
    if(fabs(Rxy) >= m_vr0cut) continue;

    iGood.push_back(i);
    nCharge += mdcTrk->charge();

  }

  int nGood = iGood.size();
  m_ngch=nGood;
  log << MSG::DEBUG << "ngood, totcharge = " << nGood << " , " << nCharge << endreq;

   if((nGood != 2)||(nCharge!=0)){
     return StatusCode::SUCCESS;
   }


 
  counter[1]++;

  //
  // Finish Good Charged Track Selection
  //




  //
  // Particle ID
  // 
  Vint ipip, ipim, iep,iem,imup,imum;
  ipip.clear();
  ipim.clear();
  iep.clear();
  iem.clear();
  imup.clear();
  imum.clear();  
  

  ParticleID *pid = ParticleID::instance();

  for(int i = 0; i < m_ngch; i++) {
    m_pidcode[i]=-99;
    m_pidprob[i]=-99;
    m_pidchiDedx[i]=-99;
    m_pidchiTof1[i]=-99;
    m_pidchiTof2[i]=-99;

    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 

    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2());//|pid->useEmc()|pid->useMuc()); // use PID sub-system
    pid->identify(pid->onlyElectron()|pid->onlyMuon()|pid->onlyPion());                 // seperater Pion/Kaon/Proton
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();

    double  prob_pi = pid->probPion();
    double  prob_K  = pid->probKaon();
    double  prob_p  = pid->probProton();
    double  prob_e  = pid->probElectron();
    double  prob_mu = pid->probMuon();    
    
    HepLorentzVector ptrk;
    ptrk.setPx(mdcTrk->px()) ; 
    ptrk.setPy(mdcTrk->py()) ;
    ptrk.setPz(mdcTrk->pz()) ; 
    double p3 = ptrk.mag()   ; 
 
    m_pidcode[i]=0;
    m_pidprob[i]=pid->prob(0);
    m_pidchiDedx[i]=pid->chiDedx(0);
    m_pidchiTof1[i]=pid->chiTof1(0);
    m_pidchiTof2[i]=pid->chiTof2(0);
    if(mdcTrk->charge() > 0) {
      iep.push_back(iGood[i]);    
    }
    if (mdcTrk->charge() < 0) {
      iem.push_back(iGood[i]);
    }
  }
  
  m_nep  = iep.size() ; 
  m_nem = iem.size() ;
  
  counter[2]++;


  // 
  // Good neutral track selection
  //
  Vint iGam;
  iGam.clear();
  int iphoton=0;
  for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
    if(i>=evtRecTrkCol->size())break;
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
           
    RecEmcID showerId = emcTrk->getShowerId();
    unsigned int npart = EmcID::barrel_ec(showerId);
    int n = emcTrk->numHits();
    int module=emcTrk->module(); 
    double x = emcTrk->x();
    double y = emcTrk->y();
    double z = emcTrk->z();
    double dx = emcTrk->dx();
    double dy = emcTrk->dy();
    double dth = emcTrk->dtheta();
    double dph = emcTrk->dphi();
    double dz = emcTrk->dz();
    double energy = emcTrk->energy();
    double dE = emcTrk->dE();
    double eSeed = emcTrk->eSeed();
    double e3x3 = emcTrk->e3x3();
    double e5x5 = emcTrk->e5x5();
    double secondMoment = emcTrk->secondMoment();
    double latMoment = emcTrk->latMoment();
    double getTime = emcTrk->time();
    double getEAll = emcTrk->getEAll();
    double a20Moment = emcTrk->a20Moment();
    double a42Moment = emcTrk->a42Moment();
    double nseed=0;

    HepPoint3D EmcPos(x,y,z);
    m_nemchits[iphoton]=n;
    m_npart[iphoton]=npart;
    m_module[iphoton]=module;
    m_theta[iphoton]=EmcPos.theta();
    m_phi[iphoton]=EmcPos.phi();
    m_x[iphoton]=x;
    m_y[iphoton]=y;
    m_z[iphoton]=z;
    m_dx[iphoton]=dx;
    m_dy[iphoton]=dy;
    m_dz[iphoton]=dz;
    m_dtheta[iphoton]=dth;
    m_dphi[iphoton]=dph;
    m_energy[iphoton]=energy;
    m_dE[iphoton]=dE;
    m_eSeed[iphoton]=eSeed;
    m_nSeed[iphoton]=nseed;
    m_e3x3[iphoton]=e3x3;
    m_e5x5[iphoton]=e5x5;
    m_secondMoment[iphoton]=secondMoment;
    m_latMoment[iphoton]=latMoment;
    m_getTime[iphoton]=getTime;
    m_getEAll[iphoton]=getEAll;
    m_a20Moment[iphoton]=a20Moment;
    m_a42Moment[iphoton]=a42Moment;
    
    double dthe = 200.;
    double dphi = 200.;
    double dang = 200.; 
	    
    // find the nearest charged track
    for(int j = 0; j < nGood; j++) {
      EvtRecTrackIterator jtTrk = evtRecTrkCol->begin() +iGood[j];
      if (!(*jtTrk)->isMdcTrackValid()) continue; 
      RecMdcTrack *jtmdcTrk = (*jtTrk)->mdcTrack(); 
      double jtcharge = jtmdcTrk->charge(); 
      if(!(*jtTrk)->isExtTrackValid()) continue;
      RecExtTrack *extTrk = (*jtTrk)->extTrack();
      if(extTrk->emcVolumeNumber() == -1) continue;
      Hep3Vector extpos = extTrk->emcPosition();
      //      double ctht = extpos.cosTheta(emcpos);
      double angd = extpos.angle(emcpos);
      double thed = extpos.theta() - emcpos.theta();
      double phid = extpos.deltaPhi(emcpos);
      thed = fmod(thed+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;
      phid = fmod(phid+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;
      
      if(fabs(thed) < fabs(dthe)) dthe = thed;
      if(fabs(phid) < fabs(dphi)) dphi = phid;
      if(angd < dang) dang = angd;
      
    }
    
    

    //
    // good photon cut will be set here
    //
    dthe = dthe * 180 / (CLHEP::pi);
    dphi = dphi * 180 / (CLHEP::pi);
    dang = dang * 180 / (CLHEP::pi);     
    double eraw = emcTrk->energy();
    double theta1 = emcTrk->theta();
    double phi = emcTrk->phi();
    double the = emcTrk->theta();     
   
    m_delphi[iphoton]=dphi;
    m_delthe[iphoton]=dthe;
    m_delang[iphoton]=dang;
//    if(energy < m_energyThreshold) continue;
    double fc_theta = fabs(cos(theta1));
    if (fc_theta < 0.80 ) { // Barrel EMC
       if ( eraw <= m_energyThreshold/2) continue;
    } else if (fc_theta > 0.86 && fc_theta < 0.92 ) { // Endcap EMC
       if (eraw <= m_energyThreshold ) continue;
    }
    else continue;
    
    if(getTime>m_gammathCut||getTime<m_gammatlCut)continue;
    
    //     if((fabs(dthe) < m_gammaThetaCut) && (fabs(dphi)<m_gammaPhiCut) ) continue;
    if(dang< m_gammaTrkCut) continue; 
   
    iphoton++;  
    iGam.push_back(i);
    if(iphoton>=40)return StatusCode::SUCCESS; 
  }

  if(iphoton>0) counter[4]++;
  int nGam = iGam.size();
  m_nGam=nGam;

  counter[3]++;

  double egam_ext=0;
  double ex_gam=0;
  double ey_gam=0;
  double ez_gam=0;  
  double et_gam=0;
  double e_gam=0;
  for(int i = 0; i < m_nGam; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin()+ iGam[i]; 
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower* emcTrk = (*itTrk)->emcShower();
    double eraw = emcTrk->energy();
    double phi = emcTrk->phi();
    double the = emcTrk->theta();
    HepLorentzVector ptrk;
    ex_gam+=eraw*sin(the)*cos(phi);
    ey_gam+=eraw*sin(the)*sin(phi);
    ez_gam+=eraw*cos(the);
    et_gam+=eraw*sin(the);
    e_gam+=eraw  ;  
    if(eraw>=egam_ext)
      {
	egam_ext=eraw;           
      }

  }





  double  px_had=0;
  double  py_had=0;
  double  pz_had=0;
  double  t_pxy2 = 0;
  double  pt_had=0;
  double  p_had=0;
  double  e_had=0;

  //
  // check good charged track's infomation
  //
  int ii ;
  m_e_emc[0]=-0.1;
  m_e_emc[1]=-0.1;
  for(int i = 0; i < m_ngch; i++ ){

    EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];

    if(!(*itTrk)->isMdcTrackValid()) continue;  //  MDC information 
    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    // if(!(*itTrk)->isEmcShowerValid()) return StatusCode::SUCCESS;///dbg
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();
 
    m_charge[i] = mdcTrk->charge();
    m_vx0[i]    = mdcTrk->x();
    m_vy0[i]    = mdcTrk->y();
    m_vz0[i]    = mdcTrk->z();

    m_px[i]   = mdcTrk->px();
    m_py[i]   = mdcTrk->py();
    m_pz[i]   = mdcTrk->pz();
    m_p[i]    = mdcTrk->p();


    mdcKalTrk->setPidType(RecMdcKalTrack::electron);
    
   
    ///    if(m_pidcode[i]==3)mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
    ///    if(m_pidcode[i]==4)mdcKalTrk->setPidType(RecMdcKalTrack::proton);

    //m_kal_vx0[i]    = mdcKalTrk->x();
    //m_kal_vy0[i]    = mdcKalTrk->y();
    //m_kal_vz0[i]    = mdcKalTrk->z();

    m_kal_vx0[i]= mdcKalTrk->dr()*cos(mdcKalTrk->fi0()); 
    
    m_kal_vy0[i] = mdcKalTrk->dr()*sin(mdcKalTrk->fi0()); 
    
    m_kal_vz0[i]= mdcKalTrk->dz(); 

    
    m_kal_px[i]   = mdcKalTrk->px();
    m_kal_py[i]   = mdcKalTrk->py();
    m_kal_pz[i]   = mdcKalTrk->pz();
// pxy() and p() are not filled in the reconstruction algorithm
    t_pxy2 = m_kal_px[i]*m_kal_px[i] + m_kal_py[i]*m_kal_py[i]; 
    m_kal_p[i]    = sqrt(t_pxy2 + m_kal_pz[i]*m_kal_pz[i]);
    double ptrk = m_kal_p[i];
    px_had+=m_kal_px[i];
    py_had+=m_kal_py[i];
    pz_had+=m_kal_pz[i];
    pt_had+=sqrt(t_pxy2);
    p_had+=m_kal_p[i];
    e_had+=sqrt(m_kal_p[i]*m_kal_p[i]+mdcKalTrk->mass()*mdcKalTrk->mass());


    if((*itTrk)->isMdcDedxValid()) { // DEDX information 

      RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
      m_probPH[i]= dedxTrk->probPH();
      m_normPH[i]= dedxTrk->normPH();
      
      m_chie[i]  = dedxTrk->chiE();
      m_chimu[i] = dedxTrk->chiMu();
      m_chipi[i] = dedxTrk->chiPi();
      m_chik[i]  = dedxTrk->chiK();
      m_chip[i]  = dedxTrk->chiP();
      m_ghit[i]  = dedxTrk->numGoodHits();
      m_thit[i]  = dedxTrk->numTotalHits();
    }
      
    if((*itTrk)->isEmcShowerValid()) {
      RecEmcShower *emcTrk = (*itTrk)->emcShower();
      m_e_emc[i] = emcTrk->energy();
      m_phi_emc[i] = emcTrk->phi();
      m_theta_emc[i] = emcTrk->theta();      
    }
    
      m_nhit_muc[i] = 0; 
      m_nlay_muc[i] = 0;    

    if((*itTrk)->isMucTrackValid()){
      RecMucTrack* mucTrk = (*itTrk)->mucTrack()   ;
      m_nhit_muc[i] = mucTrk->numHits() ; 
      m_nlay_muc[i] = mucTrk->numLayers()    ;
    }    
 
    if((*itTrk)->isTofTrackValid()) {  //TOF information

      SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
      SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();

      for(;iter_tof != tofTrkCol.end(); iter_tof++ ) {
	m_t_etof[i]=0;
	m_t_btof[i]=0;
        TofHitStatus *status = new TofHitStatus; 
        status->setStatus((*iter_tof)->status());
        if(!(status->is_barrel())){//endcap
	  if( (status->is_cluster()) )  m_t_etof[i]  = (*iter_tof)->tof();
          if( !(status->is_counter()) ){if(status) delete status; continue; }// ? 
          if( status->layer()!=0 ) {if(status) delete status;continue;}//layer1

          double path=(*iter_tof)->path(); // ? 
          double tof  = (*iter_tof)->tof();
          double ph   = (*iter_tof)->ph();
          double rhit = (*iter_tof)->zrhit();
          double qual = 0.0 + (*iter_tof)->quality();
          double cntr = 0.0 + (*iter_tof)->tofID();
          double texp[5];
          for(int j = 0; j < 5; j++) {
            double gb = ptrk/xmass[j];
            double beta = gb/sqrt(1+gb*gb);
            texp[j] = path /beta/velc;
          }
           m_qual_etof[i]  = qual;
	  m_tof_etof[i]   = tof ; 
	}
        else {//barrel
	  if( (status->is_cluster()) )  m_t_btof[i]  = (*iter_tof)->tof();
          if( !(status->is_counter()) ){if(status) delete status; continue;} // ? 
          if(status->layer()==1){ //layer1
            double path=(*iter_tof)->path(); // ? 
            double tof  = (*iter_tof)->tof();
            double ph   = (*iter_tof)->ph();
            double rhit = (*iter_tof)->zrhit();
            double qual = 0.0 + (*iter_tof)->quality();
            double cntr = 0.0 + (*iter_tof)->tofID();
            double texp[5];
            for(int j = 0; j < 5; j++) {
              double gb = ptrk/xmass[j];
              double beta = gb/sqrt(1+gb*gb);
              texp[j] = path /beta/velc;
            }
	    m_qual_btof1[i]  = qual;
	    m_tof_btof1[i]   = tof ; 
          }

          if(status->layer()==2){//layer2
            double path=(*iter_tof)->path();  
            double tof  = (*iter_tof)->tof();
            double ph   = (*iter_tof)->ph();
            double rhit = (*iter_tof)->zrhit();
            double qual = 0.0 + (*iter_tof)->quality();
            double cntr = 0.0 + (*iter_tof)->tofID();
            double texp[5];
            for(int j = 0; j < 5; j++) {
              double gb = ptrk/xmass[j];
              double beta = gb/sqrt(1+gb*gb);
              texp[j] = path /beta/velc;
            }
             m_qual_btof2[i]  = qual;
	    m_tof_btof2[i]   = tof ; 
          } 
	}
	if(status) delete status;
      }
    }

  }
  counter[5]++;



  m_bhabhatag=0;

  if(m_ngch != 2 || nCharge != 0 ) return StatusCode::SUCCESS;  
    EvtRecTrackIterator  itTrk1; 
    EvtRecTrackIterator  itTrk2; 
    RecMdcKalTrack *mdcKalTrk1;
    RecMdcKalTrack *mdcKalTrk2;
	
    HepLorentzVector p41e,p42e,p4le;
    Hep3Vector p31e,p32e,p3le;
    HepLorentzVector p41m,p42m,p4lm;
    Hep3Vector p31m,p32m,p3lm;  
    HepLorentzVector p41h,p42h,p4lh;
    Hep3Vector p31h,p32h,p3lh;  
    WTrackParameter w1_ini;
    WTrackParameter w2_ini;

    int iip=-1;
    int iim=-1;
    int mucinfo1=0;
    int mucinfo2=0;
    
    double e01=0;
    double e02=0;
    double eope1=0;    
    double eope2=0;
    double eopl=0;
    double deltatof=0;


    double ex1, ey1, ez1, epx1,epy1, epz1,epp1,pidchidedx1,pidchitof11,pidchitof21,eoeb1,exoeb1,eyoeb1,ezoeb1,etoeb1,kalpp,cmsp;
    double ex2, ey2, ez2, epx2,epy2, epz2,epp2,pidchidedx2,pidchitof12,pidchitof22,eoeb2,exoeb2,eyoeb2,ezoeb2,etoeb2,kalpm,cmsm;


    for(int i = 0; i < m_ngch; i++ ){  
      if(m_charge[i]>0){
	itTrk1= evtRecTrkCol->begin() + iGood[i];
        mdcKalTrk1 = (*itTrk1)->mdcKalTrack();
	 
	if(!(*itTrk1)->isMdcDedxValid())continue;
	RecMdcDedx* dedxTrk1 = (*itTrk1)->mdcDedx();    

	m_dedx_goodhits_ep=dedxTrk1->numGoodHits();
	m_dedx_chiep=dedxTrk1->chiE();

	iip=i;
	
	w1_ini=WTrackParameter (xmass[0],mdcKalTrk1->getZHelixE(),mdcKalTrk1->getZErrorE());
	p41e =w1_ini.p();

        p41e.boost(u_cms);
	p31e = p41e.vect();
	
	m_px_cms_ep=p41e.px();
	m_py_cms_ep=p41e.py();
	m_pz_cms_ep=p41e.pz();
	m_e_cms_ep=p41e.e();
	m_p_cms_ep=sqrt(p41e.px()*p41e.px()+p41e.py()*p41e.py()+p41e.pz()*p41e.pz());
	cmsp=m_p_cms_ep;
	
	m_kal_p_ep=m_kal_p[i]; 
	kalpp=m_kal_p_ep;
	e01=m_e_emc[i];

	ex1=m_kal_vx0[i];
	ey1=m_kal_vy0[i];
	ez1=m_kal_vz0[i];
	epx1=m_kal_px[i];
	epy1=m_kal_py[i];
	epz1=m_kal_pz[i];
	epp1=m_kal_p[i];
	
	m_kal_px_ep=epx1;
	m_kal_py_ep=epy1;
	m_kal_pz_ep=epz1;
	
	pidchidedx1=m_pidchiDedx[i];
	pidchitof11=m_pidchiTof1[i];
	pidchitof21=m_pidchiTof2[i];

	eoeb1=m_e_emc[i]/beamEnergy; 

	if(p41e.rho()>0)  eope1=m_e_emc[i]/p41e.rho();  

	  
	exoeb1=m_e_emc[i]*sin(m_theta_emc[i])*cos(m_phi_emc[i])/beamEnergy;
	eyoeb1=m_e_emc[i]*sin(m_theta_emc[i])*sin(m_phi_emc[i])/beamEnergy;   	
	ezoeb1=m_e_emc[i]*cos(m_theta_emc[i])/beamEnergy;
	etoeb1=m_e_emc[i]*sin(m_theta_emc[i])/beamEnergy;

	if(m_nhit_muc[i]>=2&&m_nlay_muc[i]>=2)  mucinfo1=1;
      }




      if(m_charge[i]<0){
	itTrk2= evtRecTrkCol->begin() + iGood[i];  
	mdcKalTrk2 = (*itTrk2)->mdcKalTrack();
	iim=i;
	
	if(!(*itTrk2)->isMdcDedxValid())continue;
	RecMdcDedx* dedxTrk2 = (*itTrk2)->mdcDedx();    

	m_dedx_goodhits_em=dedxTrk2->numGoodHits();
	m_dedx_chiem=dedxTrk2->chiE();

	
	w2_ini=WTrackParameter (xmass[0],mdcKalTrk2 ->getZHelixE(),mdcKalTrk2 ->getZErrorE());
	p42e =w2_ini.p(); 
	
	p42e.boost(u_cms); 	
	p32e = p42e.vect();
	
	m_px_cms_em=p42e.px();
	m_py_cms_em=p42e.py();
	m_pz_cms_em=p42e.pz();
	m_e_cms_em=p42e.e();
	m_p_cms_em=sqrt(p42e.px()*p42e.px()+p42e.py()*p42e.py()+p42e.pz()*p42e.pz());
	cmsm=m_p_cms_em;
	m_kal_p_em=m_kal_p[i];
	kalpm=m_kal_p_em;
	e02=m_e_emc[i];

	ex2=m_kal_vx0[i];
	ey2=m_kal_vy0[i];
	ez2=m_kal_vz0[i];
	epx2=m_kal_px[i];
	epy2=m_kal_py[i];
	epz2=m_kal_pz[i];
	epp2=m_kal_p[i];

	m_kal_px_em=epx2;
	m_kal_py_em=epy2;
	m_kal_pz_em=epz2;

	pidchidedx2=m_pidchiDedx[i];
	pidchitof12=m_pidchiTof1[i];
	pidchitof22=m_pidchiTof2[i];

	eoeb2=m_e_emc[i]/beamEnergy; 


	if(p42e.rho()>0) eope2=m_e_emc[i]/p42e.rho(); 

	exoeb2=m_e_emc[i]*sin(m_theta_emc[i])*cos(m_phi_emc[i])/beamEnergy;
	eyoeb2=m_e_emc[i]*sin(m_theta_emc[i])*sin(m_phi_emc[i])/beamEnergy;   	
	ezoeb2=m_e_emc[i]*cos(m_theta_emc[i])/beamEnergy;
	etoeb2=m_e_emc[i]*sin(m_theta_emc[i])/beamEnergy;     	

	if(m_nhit_muc[i]>=2&&m_nlay_muc[i]>=2)  mucinfo2=1; 
	

      }
    }
    

    p4le=( e01 > e02 ) ?p41e:p42e;
    p4lm=( e01 > e02 ) ?p41m:p42m;
    p3le=( e01 > e02 ) ?p31e:p32e;
    p3lm=( e01 > e02 ) ?p31m:p32m;

    double acolle=180.-p31e.angle(p32e)* 180.0 / CLHEP::pi;
    double acople=180.- (p31e.perpPart()).angle(p32e.perpPart ())* 180.0 / CLHEP::pi;
    double poeb1e=p41e.rho()/beamEnergy;
    double poeb2e=p42e.rho()/beamEnergy;
	
    int ilarge=( e01 > e02 ) ?iip:iim;
    
    double eoebl=m_e_emc[ilarge]/beamEnergy;   	
    if(p4le.rho()>0)eopl=m_e_emc[ilarge]/p4le.rho();   	
    double exoebl=  m_e_emc[ilarge]*sin(m_theta_emc[ilarge])*cos(m_phi_emc[ilarge])/beamEnergy;
    double eyoebl=  m_e_emc[ilarge]*sin(m_theta_emc[ilarge])*sin(m_phi_emc[ilarge])/beamEnergy;   	
    double ezoebl=m_e_emc[ilarge]*cos(m_theta_emc[ilarge])/beamEnergy;
    double etoebl=m_e_emc[ilarge]*sin(m_theta_emc[ilarge])/beamEnergy;
    int mucinfol=(m_nhit_muc[ilarge]>=2&&m_nlay_muc[ilarge]>=2) ? 1 : 0;

    int pidel=( e01 > e02 ) ? m_nep : m_nem;     	
	    	
    if(m_t_btof[iip]*m_t_btof[iim]!=0) deltatof=fabs(m_t_btof[iip]-m_t_btof[iim]);
    if(m_t_etof[iip]*m_t_etof[iim]!=0) deltatof=fabs(m_t_etof[iip]-m_t_etof[iim]);


    //acolle cut
    if (m_use_acolle&&acolle>m_acoll_e_cut)   return StatusCode::SUCCESS;
    counter[6]++;
 

    //acople cut
    if (m_use_acople&&acople>m_acopl_e_cut)   return StatusCode::SUCCESS;
    counter[7]++;

    //eoeb cut
    if (m_use_eoeb&&sqrt((eoeb1-1)*(eoeb1-1)+(eoeb2-1)*(eoeb2-1))>m_tetotal_e_cut)  return StatusCode::SUCCESS;
    counter[8]++;

   

    //deltatof cut
    if (m_use_deltatof&&m_useTOF&&(deltatof>m_dtof_e_cut))  return StatusCode::SUCCESS;
    counter[9]++;      


    //poeb cut
    if (m_use_poeb&&poeb1e<m_tpoeb_e_cut&&poeb2e<m_tpoeb_e_cut&&(sqrt((poeb1e-1)*(poeb1e-1)+(poeb2e-1)*(poeb2e-1))>m_tptotal_e_cut))  return StatusCode::SUCCESS;
    counter[10]++;


    //mucinfo cut
    if (m_use_mucinfo&&m_useMUC&&(mucinfo1!=0&&mucinfo2!=0))  return StatusCode::SUCCESS;
    counter[11]++;


    //ne cut
    if (m_use_ne&&m_usePID&&(m_nep!=1||m_nem!=1))  return StatusCode::SUCCESS;
    counter[12]++;





    m_acoll=acolle; 
    m_acopl=acople;
    m_poeb1=poeb1e;
    m_poeb2=poeb2e;
    m_eop1=eope1;
    m_eop2=eope2;
    m_cos_ep=p41e.cosTheta ();
    m_cos_em=p42e.cosTheta ();
    m_mass_ee=(p41e+p42e).m();
    m_px_ee=(p41e+p42e).px();
    m_py_ee=(p41e+p42e).py();
    m_pz_ee=(p41e+p42e).pz();
    m_e_ee=(p41e+p42e).e();
    m_p_ee=(p41e+p42e).rho();

    m_deltatof=deltatof;		

    m_eoeb1=eoeb1;
    m_eoeb2=eoeb2;

    m_etoeb1=etoeb1;
    m_etoeb2=etoeb2;
    m_mucinfo1=mucinfo1;
    m_mucinfo2=mucinfo2;


    nbhabha++;


  ////////////////////////////////////////////////////////////
  // DQA
  // set tag and quality


  // evtRecTrk->tagElectron(), tagMuon(), tagPion(), tagKaon(), tagProton()

  (*itTrk1)->tagElectron();
  (*itTrk2)->tagElectron();
  // Quality: defined by whether dE/dx or TOF is used to identify particle
  // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
  // 1 - only dE/dx (can be used for TOF calibration)
  // 2 - only TOF (can be used for dE/dx calibration)
  // 3 - Both dE/dx and TOF
  (*itTrk1)->setQuality(0);
  (*itTrk2)->setQuality(0);

  // DQA
  // Add the line below at the end of execute(), (before return)
  //
  setFilterPassed(true);
  ////////////////////////////////////////////////////////////
      m_ee_mass->Fill((p41e+p42e).m());
      m_ee_acoll->Fill(acolle);
      m_ee_eop_ep->Fill(eope1);
      m_ee_eop_em->Fill(eope2);
      m_ee_costheta_ep->Fill(p41e.cosTheta ());
      m_ee_costheta_em->Fill(p42e.cosTheta ());

      m_ee_phi_ep->Fill(p41e.phi ());
      m_ee_phi_em->Fill(p42e.phi ());

      m_ee_nneu->Fill(m_nGam);


      m_ee_eemc_ep->Fill(e01);
      m_ee_eemc_em->Fill(e02);

      m_ee_x_ep->Fill(ex1);
      m_ee_y_ep->Fill(ey1);
      m_ee_z_ep->Fill(ez1);
      
      m_ee_x_em->Fill(ex2);
      m_ee_y_em->Fill(ey2);
      m_ee_z_em->Fill(ez2);
      

      m_ee_px_ep->Fill(epx1);
      m_ee_py_ep->Fill(epy1);
      m_ee_pz_ep->Fill(epz1);
      m_ee_p_ep->Fill(epp1);
      
      m_ee_px_em->Fill(epx2);
      m_ee_py_em->Fill(epy2);
      m_ee_pz_em->Fill(epz2);
      m_ee_p_em->Fill(epp2);
      
      m_ee_deltatof->Fill(deltatof);
      
      m_ee_pidchidedx_ep->Fill(pidchidedx1);
      m_ee_pidchidedx_em->Fill(pidchidedx2);
      m_ee_pidchitof1_ep->Fill(pidchitof11);
      m_ee_pidchitof1_em->Fill(pidchitof12);
      m_ee_pidchitof2_ep->Fill(pidchitof21);
      m_ee_pidchitof2_em->Fill(pidchitof22);

    if(m_writentuple) m_tuple1 -> write();
  
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQASelBhabha::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  double p[13];
  
  for(int i=0;i<13;i++){
    p[i]=(counter[i]*0.1/(counter[0]*0.1))*100;
  }

  cout<<"**************************************************************************************"<<endl<<endl;
  cout<<"Cuts of Bhabha Selection"<<'\t'<<'\t'<<'\t'<<"Numbers"<<'\t'<<'\t'<<'\t'<<"Ratio"<<endl<<endl;

  cout<<"Total Number Before All Cuts"<<'\t'<<'\t'<<'\t'<<counter[0]<<'\t'<<'\t'<<'\t'<<p[0]<<"%"<<endl;
  cout<<"Finish Good Charged Track Selection"<<'\t'<<'\t'<<counter[1]<<'\t'<<'\t'<<'\t'<<p[1]<<"%"<<endl; 
  cout<<"Finish Particle ID"<<'\t'<<'\t'<<'\t'<<'\t'<<counter[2]<<'\t'<<'\t'<<'\t'<<p[2]<<"%"<<endl;
  cout<<"Finish Good Neutral Track Selection"<<'\t'<<'\t'<<counter[3]<<'\t'<<'\t'<<'\t'<<p[3]<<"%"<<endl;
  cout<<"Good Neutral Track Not Zero"<<'\t'<<'\t'<<'\t'<<counter[4]<<endl;
  cout<<"Finish Check Good Charged Track's Info."<<'\t'<<'\t'<<counter[5]<<'\t'<<'\t'<<'\t'<<p[5]<<"%"<<endl;
  if(m_use_acolle) cout<<"After Acolle Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<counter[6]<<'\t'<<'\t'<<'\t'<<p[6]<<"%"<<endl;
  else  cout<<"No Acolle Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<"NULL"<<'\t'<<'\t'<<'\t'<<"NULL"<<endl;
  if(m_use_acople)  cout<<"After Acople Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<counter[7]<<'\t'<<'\t'<<'\t'<<p[7]<<"%"<<endl;
  else  cout<<"No Acople Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<"NULL"<<'\t'<<'\t'<<'\t'<<"NULL"<<endl;
  if(m_use_eoeb)  cout<<"After Eoeb Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<counter[8]<<'\t'<<'\t'<<'\t'<<p[8]<<"%"<<endl;
  else  cout<<"No Eoeb Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<"NULL"<<'\t'<<'\t'<<'\t'<<"NULL"<<endl;
  if(m_use_deltatof)  cout<<"After Deltatof Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<counter[9]<<'\t'<<'\t'<<'\t'<<p[9]<<"%"<<endl;
  else  cout<<"No Deltatof Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<"NULL"<<'\t'<<'\t'<<'\t'<<"NULL"<<endl;
  if(m_use_poeb)  cout<<"After Poeb Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<counter[10]<<'\t'<<'\t'<<'\t'<<p[10]<<"%"<<endl;
  else  cout<<"No Poeb Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<"NULL"<<'\t'<<'\t'<<'\t'<<"NULL"<<endl;
  if(m_use_mucinfo)  cout<<"After Mucinfo Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<counter[11]<<'\t'<<'\t'<<'\t'<<p[11]<<"%"<<endl;
  else  cout<<"No Mucinfo Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<"NULL"<<'\t'<<'\t'<<'\t'<<"NULL"<<endl;
  if(m_use_ne)  cout<<"After PID_ne Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<counter[12]<<'\t'<<'\t'<<'\t'<<p[12]<<"%"<<endl<<endl;
  else  cout<<"No PID_ne Cut"<<'\t'<<'\t'<<'\t'<<'\t'<<'\t'<<"NULL"<<'\t'<<'\t'<<'\t'<<"NULL"<<endl;

  cout<<"**************************************************************************************"<<endl<<endl<<endl;

  cout<<endl;
  return StatusCode::SUCCESS;
}
