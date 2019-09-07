 #include "GaudiKernel/MsgStream.h"
 #include "GaudiKernel/AlgFactory.h"
 #include "GaudiKernel/SmartDataPtr.h"
 #include "GaudiKernel/IDataProviderSvc.h"
 #include "GaudiKernel/PropertyMgr.h"

 #include "EventModel/EventModel.h"
 #include "EventModel/Event.h"
 #include "EventModel/EventHeader.h"
 #include "EvtRecEvent/EvtRecEvent.h"
 #include "EvtRecEvent/EvtRecTrack.h"
 #include "EvtRecEvent/EvtRecDTag.h"

 #include "EvtRecEvent/EvtRecPi0.h"

 #include "TMath.h"
 #include "GaudiKernel/INTupleSvc.h"
 #include "GaudiKernel/NTuple.h"
 #include "GaudiKernel/Bootstrap.h"
 #include "GaudiKernel/IHistogramSvc.h"
 #include "CLHEP/Vector/ThreeVector.h"
 #include "CLHEP/Vector/LorentzVector.h"
 #include "CLHEP/Vector/TwoVector.h"
 #include "EmcRawEvent/EmcDigi.h"
 #include "RawEvent/RawDataUtil.h"
 #include "MdcRawEvent/MdcDigi.h"

 #include "VertexFit/IVertexDbSvc.h"
 #include "VertexFit/KinematicFit.h"
 #include "VertexFit/VertexFit.h"
 #include "VertexFit/Helix.h"

 #include "DstEvent/TofHitStatus.h"


 #include "GaudiKernel/Bootstrap.h"
 #include "GaudiKernel/ISvcLocator.h"
 using CLHEP::Hep3Vector;
 using CLHEP::Hep2Vector;
 using CLHEP::HepLorentzVector;
 #include "CLHEP/Geometry/Point3D.h"
 #ifndef ENABLE_BACKWARDS_COMPATIBILITY
 typedef HepGeom::Point3D<double> HepPoint3D;
 #endif
 #include "CalibEventSelect/CalibEventSelect.h"

 #include <vector>
 #include "mysql.h" 

 typedef std::vector<int> Vint;
 typedef std::vector<HepLorentzVector> Vp4;

 const double mpi = 0.13957;
 const double mkaon = 0.49367;
 const double mproton = 0.93827;


 double Px(RecMdcKalTrack *trk){
   double phi= trk->fi0();
   double kappa= trk->kappa();
   double tanl = trk->tanl();

   double px=-sin(phi)/fabs(kappa);
   return px;
 }

 double Py(RecMdcKalTrack *trk){
   double phi= trk->fi0();
   double kappa= trk->kappa();
   double tanl = trk->tanl();

   double py=cos(phi)/fabs(kappa);

   return py;
 }


 double Pz(RecMdcKalTrack *trk){
   double phi= trk->fi0();
   double kappa= trk->kappa();
   double tanl = trk->tanl();

   double pz=tanl/fabs(kappa);  
   return pz;
 }

 double P(RecMdcKalTrack *trk){
   double phi= trk->fi0();
   double kappa= trk->kappa();
   double tanl = trk->tanl();

   double px=-sin(phi)/fabs(kappa);
   double py=cos(phi)/fabs(kappa);
   double pz=tanl/fabs(kappa);

   return sqrt(px*px+py*py+pz*pz);
 }

double Phi(RecMdcKalTrack *trk){
  double phi0=trk->fi0();
  double kappa = trk->kappa();
  double pxy=0;
  if(kappa!=0) pxy = 1.0/fabs(kappa);
  
  
  double px = pxy * (-sin(phi0));
  double py = pxy * cos(phi0);

  return atan2(py,px);
  
}


 int CalibEventSelect::idmax[43]={40,44,48,56,64,72,80,80,76,76,
   88,88,100,100,112,112,128,128,140,140,
   160,160,160,160,176,176,176,176,208,208,
   208,208,240,240,240,240,256,256,256,256,
   288,288,288};
 /////////////////////////////////////////////////////////////////////////////

 CalibEventSelect::CalibEventSelect(const std::string& name, ISvcLocator* pSvcLocator) :  
   Algorithm(name, pSvcLocator) {
     //Declare the properties  

     declareProperty("Output",           m_output = false);
     declareProperty("Display",           m_display = false);
     declareProperty("PrintMonitor",     m_printmonitor=false);
     declareProperty("SelectRadBhabha",  m_selectRadBhabha=false);
     declareProperty("SelectGGEE",       m_selectGGEE=false);
     declareProperty("SelectGG4Pi",      m_selectGG4Pi=false);
     declareProperty("SelectRadBhabhaT", m_selectRadBhabhaT=false);
     declareProperty("SelectRhoPi",      m_selectRhoPi=false);
     declareProperty("SelectKstarK",     m_selectKstarK=false);
     declareProperty("SelectPPPiPi",     m_selectPPPiPi=false);
     declareProperty("SelectRecoJpsi",   m_selectRecoJpsi=false);
     declareProperty("SelectBhabha",     m_selectBhabha=false);
     declareProperty("SelectDimu",       m_selectDimu=false);
     declareProperty("SelectCosmic",     m_selectCosmic=false);
     declareProperty("SelectGenProton",  m_selectGenProton=false);
     declareProperty("SelectPsipRhoPi",  m_selectPsipRhoPi=false);
     declareProperty("SelectPsipKstarK", m_selectPsipKstarK=false);
     declareProperty("SelectPsipPPPiPi", m_selectPsipPPPiPi=false);
     declareProperty("SelectPsippCand",  m_selectPsippCand=false);

     declareProperty("BhabhaScale",     m_radbhabha_scale=1000);
     declareProperty("RadBhabhaScale",  m_bhabha_scale=1000);
     declareProperty("DimuScale",       m_dimu_scale=10);
     declareProperty("CosmicScale",     m_cosmic_scale=3);
     declareProperty("ProtonScale",     m_proton_scale=100);

     declareProperty("CosmicLowp",      m_cosmic_lowp=1.0);
   
     declareProperty("WriteDst",         m_writeDst=false);
     declareProperty("WriteRec",         m_writeRec=false);
     declareProperty("Ecm",              m_ecm=3.1);
     declareProperty("Vr0cut",           m_vr0cut=1.0);
     declareProperty("Vz0cut",           m_vz0cut=10.0);
     declareProperty("Pt0HighCut",       m_pt0HighCut=5.0);
     declareProperty("Pt0LowCut",        m_pt0LowCut=0.05);
     declareProperty("EnergyThreshold",  m_energyThreshold=0.05);
     declareProperty("GammaPhiCut",      m_gammaPhiCut=20.0);
     declareProperty("GammaThetaCut",    m_gammaThetaCut=20.0);


 }

 // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 StatusCode CalibEventSelect::initialize() {
   MsgStream log(msgSvc(), name());

   log << MSG::INFO << "in initialize()" << endmsg;

   m_run=0;
   //m_ecm=3.1;


   h_nGood= histoSvc()->book("1D/nGoodtracks", 1, "num of good chaged tracks", 20, 0, 20);
   h_nCharge= histoSvc()->book("1D/nCharge", 1, "net charge", 20, -10, 10);
   h_pmaxobeam= histoSvc()->book("1D/pmax", 1, "pmax over beam ratio", 100, 0, 3);
   h_eopmax= histoSvc()->book("1D/eopmax", 1, "e over pmax ratio", 100, 0, 3);
   h_eopsec= histoSvc()->book("1D/eopsec", 1, "e over psec ratio", 100, 0, 3);
   h_deltap= histoSvc()->book("1D/deltap", 1, "pmax minus psec", 100, -3, 3);
   h_esumoecm= histoSvc()->book("1D/esumoverecm", 1, "total energy over ecm ratio", 100, 0, 3);  
   h_egmax= histoSvc()->book("1D/egmax", 1, "most energetic photon energy", 100, 0, 3);
   h_deltaphi= histoSvc()->book("1D/deltaphi", 1, "phi_pmax minus phi_sec", 150, -4, 4);
   h_Pt= histoSvc()->book("1D/Pt", 1, "total Transverse Momentum", 200,-1, 4);

   StatusCode sc;

   log << MSG::INFO << "creating sub-algorithms...." << endreq;





   if(m_writeDst) {
     sc =  createSubAlgorithm( "EventWriter", "WriteDst", m_subalg1);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm WriteDst" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm WriteDst" <<endreq;
     }
   }

   if(m_writeRec) {
     sc =  createSubAlgorithm( "EventWriter", "WriteRec", m_subalg2);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm WriteRec" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm WriteRec" <<endreq;
     }
   }


   if(m_selectRadBhabha) {
     sc =  createSubAlgorithm( "EventWriter", "SelectRadBhabha", m_subalg3);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectRadBhabha" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectRadBhabha" <<endreq;
     }
   }

   if(m_selectGGEE) {
     sc =  createSubAlgorithm( "EventWriter", "SelectGGEE", m_subalg4);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectGGEE" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectGGEE" <<endreq;
     }
   }

   if(m_selectGG4Pi) {
     sc =  createSubAlgorithm( "EventWriter", "SelectGG4Pi", m_subalg5);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectGG4Pi" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectGG4Pi" <<endreq;
     }
   }


   if(m_selectRadBhabhaT) {
     sc =  createSubAlgorithm( "EventWriter", "SelectRadBhabhaT", m_subalg6);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectRadBhabhaT" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectRadBhabhaT" <<endreq;
     }
   }


   if(m_selectRhoPi) {
     sc =  createSubAlgorithm( "EventWriter", "SelectRhoPi", m_subalg7);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectRhoPi" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectRhoPi" <<endreq;
     }
   }


   if(m_selectKstarK) {
     sc =  createSubAlgorithm( "EventWriter", "SelectKstarK", m_subalg8);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectKstarK" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectKstarK" <<endreq;
     }
   }



   if(m_selectPPPiPi) {
     sc =  createSubAlgorithm( "EventWriter", "SelectPPPiPi", m_subalg9);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectPPPiPi" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectPPPiPi" <<endreq;
     }
   }


   if(m_selectRecoJpsi) {
     sc =  createSubAlgorithm( "EventWriter", "SelectRecoJpsi", m_subalg10);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectRecoJpsi" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectRecoJpsi" <<endreq;
     }
   }


   if(m_selectBhabha) {
     sc =  createSubAlgorithm( "EventWriter", "SelectBhabha", m_subalg11);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectBhabha" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectBhabha" <<endreq;
     }
   }

   if(m_selectDimu) {
     sc =  createSubAlgorithm( "EventWriter", "SelectDimu", m_subalg12);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectDimu" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectDimu" <<endreq;
     }
   }

   if(m_selectCosmic) {
     sc =  createSubAlgorithm( "EventWriter", "SelectCosmic", m_subalg13);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectCosmic" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectCosmic" <<endreq;
     }
   }

   if(m_selectGenProton) {
     sc =  createSubAlgorithm( "EventWriter", "SelectGenProton", m_subalg14);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectGenProton" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectGenProton" <<endreq;
     }
   }


   if(m_selectPsipRhoPi) {
     sc =  createSubAlgorithm( "EventWriter", "SelectPsipRhoPi", m_subalg15);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectPsipRhoPi" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectPsipRhoPi" <<endreq;
     }
   }


   if(m_selectPsipKstarK) {
     sc =  createSubAlgorithm( "EventWriter", "SelectPsipKstarK", m_subalg16);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectPsipKstarK" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectPsipKstarK" <<endreq;
     }
   }


   if(m_selectPsipPPPiPi) {
     sc =  createSubAlgorithm( "EventWriter", "SelectPsipPPPiPi", m_subalg17);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectPsipPPPiPi" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectPsipPPPiPi" <<endreq;
     }
   }


   if(m_selectPsippCand) {
     sc =  createSubAlgorithm( "EventWriter", "SelectPsippCand", m_subalg18);
     if( sc.isFailure() ) {
       log << MSG::ERROR << "Error creating Sub-Algorithm SelectPsippCand" <<endreq;
       return sc;
     } else {
       log << MSG::INFO << "Success creating Sub-Algorithm SelectPsippCand" <<endreq;
     }
   }




   if(m_output) {
     StatusCode status;
     NTuplePtr nt0(ntupleSvc(), "FILE1/hadron");
     if ( nt0 ) m_tuple0 = nt0;
     else {
       m_tuple0 = ntupleSvc()->book ("FILE1/hadron", CLID_ColumnWiseTuple, "N-Tuple example");
       if ( m_tuple0 )    {
	 status = m_tuple0->addItem ("esum",     m_esum);
	 status = m_tuple0->addItem ("eemc",     m_eemc);
	 status = m_tuple0->addItem ("etot",     m_etot);
	 status = m_tuple0->addItem ("nGood",    m_nGood);
	 status = m_tuple0->addItem ("nCharge",  m_nCharge);
	 status = m_tuple0->addItem ("nGam",     m_nGam);
	 status = m_tuple0->addItem ("ptot",     m_ptot);
	 status = m_tuple0->addItem ("pp",       m_pp);
	 status = m_tuple0->addItem ("pm",       m_pm);
	 status = m_tuple0->addItem ("run",      m_runnb);
	 status = m_tuple0->addItem ("event",    m_evtnb);
	 status = m_tuple0->addItem ("maxE",     m_maxE);
	 status = m_tuple0->addItem ("secE",     m_secE);
	 status = m_tuple0->addItem ("dthe",     m_dthe);
	 status = m_tuple0->addItem ("dphi",     m_dphi);
	 status = m_tuple0->addItem ("mdcHit1",  m_mdcHit1);
	 status = m_tuple0->addItem ("mdcHit2",  m_mdcHit2);
       }
       else    { 
	 log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple0) << endmsg;
	 return StatusCode::FAILURE;
       }
     }

     NTuplePtr nt1(ntupleSvc(), "FILE1/vxyz");
     if ( nt1 ) m_tuple1 = nt1;
     else {
       m_tuple1 = ntupleSvc()->book ("FILE1/vxyz", CLID_ColumnWiseTuple, "ks N-Tuple example");
       if ( m_tuple1 )    {
	 status = m_tuple1->addItem ("vx0",    m_vx0);
	 status = m_tuple1->addItem ("vy0",    m_vy0);
	 status = m_tuple1->addItem ("vz0",    m_vz0);
	 status = m_tuple1->addItem ("vr0",    m_vr0);
	 status = m_tuple1->addItem ("theta0", m_theta0);
	 status = m_tuple1->addItem ("p0",     m_p0);
	 status = m_tuple1->addItem ("pt0",    m_pt0);
       }
       else    { 
	 log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
	 return StatusCode::FAILURE;
       }
     }

     NTuplePtr nt2(ntupleSvc(), "FILE1/photon");
     if ( nt2 ) m_tuple2 = nt2;
     else {
       m_tuple2 = ntupleSvc()->book ("FILE1/photon", CLID_ColumnWiseTuple, "ks N-Tuple example");
       if ( m_tuple2 )    {
	 status = m_tuple2->addItem ("dthe",   m_dthe);
	 status = m_tuple2->addItem ("dphi",   m_dphi);
	 status = m_tuple2->addItem ("dang",   m_dang);
	 status = m_tuple2->addItem ("eraw",   m_eraw);
       }
       else    { 
	 log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple2) << endmsg;
	 return StatusCode::FAILURE;
       }
     }
   }
   //
   //--------end of book--------
   //

   m_events=0;
   m_radBhabhaNumber=0;
   m_GGEENumber=0;
   m_GG4PiNumber=0;
   m_radBhabhaTNumber=0;
   m_rhoPiNumber=0;
   m_kstarKNumber=0;
   m_ppPiPiNumber=0;
   m_recoJpsiNumber=0;
   m_bhabhaNumber=0;
   m_dimuNumber=0;
   m_cosmicNumber=0;
   m_genProtonNumber=0;
   m_psipRhoPiNumber=0;
   m_psipKstarKNumber=0;
   m_psipPPPiPiNumber=0;

   log << MSG::INFO << "successfully return from initialize()" <<endmsg;
   return StatusCode::SUCCESS;

 }

 // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 StatusCode CalibEventSelect::execute() {

   //setFilterPassed(false);

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in execute()" << endreq;

   if( m_writeDst) m_subalg1->execute();
   if( m_writeRec) m_subalg2->execute();


   m_isRadBhabha       = false; 
   m_isGGEE            = false; 
   m_isGG4Pi           = false; 
   m_isRadBhabhaT      = false; 
   m_isRhoPi           = false; 
   m_isKstarK          = false; 
   m_isRecoJpsi        = false; 
   m_isPPPiPi          = false; 
   m_isBhabha          = false; 
   m_isDimu            = false; 
   m_isCosmic          = false; 
   m_isGenProton       = false; 
   m_isPsipRhoPi       = false; 
   m_isPsipKstarK      = false; 
   m_isPsipPPPiPi      = false; 
   m_isPsippCand       = false; 

   SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
   if(!eventHeader)
   {
     cout<<"  eventHeader  "<<endl;
     return StatusCode::FAILURE;
   }

   int run=eventHeader->runNumber();
   int event=eventHeader->eventNumber();

   //get run by run ebeam
   if(m_run !=run){
     m_run=run;
     double beamE=0;
     readbeamEfromDb(run,beamE);
     cout<<"new run is:"<<m_run<<endl;
     cout<<"beamE is:"<<beamE<<endl;
     if(beamE>0 && beamE<3)
       m_ecm=2*beamE;
   }



   if(m_display && m_events%1000==0){   
     cout<< m_events << "  --------  run,event:  "<<run<<","<<event<<endl;
     cout<<"m_ecm="<<m_ecm<<endl;
   }
   m_events++;

   SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
   if(!evtRecEvent ) {
     cout<<"  evtRecEvent  "<<endl;
     return StatusCode::FAILURE;
   }

   log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
     << evtRecEvent->totalCharged() << " , "
     << evtRecEvent->totalNeutral() << " , "
     << evtRecEvent->totalTracks() <<endreq;
   SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
   if(!evtRecTrkCol){
     cout<<"  evtRecTrkCol  "<<endl;
     return StatusCode::FAILURE;
   }

   if(evtRecEvent->totalTracks()!=evtRecTrkCol->size()) return StatusCode::SUCCESS;


   //get pi0 reconstruction
   SmartDataPtr<EvtRecPi0Col> recPi0Col(eventSvc(), "/Event/EvtRec/EvtRecPi0Col");
   if ( ! recPi0Col ) {
     log << MSG::FATAL << "Could not find EvtRecPi0Col" << endreq;
     return StatusCode::FAILURE;
   }


   int nPi0 = recPi0Col->size(); 
   EvtRecPi0Col::iterator itpi0 = recPi0Col->begin();
   if(nPi0==1){
     double mpi0 =  (*itpi0)->unconMass();
     if ( fabs( mpi0 - 0.135 )> 0.015 )  
       nPi0=0;
   }
   
   /*
   int nPi0=0;
   for(EvtRecPi0Col::iterator it=itpi0; it!= recPi0Col->end(); it++){
     double mpi0 =  (*itpi0)->unconMass();
     if ( fabs( mpi0 - 0.135 )<= 0.015 )  
       nPi0++;
   }
   */


   // -------- Good Charged Track Selection
   Vint iGood;
   iGood.clear();
   vector<int> iCP, iCN;
   iCP.clear();
   iCN.clear();

   Vint iCosmicGood;
   iCosmicGood.clear();

   int nCharge = 0;
   int nCosmicCharge =0;

   for(int i = 0;i < evtRecEvent->totalCharged(); i++)
   {     
     //if(i>=evtRecTrkCol->size()) break;
     EvtRecTrackIterator  itTrk=evtRecTrkCol->begin() + i;

     //if(!(*itTrk)->isMdcTrackValid()) continue;
     //RecMdcTrack *mdcTrk =(*itTrk)->mdcTrack();
     if(!(*itTrk)->isMdcKalTrackValid()) continue;
     RecMdcKalTrack *mdcTrk = (*itTrk)->mdcKalTrack();
     //  mdcTrk->setPidType(RecMdcKalTrack::electron);


     double vx0 = mdcTrk->x();
     double vy0 = mdcTrk->y();
     double vz0 = mdcTrk->z();
     double vr0 = mdcTrk->r();
     double theta0 = mdcTrk->theta();
     double p0 = P(mdcTrk);
     double pt0 = sqrt( pow(Px(mdcTrk),2)+pow(Py(mdcTrk),2));
     
     
     if(m_output) {
      m_vx0 = vx0;
      m_vy0 = vy0;
      m_vz0 = vz0;
      m_vr0 = vr0;
      m_theta0 = theta0;
      m_p0 = p0;
      m_pt0 = pt0;
      m_tuple1->write();
    }

    //db 
    
    Hep3Vector xorigin(0,0,0);
    IVertexDbSvc*  vtxsvc;
    Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
    if(vtxsvc->isVertexValid()){
      double* dbv = vtxsvc->PrimaryVertex();
      double*  vv = vtxsvc->SigmaPrimaryVertex();
      xorigin.setX(dbv[0]);
      xorigin.setY(dbv[1]);
      xorigin.setZ(dbv[2]);
    }
    HepVector a = mdcTrk->getZHelix();
    HepSymMatrix Ea = mdcTrk->getZError();
    HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
    HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]);
    VFHelix helixip(point0,a,Ea);
    helixip.pivot(IP);
    HepVector vecipa = helixip.a();
    double db=fabs(vecipa[0]);
    double dz=vecipa[3];




    if(fabs(dz) >= m_vz0cut) continue;
    if(db >= m_vr0cut) continue;
    
    //cosmic tracks
    if(p0>m_cosmic_lowp && p0<20){
      iCosmicGood.push_back((*itTrk)->trackId());
      nCosmicCharge += mdcTrk->charge();
    }
      
    

    if(pt0 >= m_pt0HighCut) continue;
    if(pt0 <= m_pt0LowCut) continue;

    iGood.push_back((*itTrk)->trackId());
    nCharge += mdcTrk->charge();
    if(mdcTrk->charge()>0)
      iCP.push_back((*itTrk)->trackId());
    else if(mdcTrk->charge()<0)
      iCN.push_back((*itTrk)->trackId());
    

  }
  int nGood = iGood.size();
  int nCosmicGood = iCosmicGood.size();
  
  // -------- Good Photon Selection
  Vint iGam;
  iGam.clear();
  for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
    //if(i>=evtRecTrkCol->size()) break;
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    double eraw = emcTrk->energy();
    double time = emcTrk->time();
    double costh = cos(emcTrk->theta());
    if(fabs(costh)<0.83 && eraw<0.025)  continue;
    if(fabs(costh)>=0.83 && eraw<0.05)  continue;
    if(time<0 || time>14) continue;

    iGam.push_back((*itTrk)->trackId());
  }
  int nGam = iGam.size();

  // -------- Assign 4-momentum to each charged track
  Vint ipip, ipim;
  ipip.clear();
  ipim.clear();
  Vp4 ppip, ppim;
  ppip.clear();
  ppim.clear();

  //cout<<"charged track:"<<endl;
  double echarge = 0.;  //total energy of charged track
  double ptot = 0.; //total momentum of charged track
  double etot = 0.; //total energy in MDC and EMC
  double eemc = 0.; //total energy in EMC
  double pp = 0.;
  double pm = 0.;
  double pmax=0.0;
  double psec=0.0;
  double eccmax=0.0;
  double eccsec=0.0;
  double phimax=0.0;
  double phisec=0.0;
  double eopmax=0.0;
  double eopsec=0.0;
  Hep3Vector Pt_charge(0,0,0);
  
  for(int i = 0; i < nGood; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 

    double p3=0;
    //if((*itTrk)->isMdcTrackValid()) { 
    //RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    
    if((*itTrk)->isMdcKalTrackValid()) { 
      RecMdcKalTrack *mdcTrk = (*itTrk)->mdcKalTrack();
      mdcTrk->setPidType(RecMdcKalTrack::electron);
    
      ptot += P(mdcTrk);

      //double phi= mdcTrk->phi();
      double phi= Phi(mdcTrk);

      
      HepLorentzVector ptrk;
      ptrk.setPx(Px(mdcTrk));
      ptrk.setPy(Py(mdcTrk));
      ptrk.setPz(Pz(mdcTrk));
      p3 = fabs(ptrk.mag());
      
     
      
      //cout<<"p3 before compa="<<p3<<endl;
      //cout<<"pmax before compa ="<<pmax<<endl;
      //cout<<"psec before compa ="<<psec<<endl;

      Hep3Vector ptemp(Px(mdcTrk),Py(mdcTrk),0);
      Pt_charge+=ptemp;
      
      double ecc=0.0;
      if((*itTrk)->isEmcShowerValid()) {
	RecEmcShower* emcTrk = (*itTrk)->emcShower();
	ecc = emcTrk->energy();
      }
      
      if( p3 > pmax){
	psec=pmax;
	eccsec=eccmax;
	phisec=phimax;
	pmax=p3;
	eccmax=ecc;
	phimax=phi;
      }
      else if( p3 < pmax && p3> psec){
	psec=p3;
	eccsec=ecc;
	phisec=phi;
      }
      
      // cout<<"p3 after compa="<<p3<<endl;
      // cout<<"pmax after compa ="<<pmax<<endl;
      //cout<<"psec after compa ="<<psec<<endl;
      
      ptrk.setE(sqrt(p3*p3+mpi*mpi));
      ptrk = ptrk.boost(-0.011,0,0);//boost to cms

      
      echarge += ptrk.e();
      etot += ptrk.e();

      if(mdcTrk->charge() >0) {
        ppip.push_back(ptrk);
        pp = ptrk.rho();
      } else {
        ppim.push_back(ptrk);
        pm = ptrk.rho();
      }

    } 
    
    if((*itTrk)->isEmcShowerValid()) {

      RecEmcShower* emcTrk = (*itTrk)->emcShower();
      double eraw = emcTrk->energy();
      double phiemc = emcTrk->phi();
      double the = emcTrk->theta();

      HepLorentzVector pemc;
      pemc.setPx(eraw*sin(the)*cos(phiemc));
      pemc.setPy(eraw*sin(the)*sin(phiemc));
      pemc.setPz(eraw*cos(the));
      pemc.setE(eraw);
      pemc = pemc.boost(-0.011,0,0);// boost to cms

      // eemc += eraw;
      etot += pemc.e();

    }
    
    


  } //end of looping over good charged track

  if(pmax!=0)  eopmax=eccmax/pmax;
  if(psec!=0)  eopsec=eccsec/psec;

  eemc=eccmax+eccsec;
  
  /*
  if(nGood>1){
    cout<<"pmax="<<pmax<<endl;
    cout<<"psec="<<psec<<endl;
    cout<<"eopmax="<<eopmax<<endl;
    cout<<"dphi-180="<< (fabs(phimax-phisec)-CLHEP::pi)*180/CLHEP::pi <<endl;
  }
  */

  // -------- Assign 4-momentum to each photon
  //cout<<"neutral:"<<endl;
  Vp4 pGam;
  pGam.clear();
  double eneu=0;  //total energy of neutral track
  double egmax=0;
  Hep3Vector Pt_neu(0,0,0);
  
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
    ptrk = ptrk.boost(-0.011,0,0);// boost to cms
    pGam.push_back(ptrk);
    eneu += ptrk.e();
    etot += ptrk.e();
    eemc += eraw;
  
    Hep3Vector ptemp(eraw*sin(the)*cos(phi), eraw*sin(the)*sin(phi),0);
    Pt_neu+=ptemp;

    if(ptrk.e()>egmax)
      egmax=ptrk.e();
    
  }

  double esum = echarge + eneu;
  Hep3Vector Pt=Pt_charge+Pt_neu;
  

  double phid=phimax-phisec;
  phid = fmod(phid+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;  
  
  // -------- Select each event
  
 
  //select bhabha/dimu/cosmic events, need prescale

  if( nGood == 2 && nCharge==0 && (m_selectBhabha || m_selectDimu) ){
    
    //bhabha
    if( m_events%m_bhabha_scale == 0 && m_selectBhabha && echarge/m_ecm>0.8 && eopmax>0.8 && eopsec>0.8){
      m_isBhabha=true;
      m_bhabhaNumber++;
    }


    //dimu 
    if( m_events%m_dimu_scale == 0 && m_selectDimu && eemc/m_ecm<0.3){
      
      EvtRecTrackIterator itp = evtRecTrkCol->begin() + iCP[0];
      EvtRecTrackIterator itn = evtRecTrkCol->begin() + iCN[0];
      
      double time1=-99,depth1=-99;
      double time2=-99,depth2=-99;
      if( (*itp)->isTofTrackValid() ){
	SmartRefVector<RecTofTrack> tofTrkCol= (*itp)->tofTrack();
	SmartRefVector<RecTofTrack>::iterator iter_tof=tofTrkCol.begin();

	for(;iter_tof!=tofTrkCol.end();iter_tof++){
	  TofHitStatus* status =new TofHitStatus;
	  status->setStatus( (*iter_tof)->status() );
	  if(status->is_cluster()){
	    time1=(*iter_tof)->tof();
	  }
	  delete status;
	}
      }

      if(  (*itp)->isMucTrackValid() ){
	RecMucTrack* mucTrk=(*itp)->mucTrack();
	depth1=mucTrk->depth();
      }

      if( (*itn)->isTofTrackValid() ){
	SmartRefVector<RecTofTrack> tofTrkCol= (*itn)->tofTrack();
	SmartRefVector<RecTofTrack>::iterator iter_tof=tofTrkCol.begin();

	for(;iter_tof!=tofTrkCol.end();iter_tof++){
	  TofHitStatus* status =new TofHitStatus;
	  status->setStatus( (*iter_tof)->status() );
	  if(status->is_cluster()){
	    time2=(*iter_tof)->tof();
	  }
	  delete status;
	}
      }
      
      if(  (*itn)->isMucTrackValid() ){
	RecMucTrack* mucTrk=(*itn)->mucTrack();
	depth2=mucTrk->depth();
      }

      
      //dimu
      //if( m_selectDimu && time1!=-99 && time2!=-99 && fabs(time1-time2)<5 && depth1>5 && depth2>5){  //tight, no endcap
      // if( eopmax<0.3 && eopsec<0.3 && fabs( fabs(phid)-CLHEP::pi)*180.0/CLHEP::pi<6 && fabs(psec)>m_ecm/4.0 ){  //tight, no rad dimu
      // if( eemc<0.3*m_ecm && (fabs(pmax)>0.45*m_ecm || fabs(psec)>0.45*m_ecm) ){
      if( ((fabs(pmax)/0.5/m_ecm>0.15 && fabs(pmax)/0.5/m_ecm<.75) || (fabs(psec)/0.5/m_ecm>0.15 && fabs(psec)/0.5/m_ecm<.75)) && (eopmax<0.4 || eopsec<0.4)
	  && (depth1>=3 || depth2>=3)){
	m_isDimu=true;
	m_dimuNumber++;
      }		
    }//end of dimu
    

    
  }//end of bhabha, dimu 


  
  if(m_selectCosmic && nCosmicGood == 2 && eemc/m_ecm<0.3){
    
    EvtRecTrackIterator itp = evtRecTrkCol->begin() + iCosmicGood[0];
    EvtRecTrackIterator itn = evtRecTrkCol->begin() + iCosmicGood[1];
    
    double time1=-99,depth1=-99;
    double time2=-99,depth2=-99;
    if( (*itp)->isTofTrackValid() ){
      SmartRefVector<RecTofTrack> tofTrkCol= (*itp)->tofTrack();
      SmartRefVector<RecTofTrack>::iterator iter_tof=tofTrkCol.begin();
      
      for(;iter_tof!=tofTrkCol.end();iter_tof++){
	  TofHitStatus* status =new TofHitStatus;
	  status->setStatus( (*iter_tof)->status() );
	  if(status->is_cluster()){
	    time1=(*iter_tof)->tof();
	  }
	  delete status;
      }
    }
    
    if(  (*itp)->isMucTrackValid() ){
      RecMucTrack* mucTrk=(*itp)->mucTrack();
      depth1=mucTrk->depth();
    }
    
    if( (*itn)->isTofTrackValid() ){
      SmartRefVector<RecTofTrack> tofTrkCol= (*itn)->tofTrack();
      SmartRefVector<RecTofTrack>::iterator iter_tof=tofTrkCol.begin();
      
      for(;iter_tof!=tofTrkCol.end();iter_tof++){
	TofHitStatus* status =new TofHitStatus;
	status->setStatus( (*iter_tof)->status() );
	if(status->is_cluster()){
	  time2=(*iter_tof)->tof();
	}
	delete status;
	}
    }
    
    if(  (*itn)->isMucTrackValid() ){
      RecMucTrack* mucTrk=(*itn)->mucTrack();
      depth2=mucTrk->depth();
    }
    
    //cosmic
    //if( m_selectCosmic && time1!=-99 && time2!=-99 && fabs(time1-time2)>5 && depth1>5 && depth2>5){
    if( m_selectCosmic && time1!=-99 && time2!=-99 && fabs(time1-time2)>5 ){
      m_isCosmic=true;
      m_cosmicNumber++;
    }		
    
      
  }//end of cosmic
    

  //select generic protons
  
  if(m_events%m_proton_scale ==0 ){
 
    bool protoncand=false;
    for(int i=0; i<nGood; i++){
      
      EvtRecTrackIterator iter = evtRecTrkCol->begin() + iGood[i];
      RecMdcKalTrack *mdcTrk = (*iter)->mdcKalTrack();
      mdcTrk->setPidType(RecMdcKalTrack::proton);
      
      double pp =  P(mdcTrk);
      double chiP=-99;
      if((*iter)->isMdcDedxValid()){
	RecMdcDedx* dedxTrk=(*iter)->mdcDedx();
	chiP=dedxTrk->chiP();
    }
       
      if( fabs(pp)<0.6 && fabs(chiP)<5){
	protoncand=true;
	break;
      }  
    }
    
    if( protoncand ){
      m_isGenProton=true;
      m_genProtonNumber++;
    }  
    
  }//end of generic proton


  //fill monitoring histograms for rad bhabha
  
  
  if(m_printmonitor){
    h_nGood->fill(nGood);
    h_nCharge->fill(nCharge);
    h_pmaxobeam->fill(pmax/(m_ecm/2.0));
    h_eopmax->fill(eopmax);
    h_eopsec->fill(eopsec);
    h_deltap->fill(pmax-psec);
    h_esumoecm->fill(esum/m_ecm);
    h_egmax->fill(egmax);
    h_deltaphi->fill(phid);
    h_Pt->fill(Pt.mag());
  }

  

   //select radbhabha
  if(nGood>1  && pmax/(m_ecm/2.0)>0.4 && eopmax>0.5 && esum/m_ecm>0.75 && 
    egmax>0.08 && fabs( fabs(phid)-CLHEP::pi)*180.0/CLHEP::pi>2.86 )
    {
      m_isRadBhabha=true;
      m_radBhabhaNumber++;
    }   
  //select radbhabha tight 
  if( m_isRadBhabha )
    {
      //prescale high momentum tracks
      if(nGood==2 && nCharge==0 && eemc/m_ecm>=0.7 && eopmax>=0.85 && eopmax<=1.15 && eopsec>=0.85 && eopsec<=1.15 ){
	
	if( fabs(fabs(pmax)-m_ecm/2.0)<0.1 && fabs(fabs(psec)-m_ecm/2.0)<0.1 ){
	  if(m_events%1000==0){
	    m_isRadBhabhaT=true;
	    m_radBhabhaTNumber++;
	  }
	}
	else{
	  m_isRadBhabhaT=true;
	  m_radBhabhaTNumber++;
	}
      
      }
      

      
    }   

  //select ggee events, (exclude radee tight)
  //if(!m_isRadBhabhaT && nGood==2 && nCharge==0 && eopmax>=0.85 && eopsec>=0.85 && eemc/m_ecm<=0.8 && Pt.mag()<=0.2)
  if(!m_isRadBhabhaT && nGood==2 && nCharge==0 && eopmax>=0.85 && eopmax<=1.15 && eopsec>=0.85 && eopsec<=1.15 && eemc/m_ecm<=0.8 && Pt.mag()<=0.2)
    {
      m_isGGEE=true;
      m_GGEENumber++;
    }
  
  //select gg4pi events
  if(m_selectGG4Pi && nGood==4 && nCharge==0 && pmax/(m_ecm/2.0)>0.04 && pmax/(m_ecm/2.0)<0.9 && esum/m_ecm>0.04 && esum/m_ecm<0.75 && Pt.mag()<=0.2)
    {
      m_isGG4Pi=true;
      m_GG4PiNumber++;
    }
 
  //select rhopi/kstark events
  if( (m_selectRhoPi || m_selectKstarK) && nGood == 2 && nCharge==0 && nPi0 == 1){
    
    EvtRecTrackIterator itone = evtRecTrkCol->begin() + iGood[0];
    EvtRecTrackIterator ittwo = evtRecTrkCol->begin() + iGood[1];
 
    
    if( (*itone)->isMdcKalTrackValid() && (*ittwo)->isMdcKalTrackValid() ) {

      RecMdcKalTrack *trk1 = (*itone)->mdcKalTrack();
      RecMdcKalTrack *trk2 = (*ittwo)->mdcKalTrack();
      
      HepLorentzVector p4trk1;
      p4trk1.setPx(Px(trk1));
      p4trk1.setPy(Py(trk1));
      p4trk1.setPz(Pz(trk1));
      p4trk1.setE(sqrt( pow(P(trk1),2)+ mkaon*mkaon) );

      HepLorentzVector p4trk2;
      p4trk2.setPx(Px(trk2));
      p4trk2.setPy(Py(trk2));
      p4trk2.setPz(Pz(trk2));
      p4trk2.setE(sqrt( pow(P(trk2),2)+ mkaon*mkaon) );


      HepLorentzVector p4trk3;
      p4trk3.setPx(Px(trk1));
      p4trk3.setPy(Py(trk1));
      p4trk3.setPz(Pz(trk1));
      p4trk3.setE(sqrt( pow(P(trk1),2)+ mpi*mpi) );

      HepLorentzVector p4trk4;
      p4trk4.setPx(Px(trk2));
      p4trk4.setPy(Py(trk2));
      p4trk4.setPz(Pz(trk2));
      p4trk4.setE(sqrt( pow(P(trk2),2)+ mpi*mpi) );

      
      //EvtRecPi0Col::iterator itpi0 = recPi0Col->begin();
      itpi0 = recPi0Col->begin();
      HepLorentzVector p4gam1 = (*itpi0)->hiPfit();
      HepLorentzVector p4gam2 = (*itpi0)->loPfit();
      HepLorentzVector p4pi0 = p4gam1+p4gam2;

      HepLorentzVector p4total = p4trk1 + p4trk2 + p4pi0; //kstark
      HepLorentzVector p4total2 = p4trk3 + p4trk4 + p4pi0; // rhopi
      
      double rhopimass=p4total2.m();
      double kstarkmass=p4total.m();
      if( (kstarkmass > 3.0 && kstarkmass < 3.15) || (rhopimass > 3.0 && rhopimass < 3.15) ){
	
	//tight cuts
	  
	//remove bhabha background
	double eop1=0.0,eop2=0.0;
	if( (*itone)->isEmcShowerValid() ){
	  RecEmcShower *emcTrk = (*itone)->emcShower();
	  double etrk=emcTrk->energy();
	  //cout<<"trk1 p="<<P(trk1)<<endl;
	  if(P(trk1)!=0){
	    eop1 = etrk/P(trk1);
	    //if( fabs(eop1)> 0.8 ) return StatusCode::SUCCESS;
	  }
	}
	
	if( (*ittwo)->isEmcShowerValid() ){
	  RecEmcShower *emcTrk = (*ittwo)->emcShower();
	  double etrk=emcTrk->energy();
	  //cout<<"trk2 p="<<P(trk2)<<endl;
	  if(P(trk2)!=0){
	    eop2 = etrk/P(trk2);
	    //if( fabs(eop2)> 0.8 ) return StatusCode::SUCCESS;
	  }
	}
	
	if(eop1<0.8 && eop2<0.8){ 
	
	  if(rhopimass>3.0 && rhopimass<3.15){
	    //kinematic fit
	    //HepLorentzVector ecms(0.034,0,0,3.097);
	    HepLorentzVector ecms(0.034,0,0,m_ecm);
	    
	    
	    WTrackParameter wvpiTrk1, wvpiTrk2;
	    wvpiTrk1 = WTrackParameter(mpi, trk1->getZHelix(), trk1->getZError());
	    wvpiTrk2 = WTrackParameter(mpi, trk2->getZHelix(), trk2->getZError());
	    
	    const EvtRecTrack* gTrk1 = (*itpi0)->hiEnGamma();
	    const EvtRecTrack* gTrk2 = (*itpi0)->loEnGamma();
	    RecEmcShower *gShr1 = const_cast<EvtRecTrack*>(gTrk1)->emcShower();
	    RecEmcShower *gShr2 = const_cast<EvtRecTrack*>(gTrk2)->emcShower();
	    
	    KinematicFit * kmfit = KinematicFit::instance();
	    kmfit->init();
	    kmfit->AddTrack(0, wvpiTrk1);
	    kmfit->AddTrack(1, wvpiTrk2);
	    kmfit->AddTrack(2, 0.0, gShr1);
	    kmfit->AddTrack(3, 0.0, gShr2);
	    kmfit->AddFourMomentum(0, ecms);
	    
	    bool oksq1 = kmfit->Fit();
	    double chi1 = kmfit->chisq();
	    
	    //
	    if(oksq1 && chi1<=60){
	      m_isRhoPi = true;
	      m_rhoPiNumber++;
	    }
	  } //end of selecting rhopi


	  if(kstarkmass>3.0 && kstarkmass<3.15){
	    
	    //kstark resonances
	    double mkstarp=0, mkstarm=0;
	    if( trk1->charge() >0 ){
	      HepLorentzVector p4kstarp =  p4trk1 + p4pi0;
	      HepLorentzVector p4kstarm =  p4trk2 + p4pi0;
	      mkstarp = p4kstarp.m();
	      mkstarm = p4kstarm.m();
	    }
	    else{
	      HepLorentzVector p4kstarm =  p4trk1 + p4pi0;
	      HepLorentzVector p4kstarp =  p4trk2 + p4pi0;
	      mkstarp = p4kstarp.m();
	      mkstarm = p4kstarm.m();
	    }

	    if ( (fabs(mkstarp-0.89166)<0.1 && fabs(mkstarm-0.89166)>0.1) || (fabs(mkstarm-0.89166)<0.1 && fabs(mkstarp-0.89166)>0.1 ) ){
	      //kinematic fit
	      //HepLorentzVector ecms(0.034,0,0,3.097);
	      HepLorentzVector ecms(0.034,0,0,m_ecm);
	      
	      WTrackParameter wvpiTrk1, wvpiTrk2;
	      wvpiTrk1 = WTrackParameter(mkaon, trk1->getZHelix(), trk1->getZError());
	      wvpiTrk2 = WTrackParameter(mkaon, trk2->getZHelix(), trk2->getZError());

	      const EvtRecTrack* gTrk1 = (*itpi0)->hiEnGamma();
	      const EvtRecTrack* gTrk2 = (*itpi0)->loEnGamma();
	      RecEmcShower *gShr1 = const_cast<EvtRecTrack*>(gTrk1)->emcShower();
	      RecEmcShower *gShr2 = const_cast<EvtRecTrack*>(gTrk2)->emcShower();

	      KinematicFit * kmfit = KinematicFit::instance();
	      kmfit->init();
	      kmfit->AddTrack(0, wvpiTrk1);
	      kmfit->AddTrack(1, wvpiTrk2);
	      kmfit->AddTrack(2, 0.0, gShr1);
	      kmfit->AddTrack(3, 0.0, gShr2);
	      kmfit->AddFourMomentum(0, ecms);

	      bool oksq1 = kmfit->Fit();
	      double chi1 = kmfit->chisq();
	      //
	    
	      if(oksq1 && chi1<=60){
		m_isKstarK = true;
		m_kstarKNumber++;
	      }
	      
	    }
	    
	  } //end of selecting kstark

	}//end of non di-electron
	
	//end of tight cuts
	
      }
    } 
    


  } //end of selecting rhopi/kstark events

  //select ppPiPi events
  if(m_selectPPPiPi && nGood ==4 && nCharge == 0 && nPi0==0){

    EvtRecTrackIterator itone = evtRecTrkCol->begin() + iCP[0];
    EvtRecTrackIterator ittwo = evtRecTrkCol->begin() + iCP[1];
    EvtRecTrackIterator itthr = evtRecTrkCol->begin() + iCN[0];
    EvtRecTrackIterator itfor = evtRecTrkCol->begin() + iCN[1];
    RecMdcKalTrack *trk1 = (*itone)->mdcKalTrack();
    RecMdcKalTrack *trk2 = (*ittwo)->mdcKalTrack();
    RecMdcKalTrack *trk3 = (*itthr)->mdcKalTrack();
    RecMdcKalTrack *trk4 = (*itfor)->mdcKalTrack();

    HepLorentzVector p4trkpp;
    HepLorentzVector p4trkpm;
    HepLorentzVector p4trkpip;
    HepLorentzVector p4trkpim;

    //hypothesis 1

    trk1->setPidType(RecMdcKalTrack::proton);
    trk2->setPidType(RecMdcKalTrack::pion);
    trk3->setPidType(RecMdcKalTrack::proton);
    trk4->setPidType(RecMdcKalTrack::pion);


    p4trkpp.setPx(Px(trk1));
    p4trkpp.setPy(Py(trk1));
    p4trkpp.setPz(Pz(trk1));
    p4trkpp.setE(sqrt( pow(P(trk1),2)+ mproton*mproton) );

    p4trkpm.setPx(Px(trk3));
    p4trkpm.setPy(Py(trk3));
    p4trkpm.setPz(Pz(trk3));
    p4trkpm.setE(sqrt( pow(P(trk3),2)+ mproton*mproton) );


    p4trkpip.setPx(Px(trk2));
    p4trkpip.setPy(Py(trk2));
    p4trkpip.setPz(Pz(trk2));
    p4trkpip.setE(sqrt( pow(P(trk2),2)+ mpi*mpi) );

    p4trkpim.setPx(Px(trk4));
    p4trkpim.setPy(Py(trk4));
    p4trkpim.setPz(Pz(trk4));
    p4trkpim.setE(sqrt( pow(P(trk4),2)+ mpi*mpi) );

    double jpsimass1= (p4trkpp+p4trkpm+p4trkpip+p4trkpim).m();


    //hypothesis 2
    
    trk1->setPidType(RecMdcKalTrack::proton);
    trk2->setPidType(RecMdcKalTrack::pion);
    trk3->setPidType(RecMdcKalTrack::pion);
    trk4->setPidType(RecMdcKalTrack::proton);

    
    p4trkpp.setPx(Px(trk1));
    p4trkpp.setPy(Py(trk1));
    p4trkpp.setPz(Pz(trk1));
    p4trkpp.setE(sqrt( pow(P(trk1),2)+ mproton*mproton) );

    p4trkpm.setPx(Px(trk4));
    p4trkpm.setPy(Py(trk4));
    p4trkpm.setPz(Pz(trk4));
    p4trkpm.setE(sqrt( pow(P(trk4),2)+ mproton*mproton) );


    p4trkpip.setPx(Px(trk2));
    p4trkpip.setPy(Py(trk2));
    p4trkpip.setPz(Pz(trk2));
    p4trkpip.setE(sqrt( pow(P(trk2),2)+ mpi*mpi) );

    p4trkpim.setPx(Px(trk3));
    p4trkpim.setPy(Py(trk3));
    p4trkpim.setPz(Pz(trk3));
    p4trkpim.setE(sqrt( pow(P(trk3),2)+ mpi*mpi) );

    double jpsimass2= (p4trkpp+p4trkpm+p4trkpip+p4trkpim).m();


    //hypothesis 3

    trk1->setPidType(RecMdcKalTrack::pion);
    trk2->setPidType(RecMdcKalTrack::proton);
    trk3->setPidType(RecMdcKalTrack::proton);
    trk4->setPidType(RecMdcKalTrack::pion);


    p4trkpp.setPx(Px(trk2));
    p4trkpp.setPy(Py(trk2));
    p4trkpp.setPz(Pz(trk2));
    p4trkpp.setE(sqrt( pow(P(trk2),2)+ mproton*mproton) );

    p4trkpm.setPx(Px(trk3));
    p4trkpm.setPy(Py(trk3));
    p4trkpm.setPz(Pz(trk3));
    p4trkpm.setE(sqrt( pow(P(trk3),2)+ mproton*mproton) );


    p4trkpip.setPx(Px(trk1));
    p4trkpip.setPy(Py(trk1));
    p4trkpip.setPz(Pz(trk1));
    p4trkpip.setE(sqrt( pow(P(trk1),2)+ mpi*mpi) );

    p4trkpim.setPx(Px(trk4));
    p4trkpim.setPy(Py(trk4));
    p4trkpim.setPz(Pz(trk4));
    p4trkpim.setE(sqrt( pow(P(trk4),2)+ mpi*mpi) );

    double jpsimass3= (p4trkpp+p4trkpm+p4trkpip+p4trkpim).m();

    
    //hypothesis 4

    trk1->setPidType(RecMdcKalTrack::pion);
    trk2->setPidType(RecMdcKalTrack::proton);
    trk3->setPidType(RecMdcKalTrack::pion);
    trk4->setPidType(RecMdcKalTrack::proton);


    p4trkpp.setPx(Px(trk2));
    p4trkpp.setPy(Py(trk2));
    p4trkpp.setPz(Pz(trk2));
    p4trkpp.setE(sqrt( pow(P(trk2),2)+ mproton*mproton) );

    p4trkpm.setPx(Px(trk4));
    p4trkpm.setPy(Py(trk4));
    p4trkpm.setPz(Pz(trk4));
    p4trkpm.setE(sqrt( pow(P(trk4),2)+ mproton*mproton) );


    p4trkpip.setPx(Px(trk1));
    p4trkpip.setPy(Py(trk1));
    p4trkpip.setPz(Pz(trk1));
    p4trkpip.setE(sqrt( pow(P(trk1),2)+ mpi*mpi) );

    p4trkpim.setPx(Px(trk3));
    p4trkpim.setPy(Py(trk3));
    p4trkpim.setPz(Pz(trk3));
    p4trkpim.setE(sqrt( pow(P(trk3),2)+ mpi*mpi) );

    double jpsimass4= (p4trkpp+p4trkpm+p4trkpip+p4trkpim).m();
    

    
    
    if( fabs(jpsimass1-3.075)<=0.075 || fabs(jpsimass2-3.075)<=0.075 || 
	fabs(jpsimass3-3.075)<=0.075 || fabs(jpsimass4-3.075)<=0.075){
      
      //tight cuts
      
      //kinematic fits
      double chi1, chi2, chi3, chi4;
      int type=0;
      double chimin=-999;
      HepLorentzVector ecms(0.034,0,0,m_ecm);
      
      WTrackParameter wvpiTrk1, wvpiTrk2, wvpiTrk3, wvpiTrk4 ;

      {
	wvpiTrk1 = WTrackParameter(mproton, trk1->getZHelix(), trk1->getZError());
	wvpiTrk2 = WTrackParameter(mpi, trk2->getZHelix(), trk2->getZError());
	wvpiTrk3 = WTrackParameter(mproton, trk3->getZHelix(), trk3->getZError());
	wvpiTrk4 = WTrackParameter(mpi, trk4->getZHelix(), trk4->getZError());


	KinematicFit * kmfit = KinematicFit::instance();
	kmfit->init();
	kmfit->AddTrack(0, wvpiTrk1);
	kmfit->AddTrack(1, wvpiTrk2);
	kmfit->AddTrack(2, wvpiTrk3);
	kmfit->AddTrack(3, wvpiTrk4);
	kmfit->AddFourMomentum(0, ecms);

	bool oksq1 = kmfit->Fit();
	chi1 = kmfit->chisq();
	if(oksq1) {
	  chimin=chi1;
	  type=1;
	}
	//
      }

      
      {
	wvpiTrk1 = WTrackParameter(mproton, trk1->getZHelix(), trk1->getZError());
	wvpiTrk2 = WTrackParameter(mpi, trk2->getZHelix(), trk2->getZError());
	wvpiTrk3 = WTrackParameter(mpi, trk3->getZHelix(), trk3->getZError());
	wvpiTrk4 = WTrackParameter(mproton, trk4->getZHelix(), trk4->getZError());


	KinematicFit * kmfit = KinematicFit::instance();
	kmfit->init();
	kmfit->AddTrack(0, wvpiTrk1);
	kmfit->AddTrack(1, wvpiTrk2);
	kmfit->AddTrack(2, wvpiTrk3);
	kmfit->AddTrack(3, wvpiTrk4);
	kmfit->AddFourMomentum(0, ecms);

	bool oksq1 = kmfit->Fit();
	chi2 = kmfit->chisq();
	if(oksq1){
	  if(type==0){
	    chimin=chi2;
	    type=2;
	  }
	  else if(chi2<chimin){
	    chimin=chi2;
	    type=2;
	  }
	}
	//
      }

      {
	wvpiTrk1 = WTrackParameter(mpi, trk1->getZHelix(), trk1->getZError());
	wvpiTrk2 = WTrackParameter(mproton, trk2->getZHelix(), trk2->getZError());
	wvpiTrk3 = WTrackParameter(mpi, trk3->getZHelix(), trk3->getZError());
	wvpiTrk4 = WTrackParameter(mproton, trk4->getZHelix(), trk4->getZError());

	KinematicFit * kmfit = KinematicFit::instance();
	kmfit->init();
	kmfit->AddTrack(0, wvpiTrk1);
	kmfit->AddTrack(1, wvpiTrk2);
	kmfit->AddTrack(2, wvpiTrk3);
	kmfit->AddTrack(3, wvpiTrk4);

	kmfit->AddFourMomentum(0, ecms);

	bool oksq1 = kmfit->Fit();
	chi3 = kmfit->chisq();
	if(oksq1){
	  if(type==0){
	    chimin=chi3;
	    type=3;
	  }
	  else if(chi3<chimin){
	    chimin=chi3;
	    type=3;
	  }
	}
	
	//
      }


      {
	wvpiTrk1 = WTrackParameter(mpi, trk1->getZHelix(), trk1->getZError());
	wvpiTrk2 = WTrackParameter(mproton, trk2->getZHelix(), trk2->getZError());
	wvpiTrk3 = WTrackParameter(mproton, trk3->getZHelix(), trk3->getZError());
	wvpiTrk4 = WTrackParameter(mpi, trk4->getZHelix(), trk4->getZError());

	KinematicFit * kmfit = KinematicFit::instance();
	kmfit->init();
	kmfit->AddTrack(0, wvpiTrk1);
	kmfit->AddTrack(1, wvpiTrk2);
	kmfit->AddTrack(2, wvpiTrk3);
	kmfit->AddTrack(3, wvpiTrk4);

	kmfit->AddFourMomentum(0, ecms);

	bool oksq1 = kmfit->Fit();
	chi4 = kmfit->chisq();
	
	if(oksq1){
	  if(type==0){
	    chimin=chi4;
	    type=4;
	  }
	  else if(chi4<chimin){
	    chimin=chi4;
	    type=4;
	  }
	}

	//
      }

      if(type!=0 && chimin<100){
	m_isPPPiPi = true;
	m_ppPiPiNumber++;
      }
      
      //end of tight cuts
      

    } //end of loose cuts

  }//end of selecting pppipi
  
  //select recoil events
  //if( m_selectRecoJpsi && (nGood==4 || nGood==6) && nCharge==0 ){
  if( (m_selectPsipRhoPi || m_selectPsipKstarK || m_selectPsipPPPiPi) && (nGood==4 || nGood==6) && nCharge==0 ){
    EvtRecTrackIterator pione, pitwo;
    RecMdcKalTrack *pitrk1;
    RecMdcKalTrack *pitrk2;

    double bestmass=1.0;
    int pindex,nindex;
    vector<int> iJPsiP,iJPsiN;
    for(int ip=0; ip<iCP.size(); ip++){
      for(int in=0; in<iCN.size();in++){
	pione = evtRecTrkCol->begin() + iCP[ip];
	pitwo = evtRecTrkCol->begin() + iCN[in];
	pitrk1 = (*pione)->mdcKalTrack();
	pitrk2 = (*pitwo)->mdcKalTrack();
	Hep3Vector p1(Px(pitrk1), Py(pitrk1), Pz(pitrk1));
	Hep3Vector p2(Px(pitrk2), Py(pitrk2), Pz(pitrk2));
	double E1=sqrt( pow(P(pitrk1),2)+mpi*mpi);
	double E2=sqrt( pow(P(pitrk2),2)+mpi*mpi);
	double recomass = sqrt(pow(3.686-E1-E2,2)- (-(p1+p2)).mag2() );
	//if(fabs(recomass-3.686)< fabs(bestmass-3.686)){
	if(fabs(recomass-3.096)< fabs(bestmass-3.096)){
	  bestmass=recomass;
	  pindex=ip;
	  nindex=in;
	}
      }
    }


    //soft pions
    pione = evtRecTrkCol->begin() + iCP[pindex];
    pitwo = evtRecTrkCol->begin() + iCN[nindex];
    pitrk1 = (*pione)->mdcKalTrack();
    pitrk2 = (*pitwo)->mdcKalTrack();


    //tracks from jpsi
    double jpsicharge=0;
    for(int ip=0; ip<iCP.size(); ip++){
      if(ip!=pindex){ 
	iJPsiP.push_back(iCP[ip]);
	EvtRecTrackIterator itertmp=evtRecTrkCol->begin() + iCP[ip];
	RecMdcKalTrack* trktmp=(*itertmp)->mdcKalTrack();
	jpsicharge+=trktmp->charge();
      }
    }

    for(int in=0; in<iCN.size(); in++){
      if(in!=nindex){ 
	iJPsiN.push_back(iCN[in]);
	EvtRecTrackIterator itertmp=evtRecTrkCol->begin() + iCN[in];
	RecMdcKalTrack* trktmp=(*itertmp)->mdcKalTrack();
	jpsicharge+=trktmp->charge();
      }
    }
    
    
    HepLorentzVector ecms(0.034,0,0,m_ecm);

    //jpsi to 2 charged track and 1 pi0
    if( (m_selectPsipRhoPi || m_selectPsipKstarK) && (bestmass>3.075 && bestmass<3.120) && nGood==4 && jpsicharge==0 && nPi0==1){

      EvtRecTrackIterator itone = evtRecTrkCol->begin() + iJPsiP[0];
      EvtRecTrackIterator ittwo = evtRecTrkCol->begin() + iJPsiN[0];
      
      
      if( (*itone)->isMdcKalTrackValid() && (*ittwo)->isMdcKalTrackValid() ) {
	
	RecMdcKalTrack *trk1 = (*itone)->mdcKalTrack();
	RecMdcKalTrack *trk2 = (*ittwo)->mdcKalTrack();
      
	HepLorentzVector p4trk1;
	p4trk1.setPx(Px(trk1));
	p4trk1.setPy(Py(trk1));
	p4trk1.setPz(Pz(trk1));
	p4trk1.setE(sqrt( pow(P(trk1),2)+ mkaon*mkaon) );

	HepLorentzVector p4trk2;
	p4trk2.setPx(Px(trk2));
	p4trk2.setPy(Py(trk2));
	p4trk2.setPz(Pz(trk2));
	p4trk2.setE(sqrt( pow(P(trk2),2)+ mkaon*mkaon) );
	

	HepLorentzVector p4trk3;
	p4trk3.setPx(Px(trk1));
	p4trk3.setPy(Py(trk1));
	p4trk3.setPz(Pz(trk1));
	p4trk3.setE(sqrt( pow(P(trk1),2)+ mpi*mpi) );

	HepLorentzVector p4trk4;
	p4trk4.setPx(Px(trk2));
	p4trk4.setPy(Py(trk2));
	p4trk4.setPz(Pz(trk2));
	p4trk4.setE(sqrt( pow(P(trk2),2)+ mpi*mpi) );
	
	
	EvtRecPi0Col::iterator itpi0 = recPi0Col->begin();
	const EvtRecTrack* gTrk1 = (*itpi0)->hiEnGamma();
	const EvtRecTrack* gTrk2 = (*itpi0)->loEnGamma();
	RecEmcShower *gShr1 = const_cast<EvtRecTrack*>(gTrk1)->emcShower();
	RecEmcShower *gShr2 = const_cast<EvtRecTrack*>(gTrk2)->emcShower();
	RecEmcShower *gShr3 = const_cast<EvtRecTrack*>(gTrk1)->emcShower();
	RecEmcShower *gShr4 = const_cast<EvtRecTrack*>(gTrk2)->emcShower();


	HepLorentzVector p4gam1 = (*itpi0)->hiPfit();
	HepLorentzVector p4gam2 = (*itpi0)->loPfit();
	HepLorentzVector p4pi0 = p4gam1 + p4gam2;
	HepLorentzVector p4total = p4trk1 + p4trk2 + p4pi0;
	HepLorentzVector p4total2 = p4trk3 + p4trk4 + p4pi0;
	

	HepLorentzVector p4kstarp =  p4trk1 + p4pi0;
	HepLorentzVector p4kstarm =  p4trk2 + p4pi0;
	double mkstarp = p4kstarp.m();
	double mkstarm = p4kstarm.m();

	if( (p4total.m() > 3.0 && p4total.m() < 3.15) || (p4total2.m() > 3.0 && p4total2.m() < 3.15) ){
	  //m_isRecoJpsi = true;
	  //m_recoJpsiNumber++;
	
	  
	  //tighten cuts for rhopi and kstark
	
	  
	  WTrackParameter wvpiTrk1, wvpiTrk2;
	  wvpiTrk1 = WTrackParameter(mpi, trk1->getZHelix(), trk1->getZError());
	  wvpiTrk2 = WTrackParameter(mpi, trk2->getZHelix(), trk2->getZError());
	  
	  WTrackParameter wvkTrk1, wvkTrk2;
	  wvkTrk1 = WTrackParameter(mkaon, trk1->getZHelixK(), trk1->getZErrorK());//kaon
	  wvkTrk2 = WTrackParameter(mkaon, trk2->getZHelixK(), trk2->getZErrorK());//kaon
	  
	  //soft pions
	  WTrackParameter wvpiTrk3, wvpiTrk4;
	  wvpiTrk3 = WTrackParameter(mpi, pitrk1->getZHelix(), pitrk1->getZError());
	  wvpiTrk4 = WTrackParameter(mpi, pitrk2->getZHelix(), pitrk2->getZError());
	  
	  if(m_selectPsipRhoPi){
	    KinematicFit * kmfit = KinematicFit::instance();   
	    kmfit->init();
	    kmfit->AddTrack(0, wvpiTrk1);
	    kmfit->AddTrack(1, wvpiTrk2);
	    kmfit->AddTrack(2, 0.0, gShr1);
	    kmfit->AddTrack(3, 0.0, gShr2);
	    kmfit->AddTrack(4, wvpiTrk3);
	    kmfit->AddTrack(5, wvpiTrk4);
	    kmfit->AddFourMomentum(0, ecms);
	    
	    bool oksq1 = kmfit->Fit();
	    double chi1 = kmfit->chisq();
	    //
	  
	    if(oksq1 && chi1>0 && chi1<100){
	      m_isPsipRhoPi = true;
	      m_psipRhoPiNumber++;
	    }
	  }
	  if(m_selectPsipKstarK){
	    KinematicFit * kmfit2 = KinematicFit::instance();   
	    kmfit2->init();
	    kmfit2->AddTrack(0, wvkTrk1);
	    kmfit2->AddTrack(1, wvkTrk2);
	    kmfit2->AddTrack(2, 0.0, gShr3);
	    kmfit2->AddTrack(3, 0.0, gShr4);
	    kmfit2->AddTrack(4, wvpiTrk3);
	    kmfit2->AddTrack(5, wvpiTrk4);
	    kmfit2->AddFourMomentum(0, ecms);
	    
	    
	    bool oksq2 = kmfit2->Fit();
	    double chi2 = kmfit2->chisq();
	    
	    if(oksq2 && chi2>0 && chi2<200 && 
	       ( (fabs(mkstarp-0.89166)<0.1 && fabs(mkstarm-0.89166)>0.1)
                 || (fabs(mkstarm-0.89166)<0.1 && fabs(mkstarp-0.89166)>0.1 ) )){
	      m_isPsipKstarK = true;
	      m_psipKstarKNumber++;
	    }
	    
	  }

	  
	}//end of loose cuts
	
      }
      
    } //recoil jpsi to 2tracks and 1 pi0
    


    //jpsi to pppipi
    if(m_selectPsipPPPiPi && (bestmass>3.075 && bestmass<3.120) && nGood==6 && jpsicharge==0 && nPi0==0){

      
      EvtRecTrackIterator itone = evtRecTrkCol->begin() + iJPsiP[0];
      EvtRecTrackIterator ittwo = evtRecTrkCol->begin() + iJPsiP[1];
      EvtRecTrackIterator itthr = evtRecTrkCol->begin() + iJPsiN[0];
      EvtRecTrackIterator itfor = evtRecTrkCol->begin() + iJPsiN[1];
      RecMdcKalTrack *trk1 = (*itone)->mdcKalTrack();
      RecMdcKalTrack *trk2 = (*ittwo)->mdcKalTrack();
      RecMdcKalTrack *trk3 = (*itthr)->mdcKalTrack();
      RecMdcKalTrack *trk4 = (*itfor)->mdcKalTrack();
      
      HepLorentzVector p4trkpp;
      HepLorentzVector p4trkpm;
      HepLorentzVector p4trkpip;
      HepLorentzVector p4trkpim;
      
      //hypothesis 1

      trk1->setPidType(RecMdcKalTrack::proton);
      trk2->setPidType(RecMdcKalTrack::pion);
      trk3->setPidType(RecMdcKalTrack::proton);
      trk4->setPidType(RecMdcKalTrack::pion);
      
      
      p4trkpp.setPx(Px(trk1));
      p4trkpp.setPy(Py(trk1));
      p4trkpp.setPz(Pz(trk1));
      p4trkpp.setE(sqrt( pow(P(trk1),2)+ mproton*mproton) );
      
      p4trkpm.setPx(Px(trk3));
      p4trkpm.setPy(Py(trk3));
      p4trkpm.setPz(Pz(trk3));
      p4trkpm.setE(sqrt( pow(P(trk3),2)+ mproton*mproton) );
      
      
      p4trkpip.setPx(Px(trk2));
      p4trkpip.setPy(Py(trk2));
      p4trkpip.setPz(Pz(trk2));
      p4trkpip.setE(sqrt( pow(P(trk2),2)+ mpi*mpi) );

      p4trkpim.setPx(Px(trk4));
      p4trkpim.setPy(Py(trk4));
      p4trkpim.setPz(Pz(trk4));
      p4trkpim.setE(sqrt( pow(P(trk4),2)+ mpi*mpi) );
      
      double jpsimass1= (p4trkpp+p4trkpm+p4trkpip+p4trkpim).m();
      
      
      //hypothesis 2
      
      trk1->setPidType(RecMdcKalTrack::proton);
      trk2->setPidType(RecMdcKalTrack::pion);
      trk3->setPidType(RecMdcKalTrack::pion);
      trk4->setPidType(RecMdcKalTrack::proton);
      
      
      p4trkpp.setPx(Px(trk1));
      p4trkpp.setPy(Py(trk1));
      p4trkpp.setPz(Pz(trk1));
      p4trkpp.setE(sqrt( pow(P(trk1),2)+ mproton*mproton) );
      
      p4trkpm.setPx(Px(trk4));
      p4trkpm.setPy(Py(trk4));
      p4trkpm.setPz(Pz(trk4));
      p4trkpm.setE(sqrt( pow(P(trk4),2)+ mproton*mproton) );

      
      p4trkpip.setPx(Px(trk2));
      p4trkpip.setPy(Py(trk2));
      p4trkpip.setPz(Pz(trk2));
      p4trkpip.setE(sqrt( pow(P(trk2),2)+ mpi*mpi) );
      
      p4trkpim.setPx(Px(trk3));
      p4trkpim.setPy(Py(trk3));
      p4trkpim.setPz(Pz(trk3));
      p4trkpim.setE(sqrt( pow(P(trk3),2)+ mpi*mpi) );
      
      double jpsimass2= (p4trkpp+p4trkpm+p4trkpip+p4trkpim).m();
      
      
      //hypothesis 3
      
      trk1->setPidType(RecMdcKalTrack::pion);
      trk2->setPidType(RecMdcKalTrack::proton);
      trk3->setPidType(RecMdcKalTrack::proton);
      trk4->setPidType(RecMdcKalTrack::pion);
      
      
      p4trkpp.setPx(Px(trk2));
      p4trkpp.setPy(Py(trk2));
      p4trkpp.setPz(Pz(trk2));
      p4trkpp.setE(sqrt( pow(P(trk2),2)+ mproton*mproton) );
      
      p4trkpm.setPx(Px(trk3));
      p4trkpm.setPy(Py(trk3));
      p4trkpm.setPz(Pz(trk3));
      p4trkpm.setE(sqrt( pow(P(trk3),2)+ mproton*mproton) );
      
      
      p4trkpip.setPx(Px(trk1));
      p4trkpip.setPy(Py(trk1));
      p4trkpip.setPz(Pz(trk1));
      p4trkpip.setE(sqrt( pow(P(trk1),2)+ mpi*mpi) );
      
      p4trkpim.setPx(Px(trk4));
      p4trkpim.setPy(Py(trk4));
      p4trkpim.setPz(Pz(trk4));
      p4trkpim.setE(sqrt( pow(P(trk4),2)+ mpi*mpi) );
      
      double jpsimass3= (p4trkpp+p4trkpm+p4trkpip+p4trkpim).m();
      
      
      //hypothesis 4
      
      trk1->setPidType(RecMdcKalTrack::pion);
      trk2->setPidType(RecMdcKalTrack::proton);
      trk3->setPidType(RecMdcKalTrack::pion);
      trk4->setPidType(RecMdcKalTrack::proton);
      
      
      p4trkpp.setPx(Px(trk2));
      p4trkpp.setPy(Py(trk2));
      p4trkpp.setPz(Pz(trk2));
      p4trkpp.setE(sqrt( pow(P(trk2),2)+ mproton*mproton) );
      
      p4trkpm.setPx(Px(trk4));
      p4trkpm.setPy(Py(trk4));
      p4trkpm.setPz(Pz(trk4));
      p4trkpm.setE(sqrt( pow(P(trk4),2)+ mproton*mproton) );
      
      
      p4trkpip.setPx(Px(trk1));
      p4trkpip.setPy(Py(trk1));
      p4trkpip.setPz(Pz(trk1));
      p4trkpip.setE(sqrt( pow(P(trk1),2)+ mpi*mpi) );

      p4trkpim.setPx(Px(trk3));
      p4trkpim.setPy(Py(trk3));
      p4trkpim.setPz(Pz(trk3));
      p4trkpim.setE(sqrt( pow(P(trk3),2)+ mpi*mpi) );
      
      double jpsimass4= (p4trkpp+p4trkpm+p4trkpip+p4trkpim).m();
          
      if( fabs(jpsimass1-3.075)<=0.075 || fabs(jpsimass2-3.075)<=0.075 || 
	  fabs(jpsimass3-3.075)<=0.075 || fabs(jpsimass4-3.075)<=0.075){
	

	//tighten cuts
	double chi1, chi2, chi3, chi4;
	int type=0;
	double chimin=-999;
	
	
	WTrackParameter wvpiTrk1, wvpiTrk2, wvpiTrk3, wvpiTrk4 , wvpiTrk5, wvpiTrk6 ;
	
	{
	  wvpiTrk1 = WTrackParameter(mproton, trk1->getZHelix(), trk1->getZError());
	  wvpiTrk2 = WTrackParameter(mpi, trk2->getZHelix(), trk2->getZError());
	  wvpiTrk3 = WTrackParameter(mproton, trk3->getZHelix(), trk3->getZError());
	  wvpiTrk4 = WTrackParameter(mpi, trk4->getZHelix(), trk4->getZError());
	  wvpiTrk5 = WTrackParameter(mpi, pitrk1->getZHelix(), pitrk1->getZError());
	  wvpiTrk6 = WTrackParameter(mpi, pitrk2->getZHelix(), pitrk2->getZError());
	

     
	  KinematicFit * kmfit = KinematicFit::instance();   
	  kmfit->init();
	  kmfit->AddTrack(0, wvpiTrk1);
	  kmfit->AddTrack(1, wvpiTrk2);
	  kmfit->AddTrack(2, wvpiTrk3);
	  kmfit->AddTrack(3, wvpiTrk4);
	  kmfit->AddTrack(4, wvpiTrk5);
	  kmfit->AddTrack(5, wvpiTrk6);
	  kmfit->AddFourMomentum(0, ecms);
	  
	  bool oksq1 = kmfit->Fit();
	  chi1 = kmfit->chisq();
	  if(oksq1){
	    chimin=chi1;
	    type=1;
	  }
	  
	}
   
   
	{
	  wvpiTrk1 = WTrackParameter(mproton, trk1->getZHelix(), trk1->getZError());
	  wvpiTrk2 = WTrackParameter(mpi, trk2->getZHelix(), trk2->getZError());
	  wvpiTrk3 = WTrackParameter(mpi, trk3->getZHelix(), trk3->getZError());
	  wvpiTrk4 = WTrackParameter(mproton, trk4->getZHelix(), trk4->getZError());
	  wvpiTrk5 = WTrackParameter(mpi, pitrk1->getZHelix(), pitrk1->getZError());
	  wvpiTrk6 = WTrackParameter(mpi, pitrk2->getZHelix(), pitrk2->getZError());
     


	  KinematicFit * kmfit = KinematicFit::instance();
	  kmfit->init();
	  kmfit->AddTrack(0, wvpiTrk1);
	  kmfit->AddTrack(1, wvpiTrk2);
	  kmfit->AddTrack(2, wvpiTrk3);
	  kmfit->AddTrack(3, wvpiTrk4);
	  kmfit->AddTrack(4, wvpiTrk5);
	  kmfit->AddTrack(5, wvpiTrk6);
          kmfit->AddFourMomentum(0, ecms);
	  
	  bool oksq1 = kmfit->Fit();
	  chi2 = kmfit->chisq();
	  if(oksq1){
	    if(type==0){
	      chimin=chi2;
	      type=2;
	    }
	    else if(chi2<chimin){
	      chimin=chi2;
	      type=2;
	    }

	  }
	  //
	}
	
	{
	  wvpiTrk1 = WTrackParameter(mpi, trk1->getZHelix(), trk1->getZError());
	  wvpiTrk2 = WTrackParameter(mproton, trk2->getZHelix(), trk2->getZError());
	  wvpiTrk3 = WTrackParameter(mpi, trk3->getZHelix(), trk3->getZError());
	  wvpiTrk4 = WTrackParameter(mproton, trk4->getZHelix(), trk4->getZError());
	  wvpiTrk5 = WTrackParameter(mpi, pitrk1->getZHelix(), pitrk1->getZError());
	  wvpiTrk6 = WTrackParameter(mpi, pitrk2->getZHelix(), pitrk2->getZError());
	  
	  
	  
	  KinematicFit * kmfit = KinematicFit::instance();
	  kmfit->init();
	  kmfit->AddTrack(0, wvpiTrk1);
	  kmfit->AddTrack(1, wvpiTrk2);
	  kmfit->AddTrack(2, wvpiTrk3);
	  kmfit->AddTrack(3, wvpiTrk4);
	  kmfit->AddTrack(4, wvpiTrk5);
	  kmfit->AddTrack(5, wvpiTrk6);
          kmfit->AddFourMomentum(0, ecms);
	  
	  bool oksq1 = kmfit->Fit();
	  chi3 = kmfit->chisq();
	  if(oksq1){
	    if(type==0){
	      chimin=chi3;
	      type=3;
	    }
	    else if(chi3<chimin){
	      chimin=chi3;
	      type=3;
	    }
	  }
	  //delete kmfit;
	}
	
	{
	  wvpiTrk1 = WTrackParameter(mpi, trk1->getZHelix(), trk1->getZError());
	  wvpiTrk2 = WTrackParameter(mproton, trk2->getZHelix(), trk2->getZError());
	  wvpiTrk3 = WTrackParameter(mproton, trk3->getZHelix(), trk3->getZError());
	  wvpiTrk4 = WTrackParameter(mpi, trk4->getZHelix(), trk4->getZError());
	  wvpiTrk5 = WTrackParameter(mpi, pitrk1->getZHelix(), pitrk1->getZError());
	  wvpiTrk6 = WTrackParameter(mpi, pitrk2->getZHelix(), pitrk2->getZError());
	  
	  
	  KinematicFit * kmfit = KinematicFit::instance();
	  kmfit->init();
	  kmfit->AddTrack(0, wvpiTrk1);
	  kmfit->AddTrack(1, wvpiTrk2);
	  kmfit->AddTrack(2, wvpiTrk3);
	  kmfit->AddTrack(3, wvpiTrk4);
	  kmfit->AddTrack(4, wvpiTrk5);
	  kmfit->AddTrack(5, wvpiTrk6);
	  kmfit->AddFourMomentum(0, ecms);
	  
	  bool oksq1 = kmfit->Fit();
	  chi4 = kmfit->chisq();
	  if(oksq1){
	    if(type==0){
	      chimin=chi4;
	      type=4;
	    }
	    else if(chi4<chimin){
	      chimin=chi4;
	      type=4;
	    }
	  }

	}
	
   
	if(chimin>0 && chimin<200){
	  m_isPsipPPPiPi = true;
	  m_psipPPPiPiNumber++;
	}

      }//end of loose cuts
      
    }//end of selecting recol jpsi to pppipi
    


        
  }//end of recoil jpsi selection



  //select psi'' events using dtaging
  
  if(m_selectPsippCand){
    
    SmartDataPtr<EvtRecDTagCol> evtRecDTagCol(eventSvc(), EventModel::EvtRec::EvtRecDTagCol);
    if ( ! evtRecDTagCol ) {
      log << MSG::FATAL << "Could not find EvtRecDTagCol" << endreq;
      return StatusCode::FAILURE;
    }
    if(evtRecDTagCol->size()>0){
      
      EvtRecDTagCol::iterator m_iterbegin=evtRecDTagCol->begin();
      EvtRecDTagCol::iterator m_iterend=evtRecDTagCol->end();
      for(EvtRecDTagCol::iterator iter=m_iterbegin; iter != m_iterend; iter++){
	
	if( ((*iter)->decayMode()==EvtRecDTag::kD0toKPiPi0 && fabs((*iter)->mBC()-1.865)<0.006 && (*iter)->deltaE()>-0.05 && (*iter)->deltaE()<0.03)  || 
	    ((*iter)->decayMode()==EvtRecDTag::kD0toKPiPi0Pi0 && fabs((*iter)->mBC()-1.865)<0.006 && (*iter)->deltaE()>-0.05 && (*iter)->deltaE()<0.03)  || 
	    ((*iter)->decayMode()==EvtRecDTag::kD0toKPiPiPi && fabs((*iter)->mBC()-1.865)<0.006 && (*iter)->deltaE()>-0.03 && (*iter)->deltaE()<0.03) ||
	    ((*iter)->decayMode()==EvtRecDTag::kD0toKPiPiPiPi0 && fabs((*iter)->mBC()-1.865)<0.006 && (*iter)->deltaE()>-0.05 && (*iter)->deltaE()<0.03) ||
	    ((*iter)->decayMode()==EvtRecDTag::kD0toKsPiPi && fabs((*iter)->mBC()-1.865)<0.006 && (*iter)->deltaE()>-0.03 && (*iter)->deltaE()<0.03) ||
	    ((*iter)->decayMode()==EvtRecDTag::kD0toKsPiPiPi0 && fabs((*iter)->mBC()-1.865)<0.006 && (*iter)->deltaE()>-0.05 && (*iter)->deltaE()<0.03)|| 
	    ((*iter)->decayMode()==EvtRecDTag::kDptoKPiPi && fabs((*iter)->mBC()-1.87)<0.006 && (*iter)->deltaE()>-0.03 && (*iter)->deltaE()<0.03)  || 
	    ((*iter)->decayMode()==EvtRecDTag::kDptoKPiPiPi0 && fabs((*iter)->mBC()-1.87)<0.006 && (*iter)->deltaE()>-0.05 && (*iter)->deltaE()<0.03)|| 
	    ((*iter)->decayMode()==EvtRecDTag::kDptoKsPiPi0 && fabs((*iter)->mBC()-1.87)<0.006 && (*iter)->deltaE()>-0.05 && (*iter)->deltaE()<0.03) || 
	    ((*iter)->decayMode()==EvtRecDTag::kDptoKsPiPiPi&& fabs((*iter)->mBC()-1.87)<0.006 && (*iter)->deltaE()>-0.03 && (*iter)->deltaE()<0.03) ) {
	  m_isPsippCand = true;
	  m_psippCandNumber++;
	}

	  
      }//end of looping D modes


    }//end of non-zero dtag list
    
  }//end of selecting psi'' events
  
  // -------- Write to root file
  
  if( m_selectRadBhabha && m_isRadBhabha ) m_subalg3->execute();
  if( m_selectGGEE && m_isGGEE ) m_subalg4->execute();
  if( m_selectGG4Pi && m_isGG4Pi ) m_subalg5->execute();
  if( m_selectRadBhabhaT && m_isRadBhabhaT ) m_subalg6->execute();
  if( m_selectRhoPi && m_isRhoPi ) m_subalg7->execute();
  if( m_selectKstarK && m_isKstarK ) m_subalg8->execute();
  if( m_selectPPPiPi && m_isPPPiPi ) m_subalg9->execute();
  if( m_selectRecoJpsi && m_isRecoJpsi ) m_subalg10->execute();
  if( m_selectBhabha && m_isBhabha ) m_subalg11->execute();
  if( m_selectDimu && m_isDimu ) m_subalg12->execute();
  if( m_selectCosmic && m_isCosmic ) m_subalg13->execute();
  if( m_selectGenProton && m_isGenProton ) m_subalg14->execute();
  if( m_selectPsipRhoPi && m_isPsipRhoPi ) m_subalg15->execute();
  if( m_selectPsipKstarK && m_isPsipKstarK ) m_subalg16->execute();
  if( m_selectPsipPPPiPi && m_isPsipPPPiPi ) m_subalg17->execute();
  if( m_selectPsippCand && m_isPsippCand ) m_subalg18->execute();


 //if(m_output) {
 //  m_runnb = run;
 //  m_evtnb = event;
 //  m_esum = esum;
 //  m_eemc = eemc;
 //  m_etot = etot;
 //  m_nCharge = nCharge;
 //  m_nGood = nGood;
 //  m_nGam = nGam;
 //  m_ptot = ptot;
 //  m_pp = pp;
 //  m_pm = pm;
 //  m_maxE = maxE;
 //  m_secE = secE;
 //  m_dThe = dthe;
 //  m_dPhi = dphi;
 //  m_mdcHit1 = mdcHit1;
 //  m_mdcHit2 = mdcHit2;
 //  m_tuple0->write();
 //}

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode CalibEventSelect::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  cout<<"Total events: "<<m_events<<endl;


  if(m_selectRadBhabha) {
    cout << "Selected rad bhabha: "     << m_radBhabhaNumber   << endl;
  }


  if(m_selectGGEE) {
    cout << "Selected ggee events: "     << m_GGEENumber   << endl;
  }
 
  if(m_selectGG4Pi) {
    cout << "Selected gg4pi events: "     << m_GG4PiNumber   << endl;
  }
  
  if(m_selectRadBhabhaT) {
    cout << "Selected rad bhabha tight: "     << m_radBhabhaTNumber   << endl;
  }

  if(m_selectRhoPi) {
    cout << "Selected rhopi events: "     << m_rhoPiNumber   << endl;
  }

  if(m_selectKstarK) {
    cout << "Selected kstark events: "     << m_kstarKNumber   << endl;
  }

  if(m_selectPPPiPi) {
    cout << "Selected pppipi events: "     << m_ppPiPiNumber   << endl;
  }

  if(m_selectRecoJpsi) {
    cout << "Selected recoil jsi events: "     << m_recoJpsiNumber   << endl;
  }

  
  if(m_selectBhabha) {
    cout << "Selected bhabha events: "     << m_bhabhaNumber   << endl;
  }


  if(m_selectDimu) {
    cout << "Selected dimu events: "     << m_dimuNumber   << endl;
  }


  if(m_selectCosmic) {
    cout << "Selected cosmic events: "     << m_cosmicNumber   << endl;
  }

  if(m_selectGenProton) {
    cout << "Selected generic proton events: "     << m_genProtonNumber   << endl;
  }

  if(m_selectPsipRhoPi) {
    cout << "Selected recoil rhopi events: "     << m_psipRhoPiNumber   << endl;
  }

  if(m_selectPsipKstarK) {
    cout << "Selected recoil kstark events: "     << m_psipKstarKNumber   << endl;
  }

  if(m_selectPsipPPPiPi) {
    cout << "Selected recoil pppipi events: "     << m_psipPPPiPiNumber   << endl;
  }

 if(m_selectPsippCand) {
    cout << "Selected psi'' candi events: "     << m_psippCandNumber   << endl;
  }

  return StatusCode::SUCCESS;
}

bool CalibEventSelect::WhetherSector(double ph,double ph1,double ph2) {
  double phi1=min(ph1,ph2);
  double phi2=max(ph1,ph2);
  double delta=0.0610865; //3.5*3.1415926/180.
  if((phi2-phi1)<CLHEP::pi){
    phi1 -=delta;
    phi2 +=delta;
    if(phi1<0.) phi1 += CLHEP::twopi;
    if(phi2>CLHEP::twopi) phi2 -= CLHEP::twopi;
    double tmp1=min(phi1,phi2);
    double tmp2=max(phi1,phi2);
    phi1=tmp1;
    phi2=tmp2;
  }
  else{
    phi1 +=delta;
    phi2 -=delta;
  }
  if((phi2-phi1)<CLHEP::pi){
    if(ph<=phi2&&ph>=phi1) return true;
    else   return false;
  }
  else{
    if(ph>=phi2||ph<=phi1) return true;
    else   return false;
  }
}


 //*************************************************************************
void CalibEventSelect::readbeamEfromDb(int runNo, double & beamE){

  const char host[]     = "202.122.37.69";
  const char user[]     = "guest";
  const char passwd[]   = "guestpass";
  const char db[]       = "run";
  unsigned int port_num = 3306;


  MYSQL* mysql = mysql_init(NULL);
  mysql = mysql_real_connect(mysql, host, user, passwd, db, port_num,
                             NULL,  // socket
                             0);    // client_flag

  if (mysql == NULL) {
    fprintf(stderr, "can not open database: RunInfo for run %d lum\n", runNo);
  }

  char stmt[1024];
  
  snprintf(stmt, 1024,
           "select BER_PRB, BPR_PRB "
	   "from RunParams where run_number = %d", runNo);
  if (mysql_real_query(mysql, stmt, strlen(stmt))) {
    fprintf(stderr, "query error\n");
  }

  MYSQL_RES* result_set = mysql_store_result(mysql);
  MYSQL_ROW row = mysql_fetch_row(result_set);
  if (!row) {
    fprintf(stderr, "cannot find data for RunNo %d\n", runNo);
    return ;
  }
  
  double E_E=0, E_P=0;
  sscanf(row[0], "%lf", &E_E);
  sscanf(row[1], "%lf", &E_P);
  
  beamE=(E_E+E_P)/2.0;
} 
