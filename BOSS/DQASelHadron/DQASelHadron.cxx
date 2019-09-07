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
#include "VertexFit/Helix.h"
#include "ParticleID/ParticleID.h"
#include "VertexFit/FastVertexFit.h"
//
#include "DQASelHadron/DQASelHadron.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::HepLorentzVector;

const double mpi = 0.13957;
const double mk = 0.493677;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
//declare one counter
static int counter[10]={0,0,0,0,0,0,0,0,0,0};
static  int    nhadron=0;
static  int    n0prong=0;
static  int    n2prong=0;
static   int   n4prong=0;
static  int   ng4prong=0;

/////////////////////////////////////////////////////////////////////////////

DQASelHadron::DQASelHadron(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties 
  declareProperty("writentuple",m_writentuple = false);
  declareProperty("useVertexDB", m_useVertexDB = false );
  declareProperty("ecms",m_ecms = 3.097);
  declareProperty("beamangle",m_beamangle = 0.022); 
  declareProperty("Vr0cut", m_vr0cut=1.0);
  declareProperty("Vz0cut", m_vz0cut=10.0);
  declareProperty("Coscut", m_coscut=0.93);

  declareProperty("EnergyThreshold", m_energyThreshold=0.04);
  declareProperty("GammaPhiCut", m_gammaPhiCut=20.0);
  declareProperty("GammaThetaCut", m_gammaThetaCut=20.0);
  declareProperty("GammaTrkCut", m_gammaTrkCut=20.0);
   declareProperty("GammaTLCut", m_gammatlCut=0);
   declareProperty("GammaTHCut", m_gammathCut=60);



  declareProperty ("acoll_h_cut",  m_acoll_h_cut=10.);
  declareProperty ("poeb_h_cut",  m_poeb_h_cut=0.2);  
  declareProperty ("dtof_h_cut",  m_dtof_h_cut=6.);
  declareProperty ("eop_h_cut",  m_eop_h_cut=0.2);
  declareProperty ("etotal_h_cut",  m_etotal_h_cut=0.2);
  declareProperty ("ngam_h_cut",  m_ngam_h_cut=2);
  declareProperty ("br_h_cut",  m_br_h_cut=0.65);  
  declareProperty ("bz_h_cut",  m_bz_h_cut=0.7); 
  declareProperty ("thr_h_cut",  m_thr_h_cut=0.5); 
        
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
StatusCode DQASelHadron::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;
  status = service("THistSvc", m_thistsvc);
  if(status.isFailure() ){
    log << MSG::INFO << "Unable to retrieve pointer to THistSvc" << endreq;
    return status;
  }


  m_ha_costheta = new TH1F( "PHY_HAD_SUM_costheta", "PHY_HAD_SUM_costheta", 100, -1, 1 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_costheta", m_ha_costheta);
     m_ha_phi = new TH1F( "PHY_HAD_SUM_phi", "PHY_HAD_SUM_phi", 128, -3.2, 3.2 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_phi", m_ha_phi);
     m_ha_pmax = new TH1F( "PHY_HAD_SUM_pmax", "PHY_HAD_SUM_pmax", 100, 0, 2 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_pmax", m_ha_pmax);
     m_ha_emax = new TH1F( "PHY_HAD_SUM_emax", "PHY_HAD_SUM_emax", 100, 0, 2 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_emax", m_ha_emax);
     m_ha_etot = new TH1F( "PHY_HAD_SUM_etot", "PHY_HAD_SUM_etot", 100, 0, 4 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_etot", m_ha_etot);
     m_ha_br = new TH1F( "PHY_HAD_SUM_br", "PHY_HAD_SUM_br", 100, 0, 2 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_br", m_ha_br);
     m_ha_bz = new TH1F( "PHY_HAD_SUM_bz", "PHY_HAD_SUM_bz", 100, 0, 2 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_bz", m_ha_bz);
     m_ha_nneu = new TH1I( "PHY_HAD_SUM_nneu", "PHY_HAD_SUM_nneu", 20, 0, 20 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_nneu", m_ha_nneu);
     m_ha_nchg = new TH1I( "PHY_HAD_SUM_nchg", "PHY_HAD_SUM_nchg", 20, 0, 20 );
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_nchg", m_ha_nchg);
 
     m_ha_vx = new TH1F( "PHY_HAD_FLS_vx", "PHY_HAD_FLS_vx", 100, -1., 1.); 
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_vx", m_ha_vx);
     m_ha_vy = new TH1F( "PHY_HAD_FLS_vy", "PHY_HAD_FLS_vy", 100, -1., 1.);
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_vy", m_ha_vy);
     m_ha_vz = new TH1F( "PHY_HAD_FLS_vz", "PHY_HAD_FLS_vz", 100, -10.0, 10.);
  status = m_thistsvc->regHist("/DQAHist/Hadron/ha_vz", m_ha_vz);



  
  
  
NTuplePtr nt1(ntupleSvc(), "DQAFILE/Hadron");
if ( nt1 ) m_tuple1 = nt1;
else {
  m_tuple1 = ntupleSvc()->book ("DQAFILE/Hadron", CLID_ColumnWiseTuple, "N-Tuple");
  if ( m_tuple1 )    {
    status = m_tuple1->addItem ("run",   m_run);
    status = m_tuple1->addItem ("rec", m_rec);
    status = m_tuple1->addItem ("Nchrg",  m_ncharg);
    status = m_tuple1->addItem ("Nneu",   m_nneu,0,40);
    status = m_tuple1->addItem ("NGch",   m_ngch, 0, 40);
    status = m_tuple1->addItem ("NGam",   m_nGam);


    status = m_tuple1->addItem ("hadrontag",   m_hadrontag);

    status = m_tuple1->addItem ("br",  m_br);
    status = m_tuple1->addItem ("bz",  m_bz);
    status = m_tuple1->addItem ("evis",  m_evis);
    status = m_tuple1->addItem ("thr",  m_thr);

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
//    status = m_tuple1->addIndexedItem ("px",m_nneu,   m_px);
//    status = m_tuple1->addIndexedItem ("py",m_nneu,   m_py);   
//    status = m_tuple1->addIndexedItem ("pz",m_nneu,   m_pz);
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
StatusCode DQASelHadron::execute() {
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
//     double phi0=mdcTrk->helix(1);
//     double xv=vx.x();
//     double yv=vx.y();
//     double zv=vx.z();
//     double Rxy=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);
//     double m_vx0 = x0;
//     double m_vy0 = y0;
//     double m_vz0 = z0;
//     double m_vr0 = Rxy;
//     if(fabs(z0) >= m_vz0cut) continue;
//     if(fabs(Rxy) >= m_vr0cut) continue;


//     if(fabs(m_vz0) >= m_vz0cut) continue;
//     if(m_vr0 >= m_vr0cut) continue;


    HepVector a = mdcTrk->helix();
    HepSymMatrix Ea = mdcTrk->err();
    HepPoint3D point0(0.,0.,0.);  
    HepPoint3D IP(vx[0],vx[1],vx[2]);
    VFHelix helixip(point0,a,Ea);
    helixip.pivot(IP);
    HepVector vecipa = helixip.a();
    double  Rvxy0=fabs(vecipa[0]); //the distance to IP in xy plane
    double  Rvz0=vecipa[3];    //the distance to IP in z direction
    double  Rvphi0=vecipa[1];
    if(fabs(Rvz0) >= m_vz0cut) continue;
    if(fabs(Rvxy0) >= m_vr0cut) continue;


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


    if(m_ngch<2 ||m_ngch>20||(nCharge >4) ) {  return StatusCode::SUCCESS;  }
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

  if (m_usePID){
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
  double            t_pxy2 = 0;
  double            pt_had=0;
  double            p_had=0;
  double            e_had=0;
  double p_max=0.;
  double e_max=0.; 
  //
  // check good charged track's infomation
  //



  double ctrk_theta = -10;
  double ctrk_phi = -10;
  for(int i = 0; i < m_ngch; i++ ){

    EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];

    if(!(*itTrk)->isMdcTrackValid()) continue;  //  MDC information 
    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();


    //    if ( m_ngch==2 &&mdcTrk->charge()>0) i = 0 ;  
    //    if ( m_ngch==2 &&mdcTrk->charge()<0) i = 1 ; 
    m_charge[i] = mdcTrk->charge();
    m_vx0[i]    = mdcTrk->x();
    m_vy0[i]    = mdcTrk->y();
    m_vz0[i]    = mdcTrk->z();
    m_px[i]   = mdcTrk->px();
    m_py[i]   = mdcTrk->py();
    m_pz[i]   = mdcTrk->pz();
    m_p[i]    = mdcTrk->p();
    ctrk_theta = mdcTrk->theta();
    ctrk_phi = mdcTrk->phi();
    mdcKalTrk->setPidType(RecMdcKalTrack::pion);
    // double ptrk = mdcKalTrk->p() ;
    m_kal_vx0[i]    = mdcKalTrk->x();
    m_kal_vy0[i]    = mdcKalTrk->y();
    m_kal_vz0[i]    = mdcKalTrk->z();


    m_kal_px[i]   = mdcKalTrk->px();
    m_kal_py[i]   = mdcKalTrk->py();
    m_kal_pz[i]   = mdcKalTrk->pz();
//    m_kal_p[i]    = mdcKalTrk->p();  // pxy() and p() are not filled in the reconstruction algorithm
    t_pxy2 = m_kal_px[i]*m_kal_px[i] + m_kal_py[i]*m_kal_py[i]; 
    m_kal_p[i]    = sqrt(t_pxy2 + m_kal_pz[i]*m_kal_pz[i]);
    double ptrk = m_kal_p[i];
    px_had+=m_kal_px[i];
    py_had+=m_kal_py[i];
    pz_had+=m_kal_pz[i];
    pt_had+=sqrt(t_pxy2);
    p_had+=m_kal_p[i];
    e_had+=sqrt(m_kal_p[i]*m_kal_p[i]+xmass[2]*xmass[2]);
    if(m_useDEDX&&(*itTrk)->isMdcDedxValid()) { // DEDX information 

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
      if(m_e_emc[i]>e_max){
	p_max=m_p[i];
	e_max=m_e_emc[i];
      }      
    } else {
     m_e_emc[i] = 0;
     m_phi_emc[i] = -10;
     m_theta_emc[i] = -10;
    }
 
 
    
    if(m_useMUC&&(*itTrk)->isMucTrackValid()){
      
      RecMucTrack* mucTrk = (*itTrk)->mucTrack()   ;
      m_nhit_muc[i] = mucTrk->numHits() ; 
      m_nlay_muc[i] = mucTrk->numLayers()    ;

    }    
  
    
    if(m_useTOF&&(*itTrk)->isTofTrackValid()) {  //TOF information
  
      SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
      
      SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();

      for(;iter_tof != tofTrkCol.end(); iter_tof++ ) { 
        TofHitStatus *status = new TofHitStatus; 
        status->setStatus((*iter_tof)->status());

        if(!(status->is_barrel())){//endcap
	  if( (status->is_cluster()) )  m_t_etof[i]  = (*iter_tof)->tof();
          if( !(status->is_counter()) ){if(status) delete status; continue;} // ? 
          if( status->layer()!=0 ){if(status) delete status; continue;}//layer1
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
 
            m_qual_btof2[i]  = qual;
	    m_tof_btof2[i]   = tof ; 
          } 
	}
	if(status) delete status;
      }



    }
    
  }

  //tag

 
  m_hadrontag=0;
  FastVertexFit* fvtxfit = FastVertexFit::instance();
  if(m_ngch != 2  )m_hadrontag=11111;
  else if(m_ngch == 2 &&nCharge==0) {  
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
    WTrackParameter w1_ini,w1_ve,w1_vmu;
    WTrackParameter w2_ini,w2_ve,w2_vmu;
    int iip=0;
    int iim=0;
    for(int i = 0; i < m_ngch; i++ ){  
      if(m_charge[i]>0)itTrk1= evtRecTrkCol->begin() + iGood[i];
      if(m_charge[i]<0)    itTrk2= evtRecTrkCol->begin() + iGood[i];  
      if(m_charge[i]>0)    mdcKalTrk1 = (*itTrk1)->mdcKalTrack(); 
      if(m_charge[i]<0)    mdcKalTrk2 = (*itTrk2)->mdcKalTrack();
      if(m_charge[i]>0)iip=i;
      if(m_charge[i]<0)iim=i; 	  

      
	
	if(m_charge[i]>0)      w1_ini=WTrackParameter (xmass[2],mdcKalTrk1->helix(),mdcKalTrk1->err());
	if(m_charge[i]<0)      w2_ini=WTrackParameter (xmass[2],mdcKalTrk2 ->helix(),mdcKalTrk2 ->err());  	
	if(m_charge[i]>0)      p41h =w1_ini.p();
	if(m_charge[i]<0)      p42h =w2_ini.p();
	if(m_charge[i]>0)      p41h.boost(u_cms);		
	if(m_charge[i]<0)      p42h.boost(u_cms); 	
	if(m_charge[i]>0)      p31h = p41h.vect();
	if(m_charge[i]<0)      p32h = p42h.vect(); 			  	
 
      if(m_charge[i]>0)      p41e =w1_ini.p();
      if(m_charge[i]<0)      p42e =w2_ini.p(); 	
      if(m_charge[i]>0)      p41e.boost(u_cms);		
      if(m_charge[i]<0)      p42e.boost(u_cms); 	
      if(m_charge[i]>0)      p31e = p41e.vect();
      if(m_charge[i]<0)      p32e = p42e.vect();       
      
      
	 	
     
	if(m_charge[i]>0){
	  m_px_cms_ep=p41h.px();
	  m_py_cms_ep=p41h.py();
	  m_pz_cms_ep=p41h.pz();
	  m_e_cms_ep=p41h.e();
	}
	if(m_charge[i]<0){
	  m_px_cms_em=p42h.px();
	  m_py_cms_em=p42h.py();
	  m_pz_cms_em=p42h.pz();
	  m_e_cms_em=p42h.e();
	}
      
    }
     double e01=m_e_emc[iip];//m_e_cms_ep;
    double e02=m_e_emc[iim];//m_e_cms_em;

  int ilarge=( e01 > e02 ) ?iip:iim;

  p4lh=( e01 > e02 ) ?p41h:p42h;

  p3lh=( e01 > e02 ) ?p31h:p32h;
   
  double acollh=   180.-p31h.angle(p32h)* 180.0 / CLHEP::pi;
  double acoplh=   180.- (p31h.perpPart()).angle(p32h.perpPart ())* 180.0 / CLHEP::pi;
  double poeb1h=p41h.rho()/beamEnergy;
  double poeb2h=p42h.rho()/beamEnergy;
  double poeblh=p4lh.rho()/beamEnergy;
    
  double eoeb1=m_e_emc[iip]/beamEnergy;   	
  double eoeb2=m_e_emc[iim]/beamEnergy; 
  double eop1=0;
  if(p41h.rho()>0)eop1=m_e_emc[iip]/p41h.rho();   	
  double eop2=0;
  if(p42h.rho()>0)eop2=m_e_emc[iim]/p42h.rho();  
  
  double eope1=0;
  if(p41e.rho()>0)eope1=m_e_emc[iip]/p41e.rho();   	
  double eope2=0;
  if(p42e.rho()>0)eope2=m_e_emc[iim]/p42e.rho(); 
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
  if(p4lh.rho()>0)eopl=m_e_emc[ilarge]/p4lh.rho();   	
  
  double exoebl=  m_e_emc[ilarge]*sin(m_theta_emc[ilarge])*cos(m_phi_emc[ilarge])/beamEnergy;
  double eyoebl=  m_e_emc[ilarge]*sin(m_theta_emc[ilarge])*sin(m_phi_emc[ilarge])/beamEnergy;   	
  double ezoebl=m_e_emc[ilarge]*cos(m_theta_emc[ilarge])/beamEnergy;
  double etoebl=m_e_emc[ilarge]*sin(m_theta_emc[ilarge])/beamEnergy;
  
  int mucinfo1=(m_nhit_muc[iip]>=2&&m_nlay_muc[iip]>=2 ) ? 1 : 0;
  int mucinfo2=(m_nhit_muc[iim]>=2&&m_nlay_muc[iim]>=2) ? 1 : 0;
  int mucinfol=(m_nhit_muc[ilarge]>=2&&m_nlay_muc[ilarge]>=2) ? 1 : 0;
  int pidel=( e01 > e02 ) ? m_nep : m_nem;     	
  int pidmul=( e01 > e02 ) ? m_nmup : m_nmum;    	    	
  double deltatof=0.0;
  

//   if(m_tof_btof2[iip]*m_tof_btof2[iim]!=0) deltatof+=fabs(m_tof_btof2[iip]-m_tof_btof2[iim]);
//   if(m_tof_btof1[iip]*m_tof_btof1[iim]!=0)deltatof+=fabs(m_tof_btof1[iip]-m_tof_btof1[iim]);
//   if(m_tof_etof[iip]*m_tof_etof[iim]!=0)deltatof+=fabs(m_tof_etof[iip]-m_tof_etof[iim]);
  
    //    if(!m_endcap)  {
      if(m_t_btof[iip]*m_t_btof[iim]!=0) deltatof=fabs(m_t_btof[iip]-m_t_btof[iim]);
      //    }
      //    else      {
      //      if(m_t_etof[iip]*m_t_etof[iim]!=0)deltatof=fabs(m_t_etof[iip]-m_t_etof[iim]);
      //    }   




 

  //   if (acollh>m_acoll_h_cut)m_hadrontag+=1;
    if ((acollh>m_acoll_h_cut)||(!m_useEMC||m_nGam>=m_ngam_h_cut))m_hadrontag+=11;
    if (!m_useTOF||(deltatof<m_dtof_h_cut))m_hadrontag+=100;
    if (!m_useMUC||(mucinfo1==0||mucinfo2==0))m_hadrontag+=1000; 
    if (!m_useEMC||(fabs(eope1-1)>m_eop_h_cut&&fabs(eope2-1)>m_eop_h_cut))m_hadrontag+=10000;	

 

    m_deltatof=deltatof;		
    m_eop1=eope1;
    m_eop2=eope2;
    m_eoeb1=eoeb1;
    m_eoeb2=eoeb2;
    
    m_etoeb1=etoeb1;
    m_etoeb2=etoeb2;
    m_mucinfo1=mucinfo1;
    m_mucinfo2=mucinfo2;




 
      m_acoll=acollh; 
      m_acopl=acoplh;
      m_poeb1=poeb1h;
      m_poeb2=poeb2h;
      m_cos_ep=p41h.cosTheta ();
      m_cos_em=p42h.cosTheta ();
      m_mass_ee=(p41h+p42h).m();

 

    
  
  }
  double br=0;
  double bz=0;
  double thr=0;
  double evis=0;
  WTrackParameter w1_vh,w2_vh,w3_vh;
  
    br=sqrt((px_had+ex_gam)*(px_had+ex_gam)+
	    (py_had+ey_gam)*(py_had+ey_gam))/(pt_had+et_gam);
    bz= fabs(pz_had+ez_gam)/(p_had+e_gam);
    thr=p_had+e_gam;
    evis=e_had+e_gam;
    log << MSG::DEBUG << "hadron " << px_had << " " << py_had << " " << pz_had 
                      << " " << pt_had << " " << p_had << " " << br << " " << bz <<  endmsg;
    log << MSG::DEBUG << "Gamma " << ex_gam << " " << ey_gam << " " << ez_gam 
                      << " " << e_gam << " "  << thr/beamEnergy <<  endmsg;
    if(!m_useEMC||((br<m_br_h_cut)&&(bz<m_bz_h_cut)))m_hadrontag+=100000;
    if(!m_useEMC||thr/beamEnergy>m_thr_h_cut) m_hadrontag+=1000000;
    m_emax=egam_ext;
    m_esum=e_gam;
    m_br=br;
    m_bz=bz;
    m_thr=thr;
    m_evis=evis;	
    log << MSG::INFO << "m_hadrontag= "<<m_hadrontag << endreq;   
//  std::cout<<"m_sbhabhatag= "<<m_sbhabhatag<<std::endl;
//  std::cout<<"m_sdimutag= "<<m_sdimutag<<std::endl;
//  std::cout<<"m_hadrontag= "<<m_hadrontag<<std::endl;
  if(m_hadrontag==1111111){
    nhadron++; 
  for(int i = 0; i < m_ngch; i++ ){
//  m_ha_costheta->Fill(cos(m_theta[i]));
//   m_ha_phi->Fill(m_phi[i]);
     m_ha_costheta->Fill(ctrk_theta);
     m_ha_phi->Fill(ctrk_phi);
  }
    if(m_ngch >= 3){
      RecMdcKalTrack *ktrk0 =   (*(evtRecTrkCol->begin() + iGood[0]))->mdcKalTrack();
      RecMdcKalTrack *ktrk1 =   (*(evtRecTrkCol->begin() + iGood[1]))->mdcKalTrack();
      RecMdcKalTrack *ktrk2 =   (*(evtRecTrkCol->begin() + iGood[2]))->mdcKalTrack();	  
//       w1_vh=WTrackParameter (xmass[2],ktrk0->getZHelix(),ktrk0->getZError());
//       w2_vh=WTrackParameter (xmass[2],ktrk1->getZHelix(),ktrk1->getZError());
//       w3_vh=WTrackParameter (xmass[2],ktrk2->getZHelix(),ktrk2->getZError());
//       vtxfit->init();
//       vtxfit->AddTrack(0, w1_vh);
//       vtxfit->AddTrack(1, w2_vh);
//       vtxfit->AddTrack(2, w3_vh);
//       vtxfit->AddVertex(0, vxpar,0, 1);
// 	  if(vtxfit->Fit(0)) {
// 	    m_ha_vx->Fill((vtxfit->vx(0)).x());
// 	    m_ha_vy->Fill((vtxfit->vx(0)).y());
// 	    m_ha_vz->Fill((vtxfit->vx(0)).z());
// 	  }

      //ktrk0->setPidType(RecMdcKalTrack::pion);
      //	ktrk1->setPidType(RecMdcKalTrack::pion);
      //	ktrk2->setPidType(RecMdcKalTrack::pion);
	fvtxfit->init();
	fvtxfit->addTrack(0,ktrk0->helix(), ktrk0->err());
	fvtxfit->addTrack(1,ktrk1->helix(), ktrk1->err());
	fvtxfit->addTrack(2,ktrk2->helix(), ktrk2->err());
	if(fvtxfit->Fit()) {
	  m_ha_vx->Fill((fvtxfit->Vx())[0]);
	  m_ha_vy->Fill((fvtxfit->Vx())[1]);
	  m_ha_vz->Fill((fvtxfit->Vx())[2]);
	}
	
	
    }
    
     m_ha_br->Fill(br);
    m_ha_bz->Fill(bz); 
    m_ha_pmax->Fill(p_max);
    m_ha_emax->Fill(e_max);
    m_ha_etot->Fill(evis);
    m_ha_nneu->Fill(nGam);
    m_ha_nchg->Fill(m_ngch);
    if(m_ngch==0){
      n0prong++;
  
    }  
    if(m_ngch==2&&nCharge == 0){
      n2prong++;
  
    }
    if(m_ngch==4&&nCharge == 0){
      n4prong++;
  
    }

    if(m_ngch>4){
      ng4prong++;
  
    }
    if(m_writentuple)m_tuple1 -> write();
    setFilterPassed(true);

  }


  return StatusCode::SUCCESS;
 

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQASelHadron::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  log << MSG::INFO << counter << endmsg;
  log << MSG::ALWAYS << "nhadron = " << nhadron << endmsg;
  log << MSG::INFO << "n0prong = " << n0prong << endmsg;
  log << MSG::INFO << "n2prong = " << n2prong << endmsg;
  log << MSG::INFO << "n4prong = " << n4prong << endmsg;
  log << MSG::INFO << "ng4prong = " << ng4prong << endmsg;
  return StatusCode::SUCCESS;
}


