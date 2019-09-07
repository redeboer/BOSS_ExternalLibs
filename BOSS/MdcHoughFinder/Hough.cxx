#include "MdcHoughFinder/Hough.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/NTuple.h"                                        
#include "GaudiKernel/INTupleSvc.h"
#include "EventModel/EventHeader.h"
#include "MdcRawEvent/MdcDigi.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

#include <iostream>
#include <math.h>

#include "EvTimeEvent/RecEsTime.h"
#include "MdcGeom/EntranceAngle.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcData/MdcHit.h"

#include "McTruth/DecayMode.h"
#include "McTruth/McParticle.h"
#include "TrackUtil/Helix.h"
#include "MdcRecoUtil/Pdt.h"

#include "TrkBase/TrkFit.h"
#include "TrkBase/TrkHitList.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkFitter/TrkHelixMaker.h"
#include "TrkFitter/TrkCircleMaker.h"
#include "TrkFitter/TrkLineMaker.h"
#include "TrkFitter/TrkHelixFitter.h"
#include "MdcxReco/Mdcxprobab.h"
#include "MdcData/MdcRecoHitOnTrack.h"
#include "MdcPrintSvc/IMdcPrintSvc.h"

#include "MdcHoughFinder/HoughTrackList.h"
#include "MdcHoughFinder/HoughTrack.h"
#include <math.h>

#include "EvTimeEvent/RecEsTime.h"
//#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "McTruth/MdcMcHit.h" 
#include "TrkBase/TrkFit.h"
#include "TrkBase/TrkHitList.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkFitter/TrkHelixMaker.h"
#include "TrkFitter/TrkCircleMaker.h"
#include "TrkFitter/TrkLineMaker.h"
#include "TrkFitter/TrkHelixFitter.h"
#include "MdcPrintSvc/IMdcPrintSvc.h"
#include "MdcGeom/EntranceAngle.h"
#include "TrackUtil/Helix.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "MdcRecoUtil/Pdt.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcData/MdcHit.h"
#include "MdcTrkRecon/MdcMap.h"

#include "TTree.h"
#include "TH2D.h"

#include "BesTimerSvc/IBesTimerSvc.h"                                                         
#include "BesTimerSvc/BesTimerSvc.h"                                                          
#include "BesTimerSvc/BesTimer.h"



using namespace std;
using namespace Event;

MdcHoughFinder::MdcHoughFinder(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
  declareProperty("debug",          m_debug = 0);
  declareProperty("debugMap",       m_debugMap = 0);
  declareProperty("debug2D",          m_debug2D = 0);
  declareProperty("debugTrack",          m_debugTrack = 0);
  declareProperty("debugStereo",          m_debugStereo= 0);
  declareProperty("debugZs",          m_debugZs= 0);
  declareProperty("debug3D",          m_debug3D= 0);
  declareProperty("debugArbHit",          m_debugArbHit= 0);
  declareProperty("hist",           m_hist  = 1);
  declareProperty("filter",           m_filter= 0);
  //read raw data setup
  declareProperty("keepBadTdc",     m_keepBadTdc = 0);
  declareProperty("dropHot",        m_dropHot= 0);
  declareProperty("keepUnmatch",    m_keepUnmatch = 0);
  // combine pattsf
  declareProperty("combineTracking",m_combineTracking = false);
  declareProperty("removeBadTrack", m_removeBadTrack = 0);
  declareProperty("dropTrkDrCut",   m_dropTrkDrCut= 1.);
  declareProperty("dropTrkDzCut",   m_dropTrkDzCut= 10.);
  declareProperty("dropTrkPtCut",   m_dropTrkPtCut= 0.12);
  declareProperty("dropTrkChi2Cut", m_dropTrkChi2Cut = 10000.);
  //input setup
  declareProperty("inputType",      m_inputType = 0);
  //set MdcHoughFinder map 
  declareProperty("mapCharge",      m_mapCharge= -1);    //0 use whole ; 1 only half 
  declareProperty("useHalf",        m_useHalf= 0);    //0 use whole ; 1 only half 
  declareProperty("mapHitStyle",    m_mapHit= 0);    //0 : all ; 1 :axial
  declareProperty("nbinTheta",      m_nBinTheta = 100);
  declareProperty("nbinRho",        m_nBinRho = 100);
  declareProperty("rhoRange",       m_rhoRange = 0.1);
  declareProperty("peakWidth",      m_peakWidth= 3);
  declareProperty("peakHigh",       m_peakHigh= 1);
  declareProperty("hitPro",         m_hitPro= 0.4);

  declareProperty("recpos",         m_recpos= 1);
  declareProperty("recneg",         m_recneg= 1);
  declareProperty("combineSelf",    m_combine= 1);
  declareProperty("z0CutCompareHough",    m_z0Cut_compareHough= 10);

  //split drift circle
  declareProperty("n1",          	m_npart= 100);
  declareProperty("n2",          	m_n2= 16);

  declareProperty("dsigma",         m_dsigma= 1);
  declareProperty("dsigma2",         m_dsigma2= 2);

  declareProperty("pdtFile",        m_pdtFile = "pdt.table");
  declareProperty("sigmaFile",      m_sigmaFile = "sigma.table");
  declareProperty("eventFile",      m_evtFile= "EventList");

}
//
StatusCode MdcHoughFinder::beginRun(){
  //Initailize MdcDetector
  Global::m_gm = MdcDetector::instance(0);
  if(NULL == Global::m_gm) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcHoughFinder::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  StatusCode sc;

  //particle
  IPartPropSvc* p_PartPropSvc;
  static const bool CREATEIFNOTTHERE(true);
  sc = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!sc.isSuccess() || 0 == p_PartPropSvc) {
	log << MSG::ERROR << " Could not initialize PartPropSvc" << endreq;
	return sc;
  }
  m_particleTable = p_PartPropSvc->PDT();

  // RawData
  IRawDataProviderSvc* irawDataProviderSvc;
  sc = service ("RawDataProviderSvc", irawDataProviderSvc);
  m_rawDataProviderSvc = dynamic_cast<RawDataProviderSvc*> (irawDataProviderSvc);
  if ( sc.isFailure() ){
	log << MSG::FATAL << name()<<" Could not load RawDataProviderSvc!" << endreq;
	return StatusCode::FAILURE;
  }

  //  Geometry
  IMdcGeomSvc*	 imdcGeomSvc;
  sc = service ("MdcGeomSvc", imdcGeomSvc);
  m_mdcGeomSvc = dynamic_cast<MdcGeomSvc*> (imdcGeomSvc);
  if ( sc.isFailure() ){
	log << MSG::FATAL << "Could not load MdcGeoSvc!" << endreq;
	return StatusCode::FAILURE;
  }

  //  magneticfield
  sc = service ("MagneticFieldSvc",m_pIMF);
  if(sc!=StatusCode::SUCCESS) {
	log << MSG::ERROR << "Unable to open Magnetic field service"<<endreq;
  }
  m_bfield = new BField(m_pIMF);
  log << MSG::INFO << "field z = "<<m_bfield->bFieldNominal()<< endreq;
  m_context = new TrkContextEv(m_bfield);

  //read pdt
  Pdt::readMCppTable(m_pdtFile);

  //Get MdcCalibFunSvc
  IMdcCalibFunSvc* imdcCalibSvc; 
  sc = service ("MdcCalibFunSvc", imdcCalibSvc);
  m_mdcCalibFunSvc = dynamic_cast<MdcCalibFunSvc*>(imdcCalibSvc);
  if ( sc.isFailure() ){
	log << MSG::FATAL << "Could not load MdcCalibFunSvc!" << endreq;
	return StatusCode::FAILURE;
  }

  //initialize MdcPrintSvc
  IMdcPrintSvc* imdcPrintSvc;
  sc = service ("MdcPrintSvc", imdcPrintSvc);
  m_mdcPrintSvc = dynamic_cast<MdcPrintSvc*> (imdcPrintSvc);
  if ( sc.isFailure() ){
	log << MSG::FATAL << "Could not load MdcPrintSvc!" << endreq;
	return StatusCode::FAILURE;
  }

  //time 
  sc = service( "BesTimerSvc", m_timersvc);
  if( sc.isFailure() ) {
	log << MSG::WARNING << " Unable to locate BesTimerSvc" << endreq;
	return StatusCode::FAILURE;
  }
  m_timer_all = m_timersvc->addItem("Execution");
  m_timer_all->propName("nExecution");

  //initialize static 
  HoughHit::setMdcCalibFunSvc(m_mdcCalibFunSvc);
  HoughHit::setMdcGeomSvc(m_mdcGeomSvc);
  HoughHit::setBunchTime(m_bunchT0);
  Hough2D::setContext(m_context);
  Hough2D::setCalib(m_mdcCalibFunSvc);
  Hough3D::setContext(m_context);
  Hough3D::setCalib(m_mdcCalibFunSvc);

  HoughHit::_npart=m_npart;
  HoughMap::m_useHalfCir=m_useHalf;
  HoughMap::m_N1=m_npart;
  HoughMap::m_N2=m_n2;

  HoughPeak::m_dSigma_cut=m_dsigma;
  HoughPeak::m_dSigma_cut2=m_dsigma2;
  HoughPeak::m_file=m_sigmaFile;

  if(m_debugMap> 0)   HoughMap::m_debug = 1;
  if(m_debug2D > 0)   Hough2D::m_debug = 1;
  if(m_debug3D > 0)   Hough3D::m_debug = 1;
  if(m_debugTrack> 0) HoughTrack::m_debug = 1;
  if(m_debugStereo> 0) HoughStereo::m_debug = 1;
  if(m_debugZs> 0)    HoughZsFit::m_debug = 1;
  if(m_debug3D > 4)   TrkHelixFitter::m_debug = 1;

  if(m_hist) sc = bookTuple();
  if ( sc.isFailure() ){
	log << MSG::FATAL << "Could not book Tuple !" << endreq;
	return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcHoughFinder::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;
  cout.precision(6);


  m_timer_all->start();

  //event start time
  SmartDataPtr<RecEsTimeCol> evTimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
  if (!evTimeCol) {
	log << MSG::WARNING << "Could not find EvTimeCol" << endreq;
	return StatusCode::SUCCESS;
  }else{
	RecEsTimeCol::iterator iter_evt = evTimeCol->begin();
	if (iter_evt != evTimeCol->end()){
	  m_bunchT0 = (*iter_evt)->getTest()*1.e-9;//m_bunchT0-s, getTest-ns
	}
  }
  HoughHit::setBunchTime(m_bunchT0);

  //-- Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
	log << MSG::FATAL << "Could not find Event Header" << endreq;
	return StatusCode::FAILURE;
  }

  t_eventNum=eventHeader->eventNumber();
  t_runNum=eventHeader->runNumber();
  if(m_hist){
	m_eventNum=eventHeader->eventNumber();
	m_runNum=eventHeader->runNumber();
  }
  if(m_debug>0) cout<<"begin evt "<<eventHeader->eventNumber()<<endl;

  //prepare tds 
  RecMdcTrackCol* trackList_tds;
  RecMdcHitCol* hitList_tds;
  vector<RecMdcTrack*> vec_trackPatTds;
  int nTdsTk = storeTracks(trackList_tds,hitList_tds,vec_trackPatTds);
  //print track in pattsf with bad vertex
  if(m_debug>0){
	RecMdcTrackCol::iterator iteritrk_pattsf = vec_trackPatTds.begin();
	for(;iteritrk_pattsf!=vec_trackPatTds.end();iteritrk_pattsf++){
	  cout<<"in PATTSF LOST: "<<(*iteritrk_pattsf)->helix(0)<<" "<<(*iteritrk_pattsf)->helix(1)<<" "<<(*iteritrk_pattsf)->helix(2)<<" "<<(*iteritrk_pattsf)->helix(3)<<" "<<(*iteritrk_pattsf)->helix(4)<<" chi2 "<<(*iteritrk_pattsf)->chi2()<<endl;
	}
  }

  //for arbi hits
  MdcTrackParams  m_par;
  if(m_debugArbHit>0 ) m_par.lPrint=8;
  m_par.lRemoveInActive=1;
  //m_par.lUseQualCuts=0;
  //m_par.maxGapLength=99;
  //m_par.maxChisq=99;
  //m_par.minHits=99;

  // if filter read eventNum in file
  if(m_filter){
	ifstream lowPt_Evt;
	lowPt_Evt.open(m_evtFile.c_str());
	vector<int> evtlist;
	int evtNum;
	while( lowPt_Evt >> evtNum) {
	  evtlist.push_back(evtNum);
	}
	vector<int>::iterator iter_evt = find(evtlist.begin(),evtlist.end(),t_eventNum);
	if( iter_evt == evtlist.end() )  { setFilterPassed(false); return StatusCode::SUCCESS; }
	else setFilterPassed(true);
  }

  if(m_inputType == -1) GetMcInfo();

  //-- Get MDC digi vector
  if(m_debug>0) cout<<"step1 :  prepare digi "<<endl;
  MdcDigiVec mdcDigiVec = prepareDigi();

  //-- Create MdcHoughFinder hit list
  bool debugTruth = false;
  if(m_inputType == -1) debugTruth= true;
  if(m_debug>0) cout<<"step2 : hits-> hough hit list "<<endl;
  HoughHitList houghHitList(mdcDigiVec);
  //  if( mdcDigiVec.size() < 10 ) return StatusCode::SUCCESS;
  if( houghHitList.nHit() < 10 || houghHitList.nHit()>500 ) return StatusCode::SUCCESS;
  if(m_debug>0) houghHitList.printAll();
  if(debugTruth) houghHitList.addTruthInfo(g_tkParTruth);

  vector<MdcHit*>  mdcHitCol_neg;
  vector<MdcHit*>  mdcHitCol_pos;
  MdcDigiVec::iterator iter = mdcDigiVec.begin();
  for (;iter != mdcDigiVec.end(); iter++) {
	const MdcDigi* digi = (*iter);
	if( HoughHit(digi).driftTime()>1000 || HoughHit(digi).driftTime()<=0 ) continue;
	MdcHit *mdcHit= new MdcHit(digi, Global::m_gm);
	MdcHit *mdcHit_pos= new MdcHit(digi, Global::m_gm);
	mdcHitCol_neg.push_back(mdcHit);
	mdcHitCol_pos.push_back(mdcHit_pos);
  }

  HoughMap *m_map = new HoughMap(-1,houghHitList,m_mapHit,m_nBinTheta,m_nBinRho,-1.*m_rhoRange,m_rhoRange,m_peakWidth,m_peakHigh,m_hitPro);//, 2dOr3d);

  //  if(m_hist) dumpHoughHitList(houghHitList);
  if(m_hist) m_nHit = houghHitList.nHit();
  if(m_hist) dumpHoughMap(*m_map);

  //track
  if(m_debug>0) cout<<"step3 : neg track list "<<endl;
  vector<HoughTrack*> vec_track_neg; 
  vector<HoughTrack*> vec_track2D_neg; 
  MdcTrackList  mdcTrackList_neg(m_par) ;
  if(m_recneg){
	HoughTrackList trackList_neg(*m_map);
	int trackList_size = trackList_neg.getTrackNum();
	vector< vector<int> > stat_2d(2,vector<int>(trackList_size,0) );
	vector< vector<int> > stat_3d(2,vector<int>(trackList_size,0) );
	int ifit=0;
	int ifit3D=0;
	for(int itrack=0;itrack<trackList_size;itrack++){
	  if(m_debug>0) cout<<"begin track: "<<itrack<<endl;
	  //suppose charge -
	  if(m_debug>0) cout<<" suppose charge -1 "<<endl;
	  HoughTrack &track_neg = trackList_neg.getTrack(itrack);
	  track_neg.setMdcHit( &mdcHitCol_neg);
	  track_neg.setHoughHitList(houghHitList.getList());
	  track_neg.setCharge(-1);
	  stat_2d[0][itrack] = track_neg.fit2D(m_bunchT0);
	  int track_charge_2d = track_neg.trackCharge2D();
	  if(m_debug>0) cout<<" charge -1 stat2d "<<stat_2d[0][itrack]<<" "<<track_charge_2d<<endl; 
	  if( stat_2d[0][itrack] == 0 || track_charge_2d ==0  )  continue;
	  //vec_track2D_neg.push_back( &track_neg );
	  //fill 2D inf
	  ifit++;
	  //3D fit
	  int nHit3d = track_neg.find_stereo_hit();
	  int npair= track_neg.find_pair_hit();
	  //cout<<"npair "<<npair<<endl;

	  int track_charge_3d = track_neg.trackCharge3D();
	  if(m_debug>0) cout<<" nhitstereo -1 "<<nHit3d<<" "<<track_charge_3d<<endl; 
	  if( nHit3d <3 || track_charge_3d==0 )  continue;

	  //choose fit method
	  if( npair==0 ) stat_3d[0][itrack] = track_neg.fit3D();  
	  else stat_3d[0][itrack] = track_neg.fit3D_inner();  
	  //else stat_3d[0][itrack] = track_neg.fit3D();  

	  if(m_debug>0) cout<<" charge -1 stat3d "<<stat_3d[0][itrack]<<" "<<endl;
	  if( stat_3d[0][itrack]==0 ) continue;
	  vec_track_neg.push_back( &track_neg );
	  //fill 3D inf
	  /*
		 if(m_hist){
		 m_tanl_neg[ifit3D] = track_neg.getTanl_zs(); 
		 m_tanl3D_neg[ifit3D] = track_neg.getTanl(); 
		 m_z0_neg[ifit3D] = track_neg.getZ0_zs(); 
		 m_z0_3D_neg[ifit3D] = track_neg.getZ0(); 
		 m_pro_neg[ifit3D] = track_neg.getPro(); 
		 m_pt3D_neg[ifit3D] = track_neg.getPt3D(); 
		 m_nHit3D_neg[ifit3D] = track_neg.getNfit3D(); 
		 m_chi2_3D_neg[ifit3D] = track_neg.getChi2_3D(); 
		 }
		 ifit3D++;
		 int axial_use=0;
		 int stereo_use=0;
		 int cir0=0;
		 int cirmore=0;
		 int ncir_track;
		 for(int ister =0;ister<track_neg.getHoughHitList().size();ister++){
		 HoughRecHit *rechit = &(track_neg.getHoughHitList().at(ister));
		 if( rechit->getSlayerType()==0 ) {
		 double deltaD  = rechit->getDeltaD();
		 double flt= rechit->getFltLen();
		 if(m_hist){
		 m_axial_layer[axial_use]=rechit->getLayerId();
		 m_axial_wire[axial_use]=rechit->getWireId();
		 m_axial_deltaD[axial_use] = deltaD;
		 m_axial_flt[axial_use] = flt;
		 }
	  //judge which circle
	  int cirlist = rechit->getCirList();
	  if(cirlist == 0)  cir0++;
	  else cirmore++;
	  axial_use++;
	  }
	  if( rechit->getSlayerType()!=0 ) {
	  //stereo hit info
	  double z0= rechit->getzAmb(0);
	  double z1= rechit->getzAmb(1);
	  double s0= rechit->getsAmb(0);
	  double s1= rechit->getsAmb(1);
	  int ambig = rechit->getAmbig();
	  int ambigTruth = rechit->getLrTruth();
	  int style = rechit->getStyle();
	  //cout<<"ambig ambigTruth "<<ambig<<" "<<ambigTruth<<endl;
	  double z = rechit->getzPos();
	  double s = rechit->getsPos();
	  double zTruth;
	  double sTruth;
	  if(ambigTruth == 1 ) {zTruth = z0;sTruth=s0;}
	  else if(ambigTruth == -1 ) {zTruth = z1;sTruth=s1;}
	  //calcu dist of hitz to zsline
	  double deltaZ = zTruth - ( sTruth * track_neg.getTanl_zs()+track_neg.getZ0_zs());
	  //else cout<<" not get ambig truth info "<<endl;
	  int nsol = rechit->getnsol();
	  double disToCir= rechit->getDisToCir();
	  int multicir = rechit->getCirList();
	  if(m_hist){
	  m_stereo_layer[stereo_use]=rechit->getLayerId();
	  m_stereo_wire[stereo_use]=rechit->getWireId();
	  m_stereo_style[stereo_use]=style;
	  m_stereo_z0[stereo_use]=z0;
	  m_stereo_z1[stereo_use]=z1;
	  m_stereo_s0[stereo_use]=s0;
	  m_stereo_s1[stereo_use]=s1;
	  m_stereo_z[stereo_use]=z;
	  m_stereo_s[stereo_use]=s;
	  m_stereo_zTruth[stereo_use]=zTruth;
	  m_stereo_sTruth[stereo_use]=sTruth;
	  m_stereo_deltaZ[stereo_use]=deltaZ;
	  m_stereo_nsol[stereo_use]=nsol;
	  m_stereo_ambig[stereo_use]=ambig;
	  m_stereo_ambig_truth[stereo_use]=ambigTruth;
	  m_stereo_disToCir[stereo_use]=disToCir;
	  m_stereo_cirlist[stereo_use]=multicir;
	}
	stereo_use++;
	}
	}
	if( cir0>cirmore ) ncir_track = 0;
	else ncir_track = 1;
	if(m_hist){
	  m_evt_stereo= t_eventNum;
	  m_run_stereo= t_runNum;
	  m_cos_stereo= t_cos;
	  m_tanlTruth_stereo= t_tanl;
	  m_ncir_stereo= ncir_track;
	  m_npair_stereo= npair;
	  m_tanl_stereo= track_neg.getTanl_zs();
	  m_z0_stereo= track_neg.getZ0_zs();
	  m_tanl3D_stereo= track_neg.getTanl();
	  m_z03D_stereo= track_neg.getZ0();
	  m_nHit_axial= axial_use;
	  m_nHit_stereo = stereo_use;
	  ntuplehit->write();
	}
	*/
	}

	std::sort ( vec_track_neg.begin(),vec_track_neg.end(),more_pt); 
	//track for clear
	vector<MdcTrack*> vec_MdcTrack_neg;
	for( unsigned int i =0;i<vec_track_neg.size();i++){
	  MdcTrack *mdcTrack = new MdcTrack(vec_track_neg[i]->getTrk());
	  vec_MdcTrack_neg.push_back(mdcTrack);
	  if(m_debug>0) cout<<"trackneg: "<<i<<" pt "<<vec_track_neg[i]->getPt3D()<<endl;
	  if(m_debug>0) vec_track_neg[i]->print();
	}
	if(m_debug>0) cout<<"step4 : judge neg track list "<<endl;
	judgeHit(mdcTrackList_neg,vec_MdcTrack_neg);
	if(m_debug>0) cout<<"finish - charge "<<endl;
  }

  //do rec pos charge

  HoughMap *m_map2 = new HoughMap(1,houghHitList,m_mapHit,m_nBinTheta,m_nBinRho,-1.*m_rhoRange,m_rhoRange,m_peakWidth,m_peakHigh,m_hitPro);//, 2dOr3d);
  if(m_debug>0) cout<<"step5 : pos track list "<<endl;
  vector<HoughTrack*> vec_track_pos; 
  MdcTrackList  mdcTrackList_pos(m_par) ;
  if(m_recpos){
	HoughTrackList tracklist_pos(*m_map2);
	int tracklist_size2 = tracklist_pos.getTrackNum();
	vector< vector<int> > stat_2d2(2,vector<int>(tracklist_size2,0) );
	vector< vector<int> > stat_3d2(2,vector<int>(tracklist_size2,0) );
	int ifit=0;
	int ifit3D=0;
	for(int itrack=0;itrack<tracklist_size2;itrack++){
	  //suppose charge +
	  if(m_debug>0) cout<<" suppose charge +1 "<<endl;
	  HoughTrack &track_pos = tracklist_pos.getTrack(itrack);
	  track_pos.setMdcHit( &mdcHitCol_pos);
	  track_pos.setHoughHitList(houghHitList.getList());
	  track_pos.setCharge(1);
	  stat_2d2[0][itrack] = track_pos.fit2D(m_bunchT0);
	  int track_charge_2d = track_pos.trackCharge2D();
	  if(m_debug>0) cout<<" charge +1 stat2d "<<stat_2d2[1][itrack]<<" "<<track_charge_2d<<endl; 
	  if( stat_2d2[0][itrack] == 0 || track_charge_2d==0 )  continue;
	  //fill 2d inf
	  ifit++;
	  //3d fit
	  int nHit3d = track_pos.find_stereo_hit();
	  int npair= track_pos.find_pair_hit();
	  //cout<<"npair "<<npair<<endl;

	  int track_charge_3d = track_pos.trackCharge3D();
	  if(m_debug>0) cout<<" nhitstereo +1 "<<nHit3d<<" "<<track_charge_3d<<endl; 
	  if( nHit3d <3 || track_pos.trackCharge3D()==0 )  continue;
	  //choose fit method
	  if( npair==0 ) stat_3d2[0][itrack] = track_pos.fit3D();  
	  else stat_3d2[0][itrack] = track_pos.fit3D_inner();  
	  //else stat_3d2[0][itrack] = track_neg.fit3D();  

	  if(m_debug>0) cout<<" charge +1 stat3d "<<stat_3d2[1][itrack]<<" "<<endl;
	  if( stat_3d2[0][itrack]==0 ) continue;
	  vec_track_pos.push_back( &track_pos );
	  //fill 3d inf
	  ifit3D++;
	}

	//sort pos tracklist
	std::sort ( vec_track_pos.begin(),vec_track_pos.end(),more_pt); 

	// clear pos track
	vector<MdcTrack*> vec_MdcTrack_pos;
	for( unsigned int i =0;i<vec_track_pos.size();i++){
	  MdcTrack *mdcTrack = new MdcTrack(vec_track_pos[i]->getTrk());
	  vec_MdcTrack_pos.push_back(mdcTrack);
	  if(m_debug>0) cout<<"trackpos : "<<i<<" pt "<<vec_track_pos[i]->getPt3D()<<endl;
	  if(m_debug>0) vec_track_pos[i]->print();
	}
	if(m_debug>0) cout<<"step6 : judge pos track list "<<endl;
	judgeHit(mdcTrackList_pos,vec_MdcTrack_pos);
  }

  //combine hough itself
  //  if(m_debug>0) cout<<"step7 : combine neg&pos track list "<<endl;
  if(m_combine){
	compareHough(mdcTrackList_neg);
	compareHough(mdcTrackList_pos);
  }
  if( mdcTrackList_neg.length()!=0 && mdcTrackList_pos.length()!=0 ) judgeChargeTrack(mdcTrackList_neg,mdcTrackList_pos);
  //add tracklist
  mdcTrackList_neg+=mdcTrackList_pos;   //neg -> all charge

  //compare pattsf&hough self
  if(m_combineTracking) nTdsTk = comparePatTsf(mdcTrackList_neg,trackList_tds);

  // store tds
  if(m_debug>0) cout<<"step8 : store tds  "<<endl;
  if(m_debug>0) cout<<"store tds "<<endl;
  int tkId = nTdsTk ;   //trkid
  for( unsigned int i =0;i<mdcTrackList_neg.length();i++){
	if(m_debug>0) cout<<"- charge size i : "<<i<<" "<<mdcTrackList_neg.length()<<endl;
	int tkStat = 4;//track find by houghspace set stat=4
	mdcTrackList_neg[i]->storeTrack(tkId, trackList_tds, hitList_tds, tkStat);
	tkId++;
	delete  mdcTrackList_neg[i];
  }
  if(m_debug>0)  m_mdcPrintSvc->printRecMdcTrackCol();

  m_timer_all->stop();
  double t_teventTime = m_timer_all->elapsed();
  if(m_hist) m_time = t_teventTime;

  if( m_hist )  ntuple_evt->write();

  delete m_map;
  delete m_map2;
  if(m_debug>0) cout<<"after delete map "<<endl;
  for(int ihit=0;ihit<mdcHitCol_neg.size();ihit++){
	delete mdcHitCol_neg[ihit];
	delete mdcHitCol_pos[ihit];
  }

  if(m_debug>0) cout<<"after delete hit"<<endl;
  //clearMem(mdcTrackList_neg,mdcTrackList_pos);
  if(m_debug>0) cout<<"end event "<<endl;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcHoughFinder::finalize() {
  MsgStream log(msgSvc(), name());
  delete m_bfield ;
  log << MSG::INFO<< "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}

int MdcHoughFinder::GetMcInfo(){
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
  if (!mcParticleCol) {
	log << MSG::ERROR << "Could not find McParticle" << endreq;
	return StatusCode::FAILURE;
  }


  g_tkParTruth.clear();//clear track param.

  //t_t0Truth=-1;
  int t_qTruth = 0;
  int t_pidTruth = -999;
  int nMcTk=0;
  McParticleCol::iterator iter_mc = mcParticleCol->begin();
  for (;iter_mc != mcParticleCol->end(); iter_mc++ ) {
	t_pidTruth = (*iter_mc)->particleProperty();
	//	cout<<" pid "<<t_pidTruth<<endl;
	if(!(*iter_mc)->primaryParticle()) continue;
	//if((m_pid!=0) && (t_pidTruth != m_pid)){ continue; }
	//t_t0Truth=(*iter_mc)->initialPosition().t();
	std::string name;
	int pid = t_pidTruth; 
	if( pid == 0 ) { 
	  log << MSG::WARNING << "Wrong particle id" <<endreq;
	  continue;
	}else{
	  IPartPropSvc* p_PartPropSvc;
	  static const bool CREATEIFNOTTHERE(true);
	  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
	  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
		std::cout<< " Could not initialize Particle Properties Service" << std::endl;
	  }
	  HepPDT::ParticleDataTable* p_particleTable = p_PartPropSvc->PDT(); 
	  if( p_particleTable->particle(pid) ){
		name = p_particleTable->particle(pid)->name();
		t_qTruth = p_particleTable->particle(pid)->charge();
	  }else if( p_particleTable->particle(-pid) ){
		name = "anti " + p_particleTable->particle(-pid)->name();
		t_qTruth = (-1)*p_particleTable->particle(-pid)->charge();
	  }
	}

	//helix
	if(m_debug>1) std::cout<<__FILE__<<"   "<<__LINE__<<" new helix with pos "<<(*iter_mc)->initialPosition().v()<<" mom "<<(*iter_mc)->initialFourMomentum().v()<<" q "<<t_qTruth<<std::endl;
	Helix mchelix = Helix((*iter_mc)->initialPosition().v(), (*iter_mc)->initialFourMomentum().v(), t_qTruth);//cm
	mchelix.pivot( HepPoint3D(0.,0.,0.) );

	int mcTkId = (*iter_mc)->trackIndex();

	pair<int, const HepVector> p(mcTkId,mchelix.a());
	g_tkParTruth.insert(p);
	// exchange to rho theta   test negtive charge
	double rho , theta;
	if( mchelix.phi0()>M_PI) { rho=-1./fabs(mchelix.radius()); theta = mchelix.phi0()-M_PI;} 
	else {rho =1./fabs(mchelix.radius()) ; theta = mchelix.phi0();} 
	t_d0=mchelix.dr();
	t_z0=mchelix.dz();
	t_pt=mchelix.pt();
	t_p=mchelix.momentum(0.).mag();
	t_tanl=mchelix.tanl();
	t_cos= mchelix.cosTheta();

	//m_pzTruth[nMcTk]=mchelix.momentum(0.).z();
	//m_pidTruth[nMcTk] = t_pidTruth;
	//m_costaTruth[nMcTk] = mchelix.cosTheta();
	//m_ptTruth[nMcTk] = mchelix.pt();
	//m_pTruth[nMcTk] = mchelix.momentum(0.).mag();
	//m_qTruth[nMcTk] = t_qTruth;
	//m_drTruth[nMcTk] = mchelix.dr();
	//m_phi0Truth[nMcTk] = mchelix.phi0();
	//m_omegaTruth[nMcTk]  =1./(mchelix.radius());         //Q
	//m_dzTruth[nMcTk] = mchelix.dz();
	//m_tanl_mc[nMcTk]  =mchelix.tanl();
	//m_rho_mc[nMcTk]  = rho;
	//m_theta_mc[nMcTk]  = theta;

	if(m_hist){
	  m_pzTruth=mchelix.momentum(0.).z();
	  m_pidTruth = t_pidTruth;
	  m_costaTruth = mchelix.cosTheta();
	  m_ptTruth = mchelix.pt();
	  m_pTruth = mchelix.momentum(0.).mag();
	  m_qTruth = t_qTruth;
	  m_drTruth = mchelix.dr();
	  m_phi0Truth = mchelix.phi0();
	  m_omegaTruth  =1./(mchelix.radius());         //Q
	  m_dzTruth = mchelix.dz();
	  m_tanl_mc  =mchelix.tanl();
	  m_rho_mc  = rho;
	  m_theta_mc  = theta;
	}

	//	if(m_debug>0){
	//	  std::cout<<"Truth tk "<<nMcTk<<" " <<name<<" pid "<<pid<<" charge "<<t_qTruth << " helix "<< mchelix.a()<<" p "<<mchelix.momentum(0.)<<" p "<<mchelix.momentum(0.).mag()<<" pt "<<mchelix.momentum(0.).perp()<<" pz "<<mchelix.momentum(0.).z()<<std::endl;
	//
	//	  cout<<"ptTruth "<<mchelix.pt()<<endl;
	//	  cout<<"tanlTruth"<<mchelix.tanl()<<endl;
	//	  cout<<"d0Truth"<<mchelix.dr()<<endl;
	//	}
	//	nMcTk++;
  }
  //  m_nTrkMC  = nMcTk;
  //if(m_debug>2) m_mdcPrintSvc->printMdcMcHitCol();

  g_firstHit.set(0,0,0);
  SmartDataPtr<MdcMcHitCol> mdcMcHitCol(eventSvc(),"/Event/MC/MdcMcHitCol");
  if(mdcMcHitCol){
	MdcMcHitCol::iterator iter_mchit = mdcMcHitCol->begin();
	for (;iter_mchit != mdcMcHitCol->end(); iter_mchit++) {
	  if((*iter_mchit)->getTrackIndex()==0) {
		g_firstHit.setX((*iter_mchit)->getPositionX()/10.);//mm to cm
		g_firstHit.setY((*iter_mchit)->getPositionY()/10.);
		g_firstHit.setZ((*iter_mchit)->getPositionZ()/10.);
		break;
	  }
	}
  }else{
	std::cout<<__FILE__<<" Could not get MdcMcHitCol  "<<std::endl;
	return StatusCode::FAILURE;
  }

  //add truth info. to HoughHit in list
  return StatusCode::SUCCESS;
}

MdcDigiVec MdcHoughFinder::prepareDigi(){
  //  retrieve Mdc digi vector form RawDataProviderSvc 
  uint32_t getDigiFlag = 0;
  if(m_dropHot || m_combineTracking )getDigiFlag |= MdcRawDataProvider::b_dropHot;
  if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
  if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;

  MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
  return mdcDigiVec;
}

void MdcHoughFinder::dumpHoughHitList(const HoughHitList& houghhitlist){

  int num_first_half=0;
  vector<double> vtemp,utemp;
  double k,b,theta,rho,x_cross,y_cross;
  std::vector<HoughHit>::const_iterator iter = houghhitlist.getList().begin();
  int nsig_axial=0;
  int exit_multiturn=0;
  for(int iHit=0;iter!= houghhitlist.getList().end(); iter++,iHit++){
	const HoughHit h = (*iter);
	if( m_debug>0 ) h.printTruth();
	//if( h.getCirList()!=0 ) continue;
	m_layer[iHit] = h.getLayerId();
	m_cell[iHit]  = h.getWireId();
	m_x[iHit]  	  = h.getMidX();
	m_y[iHit]  	  = h.getMidY();
	m_u[iHit]  	  = h.getU();
	m_v[iHit]  	  = h.getV();
	m_drift[iHit] = h.getDriftDist();
	m_r[iHit]  	  = h.getR();
	m_slant[iHit]   = h.getSlayerType();
	m_uTruth[iHit]  = h.getUTruth();
	m_vTruth[iHit]  = h.getVTruth();
	m_xTruth[iHit]  = h.getPointTruth().x();
	m_yTruth[iHit]  = h.getPointTruth().y();
	m_driftTruth[iHit]  = h.getDriftDistTruth();
	m_rTruth[iHit]  = h.getRTruth();
	int type =0;                       // type : 0 1 2
	type = h.digi()->getTrackIndex();          // signal noise : 0 1 
	if ( type <0 ) type=1;
	if ( type ==0 && h.getCirList()>0 ) type = 2;  // signal && 2nd half : 2
	m_type[iHit]  = type;
	if ( h.digi()->getTrackIndex()>0 && h.getCirList()>1 ) exit_multiturn = 1;  // signal && 2nd half : 2

	num_first_half++;
	if( h.getCirList()==0 && h.digi()->getTrackIndex()>=0 && h.getStyle()!=-999 && h.getSlayerType()==0 ) nsig_axial++;

	if ( h.getSlayerType()==0 && h.getCirList()==0 && h.digi()->getTrackIndex()>=0 && h.getStyle()!=-999 && utemp.size()<10 )    //axial,1st,signal,size<10
	{
	  utemp.push_back(h.getUTruth());
	  vtemp.push_back(h.getVTruth());
	}
	m_nSig_Axial=nsig_axial;
	m_exit_multiturn = exit_multiturn;


	Leastfit(utemp,vtemp,k,b);
	//calcu truth 
	//k,b from truth
	x_cross = -b/(k+1/k);
	y_cross = b/(1+k*k);
	rho=sqrt(x_cross*x_cross+y_cross*y_cross);
	theta=atan2(y_cross,x_cross);
	if(theta<0)  {
	  theta=theta+M_PI;
	  rho=-rho;
	}

	m_rho_line  =rho;
	m_theta_line=theta;


  }
  //  m_nHit = houghhitlist.nHit();
  m_nHit = num_first_half;
}

void MdcHoughFinder::dumpHoughMap(const HoughMap& houghmap){
  //m_MapMax = houghmap.MAX;

  //  //dump max bin
  //  int BINX=m_nBinTheta;
  //  int BINY=m_nBinRho;
  //  m_xybinNum=BINX*BINY;
  //  double **s = houghmap.getS();
  //  double max = 0;
  //  for(int i=0;i<BINX;i++){
  //    for(int j=0;j<BINY;j++){
  //	if( s[i][j]>max ) max=s[i][j];
  //      m_xybinS[BINY*i+j] = s[i][j];
  //    }
  //  }
  //  m_xybinMax = max;

  //method -- narrow
  //    int BINX=m_n2;
  //    int BINY=m_n2;
  //    m_xybinNum=BINX*BINY;
  //	double **s = houghmap.getS2();

  //  m_theta_left = houghmap.Theta_left;
  //  m_theta_right= houghmap.Theta_right;
  //  m_rho_down= houghmap.Rho_down;
  //  m_rho_up= houghmap.Rho_up;
  //  m_theta= houghmap.Theta;
  //  m_rho= houghmap.Rho;
  //  m_height = houghmap.Height;

  //  m_aver= houghmap.Aver;
  //  m_sigma= houghmap.Sigma;

  // dump tracklist
  m_nMapPk=houghmap.getPeakNumber();
  //for(int iPk=0;iPk< houghmap.getPeakNumber(); iPk++){
  //  m_PeakOrder[iPk]  =  houghmap.getPeak(iPk).getPeakNum();
  //  m_PeakRho[iPk]  =  houghmap.getPeak(iPk).getRho();
  //cout<<"rho: "<<houghmap.getPeak(iPk).getRho()<<endl;
  //  m_PeakTheta[iPk]=  houghmap.getPeak(iPk).getTheta();
  //cout<<"theta : "<<houghmap.getPeak(iPk).getTheta()<<endl;
  //m_PeakHeight[iPk]= houghmap.getPeak(iPk).peakHeight();
  // m_PeakHit[iPk]=  houghmap.getPeak(iPk).getHoughHitList().size();
  // m_PeakHitA[iPk] =  houghmap.getPeak(iPk).getHitNumA(6);
  //}
  // m_nMapTrk=houghmap.getTrackNumber();
  //cout<<"npk ntrk "<<m_nMapPk<<" "<<m_nMapTrk<<endl;
  //for(int iTk=0;iTk< houghmap.getTrackNumber(); iTk++){
  //  m_TrackRho[iTk]  =  houghmap.getTrack(iTk).getRho();
  //  m_TrackTheta[iTk]=  houghmap.getTrack(iTk).getTheta();
  //  m_TrackHitA[iTk] =  houghmap.getTrack(iTk).getHitNumA(6);
  //}
  //maxlayer_slant
  //  m_nMaxLayerSlant = houghmap.get_maxlayer_slant().size();
  //  //cout<<"m_nMaxLayerSlant "<<m_nMaxLayerSlant<<endl;
  //  for(int inmax=0;inmax< houghmap.get_maxlayer_slant().size(); inmax++){
  //	m_MaxLayerSlant[inmax]=houghmap.get_maxlayer_slant().at(inmax);
  //	m_MaxLayer[inmax]=houghmap.get_maxlayer();
  //  }
  //  //nomaxlayer_slant
  //  m_nNoMaxLayerSlant = houghmap.get_nomaxlayer_slant().size();
  //  //cout<<"m_nNoMaxLayerSlant "<<m_nNoMaxLayerSlant<<endl;
  //  for(int inmax=0;inmax< houghmap.get_nomaxlayer_slant().size(); inmax++){
  //	m_NoMaxLayerSlant[inmax]=houghmap.get_nomaxlayer_slant().at(inmax);
  //	m_NoMaxLayerid[inmax]=houghmap.get_nomaxlayerid().at(inmax);
  //  }
  //
  //  std::vector<HoughHit>::const_iterator iter = houghmap.getHitList().getList().begin();
  //  for(int iHit=0;iter!= houghmap.getHitList().getList().end(); iter++,iHit++){
  //	const HoughHit h = (*iter);
  //	//if(  h.getSlayerType()!=0 || h.getCirList()!=0 ) continue;
  //	if(  h.getSlayerType()!=0 ) continue;
  //	m_deltaD[iHit]  = h.getDeltaD();
  //	m_flt[iHit]  = h.getFltLen();
  //  }
}
void MdcHoughFinder::diffMap(const HoughMap& houghmap,const HoughMap& houghmap2){
  //map 1
  m_nMap1Pk=houghmap.getPeakNumber();
  for(int iPk=0;iPk< houghmap.getPeakNumber(); iPk++){
	m_PkRho1[iPk]  =  houghmap.getPeak(iPk).getRho();
	m_PkTheta1[iPk]=  houghmap.getPeak(iPk).getTheta();
  }
  m_nMap1Tk=houghmap.getTrackNumber();
  for(int iTk=0;iTk< houghmap.getTrackNumber(); iTk++){
	m_TkRho1[iTk]  =  houghmap.getTrack(iTk).getRho();
	m_TkTheta1[iTk]=  houghmap.getTrack(iTk).getTheta();
  }
  //map 2
  m_nMap2Pk=houghmap2.getPeakNumber();
  for(int iPk=0;iPk< houghmap2.getPeakNumber(); iPk++){
	m_PkRho2[iPk]  =  houghmap2.getPeak(iPk).getRho();
	m_PkTheta2[iPk]=  houghmap2.getPeak(iPk).getTheta();
  }
  m_nMap2Tk=houghmap2.getTrackNumber();
  for(int iTk=0;iTk< houghmap2.getTrackNumber(); iTk++){
	m_TkRho2[iTk]  =  houghmap2.getTrack(iTk).getRho();
	m_TkTheta2[iTk]=  houghmap2.getTrack(iTk).getTheta();
  }
}

void MdcHoughFinder::Leastfit(vector<double> u,vector<double> v,double& k ,double& b){
  int N = u.size();
  if( N <= 2 )  return;
  double *x=new double[N];
  double *y=new double[N];
  for(int i=0;i<N;i++){
	x[i]=u[i];
	y[i]=v[i];
  }

  TF1 *fpol1=new TF1("fpol1","pol1",-50,50);
  TGraph *tg=new TGraph(N,x,y);
  tg->Fit("fpol1","QN");
  double ktemp  =fpol1->GetParameter(1);
  double btemp  =fpol1->GetParameter(0);
  k = ktemp;
  b = btemp;
  delete []x;
  delete []y;
  delete fpol1;
  delete tg;
}

bool more_pt(const HoughTrack* tracka,const HoughTrack* trackb){
  return tracka->getPt3D() > trackb->getPt3D();
}

int MdcHoughFinder::storeTracks(RecMdcTrackCol*& trackList_tds ,RecMdcHitCol*& hitList_tds,vector<RecMdcTrack*>& vec_trackPatTds){
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  // tds
  DataObject *aTrackCol;
  DataObject *aRecHitCol;
  SmartIF<IDataManagerSvc> dataManSvc(eventSvc()); 
  if(!m_combineTracking){
	eventSvc()->findObject("/Event/Recon/RecMdcTrackCol",aTrackCol);
	if(aTrackCol != NULL) {
	  dataManSvc->clearSubTree("/Event/Recon/RecMdcTrackCol");
	  eventSvc()->unregisterObject("/Event/Recon/RecMdcTrackCol");
	}
	eventSvc()->findObject("/Event/Recon/RecMdcHitCol",aRecHitCol);
	if(aRecHitCol != NULL) {
	  dataManSvc->clearSubTree("/Event/Recon/RecMdcHitCol");
	  eventSvc()->unregisterObject("/Event/Recon/RecMdcHitCol");
	}
  }

  //RecMdcTrackCol* trackList_tds;
  eventSvc()->findObject("/Event/Recon/RecMdcTrackCol",aTrackCol);
  if (aTrackCol) {
	trackList_tds = dynamic_cast<RecMdcTrackCol*> (aTrackCol);
  }else{
	trackList_tds = new RecMdcTrackCol;
	StatusCode sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcTrackCol, trackList_tds);
	if(!sc.isSuccess()) {
	  log << MSG::FATAL << " Could not register RecMdcTrack collection" <<endreq;
	  return StatusCode::FAILURE;
	}
  }
  //RecMdcHitCol* hitList_tds;
  eventSvc()->findObject("/Event/Recon/RecMdcHitCol",aRecHitCol);
  if (aRecHitCol) {
	hitList_tds = dynamic_cast<RecMdcHitCol*> (aRecHitCol);
  }else{
	hitList_tds = new RecMdcHitCol;
	StatusCode sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcHitCol, hitList_tds);
	if(!sc.isSuccess()) {
	  log << MSG::FATAL << " Could not register RecMdcHit collection" <<endreq;
	  return StatusCode::FAILURE;
	}
  }
  //remove bad quality or low pt track(s)
  if(m_removeBadTrack && m_combineTracking) {
	if( m_debug>0 ) cout<<"PATTSF collect "<<trackList_tds->size()<<" track. "<<endl;
	if(trackList_tds->size()!=0){
	  RecMdcTrackCol::iterator iter_pat = trackList_tds->begin();
	  for(;iter_pat!=trackList_tds->end();iter_pat++){
		double d0=(*iter_pat)->helix(0);
		double kap = (*iter_pat)->helix(2);
		double pt = 0.00001;
		if(fabs(kap)>0.00001) pt = fabs(1./kap);
		double dz=(*iter_pat)->helix(3);
		double chi2=(*iter_pat)->chi2();
		if( m_debug>0) cout<<"d0: "<<d0<<" z0: "<<dz<<" pt "<<pt<<" chi2 "<<chi2<<endl;
		//if( (fabs(d0)>m_dropTrkDrCut || fabs(dz)>m_dropTrkDzCut || chi2>m_dropTrkChi2Cut) && pt<=m_dropTrkPtCut) 
		if(  pt<=m_dropTrkPtCut ){ 
		  vec_trackPatTds.push_back(*iter_pat);
		  //remove hits on track
		  HitRefVec rechits = (*iter_pat)->getVecHits();
		  HitRefVec::iterator hotIter = rechits.begin();
		  while (hotIter!=rechits.end()) {
			Identifier id = (*hotIter)->getMdcId();
			int layer = MdcID::layer(id);
			int wire  = MdcID::wire(id);
			if(m_debug>0) cout <<"remove hit " << (*hotIter)->getId()<<"("<<layer<<","<<wire<<")"<<endl;
			hitList_tds->remove(*hotIter);
			hotIter++;
		  }
		  //if( (fabs(d0)>3*m_dropTrkDrCut || fabs(dz)>3*m_dropTrkDzCut) ){  // drop pattsf fate track
		  //  trackList_tds->remove(*iter_pat);
		  //  iter_pat--;
		  //}
		}
		if( m_debug>0 ) cout<<"after delete trackcol size : "<<trackList_tds->size()<<endl;
	  }
	} else {
	  if(m_debug>0) cout<<" PATTSF find 0 track. "<<endl;
	}
  }//end of remove bad quality high pt track

  int nTdsTk=trackList_tds->size();
  return nTdsTk;
}//end of stroeTracks

void MdcHoughFinder::clearMem(MdcTrackList& list1,MdcTrackList& list2){
  cout<<"length in clearMem "<<list1.length()<<" "<<list2.length()<<endl;
  cout<<"in list "<<vectrk_for_clean.size()<<endl;

  for(unsigned int i=0;i<vectrk_for_clean.size();i++){
	int sametrk=0;
	for(unsigned int j=0;j<list1.length();j++){
	  cout<<"-i j "<<i<<" "<<j<<" "<<vectrk_for_clean[i]<<" "<<&(list1[j]->track())<<endl;
	  if(vectrk_for_clean[i] == &(list1[j]->track()) )  {sametrk=1;cout<<"not delete new trk "<<endl;}
	  //delete list1[j];
	}
	for(unsigned int k=0;k<list2.length();k++){
	  cout<<"+i k "<<i<<" "<<k<<" "<<vectrk_for_clean[i]<<" "<<&(list2[k]->track())<<endl;
	  if(vectrk_for_clean[i] == &(list2[k]->track()) )  {sametrk=1;cout<<"not delete new trk "<<endl;}
	  //delete list2[k];
	}
	if( sametrk ==0 ) {
	  cout<<"delete i "<<i<<endl;
	  delete vectrk_for_clean[i];
	  vectrk_for_clean[i] = NULL;
	}
  }
  vectrk_for_clean.clear();
}

int MdcHoughFinder::judgeChargeTrack(MdcTrackList& list1 ,MdcTrackList& list2){
  if(m_debug>0) cout<<"in judgeChargeTrack"<<endl;
  if(m_debug>0) cout<<"ntrack length neg  : "<<list1.length()<<endl;
  if(m_debug>0) cout<<"ntrack length pos  : "<<list2.length()<<endl;
  for(int itrack1=0; itrack1<list1.nTrack(); itrack1++){
	MdcTrack* track_1 = list1[itrack1];
	TrkExchangePar par1 = track_1->track().fitResult()->helix(0.);
	double d0_1 = par1.d0();
	double phi0_1 = par1.phi0();
	double omega_1 = par1.omega();
	double cr=fabs(1./par1.omega());
	double cx= sin(par1.phi0()) *(par1.d0()+1/par1.omega()) * -1.; //z axis verse,x_babar = -x_belle
	double cy= -1.*cos(par1.phi0()) *(par1.d0()+1/par1.omega()) * -1;//???
	double z0_1 = par1.z0();
	for(int itrack2=0; itrack2<list2.nTrack(); itrack2++){
	  MdcTrack* track_2 = list2[itrack2];
	  TrkExchangePar par2 = track_2->track().fitResult()->helix(0.);
	  double d0_2 = par2.d0();
	  double phi0_2 = par2.phi0();
	  double omega_2 = par2.omega();
	  double cR=fabs(1./par2.omega());
	  double cX= sin(par2.phi0()) *(par2.d0()+1/par2.omega()) * -1.; //z axis verse,x_babar = -x_belle
	  double cY= -1.*cos(par2.phi0()) *(par2.d0()+1/par2.omega()) * -1;//???
	  double z0_2= par2.z0();

	  double   bestDiff = 1.0e+20;
	  if(fabs(cr)*(1.-0.25) <= fabs(cR) && fabs(cR) <= fabs(cr)*(1.+0.25)){
		if(fabs(cx-cX) <= fabs(cr)*(0.25)&& fabs(cy-cY) <= fabs(cr)*(0.25) ){
		  double diff = fabs((fabs(cr)-fabs(cR))*(cx-cX)*(cy-cY));
		  if(diff < bestDiff){
			bestDiff = diff;
		  }
		}
	  }

	  if(bestDiff != 1.0e20) { 
		if(m_debug>0) {
		  cout<<"There is ambiguous +- charge in the track list "<<endl;
		  cout<<"z0_1 : "<<z0_1<<" z0_2 : "<<z0_2<<endl;
		}
		//iter_hough2 = vec_track.erase(iter_hough2); iter_hough2--;
		if( fabs(z0_1) >= fabs(z0_2) ) { list1.remove( track_1 ); itrack1--; break;}
		if( fabs(z0_1) < fabs(z0_2) ) { list2.remove( track_2 ); itrack2--; break;}
	  }
	  if(bestDiff == 1.0e20) {  if(m_debug>0) cout<<" no (+-) track in hough"<<endl; }
	}
  }
}

void MdcHoughFinder::compareHough(MdcTrackList& mdcTrackList){
  if(m_debug>0)  cout<<"in compareHough "<<endl;
  if(m_debug>0) cout<<"ntrack  : "<<mdcTrackList.length()<<endl;
  for(int itrack=0; itrack<mdcTrackList.length(); itrack++){
	MdcTrack* track = mdcTrackList[itrack];
	TrkExchangePar par = track->track().fitResult()->helix(0.);
	int nhit = track->track().hots()->nHit();
	double pt = (1./par.omega())/333.567;  
	if(m_debug>0) cout<<"i Track : "<<itrack<<" nHit: "<<nhit<<" pt: "<<pt<<endl;
  }
  // vector<HoughTrack>::iterator iter_hough = vec_track.begin();
  //  vector<HoughTrack>::iterator iter_hough2 = vec_track.begin()+1;
  for(int itrack1=0; itrack1<mdcTrackList.length(); itrack1++){
	MdcTrack* track_1 = mdcTrackList[itrack1];
	TrkExchangePar par1 = track_1->track().fitResult()->helix(0.);
//	int nhit1 = track_1->track().hots()->nHit();
	int nhit1= track_1->track().fitResult()->nActive();
	double chi2_1 = track_1->track().fitResult()->chisq();
	double d0_1 = par1.d0();
	double phi0_1 = par1.phi0();
	double omega_1 = par1.omega();
	double z0_1= par1.z0();
	double cr=fabs(1./par1.omega());
	double cx= sin(par1.phi0()) *(par1.d0()+1/par1.omega()) * -1.; //z axis verse,x_babar = -x_belle
	double cy= -1.*cos(par1.phi0()) *(par1.d0()+1/par1.omega()) * -1;//???
	if(m_debug>0) cout<<"circle 1 : "<<cr<<","<<cx<<","<<cy<<endl;

	for(int itrack2=itrack1+1; itrack2<mdcTrackList.length(); itrack2++){
	  MdcTrack* track_2 = mdcTrackList[itrack2];
	  TrkExchangePar par2 = track_2->track().fitResult()->helix(0.);
	//  int nhit2 = track_2->track().hots()->nHit();
	int nhit2= track_2->track().fitResult()->nActive();
	double chi2_2 = track_2->track().fitResult()->chisq();
	  double d0_2 = par2.d0();
	  double phi0_2 = par2.phi0();
	  double omega_2 = par2.omega();
	  double z0_2= par2.z0();
	  double cR=fabs(1./par2.omega());
	  double cX= sin(par2.phi0()) *(par2.d0()+1/par2.omega()) * -1.; //z axis verse,x_babar = -x_belle
	  double cY= -1.*cos(par2.phi0()) *(par2.d0()+1/par2.omega()) * -1;//???
	  if(m_debug>0)	  cout<<"circle 2 : "<<cR<<","<<cX<<","<<cY<<endl;

	  double   bestDiff = 1.0e+20;
	  if(fabs(cr)*(1.-0.25) <= fabs(cR) && fabs(cR) <= fabs(cr)*(1.+0.25)){
		if(fabs(cx-cX) <= fabs(cr)*(0.25)&& fabs(cy-cY) <= fabs(cr)*(0.25) ){
		  double diff = fabs((fabs(cr)-fabs(cR))*(cx-cX)*(cy-cY));
		  if(diff < bestDiff){
			bestDiff = diff;
		  }
		}
	  }

	  double zdiff = z0_1-z0_2;
	  if(bestDiff != 1.0e20 &&  fabs(zdiff)<25.){ 
		if(m_debug>0) cout<<"z0_1 : "<<z0_1<<" z0_2 : "<<z0_2<<endl;
		//if( fabs(z0_1)>fabs(z0_2) && fabs(z0_1)>m_z0Cut_compareHough)
//		if( nhit1<nhit2 && (fabs(z0_1)>fabs(z0_2) && fabs(z0_1)>m_z0Cut_compareHough) )   //FIXME
		if( chi2_1/nhit1 > chi2_2/nhit2  ){   //FIXME
		  if(m_debug>0) cout<<"remove track1 "<<1./par1.omega()/333.567<<endl;
		  //remove 1 
		  mdcTrackList.remove( track_1 );
		  itrack1--;
		  break;
		}
		else{
		  //remove 2 
		  if(m_debug>0) cout<<"remove track2 "<<1./par2.omega()/333.567<<endl;
		  //remove 1 
		  mdcTrackList.remove( track_2 );
		  itrack2--;
		}
	  }
	  if(bestDiff == 1.0e20) {  if(m_debug>0) cout<<" no same track in hough"<<endl; }
	}
  }
}
int MdcHoughFinder::comparePatTsf(MdcTrackList& mdcTrackList, RecMdcTrackCol* trackList_tds){
  //  vector<HoughTrack>::iterator iter_hough = vec_track.begin();
  //  int itrk_hough=0;
  //  for(;iter_hough != vec_track.end();iter_hough++)
  //	if(m_debug>0) cout<<"being hough trk "<<itrk_hough<<endl;
  //	itrk_hough++;
  //	double cr= (*iter_hough).getCirR();
  //	double cx= (*iter_hough).getCirX(); 
  //	double cy= (*iter_hough).getCirY(); 
  for(int itrack1=0; itrack1<mdcTrackList.length(); itrack1++){
	MdcTrack* track_1 = mdcTrackList[itrack1];
	TrkExchangePar par1 = track_1->track().fitResult()->helix(0.);
	//int nhit1 = track_1->track().hots()->nHit();
	int nhit1= track_1->track().fitResult()->nActive();
	double chi2_1 = track_1->track().fitResult()->chisq();
	double d0_1 = par1.d0();
	double phi0_1 = par1.phi0();
	double omega_1 = par1.omega();
	double z0_1= par1.z0();
	double cr=fabs(1./par1.omega());
	double cx= sin(par1.phi0()) *(par1.d0()+1/par1.omega()) * -1.; //z axis verse,x_babar = -x_belle
	double cy= -1.*cos(par1.phi0()) *(par1.d0()+1/par1.omega()) * -1;//???

	//track2
	double   bestDiff = 1.0e+20;
	double   ptDiff = 0.0;
	double   cR, cX, cY;
	vector<double> a0,a1,a2,a3,a4;
	RecMdcTrackCol::iterator iterBest; 
	RecMdcTrackCol::iterator iteritrk = trackList_tds->begin();
	int itrk=0;
	vector< RecMdcTrackCol::iterator > iter_pat_temp; 
	for(;iteritrk!=trackList_tds->end();iteritrk++){
	  if(m_debug>0) cout<<"being pattsf trk "<<itrk<<endl;
	  double pt=(*iteritrk)->pxy();
	  a0.push_back( (*iteritrk)->helix(0) );
	  a1.push_back( (*iteritrk)->helix(1) );
	  a2.push_back( (*iteritrk)->helix(2) );
	  a3.push_back( (*iteritrk)->helix(3) );
	  a4.push_back( (*iteritrk)->helix(4) );
	  cR=(-333.567)/a2[itrk];
	  cX=(cR+a0[itrk])*cos(a1[itrk]);                   
	  cY=(cR+a0[itrk])*sin(a1[itrk]);
	  if(m_debug>0) 
	  {
		cout<<" compare PATTSF and HOUGH "<<endl;
		cout<<"  fabs(cX) "<<  fabs(cX)<< "fabs(cx) "<<fabs(cx)<<endl;
		cout<<"  fabs(cY) "<<  fabs(cY)<< "fabs(cy) "<<fabs(cy)<<endl;
		cout<<"  fabs(cR) "<<  fabs(cR)<< "fabs(cr) "<<fabs(cr)<<endl;
		cout<<"  fabs(cx-cX) "<<  fabs(cx-cX)<< "fabs(cy-cY) "<<fabs(cy-cY)<<" fabs(cr-cR) "<< fabs(cr-cR)<<endl;
	  }

	  if(fabs(cr)*(1.-0.25) <= fabs(cR) && fabs(cR) <= fabs(cr)*(1.+0.25)){
		if(fabs(cx-cX) <= fabs(cr)*(0.25)&& fabs(cy-cY) <= fabs(cr)*(0.25) ){
		  double diff = fabs((fabs(cr)-fabs(cR))*(cx-cX)*(cy-cY));
		  if(m_debug>0) cout<<" diff "<<diff<<" pt "<<pt<<endl;
		  iter_pat_temp.push_back(iteritrk);
		  if( fabs(pt) >  ptDiff){
			ptDiff= pt;
			iterBest=iteritrk;
			bestDiff = diff;
		  }
		  //if(diff < bestDiff){
		  //  bestDiff = diff;
		  //  //			bestIndex = itrk;
		  //  iterBest=iteritrk;
		  //}
		}
	  }
	  itrk++;
	}
	if(bestDiff != 1.0e20) {
	  for(int itrkter = 0;itrkter<iter_pat_temp.size();itrkter++){
		iterBest = iter_pat_temp[itrkter];
		if(m_debug>0) cout<<" same track pattsf & hough , then compare nhit. "<<endl;
		double d0_pattsf =(*iterBest)->helix(0);
		double z0_pattsf =(*iterBest)->helix(3);
		double d0_hough = d0_1;
		double z0_hough = z0_1;
		int use_pattsf(1),use_hough(1);
		if( fabs(d0_pattsf)>m_dropTrkDrCut || fabs(z0_pattsf)>m_dropTrkDzCut ) use_pattsf=0;
		if( fabs(d0_hough)>m_dropTrkDrCut || fabs(z0_hough)>m_dropTrkDzCut ) use_hough=0;
		if( use_pattsf==0 && use_hough==1 ) {
		  trackList_tds->remove(*iterBest); 
		  if(m_debug>0) cout<<" use houghTrack, vertex pattsf wrong"<<endl;
		}
		if( use_pattsf==1 && use_hough==0 ) {
		  mdcTrackList.remove( track_1 );
		  itrack1--;
		  if(m_debug>0) cout<<" use pattsfTrack, vertex hough wrong"<<endl;
		  break;
		}
		if( (use_pattsf==0 && use_hough==0) || (use_pattsf==1 && use_hough==1) ){
		  int nhit_pattsf=( (*iterBest)->ndof()+5 );
		  double chi2_2 = (*iterBest)->chi2();
		  int nhit_hough =  nhit1;
		  if(m_debug>0) cout<<" nhit "<<nhit_pattsf<<" "<<nhit_hough<<endl;
		  if( nhit_pattsf>chi2_1) {
			mdcTrackList.remove( track_1 );
			itrack1--;
			if(m_debug>0) cout<<" use pattsfTrack "<<endl;
			break;
		  }
		  else{
			trackList_tds->remove(*iterBest); 
			if(m_debug>0) cout<<" use houghTrack "<<endl;
		  }
		}
	  }
	}
	//	if(m_debug>0) cout<<" d0"<<d0_pattsf<<" "<<d0_hough<<endl;
	//	if(m_debug>0) cout<<" z0"<<z0_pattsf<<" "<<z0_hough<<endl;
	//	if( fabs(d0_pattsf) >= fabs(d0_hough) && fabs(z0_pattsf) >= fabs(z0_hough) ) {
	//	  trackList_tds->remove(*iterBest); 
	//	  if(m_debug>0) cout<<" use houghTrack "<<endl;
	//	}
	//	else{
	//	  iter_hough = vec_track.erase(iter_hough); iter_hough--;
	//	  if(m_debug>0) cout<<" use pattsfTrack "<<endl;
	//	}

	if(bestDiff == 1.0e20) { if(m_debug>0) cout<<" no same track in pattsf"<<endl; }
	//if(bestDiff != 1.0e20) { iter_hough = vec_track.erase(iter_hough); iter_hough--;}
	//if(bestDiff == 1.0e20) { if(m_debug>0) cout<<" no merge "<<endl; }
  }
  //cout<<"size after combine "<<trackList_tds->size()<<endl;
  if(trackList_tds->size()!=0){
	int digi=0;
	RecMdcTrackCol::iterator iter_pat = trackList_tds->begin();
	for(;iter_pat!=trackList_tds->end();iter_pat++,digi++){
	  (*iter_pat)->setTrackId(digi);
	  //	cout<<"digi "<<(*iter_pat)->trackId()<<endl;
	}
  }
  return trackList_tds->size();
}
//int MdcHoughFinder::judgeHit(MdcTrackList& list,const vector<HoughTrack>& trackCol)
int MdcHoughFinder::judgeHit(MdcTrackList& list,vector<MdcTrack*>& trackCol){
  if(m_debug>0) cout<<"in judgHit: "<<endl;
  for( unsigned int i =0;i<trackCol.size();i++){
	//MdcTrack *mdcTrack = new  MdcTrack(trackCol[i].getTrk());
	//list.append(mdcTrack);                                                            
	list.append(trackCol[i]);
  } 
  int nDeleted = list.arbitrateHits();
  return nDeleted;
}
StatusCode MdcHoughFinder::bookTuple(){
  MsgStream log(msgSvc(), name());
  NTuplePtr nt1(ntupleSvc(), "MdcHoughFinder/evt");
  if ( nt1 ){
	ntuple_evt = nt1;
  } else {
	ntuple_evt = ntupleSvc()->book("MdcHoughFinder/evt", CLID_ColumnWiseTuple, "evt");
	if(ntuple_evt){
	  ntuple_evt->addItem("eventNum",           m_eventNum);
	  ntuple_evt->addItem("runNum",             m_runNum);

	  ntuple_evt->addItem("nHit",          m_nHit,0, 6796);

	  //ntuple_evt->addItem("layer",         m_nHit,   m_layer);
	  //ntuple_evt->addItem("cell",          m_nHit,   m_cell);
	  //ntuple_evt->addItem("x",             m_nHit,   m_x);
	  //ntuple_evt->addItem("y",             m_nHit,   m_y);
	  //ntuple_evt->addItem("u",             m_nHit,   m_u);
	  //ntuple_evt->addItem("v",             m_nHit,   m_v);
	  //ntuple_evt->addItem("drift",         m_nHit,   m_drift);
	  //ntuple_evt->addItem("r",             m_nHit,   m_r);

	  //ntuple_evt->addItem("uTruth",        m_nHit,   m_uTruth);
	  //ntuple_evt->addItem("vTruth",        m_nHit,   m_vTruth);
	  //ntuple_evt->addItem("xTruth",        m_nHit,   m_xTruth);
	  //ntuple_evt->addItem("yTruth",        m_nHit,   m_yTruth);
	  //ntuple_evt->addItem("driftTruth",    m_nHit,   m_driftTruth);
	  //ntuple_evt->addItem("rTruth",        m_nHit,   m_rTruth);
	  //ntuple_evt->addItem("type",          m_nHit,   m_type);

	  //ntuple_evt->addItem("deltaD",        m_nHit,   m_deltaD);
	  //ntuple_evt->addItem("flt",           m_nHit,   m_flt);
	  //ntuple_evt->addItem("slant",         m_nHit,   m_slant);

	  //ntuple_evt->addItem("nSigAxial",     m_nSig_Axial, 0, 6796);
	  //ntuple_evt->addItem("xybinNum",      m_xybinNum,   0,10000000); 
	  //ntuple_evt->addItem("xybinMax",         m_xybinMax); 
	  //ntuple_evt->addItem("xybinNL",       m_xybinNum,   m_xybinNL); 
	  //ntuple_evt->addItem("xybinRL",       m_xybinNum,   m_xybinRL); 
	  //ntuple_evt->addItem("xybinS",        m_xybinNum,   m_xybinS); 
	  //ntuple_evt->addItem("theta_left",    m_theta_left); 
	  //ntuple_evt->addItem("theta_right",   m_theta_right); 
	  //ntuple_evt->addItem("rho_down",      m_rho_down); 
	  //ntuple_evt->addItem("rho_up",        m_rho_up); 
	  ////by houghmap maxbin value  0~M_PI
	  //ntuple_evt->addItem("theta",         m_theta); 
	  //ntuple_evt->addItem("rho",           m_rho); 
	  //ntuple_evt->addItem("height",        m_height); 
	  //ntuple_evt->addItem("aver",          m_aver); 
	  //ntuple_evt->addItem("sigma",         m_sigma); 

	  ////multiturn
	  //ntuple_evt->addItem("multiturn",         m_exit_multiturn); 

	  ////diff two charge
	  //ntuple_evt->addItem("nMap1pk",          m_nMap1Pk,  0,100); 
	  //ntuple_evt->addItem("nMap1tk",          m_nMap1Tk,  0,100); 
	  //ntuple_evt->addItem("nMap2pk",          m_nMap2Pk,  0,100); 
	  //ntuple_evt->addItem("nMap2tk",          m_nMap2Tk,  0,100); 
	  //ntuple_evt->addItem("map1_pkrho",       m_nMap1Pk,  m_PkRho1); 
	  //ntuple_evt->addItem("map1_pktheta",     m_nMap1Pk,  m_PkTheta1); 
	  //ntuple_evt->addItem("map1_tkrho",       m_nMap1Tk,  m_TkRho1); 
	  //ntuple_evt->addItem("map1_tktheta",     m_nMap1Tk,  m_TkTheta1); 
	  //ntuple_evt->addItem("map2_pkrho",       m_nMap2Pk,  m_PkRho2); 
	  //ntuple_evt->addItem("map2_pktheta",     m_nMap2Pk,  m_PkTheta2); 
	  //ntuple_evt->addItem("map2_tkrho",       m_nMap2Tk,  m_TkRho2); 
	  //ntuple_evt->addItem("map2_tktheta",     m_nMap2Tk,  m_TkTheta2); 

	  ////MC truth
	  //ntuple_evt->addItem("thetaline",     m_theta_line); 
	  //ntuple_evt->addItem("rholine",       m_rho_line); 

	  //	  ntuple_evt->addItem("nmax",          m_nMaxLayerSlant,0,200); 
	  //	  ntuple_evt->addItem("maxslant",      m_nMaxLayerSlant,   m_MaxLayerSlant); 
	  //	  ntuple_evt->addItem("maxslantlayer", m_nMaxLayerSlant,   m_MaxLayer); 
	  //	  ntuple_evt->addItem("nnomax",        m_nNoMaxLayerSlant,0,1000); 
	  //	  ntuple_evt->addItem("nomaxslant",    m_nNoMaxLayerSlant, m_NoMaxLayerSlant); 
	  //	  ntuple_evt->addItem("nomaxlayerid",  m_nNoMaxLayerSlant, m_NoMaxLayerid); 

	  //	  ntuple_evt->addItem("MapMax",        m_MapMax ); 
	  ntuple_evt->addItem("nMapPk",        m_nMapPk,  0,100); 
	  //	  ntuple_evt->addItem("nPeakNum",      m_nMapPk,  m_PeakOrder); 
	  //	  ntuple_evt->addItem("nPeakRho",      m_nMapPk,  m_PeakRho); 
	  //	  ntuple_evt->addItem("nPeakTheta",    m_nMapPk,  m_PeakTheta); 
	  //	  ntuple_evt->addItem("nPeakHeight",      m_nMapPk,  m_PeakHeight); 
	  //	  ntuple_evt->addItem("nPeakHit",      m_nMapPk,  m_PeakHit); 
	  //	  ntuple_evt->addItem("nPeakHitA",     m_nMapPk,  m_PeakHitA); 
	  //	  ntuple_evt->addItem("nMapTrk",       m_nMapTrk, 0,1000); 
	  //	  ntuple_evt->addItem("nTrkRho",       m_nMapTrk,  m_TrackRho); 
	  //	  ntuple_evt->addItem("nTrkTheta",     m_nMapTrk,  m_TrackTheta); 
	  //	  ntuple_evt->addItem("nTrkHitA",      m_nMapTrk,  m_TrackHitA); 

	  //rec - charge
	  //global 2d fit 
	  //	  ntuple_evt->addItem("nTrk2D_neg",        m_nTrk2D_neg,  0,100); 
	  //	  ntuple_evt->addItem("pt2D_neg",          m_nTrk2D_neg,  m_pt2D_neg); 
	  //	  ntuple_evt->addItem("nHit2D_neg",        m_nTrk2D_neg,  m_nHit2D_neg); 
	  //	  ntuple_evt->addItem("chi2_2D_neg",       m_nTrk2D_neg,  m_chi2_2D_neg); 
	  //	  //global 3d fit 
	  //	  ntuple_evt->addItem("nTrk3D_neg",        m_nTrk3D_neg,  0,100); 
	  //	  ntuple_evt->addItem("tanl_neg",          m_nTrk3D_neg,  m_tanl_neg); 
	  //	  ntuple_evt->addItem("tanl3D_neg",        m_nTrk3D_neg,  m_tanl3D_neg); 
	  //	  ntuple_evt->addItem("z0_neg",            m_nTrk3D_neg,  m_z0_neg); 
	  //	  ntuple_evt->addItem("z0_3D_neg",         m_nTrk3D_neg,  m_z0_3D_neg); 
	  //	  ntuple_evt->addItem("pro_neg",		      m_nTrk3D_neg,  m_pro_neg); 
	  //	  ntuple_evt->addItem("pt3D_neg",          m_nTrk3D_neg,  m_pt3D_neg); 
	  //	  ntuple_evt->addItem("nHit3D_neg",        m_nTrk3D_neg,  m_nHit3D_neg); 
	  //	  ntuple_evt->addItem("chi2_3D_neg",       m_nTrk3D_neg,  m_chi2_3D_neg); 
	  //
	  //	  //truth inf
	  //	  //	  ntuple_evt->addItem("nTrkMC",     m_nTrkMC,0,10);
	  //	  ntuple_evt->addItem("pidTruth",   m_pidTruth);
	  //	  ntuple_evt->addItem("costaTruth", m_costaTruth);
	  //	  ntuple_evt->addItem("ptTruth",    m_ptTruth);
	  //	  ntuple_evt->addItem("pzTruth",    m_pzTruth);
	  //	  ntuple_evt->addItem("pTruth",     m_pTruth);
	  //	  ntuple_evt->addItem("qTruth",     m_qTruth);
	  //	  ntuple_evt->addItem("drTruth",    m_drTruth);
	  //	  ntuple_evt->addItem("phiTruth",   m_phi0Truth);
	  //	  ntuple_evt->addItem("omegaTruth", m_omegaTruth);
	  //	  ntuple_evt->addItem("dzTruth",    m_dzTruth);
	  //	  ntuple_evt->addItem("tanlTruth",  m_tanl_mc);
	  //	  ntuple_evt->addItem("rhoTruth",   m_rho_mc);
	  //	  ntuple_evt->addItem("thetaTruth", m_theta_mc);


	  ntuple_evt->addItem("time", m_time);

	} else { log << MSG::ERROR << "Cannot book tuple MdcHoughFinder/hough" <<endmsg;
	  return StatusCode::FAILURE;
	}
  }
  /*
	 NTuplePtr nt2(ntupleSvc(), "MdcHoughFinder/hit");
	 if ( nt2 ){
	 ntuplehit= nt2;
	 } else {
	 ntuplehit= ntupleSvc()->book("MdcHoughFinder/hit", CLID_ColumnWiseTuple, "hit");
	 if(ntuplehit){
	 ntuplehit->addItem("evt_stereo",           m_evt_stereo);
	 ntuplehit->addItem("run_stereo",           m_run_stereo);
	 ntuplehit->addItem("cos_stereo",           m_cos_stereo);
	 ntuplehit->addItem("tanlTruth_stereo",     m_tanlTruth_stereo);
	 ntuplehit->addItem("ncir_stereo",          m_ncir_stereo);
	 ntuplehit->addItem("npair_stereo",         m_npair_stereo);
	 ntuplehit->addItem("tanl_stereo",          m_tanl_stereo);
	 ntuplehit->addItem("tanl3D_stereo",        m_tanl3D_stereo);
	 ntuplehit->addItem("z0_stereo",            m_z0_stereo);
	 ntuplehit->addItem("z03D_stereo",          m_z03D_stereo);
	 ntuplehit->addItem("nHit_axial",           m_nHit_axial, 0, 6796);
	 ntuplehit->addItem("axial_layer",          m_nHit_axial, m_axial_layer);
	 ntuplehit->addItem("axial_wire",           m_nHit_axial, m_axial_wire);
	 ntuplehit->addItem("axial_deltaD",         m_nHit_axial, m_axial_deltaD);
	 ntuplehit->addItem("axial_flt",            m_nHit_axial, m_axial_flt);
	 ntuplehit->addItem("nHit_stereo",          m_nHit_stereo, 0, 6796);
	 ntuplehit->addItem("stereo_layer",         m_nHit_stereo, m_stereo_layer);
	 ntuplehit->addItem("stereo_wire",          m_nHit_stereo, m_stereo_wire);
	 ntuplehit->addItem("stereo_style",         m_nHit_stereo, m_stereo_style);
	 ntuplehit->addItem("stereo_z0",            m_nHit_stereo, m_stereo_z0);
	 ntuplehit->addItem("stereo_z1",            m_nHit_stereo, m_stereo_z1);
	 ntuplehit->addItem("stereo_s0",            m_nHit_stereo, m_stereo_s0);
	 ntuplehit->addItem("stereo_s1",            m_nHit_stereo, m_stereo_s1);
	 ntuplehit->addItem("stereo_z",             m_nHit_stereo, m_stereo_z);
	 ntuplehit->addItem("stereo_s",             m_nHit_stereo, m_stereo_s);
	 ntuplehit->addItem("stereo_zTruth",             m_nHit_stereo, m_stereo_zTruth);
	 ntuplehit->addItem("stereo_sTruth",             m_nHit_stereo, m_stereo_sTruth);
	 ntuplehit->addItem("stereo_deltaZ",             m_nHit_stereo, m_stereo_deltaZ);
	 ntuplehit->addItem("stereo_nsol",          m_nHit_stereo, m_stereo_nsol);
	 ntuplehit->addItem("stereo_ambig",         m_nHit_stereo, m_stereo_ambig);
	 ntuplehit->addItem("stereo_ambig_truth",   m_nHit_stereo, m_stereo_ambig_truth);
	 ntuplehit->addItem("stereo_disToCir",      m_nHit_stereo, m_stereo_disToCir);
	 ntuplehit->addItem("stereo_cirlist",       m_nHit_stereo, m_stereo_cirlist);
	 }
	 }
	 */

  return StatusCode::SUCCESS;
}
