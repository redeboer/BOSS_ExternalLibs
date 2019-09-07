#include <vector>

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

//
#include "DQASelDimu/DQASelDimu.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::HepLorentzVector;
const double mpsi2s=3.68609;
const double mpi = 0.13957;
const double mk = 0.493677;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
//declare one counter
static int counter[10]={0,0,0,0,0,0,0,0,0,0};
static  int    ndimu=0;
/////////////////////////////////////////////////////////////////////////////

DQASelDimu::DQASelDimu(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties 
  declareProperty("writentuple",m_writentuple = false);
  declareProperty("useVertexDB", m_useVertexDB = false );
  declareProperty("ecms",m_ecms = 3.097);
  declareProperty("beamangle",m_beamangle = 0.022); 
  declareProperty("Vr0cut", m_vr0cut=1.0);
  declareProperty("Vz0cut", m_vz0cut=8.0);
  declareProperty("Coscut", m_coscut=0.93);

  declareProperty("EnergyThreshold", m_energyThreshold=0.04);
  declareProperty("GammaPhiCut", m_gammaPhiCut=20.0);
  declareProperty("GammaThetaCut", m_gammaThetaCut=20.0);
  declareProperty("GammaTrkCut", m_gammaTrkCut=20.0);
   declareProperty("GammaTLCut", m_gammatlCut=0);
   declareProperty("GammaTHCut", m_gammathCut=60);

  declareProperty ("acoll_mu_cut",  m_acoll_mu_cut=6.);
  declareProperty ("acopl_mu_cut",  m_acopl_mu_cut=6.);
  declareProperty ("poeb_mu_cut",  m_poeb_mu_cut=0.15);  
  declareProperty ("dtof_mu_cut",  m_dtof_mu_cut=4.);
  declareProperty ("eoeb_mu_cut",  m_eoeb_mu_cut=0.35);
  declareProperty ("etotal_mu_cut",  m_etotal_mu_cut=0.6);
  declareProperty ("tpoebh_mu_cut",  m_tpoebh_mu_cut=0.9); 
  declareProperty ("tpoebl_mu_cut",  m_tpoebl_mu_cut=0.7); 
  declareProperty ("tptotal_mu_cut",  m_tptotal_mu_cut=1.0);  


        
  //normally, MDC+EMC, otherwise EMC only
  declareProperty ("m_useEMConly",  m_useEMConly= false);
  declareProperty ("m_usePID",  m_usePID= false);// sub-system is under study
  declareProperty ("m_useMDC",  m_useMDC= true);
  declareProperty ("m_useDEDX",  m_useDEDX= false);// not used
  declareProperty ("m_useTOF",  m_useTOF= false);//sub-system is under study
  declareProperty ("m_useEMC",  m_useEMC= true);
  declareProperty ("m_useMUC",  m_useMUC= false);// efficiency    
 
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQASelDimu::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;
  status = service("THistSvc", m_thistsvc);
  if(status.isFailure() ){
    log << MSG::INFO << "Unable to retrieve pointer to THistSvc" << endreq;
    return status;
  }




  m_mumu_mass = new TH1F( "mumu_mass", "mumu_mass", 80, m_ecms-0.3, m_ecms+0.5);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_mass", m_mumu_mass);
  m_mumu_acoll = new TH1F( "mumu_acoll", "mumu_acoll", 60, 0, 6 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_acoll", m_mumu_acoll);
  m_mumu_eop_mup = new TH1F( "mumu_eop_mup", "mumu_eop_mup", 100,0.,0.5 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_eop_mup", m_mumu_eop_mup);
  m_mumu_eop_mum = new TH1F( "mumu_eop_mum", "mumu_eop_mum", 100,0.,0.5 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_eop_mum", m_mumu_eop_mum);
  m_mumu_costheta_mup = new TH1F( "mumu_costheta_mup", "mumu_costheta_mup", 100,-1,1 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_costheta_mup", m_mumu_costheta_mup);
  m_mumu_costheta_mum = new TH1F( "mumu_costheta_mum", "mumu_costheta_mum", 100,-1,1 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_costheta_mum", m_mumu_costheta_mum);

  m_mumu_phi_mup = new TH1F( "mumu_phi_mup", "mumu_phi_mup", 120,-3.2,3.2 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_phi_mup", m_mumu_phi_mup);
  m_mumu_phi_mum = new TH1F( "mumu_phi_mum", "mumu_phi_mum", 120,-3.2,3.2 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_phi_mum", m_mumu_phi_mum);

  m_mumu_nneu = new TH1F( "mumu_nneu", "mumu_nneu", 5,0,5 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_nneu", m_mumu_nneu);
  m_mumu_nlay = new TH1F( "mumu_nlay", "mumu_nlay", 9,0,10 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_nlay", m_mumu_nlay);
  m_mumu_nhit = new TH1F( "mumu_nhit", "mumu_nhit", 19,0,20 );
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_nhit", m_mumu_nhit);

  m_mumu_eemc_mup=new TH1F("mumu_eemc_mup","mumu_eemc_mup",100,0.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_eemc_mup", m_mumu_eemc_mup);
  m_mumu_eemc_mum=new TH1F("mumu_eemc_mum","mumu_eemc_mum",100,0.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_eemc_mum", m_mumu_eemc_mum);
  m_mumu_x_mup=new TH1F("mumu_x_mup","mumu_x_mup",100,-1.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_x_mup", m_mumu_x_mup);
  m_mumu_y_mup=new TH1F("mumu_y_mup","mumu_y_mup",100,-1.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_y_mup", m_mumu_y_mup);
  m_mumu_z_mup=new TH1F("mumu_z_mup","mumu_z_mup",100,-10.0,10.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_z_mup", m_mumu_z_mup);
  m_mumu_x_mum=new TH1F("mumu_x_mum","mumu_x_mum",100,-1.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_x_mum", m_mumu_x_mum);
  m_mumu_y_mum=new TH1F("mumu_y_mum","mumu_y_mum",100,-1.0,1.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_y_mum", m_mumu_y_mum);
  m_mumu_z_mum=new TH1F("mumu_z_mum","mumu_z_mum",100,-10.0,10.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_z_mum", m_mumu_z_mum);

  m_mumu_px_mup=new TH1F("mumu_px_mup","mumu_px_mup",200,-2.0,2.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_px_mup", m_mumu_px_mup);
  m_mumu_py_mup=new TH1F("mumu_py_mup","mumu_py_mup",200,-2.0,2.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_py_mup", m_mumu_py_mup);
  m_mumu_pz_mup=new TH1F("mumu_pz_mup","mumu_pz_mup",200,-2.0,2.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_pz_mup", m_mumu_pz_mup);
  m_mumu_p_mup=new TH1F("mumu_p_mup","mumu_p_mup",100,1.0,2.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_p_mup", m_mumu_p_mup);
  m_mumu_px_mum=new TH1F("mumu_px_mum","mumu_px_mum",100,-2.0,2.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_px_mum", m_mumu_px_mum);
  m_mumu_py_mum=new TH1F("mumu_py_mum","mumu_py_mum",100,-2.0,2.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_py_mum", m_mumu_py_mum);
  m_mumu_pz_mum=new TH1F("mumu_pz_mum","mumu_pz_mum",100,-2.0,2.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_pz_mum", m_mumu_pz_mum);
  m_mumu_p_mum=new TH1F("mumu_p_mum","mumu_p_mum",100,1.0,2.0);  
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_p_mum", m_mumu_p_mum);
  m_mumu_deltatof=new TH1F("mumu_deltatof","mumu_deltatof",50,0.0,10.0);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_deltatof", m_mumu_deltatof);

  m_mumu_pidchidedx_mup=new TH1F("mumu_pidchidedx_mup","mumu_pidchidedx_mup",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_pidchidedx_mup", m_mumu_pidchidedx_mup);
  m_mumu_pidchidedx_mum=new TH1F("mumu_pidchidedx_mum","mumu_pidchidedx_mum",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_pidchidedx_mum", m_mumu_pidchidedx_mum);
  m_mumu_pidchitof1_mup=new TH1F("mumu_pidchitof1_mup","mumu_pidchitof1_mup",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_pidchitof1_mup", m_mumu_pidchitof1_mup);
  m_mumu_pidchitof1_mum=new TH1F("mumu_pidchitof1_mum","mumu_pidchitof1_mum",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_pidchitof1_mum", m_mumu_pidchitof1_mum);
  m_mumu_pidchitof2_mup=new TH1F("mumu_pidchitof2_mup","mumu_pidchitof2_mup",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_pidchitof2_mup", m_mumu_pidchitof2_mup);
  m_mumu_pidchitof2_mum=new TH1F("mumu_pidchitof2_mum","mumu_pidchitof2_mum",160,-4,4);
  status = m_thistsvc->regHist("/DQAHist/Dimu/mumu_pidchitof2_mum", m_mumu_pidchitof2_mum);


  
  
  
NTuplePtr nt1(ntupleSvc(), "DQAFILE/Dimu");
if ( nt1 ) m_tuple1 = nt1;
else {
  m_tuple1 = ntupleSvc()->book ("DQAFILE/Dimu", CLID_ColumnWiseTuple, "N-Tuple");
  if ( m_tuple1 )    {
    status = m_tuple1->addItem ("run",   m_run);
    status = m_tuple1->addItem ("rec", m_rec);
    status = m_tuple1->addItem ("Nchrg",  m_ncharg);
    status = m_tuple1->addItem ("Nneu",   m_nneu,0,40);
    status = m_tuple1->addItem ("NGch",   m_ngch, 0, 40);
    status = m_tuple1->addItem ("NGam",   m_nGam);


    status = m_tuple1->addItem ("dimutag",   m_dimutag);

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
      
    status = m_tuple1->addItem ("px_cms_ep",   m_px_cms_ep); //momentum of electron+
    status = m_tuple1->addItem ("py_cms_ep",   m_py_cms_ep); //momentum of electron+
    status = m_tuple1->addItem ("pz_cms_ep",   m_pz_cms_ep); //momentum of electron+
    status = m_tuple1->addItem ("e_cms_ep",   m_e_cms_ep); //momentum of electron+
    status = m_tuple1->addItem ("cos_ep",   m_cos_ep); //momentum of electron+ 
    status = m_tuple1->addItem ("px_cms_em",   m_px_cms_em); //momentum of electron-
    status = m_tuple1->addItem ("py_cms_em",   m_py_cms_em); //momentum of electron-
    status = m_tuple1->addItem ("pz_cms_em",   m_pz_cms_em); //momentum of electron-
    status = m_tuple1->addItem ("e_cms_em",   m_e_cms_em); //momentum of electron-      
    status = m_tuple1->addItem ("cos_em",   m_cos_em); //momentum of electron-      
    status = m_tuple1->addItem ("mass_ee",   m_mass_ee); // 
    status = m_tuple1->addItem ("emax",   m_emax); // 
    status = m_tuple1->addItem ("esum",   m_esum); // 
    status = m_tuple1->addItem ( "npip", m_npip );
    status = m_tuple1->addItem ( "npim", m_npim );
    status = m_tuple1->addItem ( "nkp",  m_nkp );
    status = m_tuple1->addItem ( "nkm",  m_nkm );
    status = m_tuple1->addItem ( "np",   m_np   );
    status = m_tuple1->addItem ( "npb",  m_npb  );

    status = m_tuple1->addItem ( "nep", m_nep );
    status = m_tuple1->addItem ( "nem", m_nem );
    status = m_tuple1->addItem ( "nmup", m_nmup );
    status = m_tuple1->addItem ( "nmum", m_nmum );

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
StatusCode DQASelDimu::execute() {
  setFilterPassed(false);
  const double beamEnergy = m_ecms/2.;
  const HepLorentzVector p_cms(m_ecms*sin(m_beamangle*0.5),0.0,0.0,m_ecms);
  const Hep3Vector u_cms = -p_cms.boostVector();    
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;


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
  //  if(evtRecEvent->totalNeutral()>30)return sc;
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
    //  if (m_reader.isRunNumberValid( m_run)) {
    //   HepVector dbv = m_reader.PrimaryVertex( m_run);
    //    HepVector vv = m_reader.SigmaPrimaryVertex( m_run);
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
    if(fabs(z0) >= m_vz0cut) continue;
    if(fabs(Rxy) >= m_vr0cut) continue;


    if(fabs(m_vz0) >= m_vz0cut) continue;
    if(m_vr0 >= m_vr0cut) continue;

    // double cost = cos(mdcTrk->theta());
    // if(fabs(cost) >= m_coscut ) continue; 
//     iGood.push_back((*itTrk)->trackId());
    iGood.push_back(i);
    nCharge += mdcTrk->charge();

  }




  
  //
  // Finish Good Charged Track Selection
  //
  int nGood = iGood.size();
  m_ngch=nGood;
  log << MSG::DEBUG << "ngood, totcharge = " << nGood << " , " << nCharge << endreq;

  if((nGood != 2)||(nCharge!=0)){
    return StatusCode::SUCCESS;
  }

  counter[1]++;

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
      EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
      //    if(pid) delete pid;
      pid->init();
      pid->setMethod(pid->methodProbability());
      pid->setChiMinCut(4);
      pid->setRecTrack(*itTrk);
      pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2());//|pid->useEmc()|pid->useMuc()); // use PID sub-system
      pid->identify(pid->onlyElectron()|pid->onlyMuon()|pid->onlyPion());                 // seperater Pion/Kaon/Proton
      pid->calculate();
      if(!(pid->IsPidInfoValid())) continue;
      RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
      ///      RecMdcKalTrack* mdcKalTrk = 0 ; 
      ///      if((*itTrk)->isMdcKalTrackValid()) mdcKalTrk = (*itTrk)->mdcKalTrack(); 
      double  prob_pi = pid->probPion();
      double  prob_K  = pid->probKaon();
      double  prob_p  = pid->probProton();
      double  prob_e  = pid->probElectron();
      double prob_mu = pid->probMuon();    
      //    std::cout << "prob "<< prob_pi << ", "<< prob_K << ", "<< prob_p << std::endl; 
      HepLorentzVector ptrk;
      ptrk.setPx(mdcTrk->px()) ; 
      ptrk.setPy(mdcTrk->py()) ;
      ptrk.setPz(mdcTrk->pz()) ; 
      double p3 = ptrk.mag()   ; 

      
      
	m_pidcode[i]=1;
	m_pidprob[i]=pid->prob(1);
	m_pidchiDedx[i]=pid->chiDedx(1);
	m_pidchiTof1[i]=pid->chiTof1(1);
	m_pidchiTof2[i]=pid->chiTof2(1);
	if(mdcTrk->charge() > 0) {
	  imup.push_back(iGood[i]);
	  
	}
	if (mdcTrk->charge() < 0) {
	  imum.push_back(iGood[i]);
	
	}
      
    
    }
  
  m_nep  = iep.size() ; 
  m_nem = iem.size() ;  
  m_nmup  = imup.size() ; 
  m_nmum = imum.size() ;
  
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
	    //           int phigap=emcTrk->PhiGap();
	    //            int thetagap=emcTrk->ThetaGap();
	    //	    double getETof2x1 = emcTrk->getETof2x1();
	    //	    double getETof2x3 = emcTrk->getETof2x3();
	    //	    double getELepton = emcTrk->getELepton();
	    double nseed=0;//(emcTrk->getCluster() )->getSeedSize()  ;
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

	    //            m_getELepton[iphoton]=getELepton;
	    //            m_getETof2x1[iphoton]=getETof2x1;
	    //            m_getETof2x3[iphoton]=getETof2x3;
	    //	    m_PhiGap[iphoton]=phigap;
	    //	    m_ThetaGap[iphoton]=thetagap;
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
    double phi = emcTrk->phi();
    double the = emcTrk->theta();     
   
    m_delphi[iphoton]=dphi;
    m_delthe[iphoton]=dthe;
    m_delang[iphoton]=dang;
    if(energy < m_energyThreshold) continue;
    if(getTime>m_gammathCut||getTime<m_gammatlCut)continue;
    //     if((fabs(dthe) < m_gammaThetaCut) && (fabs(dphi)<m_gammaPhiCut) ) continue;
    if(dang< m_gammaTrkCut) continue; 
    iphoton++;  
    iGam.push_back(i);
    if(iphoton>=40)return StatusCode::SUCCESS; 
  }

  int nGam = iGam.size();
  m_nGam=nGam;
  //  std::cout  << "num Good Photon " << m_nGam  << " , " <<evtRecEvent->totalNeutral()<<std::endl;
  //std::cout<<"dbg_4"<<std::endl;
  counter[3]++;

  double egam_ext=0;
  double  			ex_gam=0;
  double            ey_gam=0;
  double            ez_gam=0;  
  double            et_gam=0;
  double            e_gam=0;
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





  double  			px_had=0;
  double            py_had=0;
  double            pz_had=0;
  double            pt_had=0;
  double            t_pxy2 = 0;
  double            p_had=0;
  double            e_had=0;
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
    ii=i;


    m_charge[ii] = mdcTrk->charge();
    m_vx0[ii]    = mdcTrk->x();
    m_vy0[ii]    = mdcTrk->y();
    m_vz0[ii]    = mdcTrk->z();

    
    m_px[ii]   = mdcTrk->px();
    m_py[ii]   = mdcTrk->py();
    m_pz[ii]   = mdcTrk->pz();
    m_p[ii]    = mdcTrk->p();


    mdcKalTrk->setPidType(RecMdcKalTrack::muon);
    
   
    ///    if(m_pidcode[ii]==3)mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
    ///    if(m_pidcode[ii]==4)mdcKalTrk->setPidType(RecMdcKalTrack::proton);
    m_kal_vx0[ii]    = mdcKalTrk->x();
    m_kal_vy0[ii]    = mdcKalTrk->y();
    m_kal_vz0[ii]    = mdcKalTrk->z();

    
    m_kal_px[ii]   = mdcKalTrk->px();
    m_kal_py[ii]   = mdcKalTrk->py();
    m_kal_pz[ii]   = mdcKalTrk->pz();
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
      m_probPH[ii]= dedxTrk->probPH();
      m_normPH[ii]= dedxTrk->normPH();
      
      m_chie[ii]  = dedxTrk->chiE();
      m_chimu[ii] = dedxTrk->chiMu();
      m_chipi[ii] = dedxTrk->chiPi();
      m_chik[ii]  = dedxTrk->chiK();
      m_chip[ii]  = dedxTrk->chiP();
      m_ghit[ii]  = dedxTrk->numGoodHits();
      m_thit[ii]  = dedxTrk->numTotalHits();
    }
      
    if((*itTrk)->isEmcShowerValid()) {

      RecEmcShower *emcTrk = (*itTrk)->emcShower();
      m_e_emc[ii] = emcTrk->energy();
      m_phi_emc[ii] = emcTrk->phi();
      m_theta_emc[ii] = emcTrk->theta();      
    }
    
    
    if((*itTrk)->isMucTrackValid()){

      RecMucTrack* mucTrk = (*itTrk)->mucTrack()   ;
      m_nhit_muc[ii] = mucTrk->numHits() ; 
      m_nlay_muc[ii] = mucTrk->numLayers()    ;

    } else {
      m_nhit_muc[ii] = 0;
      m_nlay_muc[ii] = 0;
    }   
    m_t_btof[ii] = 0;
    m_t_etof[ii] = 0;
 
    if((*itTrk)->isTofTrackValid()) {  //TOF information

      SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
      
      SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();

      for(;iter_tof != tofTrkCol.end(); iter_tof++ ) { 
        TofHitStatus *status = new TofHitStatus; 
        status->setStatus((*iter_tof)->status());

        if(!(status->is_barrel())){//endcap
	  if( (status->is_cluster()) )  m_t_etof[ii]  = (*iter_tof)->tof();
          if( !(status->is_counter()) ){	if(status) delete status; continue;} // ? 
          if( status->layer()!=0 ) {	if(status) delete status;continue;}//layer1
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
	  
          m_qual_etof[ii]  = qual;
	  m_tof_etof[ii]   = tof ; 
	}
        else {//barrel
	  if( (status->is_cluster()) )  m_t_btof[ii]  = (*iter_tof)->tof();
          if( !(status->is_counter()) ){	if(status) delete status; continue;} // ? 
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
	    
            m_qual_btof1[ii]  = qual;
	    m_tof_btof1[ii]   = tof ; 
          }

          if(status->layer()==2){//layer2
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
 
            m_qual_btof2[ii]  = qual;
	    m_tof_btof2[ii]   = tof ; 
          } 
	}
	if(status) delete status;
      }
    }

  }
  counter[4]++;

  //std::cout<<"dbg_5"<<std::endl;
  //tag

 
  m_dimutag=0;
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
    for(int i = 0; i < m_ngch; i++ ){  
      if(m_charge[i]>0)itTrk1= evtRecTrkCol->begin() + iGood[i];
      if(m_charge[i]<0)    itTrk2= evtRecTrkCol->begin() + iGood[i];  
      if(m_charge[i]>0)    mdcKalTrk1 = (*itTrk1)->mdcKalTrack(); 
      if(m_charge[i]<0)    mdcKalTrk2 = (*itTrk2)->mdcKalTrack();
      if(m_charge[i]>0)iip=i;
      if(m_charge[i]<0)iim=i; 	  
      
               
	if(m_charge[i]>0)      w1_ini=WTrackParameter (xmass[1],mdcKalTrk1->getZHelixMu(),mdcKalTrk1->getZErrorMu());
	if(m_charge[i]<0)      w2_ini=WTrackParameter (xmass[1],mdcKalTrk2 ->getZHelixMu(),mdcKalTrk2 ->getZErrorMu());

      
    if(m_charge[i]>0)      p41m =w1_ini.p();
    if(m_charge[i]<0)      p42m =w2_ini.p(); 
    if(m_charge[i]>0)      p41m.boost(u_cms);		
    if(m_charge[i]<0)      p42m.boost(u_cms); 	
    if(m_charge[i]>0)      p31m = p41m.vect();
    if(m_charge[i]<0)      p32m = p42m.vect();


	if(m_charge[i]>0){
	  m_px_cms_ep=p41m.px();
	  m_py_cms_ep=p41m.py();
	  m_pz_cms_ep=p41m.pz();
	  m_e_cms_ep=p41m.e();
	}
	if(m_charge[i]<0){
	  m_px_cms_em=p42m.px();
	  m_py_cms_em=p42m.py();
	  m_pz_cms_em=p42m.pz();
	  m_e_cms_em=p42m.e();
	}
 	 	

    }

    double e01=(iip!=-1)?m_e_emc[iip]:0;//m_e_cms_ep;
    double e02=(iim!=-1)?m_e_emc[iim]:0;//m_e_cms_em;
    int ilarge=( e01 > e02 ) ?iip:iim;

    p4lm=( e01 > e02 ) ?p41m:p42m;
 

    p3lm=( e01 > e02 ) ?p31m:p32m;
 

    double acollm=   180.-p31m.angle(p32m)* 180.0 / CLHEP::pi;
    double acoplm=   180.- (p31m.perpPart()).angle(p32m.perpPart ())* 180.0 / CLHEP::pi;
    double poeb1m=p41m.rho()/beamEnergy;
    double poeb2m=p42m.rho()/beamEnergy;
    double poeblm=p4lm.rho()/beamEnergy;    

    double eemc1=m_e_emc[iip];
    double eemc2=m_e_emc[iim];


    double ex1=m_kal_vx0[iip];
    double ey1=m_kal_vy0[iip];
    double ez1=m_kal_vz0[iip];
    double epx1=m_kal_px[iip];
    double epy1=m_kal_py[iip];
    double epz1=m_kal_pz[iip];
    double epp1=m_kal_p[iip];
    double ex2=m_kal_vx0[iim];
    double ey2=m_kal_vy0[iim];
    double ez2=m_kal_vz0[iim];
    double epx2=m_kal_px[iim];
    double epy2=m_kal_py[iim];
    double epz2=m_kal_pz[iim];
    double epp2=m_kal_p[iim];

    double pidchidedx1=m_pidchiDedx[iip];
    double pidchitof11=  m_pidchiTof1[iip];
    double pidchitof21=  m_pidchiTof2[iip];
    double pidchidedx2=m_pidchiDedx[iim];
    double pidchitof12=  m_pidchiTof1[iim];
    double pidchitof22=  m_pidchiTof2[iim];

    double eoeb1=m_e_emc[iip]/beamEnergy;   	
    double eoeb2=m_e_emc[iim]/beamEnergy; 
 
 
    double eopm1=0;
    if(p41m.rho()>0)eopm1=m_e_emc[iip]/p41m.rho();   	
    double eopm2=0;
    if(p42m.rho()>0)eopm2=m_e_emc[iim]/p42m.rho();

  	
    double exoeb1=  m_e_emc[iip]*sin(m_theta_emc[iip])*cos(m_phi_emc[iip])/beamEnergy;
    double eyoeb1=  m_e_emc[iip]*sin(m_theta_emc[iip])*sin(m_phi_emc[iip])/beamEnergy;   	
    double ezoeb1=m_e_emc[iip]*cos(m_theta_emc[iip])/beamEnergy;
    double etoeb1=m_e_emc[iip]*sin(m_theta_emc[iip])/beamEnergy;
    
    double exoeb2=  m_e_emc[iim]*sin(m_theta_emc[iim])*cos(m_phi_emc[iim])/beamEnergy;
    double eyoeb2=  m_e_emc[iim]*sin(m_theta_emc[iim])*sin(m_phi_emc[iim])/beamEnergy;   	
    double ezoeb2=m_e_emc[iim]*cos(m_theta_emc[iim])/beamEnergy;
    double etoeb2=m_e_emc[iim]*sin(m_theta_emc[iim])/beamEnergy;     	

    double eoebl=m_e_emc[ilarge]/beamEnergy;   	

    double eopl=0;
    if(p4lm.rho()>0)eopl=m_e_emc[ilarge]/p4lh.rho();   	
 	
    double exoebl=  m_e_emc[ilarge]*sin(m_theta_emc[ilarge])*cos(m_phi_emc[ilarge])/beamEnergy;
    double eyoebl=  m_e_emc[ilarge]*sin(m_theta_emc[ilarge])*sin(m_phi_emc[ilarge])/beamEnergy;   	
    double ezoebl=m_e_emc[ilarge]*cos(m_theta_emc[ilarge])/beamEnergy;
    double etoebl=m_e_emc[ilarge]*sin(m_theta_emc[ilarge])/beamEnergy;

    int mucinfo1=(m_nhit_muc[iip]>=2&&m_nlay_muc[iip]>=2 ) ? 1 : 0;
    int mucinfo2=(m_nhit_muc[iim]>=2&&m_nlay_muc[iim]>=2) ? 1 : 0;
    int mucinfol=(m_nhit_muc[ilarge]>=2&&m_nlay_muc[ilarge]>=2) ? 1 : 0;
    int pidel=( e01 > e02 ) ? m_nep : m_nem;     	
    int pidmul=( e01 > e02 ) ? m_nmup : m_nmum;    	    	
    double deltatof=0;

    if(m_t_btof[iip]*m_t_btof[iim]!=0) deltatof+=fabs(m_t_btof[iip]-m_t_btof[iim]);
    if(m_t_etof[iip]*m_t_etof[iim]!=0) deltatof+=fabs(m_t_etof[iip]-m_t_etof[iim]);


       if(acollm<m_acoll_mu_cut)m_dimutag+=1;
       if (acoplm<m_acopl_mu_cut)m_dimutag+=10;
	   if (fabs(m_ecms-mpsi2s)<0.001){
       if((sqrt(((poeb1m-poeb2m)/0.35)*((poeb1m-poeb2m)/0.35)+((poeb1m+poeb2m-1.68)/0.125)*((poeb1m+poeb2m-1.68)/0.125))>m_tptotal_mu_cut)
	  &&(((poeb1m>=m_tpoebh_mu_cut)&&(poeb2m>=m_tpoebl_mu_cut))
	     ||((poeb2m>=m_tpoebh_mu_cut)&&(poeb1m>=m_tpoebl_mu_cut))))m_dimutag+=100;
		 }
		 else{
		 if((fabs(poeb1m-1)<m_poeb_mu_cut)&&(fabs(poeb2m-1)<m_poeb_mu_cut))m_dimutag+=100;
		 }
       if(!m_useTOF||(deltatof<m_dtof_mu_cut))m_dimutag+=1000;
       if(!m_useMUC||(mucinfo1==1&&mucinfo2==1))m_dimutag+=10000;
       if(etoeb1<m_eoeb_mu_cut&&eoeb2<m_eoeb_mu_cut)m_dimutag+=100000;
       if(etoeb1+etoeb2<m_etotal_mu_cut)m_dimutag+=1000000;
       if(!m_usePID||(m_nmup==1&&m_nmum==1))m_dimutag+=10000000;

 


 

  
    m_acoll=acollm; 
    m_acopl=acoplm;
    m_poeb1=poeb1m;
    m_poeb2=poeb2m;
    m_eop1=eopm1;
    m_eop2=eopm2;
    m_cos_ep=p41m.cosTheta ();
    m_cos_em=p42m.cosTheta ();
    m_mass_ee=(p41m+p42m).m();

    m_deltatof=deltatof;		

    m_eoeb1=eoeb1;
    m_eoeb2=eoeb2;

    m_etoeb1=etoeb1;
    m_etoeb2=etoeb2;
    m_mucinfo1=mucinfo1;
    m_mucinfo2=mucinfo2;
      
    if(m_dimutag==11111111){
      ndimu++;
  ////////////////////////////////////////////////////////////
  // DQA
  // set tag and quality

  // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
  (*itTrk1)->tagMuon();
  (*itTrk2)->tagMuon();

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



    m_mumu_mass->Fill((p41m+p42m).m());
      m_mumu_acoll->Fill(acollm);
      m_mumu_eop_mup->Fill(eopm1);
      m_mumu_eop_mum->Fill(eopm2);
      m_mumu_costheta_mup->Fill(p41m.cosTheta ());
      m_mumu_costheta_mum->Fill(p42m.cosTheta ());
      m_mumu_phi_mup->Fill(p41m.phi ());
      m_mumu_phi_mum->Fill(p42m.phi ());
      m_mumu_nneu->Fill(m_nGam);
      m_mumu_nhit->Fill(m_nhit_muc[ilarge]);
      m_mumu_nlay->Fill(m_nlay_muc[ilarge]);


     m_mumu_eemc_mup->Fill(eemc1);
     m_mumu_eemc_mum->Fill(eemc2);

     m_mumu_x_mup->Fill(ex1);
     m_mumu_y_mup->Fill(ey1);
     m_mumu_z_mup->Fill(ez1);

     m_mumu_x_mum->Fill(ex2);
     m_mumu_y_mum->Fill(ey2);
     m_mumu_z_mum->Fill(ez2);


     m_mumu_px_mup->Fill(epx1);
     m_mumu_py_mup->Fill(epy1);
     m_mumu_pz_mup->Fill(epz1);
     m_mumu_p_mup->Fill(epp1);

     m_mumu_px_mum->Fill(epx2);
     m_mumu_py_mum->Fill(epy2);
     m_mumu_pz_mum->Fill(epz2);
     m_mumu_p_mum->Fill(epp2);

     m_mumu_deltatof->Fill(deltatof);

     m_mumu_pidchidedx_mup->Fill(pidchidedx1);
     m_mumu_pidchidedx_mum->Fill(pidchidedx2);
     m_mumu_pidchitof1_mup->Fill(pidchitof11);
     m_mumu_pidchitof1_mum->Fill(pidchitof12);
     m_mumu_pidchitof2_mup->Fill(pidchitof21);
     m_mumu_pidchitof2_mum->Fill(pidchitof22);

    if(m_writentuple)m_tuple1 -> write();
    }



  




  return StatusCode::SUCCESS;

 

  
 

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQASelDimu::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


