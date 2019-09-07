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
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "PpjrhopiAlg/Ppjrhopi.h"

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "ParticleID/ParticleID.h"

#include <vector>
//const double twopi = 6.2831853;
//const double pi = 3.1415927;
const double mpi = 0.13957;
const double mk = 0.493677;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
//const double velc = 29.9792458;  tof_path unit in cm.
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

int Ncut0,Ncut1,Ncut2,Ncut3,Ncut4,Ncut5,Ncut6,Ncut7,Ncut8,Ncut9,Ncut10;

/////////////////////////////////////////////////////////////////////////////

Ppjrhopi::Ppjrhopi(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("Vr0cut", m_vr0cut=5.0);
  declareProperty("Vz0cut", m_vz0cut=20.0);
  declareProperty("Vr1cut", m_vr1cut=1.0);
  declareProperty("Vz1cut", m_vz1cut=5.0);
  declareProperty("Vctcut", m_cthcut=0.93);
  declareProperty("EnergyThreshold", m_energyThreshold=0.04);
  declareProperty("GammaAngCut", m_gammaAngCut=20.0);
  declareProperty("Test4C", m_test4C = 1);
  declareProperty("Test5C", m_test5C = 1);
  declareProperty("CheckDedx", m_checkDedx = 1);
  declareProperty("CheckTof",  m_checkTof = 1);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode Ppjrhopi::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;

  if(m_test4C==1) {
/*
    NTuplePtr nt4(ntupleSvc(), "FILE1/fit4c");
    if ( nt4 ) m_tuple4 = nt4;
    else {
      m_tuple4 = ntupleSvc()->book ("FILE1/fit4c", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if ( m_tuple4 )    {
        status = m_tuple4->addItem ("run",    m_run);
        status = m_tuple4->addItem ("rec",    m_rec);
        status = m_tuple4->addItem ("nch",    m_nch);
        status = m_tuple4->addItem ("nneu",   m_nneu);
        status = m_tuple4->addItem ("mgdgam", m_gdgam);
        status = m_tuple4->addItem ("recpp",  m_recpp);
	status = m_tuple4->addItem ("chi2",   m_chi1);
	status = m_tuple4->addItem ("mpi0",   m_mpi0); 
	status = m_tuple4->addItem ("mprho0", m_mprho0);  
	status = m_tuple4->addItem ("mprhop", m_mprhop);
	status = m_tuple4->addItem ("mprhom", m_mprhom);
	status = m_tuple4->addItem ("mpjjj",  m_mpjjj);
        status = m_tuple4->addItem ("mbepi0", m_bepi0);
        status = m_tuple4->addItem ("mbe4cjpsi",m_be4cjpsi); 
	status = m_tuple4->addItem ("mp2pi1", m_mp2pi1); 
	status = m_tuple4->addItem ("mf2pi1g1", m_mf2pi1g1);
	status = m_tuple4->addItem ("mf2pi1g2", m_mf2pi1g2);
	status = m_tuple4->addItem ("mf2pi1pi0",m_mf2pi1pi0);
	status = m_tuple4->addItem ("mt2pi2g1", m_mt2pi2g1);
	status = m_tuple4->addItem ("mt2pi2g2", m_mt2pi2g2);
	status = m_tuple4->addItem ("mp2pi3",   m_mp2pi3); 
	status = m_tuple4->addItem ("mf2pi3g1", m_mf2pi3g1);
	status = m_tuple4->addItem ("mf2pi3g2", m_mf2pi3g2);
	status = m_tuple4->addItem ("mf2pi3pi0",m_mf2pi3pi0);
	status = m_tuple4->addItem ("mp2pi4",   m_mp2pi4); 
	status = m_tuple4->addItem ("mf2pi4g1", m_mf2pi4g1);
	status = m_tuple4->addItem ("mf2pi4g2", m_mf2pi4g2);
	status = m_tuple4->addItem ("mf2pi4pi0",m_mf2pi4pi0);
	status = m_tuple4->addItem ("mp4pi",    m_mp4pi); 
	status = m_tuple4->addItem ("mppptot",  m_mppptot);
	status = m_tuple4->addItem ("mp4pig1",  m_mp4pig1);
	status = m_tuple4->addItem ("mp4pig2",  m_mp4pig2);    
	status = m_tuple4->addItem ("mpx1",     m_mpx1);
	status = m_tuple4->addItem ("mpy1",     m_mpy1);
	status = m_tuple4->addItem ("mpz1",     m_mpz1);
	status = m_tuple4->addItem ("mpe1",     m_mpe1);
	status = m_tuple4->addItem ("mpx2",     m_mpx2);
	status = m_tuple4->addItem ("mpy2",     m_mpy2);
	status = m_tuple4->addItem ("mpz2",     m_mpz2);
	status = m_tuple4->addItem ("mpe2",     m_mpe2);
	status = m_tuple4->addItem ("mpx3",     m_mpx3);
	status = m_tuple4->addItem ("mpy3",     m_mpy3);
	status = m_tuple4->addItem ("mpz3",     m_mpz3);
	status = m_tuple4->addItem ("mpe3",     m_mpe3);
	status = m_tuple4->addItem ("mpx4",     m_mpx4);
	status = m_tuple4->addItem ("mpy4",     m_mpy4);
	status = m_tuple4->addItem ("mpz4",     m_mpz4);
	status = m_tuple4->addItem ("mpe4",     m_mpe4);
	status = m_tuple4->addItem ("mpxg1",    m_mpxg1);
	status = m_tuple4->addItem ("mpyg1",    m_mpyg1);
	status = m_tuple4->addItem ("mpzg1",    m_mpzg1);
	status = m_tuple4->addItem ("mpeg1",    m_mpeg1);
	status = m_tuple4->addItem ("mpxg2",    m_mpxg2);
	status = m_tuple4->addItem ("mpyg2",    m_mpyg2);
	status = m_tuple4->addItem ("mpzg2",    m_mpzg2);
	status = m_tuple4->addItem ("mpeg2",    m_mpeg2);
	status = m_tuple4->addItem ("chikk",    m_chikk);
	status = m_tuple4->addItem ("p1vx",     m_p1vx);
	status = m_tuple4->addItem ("p1vy",     m_p1vy);
	status = m_tuple4->addItem ("p1vz",     m_p1vz);
	status = m_tuple4->addItem ("p1vr",     m_p1vr);
	status = m_tuple4->addItem ("p1vct",    m_p1vct);
	status = m_tuple4->addItem ("m1vx",     m_m1vx);
	status = m_tuple4->addItem ("m1vy",     m_m1vy);
	status = m_tuple4->addItem ("m1vz",     m_m1vz);
	status = m_tuple4->addItem ("m1vr",     m_m1vr);
	status = m_tuple4->addItem ("m1vct",    m_m1vct);
	status = m_tuple4->addItem ("p2vx",     m_p2vx);
	status = m_tuple4->addItem ("p2vy",     m_p2vy);
	status = m_tuple4->addItem ("p2vz",     m_p2vz);
	status = m_tuple4->addItem ("p2vr",     m_p2vr);
	status = m_tuple4->addItem ("p2vct",    m_p2vct);
	status = m_tuple4->addItem ("m2vx",     m_m2vx);
	status = m_tuple4->addItem ("m2vy",     m_m2vy);
	status = m_tuple4->addItem ("m2vz",     m_m2vz);
	status = m_tuple4->addItem ("m2vr",     m_m2vr);
	status = m_tuple4->addItem ("m2vct",    m_m2vct);
        status = m_tuple4->addItem ("mgood",    m_good);
        status = m_tuple4->addItem ("mgam",     m_gam);
        status = m_tuple4->addItem ("mpip",     m_pip);
        status = m_tuple4->addItem ("mpim",     m_pim);
	status = m_tuple4->addItem ("mp1ptot",  m_p1ptot);
	status = m_tuple4->addItem ("memcTp1",  m_emcTp1);
	status = m_tuple4->addItem ("mm1ptot",  m_m1ptot);
	status = m_tuple4->addItem ("memcTm1",  m_emcTm1);
	status = m_tuple4->addItem ("mp2ptot",  m_p2ptot);
	status = m_tuple4->addItem ("memcTp2",  m_emcTp2);
	status = m_tuple4->addItem ("mm2ptot",  m_m2ptot);
	status = m_tuple4->addItem ("memcTm2",  m_emcTm2);
	status = m_tuple4->addItem ("p1pxy",    m_p1pxy);
	status = m_tuple4->addItem ("m1pxy",    m_m1pxy);
	status = m_tuple4->addItem ("p2pxy",    m_p2pxy);
	status = m_tuple4->addItem ("m2pxy",    m_m2pxy);

        status = m_tuple4->addItem ("mpidpip", m_pidpip, 0, 10);        
        status = m_tuple4->addIndexedItem ("mipipin" , m_pidpip, m_ipipin);
        status = m_tuple4->addItem ("mpidpim", m_pidpim, 0, 10);        
        status = m_tuple4->addIndexedItem ("mipimin" , m_pidpim, m_ipimin);

	status = m_tuple4->addItem ("laypip1",  m_laypip1);
	status = m_tuple4->addItem ("laypim1",  m_laypim1);
	status = m_tuple4->addItem ("laypip2",  m_laypip2);
	status = m_tuple4->addItem ("laypim2",  m_laypim2);
	status = m_tuple4->addItem ("mangle",   m_angle);
	status = m_tuple4->addItem ("cosuubr",m_cosuubr );
	status = m_tuple4->addItem ("cosmupbr", m_cosmupbr);
	status = m_tuple4->addItem ("cosmumbr", m_cosmumbr);
	status = m_tuple4->addItem ("phimupbr", m_phimupbr);
	status = m_tuple4->addItem ("phimumbr", m_phimumbr);  
        status = m_tuple4->addItem ("ngch",     m_ngch,  0, 10);
        status = m_tuple4->addItem ("nggneu",   m_nggneu,0, 10);
	// modifiey by Zhu
	//        status = m_tuple4->addItem ("indx0",   indx0, 0, 10);
        status = m_tuple4->addIndexedItem ("mptrk" , m_ngch, m_ptrk);
        status = m_tuple4->addIndexedItem ("chie",   m_ngch, m_chie);
        status = m_tuple4->addIndexedItem ("chimu",  m_ngch,m_chimu);
        status = m_tuple4->addIndexedItem ("chipi",  m_ngch,m_chipi);
        status = m_tuple4->addIndexedItem ("chik",   m_ngch,m_chik);
        status = m_tuple4->addIndexedItem ("chip",   m_ngch,m_chip);
        status = m_tuple4->addIndexedItem ("probPH", m_ngch,m_probPH);
        status = m_tuple4->addIndexedItem ("normPH", m_ngch,m_normPH);
        status = m_tuple4->addIndexedItem ("ghit",   m_ngch,m_ghit);
        status = m_tuple4->addIndexedItem ("thit",   m_ngch,m_thit);

        status = m_tuple4->addIndexedItem ("ptot_etof", m_ngch,m_ptot_etof);
        status = m_tuple4->addIndexedItem ("cntr_etof", m_ngch,m_cntr_etof);
        status = m_tuple4->addIndexedItem ("te_etof",   m_ngch,m_te_etof);
        status = m_tuple4->addIndexedItem ("tmu_etof",  m_ngch,m_tmu_etof);
        status = m_tuple4->addIndexedItem ("tpi_etof",  m_ngch,m_tpi_etof);
        status = m_tuple4->addIndexedItem ("tk_etof",   m_ngch,m_tk_etof);
        status = m_tuple4->addIndexedItem ("tp_etof",   m_ngch,m_tp_etof);
        status = m_tuple4->addIndexedItem ("ph_etof",   m_ngch,m_ph_etof);
        status = m_tuple4->addIndexedItem ("rhit_etof", m_ngch,m_rhit_etof);
        status = m_tuple4->addIndexedItem ("qual_etof", m_ngch,m_qual_etof);
        status = m_tuple4->addIndexedItem ("ec_toff_e", m_ngch,m_ec_toff_e);
        status = m_tuple4->addIndexedItem ("ec_toff_mu",m_ngch,m_ec_toff_mu);
        status = m_tuple4->addIndexedItem ("ec_toff_pi",m_ngch,m_ec_toff_pi);
        status = m_tuple4->addIndexedItem ("ec_toff_k", m_ngch,m_ec_toff_k);
        status = m_tuple4->addIndexedItem ("ec_toff_p", m_ngch,m_ec_toff_p);
        status = m_tuple4->addIndexedItem ("ec_tsig_e", m_ngch,m_ec_tsig_e);
        status = m_tuple4->addIndexedItem ("ec_tsig_mu",m_ngch,m_ec_tsig_mu);
        status = m_tuple4->addIndexedItem ("ec_tsig_pi",m_ngch,m_ec_tsig_pi);
        status = m_tuple4->addIndexedItem ("ec_tsig_k", m_ngch,m_ec_tsig_k);
        status = m_tuple4->addIndexedItem ("ec_tsig_p", m_ngch,m_ec_tsig_p);
        status = m_tuple4->addIndexedItem ("ec_tof",    m_ngch,m_ec_tof);
        status = m_tuple4->addIndexedItem ("ptot_btof1",m_ngch,m_ptot_btof1);
        status = m_tuple4->addIndexedItem ("cntr_btof1",m_ngch,m_cntr_btof1);
        status = m_tuple4->addIndexedItem ("te_btof1",  m_ngch,m_te_btof1);
        status = m_tuple4->addIndexedItem ("tmu_btof1", m_ngch,m_tmu_btof1);
        status = m_tuple4->addIndexedItem ("tpi_btof1", m_ngch,m_tpi_btof1);
        status = m_tuple4->addIndexedItem ("tk_btof1",  m_ngch,m_tk_btof1);
        status = m_tuple4->addIndexedItem ("tp_btof1",  m_ngch,m_tp_btof1);
        status = m_tuple4->addIndexedItem ("ph_btof1",  m_ngch,m_ph_btof1);
        status = m_tuple4->addIndexedItem ("zhit_btof1",m_ngch,m_zhit_btof1);
        status = m_tuple4->addIndexedItem ("qual_btof1",m_ngch,m_qual_btof1);
        status = m_tuple4->addIndexedItem ("b1_toff_e", m_ngch,m_b1_toff_e);
        status = m_tuple4->addIndexedItem ("b1_toff_mu",m_ngch,m_b1_toff_mu);
        status = m_tuple4->addIndexedItem ("b1_toff_pi",m_ngch,m_b1_toff_pi);
        status = m_tuple4->addIndexedItem ("b1_toff_k", m_ngch,m_b1_toff_k);
        status = m_tuple4->addIndexedItem ("b1_toff_p", m_ngch,m_b1_toff_p);
        status = m_tuple4->addIndexedItem ("b1_tsig_e", m_ngch,m_b1_tsig_e);
        status = m_tuple4->addIndexedItem ("b1_tsig_mu",m_ngch,m_b1_tsig_mu);
        status = m_tuple4->addIndexedItem ("b1_tsig_pi",m_ngch,m_b1_tsig_pi);
        status = m_tuple4->addIndexedItem ("b1_tsig_k", m_ngch,m_b1_tsig_k);
        status = m_tuple4->addIndexedItem ("b1_tsig_p", m_ngch,m_b1_tsig_p);
        status = m_tuple4->addIndexedItem ("b1_tof",    m_ngch,m_b1_tof);                                                    

        status = m_tuple4->addIndexedItem ("mdedx_pid", m_ngch,m_dedx_pid);
        status = m_tuple4->addIndexedItem ("mtof1_pid", m_ngch,m_tof1_pid);
        status = m_tuple4->addIndexedItem ("mtof2_pid", m_ngch,m_tof2_pid);
        status = m_tuple4->addIndexedItem ("mprob_pid", m_ngch,m_prob_pid);
        status = m_tuple4->addIndexedItem ("mptrk_pid", m_ngch,m_ptrk_pid);
        status = m_tuple4->addIndexedItem ("mcost_pid", m_ngch,m_cost_pid);

        status = m_tuple4->addIndexedItem ("numHits",      m_nggneu,m_numHits);    // Total number of hits
        status = m_tuple4->addIndexedItem ("secondmoment", m_nggneu,m_secondmoment);
        status = m_tuple4->addIndexedItem ("mx",           m_nggneu,m_x);       //  Shower coordinates and errors
        status = m_tuple4->addIndexedItem ("my",           m_nggneu,m_y);
        status = m_tuple4->addIndexedItem ("mz",           m_nggneu,m_z);
        status = m_tuple4->addIndexedItem ("cosemc",       m_nggneu,m_cosemc);   // Shower Counter angles and errors
        status = m_tuple4->addIndexedItem ("phiemc",       m_nggneu,m_phiemc);
        status = m_tuple4->addIndexedItem ("energy",       m_nggneu,m_energy);  // Total energy observed in Emc
        status = m_tuple4->addIndexedItem ("eseed",        m_nggneu,m_eSeed);
        status = m_tuple4->addIndexedItem ("me9",          m_nggneu,m_e3x3); 
        status = m_tuple4->addIndexedItem ("me25",         m_nggneu,m_e5x5); 
        status = m_tuple4->addIndexedItem ("mlat",         m_nggneu,m_lat);
        status = m_tuple4->addIndexedItem ("ma20",         m_nggneu,m_a20);
        status = m_tuple4->addIndexedItem ("ma42",         m_nggneu,m_a42);  


      }
      else    { 
	log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple4) << endmsg;
	return StatusCode::FAILURE;
      }
    }
*/
  } // test 4C

  //
  //--------end of book--------
  //

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode Ppjrhopi::execute() {
  
//  std::cout << "execute()" << std::endl;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  setFilterPassed(false);

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int runNo=eventHeader->runNumber(); 
  int event=eventHeader->eventNumber();
  log << MSG::DEBUG <<"runNo, evtnum = "
      << runNo << " , "
      << event <<endreq;

  Ncut0++;

//   FOR 6.4.0 EventModel::Recon--->EventModel::EvtRec IN 6.4.1
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  log << MSG::INFO << "get event tag OK" << endreq;
    log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;



  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
  //
  // check x0, y0, z0, r0
  // suggest cut: |z0|<5 && r0<1
  //
  if(evtRecEvent->totalNeutral()>100) {
    return StatusCode::SUCCESS;
  }

  Vint iGood, ipip, ipim;
  iGood.clear();
  ipip.clear();
  ipim.clear();
  Vp4 ppip, ppim;
  ppip.clear();
  ppim.clear();

  Hep3Vector xorigin(0,0,0);
 
  //if (m_reader.isRunNumberValid(runNo)) {
   IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid()){ 
  double* dbv = vtxsvc->PrimaryVertex();
  double*  vv = vtxsvc->SigmaPrimaryVertex();
//    HepVector dbv = m_reader.PrimaryVertex(runNo);
//    HepVector vv = m_reader.SigmaPrimaryVertex(runNo);
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);
  }

  int nCharge = 0;
  for(int i = 0; i < evtRecEvent->totalCharged(); i++){
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    if (!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();

    double pch =mdcTrk->p();
    double x0  =mdcTrk->x(); 
    double y0  =mdcTrk->y(); 
    double z0  =mdcTrk->z(); 
    double phi0=mdcTrk->helix(1);
    double xv=xorigin.x();
    double yv=xorigin.y();
    double Rxy=fabs((x0-xv)*cos(phi0)+(y0-yv)*sin(phi0));
// 2009//4
    double m_vx0 = x0;
    double m_vy0 = y0;
    double m_vz0 = z0-xorigin.z();
    double m_vr0 = Rxy;
    double m_Vctc=z0/sqrt(Rxy*Rxy+z0*z0);
    double m_Vct =cos(mdcTrk->theta());

//    m_tuple1->write();
//2009//4
    if(fabs(m_vz0) >= m_vz0cut) continue;
    if(m_vr0 >= m_vr0cut) continue;
//    if(fabs(m_Vct)>=m_cthcut) continue;
    iGood.push_back((*itTrk)->trackId());
    nCharge += mdcTrk->charge();
  }
  
  //
  // Finish Good Charged Track Selection
  //
  int nGood = iGood.size();

  log << MSG::DEBUG << "ngood, totcharge = " << nGood << " , " << nCharge << endreq;
  if((nGood != 4)||(nCharge!=0)){
    return StatusCode::SUCCESS;
  }

  Ncut1++;

  Vint iGam;
  iGam.clear();
  for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
    // find the nearest charged track
    double dthe = 200.;
    double dphi = 200.;
    double dang = 200.; 
  log << MSG::DEBUG << "liuf neu= "  <<i <<endreq;
    for(int j = 0; j < evtRecEvent->totalCharged(); j++) {
      EvtRecTrackIterator jtTrk = evtRecTrkCol->begin() + j;
      if(!(*jtTrk)->isExtTrackValid()) continue;
      RecExtTrack *extTrk = (*jtTrk)->extTrack();
      if(extTrk->emcVolumeNumber() == -1) continue;
      Hep3Vector extpos = extTrk->emcPosition();
  log << MSG::DEBUG << "liuf charge= "  <<j <<endreq;
      //      double ctht = extpos.cosTheta(emcpos);
      double angd = extpos.angle(emcpos);
      double thed = extpos.theta() - emcpos.theta();
      double phid = extpos.deltaPhi(emcpos);
      thed = fmod(thed+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;
      phid = fmod(phid+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;

//      if(fabs(thed) < fabs(dthe)) dthe = thed;
//      if(fabs(phid) < fabs(dphi)) dphi = phid;
      if(angd < dang) {
         dang = angd;
         dthe = thed;
         dphi = phid;
         }
    }
    if(dang>=200) continue;
    double eraw = emcTrk->energy();
    dthe = dthe * 180 / (CLHEP::pi);
    dphi = dphi * 180 / (CLHEP::pi);
    dang = dang * 180 / (CLHEP::pi);
// 2009//4
    double m_dthe = dthe;
    double m_dphi = dphi;
    double m_dang = dang;
    double m_eraw = eraw;
//    m_tuple2->write();
// 2009//4
  log << MSG::DEBUG << "eraw dang= " << eraw  << " , " <<dang <<"," <<i <<endreq;
    if(eraw < m_energyThreshold) continue;
    if(dang < m_gammaAngCut) continue;
//    if((fabs(dthe) < m_gammaThetaCut) && (fabs(dphi)<m_gammaPhiCut) ) continue;
    //
    // good photon cut will be set here
    //
    iGam.push_back((*itTrk)->trackId());
  }
  
  //
  // Finish Good Photon Selection
  //
  int nGam = iGam.size();

  log << MSG::DEBUG << "num Good Photon " << nGam  << " , " <<evtRecEvent->totalNeutral()<<endreq;
  if(nGam<2){
    return StatusCode::SUCCESS;
  }

  Ncut2++;




  //
  // Assign 4-momentum to each photon
  // 

  Vp4 pGam;
  pGam.clear();
  for(int i = 0; i < nGam; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGam[i]; 
    RecEmcShower* emcTrk = (*itTrk)->emcShower();
    double eraw = emcTrk->energy();
    double phi = emcTrk->phi();
    double the = emcTrk->theta();
    HepLorentzVector ptrk;
    ptrk.setPx(eraw*sin(the)*cos(phi));
    ptrk.setPy(eraw*sin(the)*sin(phi));
    ptrk.setPz(eraw*cos(the));
    ptrk.setE(eraw);

//    ptrk = ptrk.boost(-0.011,0,0);// boost to cms

    pGam.push_back(ptrk);
  }


  for(int i = 0; i < nGood; i++) {//for rhopi without PID
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i];
    if (!(*itTrk)->isMdcTrackValid()) continue;
    RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
    if (!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();
    mdcKalTrk->setPidType(RecMdcKalTrack::pion);
    if(mdcKalTrk->charge() >0 ) {
      ipip.push_back(iGood[i]);
      HepLorentzVector ptrk;
      ptrk.setPx(mdcKalTrk->px());
      ptrk.setPy(mdcKalTrk->py());
      ptrk.setPz(mdcKalTrk->pz());
      double p3 = ptrk.mag();
      ptrk.setE(sqrt(p3*p3+mpi*mpi));
      ppip.push_back(ptrk);
    } else {
      ipim.push_back(iGood[i]);
      HepLorentzVector ptrk;
      ptrk.setPx(mdcKalTrk->px());
      ptrk.setPy(mdcKalTrk->py());
      ptrk.setPz(mdcKalTrk->pz());
      double p3 = ptrk.mag();
      ptrk.setE(sqrt(p3*p3+mpi*mpi));
      ppim.push_back(ptrk);
    }
  }// without PID


  int npip = ipip.size();
  int npim = ipim.size();
  if(npip!=2||npim != 2) return SUCCESS;
/*
  log << MSG::DEBUG << "ngood track ID = " << ipip[0] << " , " 
      << ipim[0]<< " , " << ipip[1] << " , " << ipim[1] << endreq;
*/
  Ncut3++;


  //
  //   find the two pi from the primary vetex
  //   ipip[0] && ipim[0] from ppsi
  //   ipip[1] && ipim[1] from jpsi
  //   should change track ID
  //
  HepLorentzVector pTot0(0.011*3.6862,0,0,3.6862);
  HepLorentzVector pTrec1,pTrec2,pTrec3,pTrec4;
  HepLorentzVector pTrecf;
  double  m_recjpsi1,m_recjpsi2,m_recjpsi3,m_recjpsi4,m_recppf;   
  double deljp1,deljp2,deljp3,deljp4;
      pTrec1 = pTot0 - ppip[0] - ppim[0];
      pTrec2 = pTot0 - ppip[0] - ppim[1];
      pTrec3 = pTot0 - ppip[1] - ppim[0];
      pTrec4 = pTot0 - ppip[1] - ppim[1];
      m_recjpsi1 = pTrec1.m();
      m_recjpsi2 = pTrec2.m();
      m_recjpsi3 = pTrec3.m();
      m_recjpsi4 = pTrec4.m();
      deljp1=fabs(m_recjpsi1-3.097);
      deljp2=fabs(m_recjpsi2-3.097);
      deljp3=fabs(m_recjpsi3-3.097);
      deljp4=fabs(m_recjpsi4-3.097);

      int itmp,itmp1,itmp2;
      HepLorentzVector ptmp,ptmp1,ptmp2;

      pTrecf =pTrec1;
      m_recppf=pTrec1.m();

      if(deljp2<deljp1&&deljp2<deljp3&&deljp2<deljp4) 
      { itmp= ipim[1];
        ipim[1]=ipim[0];
        ipim[0]=itmp;

        ptmp =ppim[1];
        ppim[1]=ppim[0];
        ppim[0]=ptmp;  

        pTrecf =pTrec2;
        m_recppf=pTrec2.m();
      }

      if(deljp3<deljp1&&deljp3<deljp2&&deljp3<deljp4) 
      { itmp= ipip[1];
        ipip[1]=ipip[0];
        ipip[0]=itmp;

        ptmp =ppip[1];
        ppip[1]=ppip[0];
        ppip[0]=ptmp;  

        pTrecf =pTrec3;
        m_recppf=pTrec3.m();
      }

      if(deljp4<deljp1&&deljp4<deljp2&&deljp4<deljp3) 
      { itmp1= ipip[1];
        ipip[1]=ipip[0];
        ipip[0]=itmp1;
        itmp2= ipim[1];
        ipim[1]=ipim[0];
        ipim[0]=itmp2;

        ptmp1 =ppip[1];
        ppip[1]=ppip[0];
        ppip[0]=ptmp1;  
        ptmp2 =ppim[1];
        ppim[1]=ppim[0];
        ppim[0]=ptmp2;  

        pTrecf =pTrec4;
        m_recppf=pTrec4.m();
      }

        if(fabs(m_recppf-3.097)>0.2)  return SUCCESS;

  log << MSG::DEBUG << "ngood track ID after jpsi = " << ipip[0] << " , " 
      << ipim[0]<< " , " << ipip[1] << " , " << ipim[1] << endreq;
  Ncut4++;

  HepLorentzVector ppi2_no1 = ppip[0] + ppim[0];
  HepLorentzVector ppi2_no2 = ppip[1] + ppim[1];
  HepLorentzVector ppi2_no3 = ppip[0] + ppim[1];
  HepLorentzVector ppi2_no4 = ppip[1] + ppim[0];
  HepLorentzVector p4pi_no = ppi2_no1+ ppi2_no2;

    double emcTg1=0.0;
    double emcTg2=0.0;
    double emcTg3=0.0;
    double emcTg4=0.0;
    double laypi1=-1.0;
    double laypi2=-1.0;
    double laypi3=-1.0;
    double laypi4=-1.0;

    EvtRecTrackIterator itTrkp1=evtRecTrkCol->begin() + ipip[0];
    RecMdcTrack*  mdcTrkp1 = (*itTrkp1)->mdcTrack();
    RecMdcKalTrack *mdcKalTrkp1 = (*itTrkp1)->mdcKalTrack();
    RecEmcShower* emcTrkp1 = (*itTrkp1)->emcShower();
    RecMucTrack *mucTrkp1=(*itTrkp1)->mucTrack();

    double phi01=mdcTrkp1->helix(1);  
    double m_p1vx = mdcTrkp1->x();
    double m_p1vy = mdcTrkp1->y();
    double m_p1vz = mdcTrkp1->z()-xorigin.z();
    double m_p1vr = fabs((mdcTrkp1->x()-xorigin.x())*cos(phi01)+(mdcTrkp1->y()-xorigin.y())*sin(phi01));
    double m_p1vct=cos(mdcTrkp1->theta());
    double m_p1ptot=mdcKalTrkp1->p();
    double m_p1pxy=sqrt(mdcKalTrkp1->px()*mdcKalTrkp1->px()+mdcKalTrkp1->py()*mdcKalTrkp1->py());

    if((*itTrkp1)->isEmcShowerValid()){
    emcTg1=emcTrkp1->energy();
    }
    if((*itTrkp1)->isMucTrackValid()){
    laypi1=mucTrkp1->numLayers();
    }
    double m_laypip1=laypi1;

    EvtRecTrackIterator itTrkm1=evtRecTrkCol->begin() + ipim[0];
    RecMdcTrack*  mdcTrkm1 = (*itTrkm1)->mdcTrack();
    RecMdcKalTrack *mdcKalTrkm1 = (*itTrkm1)->mdcKalTrack();
    RecEmcShower* emcTrkm1 = (*itTrkm1)->emcShower();
    RecMucTrack *mucTrkm1=(*itTrkm1)->mucTrack();

    double phi02=mdcTrkm1->helix(1);  
    double m_m1vx = mdcTrkm1->x();
    double m_m1vy = mdcTrkm1->y();
    double m_m1vz = mdcTrkm1->z()-xorigin.z();
    double m_m1vr = fabs((mdcTrkm1->x()-xorigin.x())*cos(phi02)+(mdcTrkm1->y()-xorigin.y())*sin(phi02));
    double m_m1vct=cos(mdcTrkm1->theta());
    double m_m1ptot=mdcKalTrkm1->p();
    double m_m1pxy=sqrt(mdcKalTrkm1->px()*mdcKalTrkm1->px()+mdcKalTrkm1->py()*mdcKalTrkm1->py());

    if((*itTrkm1)->isEmcShowerValid()){
    emcTg2= emcTrkm1->energy();
    }
    if((*itTrkm1)->isMucTrackValid()){
    laypi2=mucTrkm1->numLayers();
    }
    double m_laypim1=laypi2;

    EvtRecTrackIterator itTrkp2=evtRecTrkCol->begin() + ipip[1];
    RecMdcTrack*  mdcTrkp2 = (*itTrkp2)->mdcTrack();
    RecMdcKalTrack *mdcKalTrkp2 = (*itTrkp2)->mdcKalTrack();
    RecEmcShower* emcTrkp2 = (*itTrkp2)->emcShower();
    RecMucTrack *mucTrkp2=(*itTrkp2)->mucTrack();

    double phi03=mdcTrkp2->helix(1);  
    double m_p2vx = mdcTrkp2->x();
    double m_p2vy = mdcTrkp2->y();
    double m_p2vz = mdcTrkp2->z()-xorigin.z();
    double m_p2vr = fabs((mdcTrkp2->x()-xorigin.x())*cos(phi03)+(mdcTrkp2->y()-xorigin.y())*sin(phi03));
    double m_p2vct=cos(mdcTrkp2->theta());
    double m_p2ptot=mdcKalTrkp2->p();
    double m_p2pxy=sqrt(mdcKalTrkp2->px()*mdcKalTrkp2->px()+mdcKalTrkp2->py()*mdcKalTrkp2->py());

    if((*itTrkp2)->isEmcShowerValid()){
    emcTg3= emcTrkp2->energy();
    }
    if((*itTrkp2)->isMucTrackValid()){
    laypi3=mucTrkp2->numLayers();
    }
    double m_laypip2=laypi3;

    EvtRecTrackIterator itTrkm2=evtRecTrkCol->begin() + ipim[1];
    RecMdcTrack*  mdcTrkm2 = (*itTrkm2)->mdcTrack();
    RecMdcKalTrack *mdcKalTrkm2 = (*itTrkm2)->mdcKalTrack();
    RecEmcShower* emcTrkm2 = (*itTrkm2)->emcShower();
    RecMucTrack *mucTrkm2=(*itTrkm2)->mucTrack();

    double phi04=mdcTrkm2->helix(1);  
    double m_m2vx = mdcTrkm2->x();
    double m_m2vy = mdcTrkm2->y();
    double m_m2vz = mdcTrkm2->z()-xorigin.z();
    double m_m2vr = fabs((mdcTrkm2->x()-xorigin.x())*cos(phi04)+(mdcTrkm2->y()-xorigin.y())*sin(phi04));
    double m_m2vct=cos(mdcTrkm2->theta());
    double m_m2ptot=mdcKalTrkm2->p();
    double m_m2pxy=sqrt(mdcKalTrkm2->px()*mdcKalTrkm2->px()+mdcKalTrkm2->py()*mdcKalTrkm2->py());

    if((*itTrkm2)->isEmcShowerValid()){
    emcTg4= emcTrkm2->energy();
    }
    if((*itTrkm2)->isMucTrackValid()){
    laypi4=mucTrkm2->numLayers();
    }
    double m_laypim2=laypi4;

    double m_emcTp1 =emcTg1; 
    double m_emcTm1 =emcTg2; 
    double m_emcTp2 =emcTg3;
    double m_emcTm2 =emcTg4; 

    if(fabs(m_p1vz) >= m_vz1cut) return SUCCESS;
    if(m_p1vr >= m_vr1cut) return SUCCESS;
    if(fabs(m_p1vct)>=m_cthcut) return SUCCESS;

    if(fabs(m_m1vz) >= m_vz1cut) return SUCCESS;
    if(m_m1vr >= m_vr1cut) return SUCCESS;
    if(fabs(m_m1vct)>=m_cthcut) return SUCCESS;
    Ncut5++;

  HepLorentzVector p4muonp = ppip[1];
  HepLorentzVector p4muonm = ppim[1];
  HepLorentzVector p4uu  =   pTrecf;

 //Lorentz transformation : boost and rotate
  Hep3Vector  p3jpsiUnit = (p4uu.vect()).unit();
  double      jBeta = p4uu.beta(); // just same as the P/E

//  std::cout << jBeta << " " << p4uu.beta() << std::endl;

  //
  // Loop each gamma pair, check ppi0, pTot
  // and other mass from MDC momentum  
  //

  HepLorentzVector pTot;
  double minpi0=999.0;
  for(int i = 0; i < nGam - 1; i++){
    for(int j = i+1; j < nGam; j++) {
      HepLorentzVector p2g = pGam[i] + pGam[j];
      pTot = ppip[0] + ppim[0] + ppip[1] + ppim[1];
      pTot += p2g;
      if(fabs(p2g.m()-0.135)<minpi0){
           minpi0 = fabs(p2g.m()-0.135);
// 2009//4
           double  m_m2gg = p2g.m();
// 2009//4
           HepLorentzVector prho0_no = ppi2_no2;
           HepLorentzVector prhop_no = ppip[1] + p2g;
           HepLorentzVector prhom_no = ppim[1] + p2g;
           HepLorentzVector prho0pi0 = ppi2_no2 + p2g;
           HepLorentzVector frho1pi0 = ppi2_no1 + p2g;
           HepLorentzVector frho2pi0 = ppi2_no3 + p2g;
           HepLorentzVector frho3pi0 = ppi2_no4 + p2g;
           HepLorentzVector prho0g1 = ppi2_no2 + pGam[i];
           HepLorentzVector prho0g2 = ppi2_no2 + pGam[j];
           HepLorentzVector frho1g1 = ppi2_no1 + pGam[i];
           HepLorentzVector frho1g2 = ppi2_no1 + pGam[j];
           HepLorentzVector frho2g1 = ppi2_no3 + pGam[i];
           HepLorentzVector frho2g2 = ppi2_no3 + pGam[j];
           HepLorentzVector frho3g1 = ppi2_no4 + pGam[i];
           HepLorentzVector frho3g2 = ppi2_no4 + pGam[j];
           HepLorentzVector p5pi_no = p4pi_no + p2g;

// 2009//4
      double m_prho0_no = prho0_no.m();
      double m_prhop_no = prhop_no.m();
      double m_prhom_no = prhom_no.m();
      double m_prho0pi0 = prho0pi0.m();
      double m_frho1pi0 = frho1pi0.m();
      double m_frho2pi0 = frho2pi0.m();
      double m_frho3pi0 = frho3pi0.m();
      double m_prho0g1  = prho0g1.m();
      double m_prho0g2  = prho0g2.m();
      double m_frho1g1  = frho1g1.m();
      double m_frho1g2  = frho1g2.m();
      double m_frho2g1  = frho2g1.m();
      double m_frho2g2  = frho2g2.m();
      double m_frho3g1  = frho3g1.m();
      double m_frho3g2  = frho3g2.m();
      double m_p4pi_no  = p4pi_no.m();
      double m_p5pi_no  = p5pi_no.m();
      double m_mdcpx1=ppip[0].px();
      double m_mdcpy1=ppip[0].py();
      double m_mdcpz1=ppip[0].pz();
      double m_mdcpe1=ppip[0].e();
      double m_mdcpx2=ppim[0].px();
      double m_mdcpy2=ppim[0].py();
      double m_mdcpz2=ppim[0].pz();
      double m_mdcpe2=ppim[0].e();
      double m_mdcpx3=ppip[1].px();
      double m_mdcpy3=ppip[1].py();
      double m_mdcpz3=ppip[1].pz();
      double m_mdcpe3=ppip[1].e();
      double m_mdcpx4=ppim[1].px();
      double m_mdcpy4=ppim[1].py();
      double m_mdcpz4=ppim[1].pz();
      double m_mdcpe4=ppim[1].e();
      double m_mdcpxg1=pGam[i].px();
      double m_mdcpyg1=pGam[i].py();
      double m_mdcpzg1=pGam[i].pz();
      double m_mdcpeg1=pGam[i].e();
      double m_mdcpxg2=pGam[j].px();
      double m_mdcpyg2=pGam[j].py();
      double m_mdcpzg2=pGam[j].pz();
      double m_mdcpeg2=pGam[j].e();
      double m_etot = pTot.e();
      double m_mrecjp1=m_recjpsi1;
      double m_mrecjp2=m_recjpsi2;
      double m_mrecjp3=m_recjpsi3;
      double m_mrecjp4=m_recjpsi4;
//      m_tuple3 -> write();
// 2009//4
     }
    }
  }
    Ncut6++;  


  //
  //    Test vertex fit
  //

  HepPoint3D vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  double bx = 1E+6;
  double by = 1E+6;
  double bz = 1E+6;
  Evx[0][0] = bx*bx;
  Evx[1][1] = by*by;
  Evx[2][2] = bz*bz;

  VertexParameter vxpar;
  vxpar.setVx(vx);
  vxpar.setEvx(Evx);
  
  VertexFit* vtxfit = VertexFit::instance();
  vtxfit->init();

  // assume charged tracks to pi 


  RecMdcKalTrack *pipTrk1 = (*(evtRecTrkCol->begin()+ipip[0]))->mdcKalTrack();
  RecMdcKalTrack *pimTrk1 = (*(evtRecTrkCol->begin()+ipim[0]))->mdcKalTrack();
  RecMdcKalTrack *pipTrk2 = (*(evtRecTrkCol->begin()+ipip[1]))->mdcKalTrack();
  RecMdcKalTrack *pimTrk2 = (*(evtRecTrkCol->begin()+ipim[1]))->mdcKalTrack();

  WTrackParameter wvpipTrk1, wvpimTrk1,wvpipTrk2, wvpimTrk2;
  wvpipTrk1 = WTrackParameter(mpi, pipTrk1->getZHelix(), pipTrk1->getZError());
  wvpimTrk1 = WTrackParameter(mpi, pimTrk1->getZHelix(), pimTrk1->getZError());
  wvpipTrk2 = WTrackParameter(mpi, pipTrk2->getZHelix(), pipTrk2->getZError());
  wvpimTrk2 = WTrackParameter(mpi, pimTrk2->getZHelix(), pimTrk2->getZError());

  vtxfit->AddTrack(0,  wvpipTrk1);
  vtxfit->AddTrack(1,  wvpimTrk1);
  vtxfit->AddVertex(0, vxpar,0, 1);
  if(!vtxfit->Fit(0)) return SUCCESS;
  vtxfit->Swim(0);

  Ncut7++;

  WTrackParameter wpip1 = vtxfit->wtrk(0);
  WTrackParameter wpim1 = vtxfit->wtrk(1);

  KinematicFit * kmfit = KinematicFit::instance();

  //
  //  Apply Kinematic 4C fit
  // 
    int igbf1 = -1;
    int igbf2 = -1;
  HepLorentzVector pTgam1(0,0,0,0);
  HepLorentzVector pTgam2(0,0,0,0);

  if(m_test4C==1) {
//    double ecms = 3.097;
    HepLorentzVector ecms(0.011*3.6862,0,0,3.6862);

    //
    // kinematic fit to pi pi K  K pi0
    //
  WTrackParameter wvkipTrk2, wvkimTrk2;
  wvkipTrk2 = WTrackParameter(mk, pipTrk2->getZHelixK(), pipTrk2->getZErrorK());    
  wvkimTrk2 = WTrackParameter(mk, pimTrk2->getZHelixK(), pimTrk2->getZErrorK());     
    double chisq = 9999.;
     int ig11 = -1;
     int ig21 = -1;
    double chikk=9999.;
    for(int i = 0; i < nGam-1; i++) {
      RecEmcShower *g1Trk = (*(evtRecTrkCol->begin()+iGam[i]))->emcShower();
      for(int j = i+1; j < nGam; j++) {
	RecEmcShower *g2Trk = (*(evtRecTrkCol->begin()+iGam[j]))->emcShower();
	kmfit->init();
        kmfit->setDynamicerror(1);
	kmfit->AddTrack(0, wpip1);
	kmfit->AddTrack(1, wpim1);
        kmfit->AddTrack(2, wvkipTrk2);
        kmfit->AddTrack(3, wvkimTrk2);
	kmfit->AddTrack(4, 0.0, g1Trk);
	kmfit->AddTrack(5, 0.0, g2Trk);
	kmfit->AddFourMomentum(0, ecms);
	bool oksq = kmfit->Fit();
	if(oksq&&kmfit->chisq()<chikk) {
	  chikk = kmfit->chisq();
	}
      }
    }
  Ncut8++;

    //
    // kinematic fit to pi pi pi pi pi0
    //

    chisq = 9999.;
    int ig1 = -1;
    int ig2 = -1;
    for(int i = 0; i < nGam-1; i++) {
      RecEmcShower *g1Trk = (*(evtRecTrkCol->begin()+iGam[i]))->emcShower();
      for(int j = i+1; j < nGam; j++) {
	RecEmcShower *g2Trk = (*(evtRecTrkCol->begin()+iGam[j]))->emcShower();
	kmfit->init();
        kmfit->setDynamicerror(1);
	kmfit->AddTrack(0, wpip1);
	kmfit->AddTrack(1, wpim1);
        kmfit->AddTrack(2, wvpipTrk2);
        kmfit->AddTrack(3, wvpimTrk2);
	kmfit->AddTrack(4, 0.0, g1Trk);
	kmfit->AddTrack(5, 0.0, g2Trk);
	kmfit->AddFourMomentum(0, ecms);
	bool oksq = kmfit->Fit();
	if(oksq) {
	  double chi2 = kmfit->chisq();
	  if(chi2 < chisq) {
	    chisq = chi2;
	    ig1 = iGam[i];
	    ig2 = iGam[j];
            igbf1 = iGam[i];
            igbf2 = iGam[j];
            pTgam1=pGam[i];
            pTgam2=pGam[j];
	  }
	}
      }
    }
//    log << MSG::DEBUG << "photon ID from 4c fit to 4pi+pi0 " << ig1 << " , " 
//        << ig2 << endreq;    
    if(chisq > 200) return SUCCESS;
    Ncut9++;

// select charge track and nneu track
  Vint jGood;
  jGood.clear();
  jGood.push_back(ipip[0]);  
  jGood.push_back(ipim[0]);
  jGood.push_back(ipip[1]);
  jGood.push_back(ipim[1]);



  Vint jGgam;
  jGgam.clear();
  jGgam.push_back(igbf1);
  jGgam.push_back(igbf2);

      double chi1_pp=9999.0;
    
      RecEmcShower *g1Trk = (*(evtRecTrkCol->begin()+ig1))->emcShower();
      RecEmcShower *g2Trk = (*(evtRecTrkCol->begin()+ig2))->emcShower();
      kmfit->init();
      kmfit->AddTrack(0, wpip1);
      kmfit->AddTrack(1, wpim1);
      kmfit->AddTrack(2, wvpipTrk2);
      kmfit->AddTrack(3, wvpimTrk2);
      kmfit->AddTrack(4, 0.0, g1Trk);
      kmfit->AddTrack(5, 0.0, g2Trk);
      kmfit->AddFourMomentum(0, ecms);
      bool oksq = kmfit->Fit();
      if(oksq) {
	chi1_pp = kmfit->chisq();
	HepLorentzVector ppi0 = kmfit->pfit(4) + kmfit->pfit(5);
        HepLorentzVector prho0= kmfit->pfit(2) + kmfit->pfit(3);
        HepLorentzVector prhop= kmfit->pfit(2) + ppi0;
        HepLorentzVector prhom= kmfit->pfit(3) + ppi0;
        HepLorentzVector pjjj = prho0 + ppi0; 

        HepLorentzVector p2pi1=kmfit->pfit(0) + kmfit->pfit(1);
        HepLorentzVector f2pi1g1= p2pi1 + kmfit->pfit(4);
        HepLorentzVector f2pi1g2= p2pi1 + kmfit->pfit(5);
        HepLorentzVector f2pi1pi0=p2pi1 + ppi0;

        HepLorentzVector t2pi2g1= prho0 + kmfit->pfit(4);
        HepLorentzVector t2pi2g2= prho0 + kmfit->pfit(5);

        HepLorentzVector p2pi3=kmfit->pfit(0) + kmfit->pfit(3);
        HepLorentzVector f2pi3g1= p2pi3 + kmfit->pfit(4);
        HepLorentzVector f2pi3g2= p2pi3 + kmfit->pfit(5);
        HepLorentzVector f2pi3pi0=p2pi3 + ppi0;

        HepLorentzVector p2pi4=kmfit->pfit(1) + kmfit->pfit(2);
        HepLorentzVector f2pi4g1= p2pi4 + kmfit->pfit(4);
        HepLorentzVector f2pi4g2= p2pi4 + kmfit->pfit(5);
        HepLorentzVector f2pi4pi0=p2pi4 + ppi0;

       HepLorentzVector p4pi= p2pi1 + prho0;
       HepLorentzVector p4pig1= p4pi + kmfit->pfit(4);
       HepLorentzVector p4pig2= p4pi + kmfit->pfit(5);
       HepLorentzVector ppptot= p4pi + ppi0;

//     add
       HepLorentzVector be4cpi0= pTgam1 + pTgam2;
       HepLorentzVector be4c_ppi1 = ppip[0] + ppim[0];        
       HepLorentzVector be4c_ppi2 = ppip[1] + ppim[1];
       HepLorentzVector be4cjp= be4cpi0 + be4c_ppi2;

//**********************************//
//      final event selection       //
//      for pion control sample     //
//**********************************//
/*
      if(fabs(ppi0.m()-0.135)>0.02) return SUCCESS;
      if(fabs(m_recppf-3.097)>0.01) return SUCCESS;
      if(fabs(m_emcTp2+m_emcTm2)>2.6) return SUCCESS;
      if(chi1_pp>chikk) return SUCCESS;

*/
//**********************************//

/*  
  m_run = eventHeader->runNumber();  
  m_rec = eventHeader->eventNumber();
  m_nch  = evtRecEvent->totalCharged();
  m_nneu = evtRecEvent->totalNeutral();

        m_gdgam=nGam;
        m_recpp=m_recppf;
        m_ngch = jGood.size();
        m_nggneu=jGgam.size();
 
        m_chi1=chi1_pp;
        m_bepi0=be4cpi0.m();
        m_be4cjpsi = be4cjp.m();
//
	m_mpi0 = ppi0.m();
        m_mprho0=prho0.m();
        m_mprhop=prhop.m();
        m_mprhom=prhom.m();
        m_mpjjj =pjjj.m();
        m_mp2pi1 = p2pi1.m();
        m_mf2pi1g1=f2pi1g1.m();
        m_mf2pi1g2=f2pi1g2.m();
        m_mf2pi1pi0=f2pi1pi0.m();
        m_mt2pi2g1=t2pi2g1.m();
        m_mt2pi2g2=t2pi2g2.m();
        m_mp2pi3 = p2pi3.m();
        m_mf2pi3g1=f2pi3g1.m();
        m_mf2pi3g2=f2pi3g2.m();
        m_mf2pi3pi0=f2pi3pi0.m();
        m_mp2pi4 = p2pi4.m();
        m_mf2pi4g1=f2pi4g1.m();
        m_mf2pi4g2=f2pi4g2.m();
        m_mf2pi4pi0=f2pi4pi0.m();
        m_mp4pi = p4pi.m();
        m_mppptot=ppptot.m();
        m_mp4pig1=p4pig1.m();
        m_mp4pig2=p4pig2.m();
        m_mpx1=kmfit->pfit(0).px();
        m_mpy1=kmfit->pfit(0).py();
        m_mpz1=kmfit->pfit(0).pz();
        m_mpe1=kmfit->pfit(0).e();
        m_mpx2=kmfit->pfit(1).px();
        m_mpy2=kmfit->pfit(1).py();
        m_mpz2=kmfit->pfit(1).pz();
        m_mpe2=kmfit->pfit(1).e();
        m_mpx3=kmfit->pfit(2).px();
        m_mpy3=kmfit->pfit(2).py();
        m_mpz3=kmfit->pfit(2).pz();
        m_mpe3=kmfit->pfit(2).e();
        m_mpx4=kmfit->pfit(3).px();
        m_mpy4=kmfit->pfit(3).py();
        m_mpz4=kmfit->pfit(3).pz();
        m_mpe4=kmfit->pfit(3).e();
        m_mpxg1=kmfit->pfit(4).px();
        m_mpyg1=kmfit->pfit(4).py();
        m_mpzg1=kmfit->pfit(4).pz();
        m_mpeg1=kmfit->pfit(4).e();
        m_mpxg2=kmfit->pfit(5).px();
        m_mpyg2=kmfit->pfit(5).py();
        m_mpzg2=kmfit->pfit(5).pz();
        m_mpeg2=kmfit->pfit(5).e();
        m_good = nGood;
        m_chikk=chikk;
        m_gam  = nGam;
        m_pip  = npip;
        m_pim  = npim;

//
//          fill information of dedx and tof
//

    for(int jk = 0; jk < 2; jk++) {
    m_ipipin[jk]=0;
    m_ipimin[jk]=0;
    }
   int ipidpip=0;
   int ipidpim=0;

  ParticleID *pid = ParticleID::instance();
  for(int i = 0; i < npip; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + ipip[i];
    pid->init();
    pid->setMethod(pid->methodProbability()); 
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); // use PID sub-system
    pid->identify(pid->onlyPion() | pid->onlyKaon());    // seperater Pion/Kaon
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    if(pid->probPion() > pid->probKaon()) {
     m_ipipin[ipidpip]=1;
     ipidpip++;
    }
   }

//  ParticleID *pid = ParticleID::instance();
  for(int j = 0; j < npim; j++) {
    EvtRecTrackIterator jtTrk = evtRecTrkCol->begin() + ipim[j];
    pid->init();
    pid->setMethod(pid->methodProbability()); 
    pid->setChiMinCut(4);
    pid->setRecTrack(*jtTrk);
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); // use PID sub-system
    pid->identify(pid->onlyPion() | pid->onlyKaon());    // seperater Pion/Kaon
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    if(pid->probPion() > pid->probKaon()) {
    m_ipimin[ipidpim]=1;
    ipidpim++;
    }
   }

   m_pidpip=ipidpip;
   m_pidpim=ipidpim;

  //
  // check dedx infomation
  //

    for(int ii = 0; ii < 4; ii++) {
// dedx
      m_ptrk[ii] = 9999.0;
      m_chie[ii] = 9999.0;
      m_chimu[ii] = 9999.0;
      m_chipi[ii] = 9999.0;
      m_chik[ii] = 9999.0;
      m_chip[ii] = 9999.0;
      m_ghit[ii] = 9999.0;
      m_thit[ii] = 9999.0;
      m_probPH[ii] = 9999.0;
      m_normPH[ii] = 9999.0;

//endtof
          m_cntr_etof[ii] = 9999.0;
          m_ptot_etof[ii] = 9999.0;
          m_ph_etof[ii]   = 9999.0;
          m_rhit_etof[ii] = 9999.0;
          m_qual_etof[ii] = 9999.0;
          m_te_etof[ii]   = 9999.0;
          m_tmu_etof[ii]  = 9999.0;
          m_tpi_etof[ii]  = 9999.0;
          m_tk_etof[ii]   = 9999.0;
          m_tp_etof[ii]   = 9999.0;
          m_ec_tof[ii]    =  9999.0;
          m_ec_toff_e[ii]   = 9999.0;
          m_ec_toff_mu[ii]  = 9999.0;
          m_ec_toff_pi[ii]  = 9999.0;
          m_ec_toff_k[ii]   = 9999.0;
          m_ec_toff_p[ii]   = 9999.0;
          m_ec_tsig_e[ii]   = 9999.0;
          m_ec_tsig_mu[ii]  = 9999.0;
          m_ec_tsig_pi[ii]  = 9999.0;
          m_ec_tsig_k[ii]   = 9999.0;
          m_ec_tsig_p[ii]   = 9999.0;

// barrel tof
          m_cntr_btof1[ii]  = 9999.0;
          m_ptot_btof1[ii]  = 9999.0;
          m_ph_btof1[ii]    = 9999.0;
          m_zhit_btof1[ii]  = 9999.0;
          m_qual_btof1[ii]  = 9999.0;
          m_te_btof1[ii]    = 9999.0;
          m_tmu_btof1[ii]   = 9999.0;
          m_tpi_btof1[ii]   = 9999.0;
          m_tk_btof1[ii]    = 9999.0;
          m_tp_btof1[ii]    = 9999.0;
          m_b1_tof[ii]      = 9999.0;
          m_b1_toff_e[ii]   = 9999.0;
          m_b1_toff_mu[ii]  = 9999.0;
          m_b1_toff_pi[ii]  = 9999.0;
          m_b1_toff_k[ii]   = 9999.0;
          m_b1_toff_p[ii]   = 9999.0;
          m_b1_tsig_e[ii]   = 9999.0;
          m_b1_tsig_mu[ii]  = 9999.0;
          m_b1_tsig_pi[ii]  = 9999.0;
          m_b1_tsig_k[ii]   = 9999.0;
          m_b1_tsig_p[ii]   = 9999.0;
//pid
          m_dedx_pid[ii] = 9999.0;
          m_tof1_pid[ii] = 9999.0;
          m_tof2_pid[ii] = 9999.0;
          m_prob_pid[ii] = 9999.0; 
          m_ptrk_pid[ii] = 9999.0;  
          m_cost_pid[ii] = 9999.0;
       }


      int  indx0=0;
      for(int i = 0; i < m_ngch; i++) {
      EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + jGood[i];
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if(!(*itTrk)->isMdcDedxValid())continue;
      RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
      RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
      m_ptrk[indx0] = mdcTrk->p();

      m_chie[indx0] = dedxTrk->chiE();
      m_chimu[indx0] = dedxTrk->chiMu();
      m_chipi[indx0] = dedxTrk->chiPi();
      m_chik[indx0] = dedxTrk->chiK();
      m_chip[indx0] = dedxTrk->chiP();
      m_ghit[indx0] = dedxTrk->numGoodHits();
      m_thit[indx0] = dedxTrk->numTotalHits();
      m_probPH[indx0] = dedxTrk->probPH();
      m_normPH[indx0] = dedxTrk->normPH();
      indx0++;
//      m_tuple7->write();
    }


  //
  // check TOF infomation
  //


     int indx1=0;
    for(int i = 0; i < m_ngch; i++) {
      EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + jGood[i];
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if(!(*itTrk)->isTofTrackValid()) continue;

      RecMdcTrack * mdcTrk = (*itTrk)->mdcTrack();
      SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();

      double ptrk = mdcTrk->p();
      SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
      for(;iter_tof != tofTrkCol.end(); iter_tof++ ) { 
        TofHitStatus *status = new TofHitStatus; 
        status->setStatus((*iter_tof)->status());
        if(!(status->is_barrel())){//endcap
          if( !(status->is_counter()) ) continue; // ? 
          if( status->layer()!=1 ) continue;//layer1
          double path=(*iter_tof)->path(); // ? 
          double tof  = (*iter_tof)->tof();
          double ph   = (*iter_tof)->ph();
          double rhit = (*iter_tof)->zrhit();
          double qual = 0.0 + (*iter_tof)->quality();
          double cntr = 0.0 + (*iter_tof)->tofID();
          double texp[5];
          double tsig[5];
          for(int j = 0; j < 5; j++) {//0 e, 1 mu, 2 pi, 3 K, 4 p
           texp[j] = (*iter_tof)->texp(j);
//           tsig[j] = (*iter_tof)->sigma(j);
//           toffset[j] = (*iter_tof)->offset(j);
          }
          m_cntr_etof[indx1]  = cntr;
          m_ptot_etof[indx1] = ptrk;
          m_ph_etof[indx1]   = ph;
          m_rhit_etof[indx1]  = rhit;
          m_qual_etof[indx1]  = qual;
          m_te_etof[indx1]    = tof - texp[0];
          m_tmu_etof[indx1]   = tof - texp[1];
          m_tpi_etof[indx1]   = tof - texp[2];
          m_tk_etof[indx1]    = tof - texp[3];
          m_tp_etof[indx1]    = tof - texp[4];

          m_ec_tof[indx1]      =  tof;

          m_ec_toff_e[indx1]   = (*iter_tof)->toffset(0);
          m_ec_toff_mu[indx1]  = (*iter_tof)->toffset(1);
          m_ec_toff_pi[indx1]  = (*iter_tof)->toffset(2);
          m_ec_toff_k[indx1]   = (*iter_tof)->toffset(3);
          m_ec_toff_p[indx1]   = (*iter_tof)->toffset(4);

          m_ec_tsig_e[indx1]   = (*iter_tof)->sigma(0);
          m_ec_tsig_mu[indx1]  = (*iter_tof)->sigma(1);
          m_ec_tsig_pi[indx1]  = (*iter_tof)->sigma(2);
          m_ec_tsig_k[indx1]   = (*iter_tof)->sigma(3);
          m_ec_tsig_p[indx1]   = (*iter_tof)->sigma(4);

//          m_tuple8->write();
        }
        else {//barrel
          if( !(status->is_cluster()) ) continue; // ? 
            double path=(*iter_tof)->path(); // ? 
            double tof  = (*iter_tof)->tof();
            double ph   = (*iter_tof)->ph();
            double rhit = (*iter_tof)->zrhit();
            double qual = 0.0 + (*iter_tof)->quality();
            double cntr = 0.0 + (*iter_tof)->tofID();
          double texp[5];
          for(int j = 0; j < 5; j++) {
            texp[j] = (*iter_tof)->texp(j);
            }
            m_cntr_btof1[indx1]  = cntr;
            m_ptot_btof1[indx1] = ptrk;
            m_ph_btof1[indx1]   = ph;
            m_zhit_btof1[indx1]  = rhit;
            m_qual_btof1[indx1]  = qual;
            m_te_btof1[indx1]    = tof - texp[0];
            m_tmu_btof1[indx1]   = tof - texp[1];
            m_tpi_btof1[indx1]   = tof - texp[2];
            m_tk_btof1[indx1]    = tof - texp[3];
            m_tp_btof1[indx1]    = tof - texp[4];

          m_b1_tof[indx1]      =  tof;

          m_b1_toff_e[indx1]   = (*iter_tof)->toffset(0);
          m_b1_toff_mu[indx1]  = (*iter_tof)->toffset(1);
          m_b1_toff_pi[indx1]  = (*iter_tof)->toffset(2);
          m_b1_toff_k[indx1]   = (*iter_tof)->toffset(3);
          m_b1_toff_p[indx1]   = (*iter_tof)->toffset(4);

          m_b1_tsig_e[indx1]   = (*iter_tof)->sigma(0);
          m_b1_tsig_mu[indx1]  = (*iter_tof)->sigma(1);
          m_b1_tsig_pi[indx1]  = (*iter_tof)->sigma(2);
          m_b1_tsig_k[indx1]   = (*iter_tof)->sigma(3);
          m_b1_tsig_p[indx1]   = (*iter_tof)->sigma(4);

//            m_tuple9->write();
          }
        delete status; 
      } 
      indx1++;
    } // loop all charged track

  //
  // Assign 4-momentum to each charged track
  //
    int indx2=0;
//  ParticleID *pid = ParticleID::instance();
  for(int i = 0; i < m_ngch; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + jGood[i]; 
    //    if(pid) delete pid;
    pid->init();
    pid->setMethod(pid->methodProbability());
//    pid->setMethod(pid->methodLikelihood());  //for Likelihood Method  

    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); // use PID sub-system
    pid->identify(pid->onlyPion() | pid->onlyKaon());    // seperater Pion/Kaon
    //    pid->identify(pid->onlyPion());
    //  pid->identify(pid->onlyKaon());
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();

    m_dedx_pid[indx2] = pid->chiDedx(2);
    m_tof1_pid[indx2] = pid->chiTof1(2);
    m_tof2_pid[indx2] = pid->chiTof2(2);
    m_prob_pid[indx2] = pid->probPion();

//    if(pid->probPion() < 0.001 || (pid->probPion() < pid->probKaon())) continue;
//    if(pid->probPion() < 0.001) continue;
//    if(pid->pdf(2)<pid->pdf(3)) continue; //  for Likelihood Method(0=electron 1=muon 2=pion 3=kaon 4=proton) 

    RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();//After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
    RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion

    m_ptrk_pid[indx2] = mdcKalTrk->p();
    m_cost_pid[indx2] = cos(mdcTrk->theta());
  }


  int iphoton = 0;
  for (int i=0; i<m_nggneu; i++)
  {
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + jGgam[i];
    if (!(*itTrk)->isEmcShowerValid()) continue; 
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    m_numHits[iphoton] = emcTrk->numHits();
    m_secondmoment[iphoton] = emcTrk->secondMoment();
    m_x[iphoton] = emcTrk->x();
    m_y[iphoton]= emcTrk->y();
    m_z[iphoton]= emcTrk->z();
    m_cosemc[iphoton]   = cos(emcTrk->theta());
    m_phiemc[iphoton]   = emcTrk->phi();      
    m_energy[iphoton]   = emcTrk->energy();
    m_eSeed[iphoton]    = emcTrk->eSeed(); 
    m_e3x3[iphoton]     = emcTrk->e3x3();  
    m_e5x5[iphoton]     = emcTrk->e5x5();  
    m_lat[iphoton]      = emcTrk->latMoment();
    m_a20[iphoton]      = emcTrk->a20Moment();
    m_a42[iphoton]      = emcTrk->a42Moment();
    iphoton++;
  }
//	m_tuple4->write();
*/
        Ncut10++;
//    log << MSG::DEBUG << "chisquare from 4c fit to 4pi+pi0 " << m_chi1 << endreq;    
      }
  }
  
  //
  //  Apply Kinematic 5C Fit
  //

  // find the best combination over all possible pi+ pi- gamma gamma pair

  setFilterPassed(true);

  (*(evtRecTrkCol->begin()+ipip[0]))->setPartId(2);
  (*(evtRecTrkCol->begin()+ipim[0]))->setPartId(2);
  (*(evtRecTrkCol->begin()+ipip[1]))->setPartId(2);
  (*(evtRecTrkCol->begin()+ipim[1]))->setPartId(2);


  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode Ppjrhopi::finalize() {
  cout<<"total number:         "<<Ncut0<<endl;
  cout<<"nGood==4, nCharge==0: "<<Ncut1<<endl;
  cout<<"nGam>=2:              "<<Ncut2<<endl;
  cout<<"Pass no Pid:          "<<Ncut3<<endl;
  cout<<"ChangeID recfrom psp: "<<Ncut4<<endl;
  cout<<"vetex position:       "<<Ncut5<<endl;
  cout<<"Mass from MDC:        "<<Ncut6<<endl;
  cout<<"primary vetex fit:    "<<Ncut7<<endl;
  cout<<"Pass 4C for ppkkp0:   "<<Ncut8<<endl;
  cout<<"Pass 4C for 4pi+pi0:  "<<Ncut9<<endl;
  cout<<"Pass 4C <200:         "<<Ncut10<<endl;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

