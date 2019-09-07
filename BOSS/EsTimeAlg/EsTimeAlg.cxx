#include <iostream>
#include <vector>
#include <algorithm>

#include "EsTimeAlg/EsTimeAlg.h"
#include "EsTimeAlg/Toffz_helix.h"
#include "TrackUtil/Helix.h"
#include "EsTimeAlg/Emc_helix.h"
#include "EsTimeAlg/EstParameter.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"  
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogramFactory.h"

#include "McTruth/McParticle.h"
#include "EventModel/EventHeader.h"
#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "McTruth/TofMcHit.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "ReconEvent/ReconEvent.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "TrigEvent/TrigData.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataProviderSvc/TofData.h"
#include "EstTofCaliSvc/IEstTofCaliSvc.h"
#include "DetVerSvc/IDetVerSvc.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"
#include "Identifier/MdcID.h"
#include "EvTimeEvent/RecEsTime.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h" 
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif


using CLHEP::HepVector; 
using CLHEP::Hep3Vector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;
typedef std::vector<double> Vdouble;

using namespace std;
using namespace Event;

// Constants

const double VLIGHT=29.98;
const double ELMAS2=0.511E-3*0.511E-3;
const double MUMAS2=105.658E-3*105.658E-3;
const double PIMAS2=139.569E-3*139.569E-3;
const double PROTONMAS2=938.272E-3*938.272E-3;
const double RCTOF2=81.*81.;
const double RCEMC2=94.2*94.2;
const double TDC2NSEC=0.5, NSEC2TDC=2.0;
const double PI=3.141593,PIBY44=3.141593/44.;
const double VELPROP=33.33;

EsTimeAlg::EsTimeAlg(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator){
    for(int i = 0; i < 5; i++)    m_pass[i] = 0;
    m_flag=1;
    m_nbunch=3;
    m_ntupleflag=1;
    m_beforrec=1;
    m_optCosmic=0;
    m_cosmicScheme=0;
    m_evtNo=0;
    m_ebeam=1.85;
    m_userawtime_opt=0;
    m_debug=0;
    declareProperty("MdcMethod",m_flag);
    declareProperty("Nbunch" ,m_nbunch);
    declareProperty("BunchtimeMC" ,m_bunchtime_MC=8.0);//assign bunch interval for MC; for data it's always obtained from calibration constants.
    declareProperty("NtupleFlag",m_ntupleflag);
    declareProperty("beforrec",m_beforrec);
    declareProperty("Cosmic", m_optCosmic);
    declareProperty("CosmScheme",m_cosmicScheme); 
    declareProperty("EventNo", m_evtNo);
    declareProperty("Ebeam", m_ebeam);
    declareProperty("UseRawTime", m_userawtime_opt);
    declareProperty("RawOffset_b", toffset_rawtime=0.0);
    declareProperty("RawOffset_e", toffset_rawtime_e=0.0);
    declareProperty("Offset_dt_b", offset_dt=0.0);
    declareProperty("Offset_dt_e", offset_dt_end=0.0);
    declareProperty("debug", m_debug);
    declareProperty("UseXT", m_useXT=true);
    declareProperty("UseT0", m_useT0cal=true);
    declareProperty("UseSw", m_useSw=false);
    declareProperty("MdcOpt", m_mdcopt=false);
    declareProperty("TofOpt", m_TofOpt=false);
    declareProperty("TofOptCut",m_TofOpt_Cut=12.);
    declareProperty("UseTimeFactor",m_useTimeFactor=true);
  }

StatusCode EsTimeAlg::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  if(m_ntupleflag){

    NTuplePtr nt2(ntupleSvc(),"FILE105/h2");

    if ( nt2 ) m_tuple2 = nt2;
    else {
      m_tuple2=ntupleSvc()->book("FILE105/h2",CLID_ColumnWiseTuple,"Event Start Time");

      if(  m_tuple2 ) {

	m_tuple2->addItem ("eventNo", g_eventNo);
	m_tuple2->addItem ("runNo", g_runNo);
	//#ifndef McTruth
	m_tuple2->addItem ("NtrackMC", g_ntrkMC,0,5000);
	m_tuple2->addItem ("MCtheta0", g_ntrkMC, g_theta0MC);
	m_tuple2->addItem ("MCphi0", g_ntrkMC, g_phi0MC);
	m_tuple2->addItem ("pxMC",g_ntrkMC, g_pxMC);
	m_tuple2->addItem ("pyMC", g_ntrkMC, g_pyMC);
	m_tuple2->addItem ("pzMC",g_ntrkMC, g_pzMC);
	m_tuple2->addItem ("ptMC",  g_ntrkMC, g_ptMC);
	m_tuple2->addItem ("mct0",g_mcTestime);
	//#endif     
	m_tuple2->addItem ("Ntrack", g_ntrk,0,5000);
	m_tuple2->addItem ("ttof",g_ntrk, g_ttof);
	m_tuple2->addItem ("velocity",g_ntrk,g_vel); 
	m_tuple2->addItem ("abmom",g_ntrk,g_abmom);
	m_tuple2->addItem ("pid",g_ntrk,g_pid);
	m_tuple2->addItem ("nmatchBarrel",g_nmatchbarrel);
	m_tuple2->addItem ("nmatchBarrel_1",g_nmatchbarrel_1);
	m_tuple2->addItem ("nmatchBarrel_2",g_nmatchbarrel_2);
	m_tuple2->addItem ("nmatchend",g_nmatchend);
	m_tuple2->addItem ("nmatch",g_nmatch_tot);
	m_tuple2->addItem ("t0forward",g_ntrk,g_t0for);
	m_tuple2->addItem ("t0backward",g_ntrk,g_t0back);
	m_tuple2->addItem ("meant0",g_meant0);
	m_tuple2->addItem ("nmatchmdc",g_nmatchmdc);
	m_tuple2->addItem ("ndriftt",g_ndriftt);
	m_tuple2->addItem ("MdcEsTime",g_EstimeMdc);
	m_tuple2->addItem ("Mdct0mean",g_t0mean);
	m_tuple2->addItem ("Mdct0try",g_t0);
	m_tuple2->addItem ("Mdct0sq",g_T0);
	m_tuple2->addItem ("trigtiming",g_trigtiming);
	m_tuple2->addItem ( "meantdc" , g_meantdc);
	// m_tuple2->addItem ( "meantev" , g_meantev);
	m_tuple2->addItem ( "ntofup" , g_ntofup,0,500);
	m_tuple2->addItem ( "ntofdown" , g_ntofdown,0,500);
	m_tuple2->addIndexedItem ( "meantevup" , g_ntofup,g_meantevup);
	m_tuple2->addIndexedItem ( "meantevdown" ,g_ntofdown, g_meantevdown);
	m_tuple2->addItem ( "ntofup1" , g_ntofup1);
	m_tuple2->addItem ( "ntofdown1" , g_ntofdown1);
	m_tuple2->addItem ( "Testime_fzisan", g_Testime_fzisan);
	m_tuple2->addItem ( "Testime", g_Testime);
	m_tuple2->addItem ( "T0barrelTof", g_t0barrelTof);
	m_tuple2->addItem ( "difftofb", g_difftof_b);
	m_tuple2->addItem ( "difftofe", g_difftof_e);
	m_tuple2->addItem ("EstFlag",m_estFlag);
	m_tuple2->addItem ("EstTime",m_estTime);
      }
      else    {   // did not manage to book the N tuple....
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple2) << endmsg;
      }
    }
    NTuplePtr nt9(ntupleSvc(),"FILE105/h9");

    if ( nt9 ) m_tuple9 = nt9;
    else {
      m_tuple9=ntupleSvc()->book("FILE105/h9",CLID_ColumnWiseTuple,"Event Start time");

      if(  m_tuple9 ) {
	m_tuple9->addItem ( "Nmatch" , g_nmatch,0,500);
	m_tuple9->addIndexedItem ( "meantev" , g_nmatch,g_meantev);
      }
      else{
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple9) << endmsg;
      }
    }
    NTuplePtr nt3(ntupleSvc(),"FILE105/calibconst");

    if ( nt3 ) m_tuple3 = nt3;
    else {
      m_tuple3=ntupleSvc()->book("FILE105/calibconst",CLID_ColumnWiseTuple,"Event Start time");

      if(  m_tuple3 ) {
	m_tuple3->addItem ( "t0offsetb" , g_t0offset_b);
	m_tuple3->addItem ( "t0offsete" , g_t0offset_e);
	m_tuple3->addItem ( "bunchtime", g_bunchtime);
      }
      else{
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple3) << endmsg;
      }
    }
  }   


  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc;
  static const bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
    log << MSG::ERROR << " Could not initialize Particle Properties Service" << endreq;
    return PartPropStatus;
  }
  m_particleTable = p_PartPropSvc->PDT();
  //IRawDataProviderSvc* m_rawDataProviderSvc;
  StatusCode RawDataStatus = service ("RawDataProviderSvc", m_rawDataProviderSvc, CREATEIFNOTTHERE);
  if ( !RawDataStatus.isSuccess() ){
    log<<MSG::ERROR  << "Could not load RawDataProviderSvc!" << m_rawDataProviderSvc << endreq;
    return RawDataStatus;
  }

  IDetVerSvc* detVerSvc;
  StatusCode sc_det = service("DetVerSvc", detVerSvc);
  if( sc_det.isFailure() ) {
    log << MSG::ERROR << "can't retrieve DetVerSvc instance" << endreq;
    return sc_det;
  }
  m_phase = detVerSvc->phase();


  StatusCode  sc = service("CalibDataSvc", m_pCalibDataSvc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR
      << "Could not get IDataProviderSvc interface of CalibXmlCnvSvc"
      << endreq;
    return sc;
  } else {
    log << MSG::DEBUG
      << "Retrieved IDataProviderSvc interface of CalibXmlCnvSvc"
      << endreq;
  }
  sc = service("CalibRootCnvSvc", m_pRootSvc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR
      << "Could not get ICalibRootSvc interface of CalibRootCnvSvc"
      << endreq;
    return sc;
  }
  // Get properties from the JobOptionsSvc

  sc = setProperties();

  //Get TOF Calibtration Service
  //IEstTofCaliSvc* tofCaliSvc;
  StatusCode scc = service("EstTofCaliSvc", tofCaliSvc);
  if (scc ==  StatusCode::SUCCESS) {
    //log<<MSG::INFO<<"begin dump Calibration Constants"<<endreq;
    // tofCaliSvc->Dump();
    log << MSG::INFO << " Get EstTof Calibration Service Sucessfully!! " << endreq;
  } else {
    log << MSG::ERROR << " Get EstTof Calibration Service Failed !! " << endreq;
    return scc;
  }

  if(m_useXT||m_useT0cal)
  {
    StatusCode scc = Gaudi::svcLocator()->service("MdcCalibFunSvc", imdcCalibSvc);
    if ( scc.isFailure() ){
      log << MSG::FATAL << "Could not load MdcCalibFunSvc!" << endreq;
    }
    else{
      m_mdcCalibFunSvc = dynamic_cast<MdcCalibFunSvc*>(imdcCalibSvc);
    }
  }



  //Initailize MdcUtilitySvc

  IMdcUtilitySvc * imdcUtilitySvc;
  sc = service ("MdcUtilitySvc", imdcUtilitySvc);
  m_mdcUtilitySvc = dynamic_cast<MdcUtilitySvc*> (imdcUtilitySvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load MdcUtilitySvc!" << endreq;
    return StatusCode::FAILURE;
  }


  return  StatusCode::SUCCESS;
}

StatusCode EsTimeAlg:: execute(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " tof " << endreq;

  // Constants
  EstParameter Estparam;
  double offset=0, t_quality=0, tOffset_b=0, tOffset_e=0;
  int   idtof , tofid_helix[30]={-9},idmatch[3][88]={0},idmatch_emc[3][88]={0} ,idt[15]={0},particleId[30]={0}, tofid_emc[2]={0}, module[20]={0};
  int   idetf, etfid_helix[30]={-9}, idetfmatch[3][36]={-9}, idmatch_etf_emc[3][36]={0}, etfid_emc[2]={0};
  int   ntot=0,in=-1,out=-1, emcflag1=0, emcflag2=0, tof_flag=0; double bunchtime=m_bunchtime_MC;
  double meant[15]={0.},adc[15]={0.}, momentum[15]={0.},r_endtof[10]={0.};
  double ttof[30]={0.},helztof[30]={0.0},mcztof=0.0,forevtime=0.0,backevtime=0.0,meantev[200]={0.},meantevup[200]={0.0},meantevdown[200]={0.0};
  double t0forward=0,t0backward=0,t0forward_trk=0,t0backward_trk=0;
  double t0forward_add=0,t0backward_add=0,t_Est=-999;
  double thetaemc_rec[20]={0.},phiemc_rec[20]={0.},energy_rec[20]={0.},xemc_rec[20]={0.},yemc_rec[20]={0.},zemc_rec[20]={0.};
  double r_endetf[10]={0.}, tetf[30]={0.}, helzetf[30]={0.}, helpathetf[36]={0.}, abmom2etf[36]={0.};

  int   nmatch1=0,nmatch2=0,nmatch_barrel=0,nmatch_end=0,nmatch_mdc=0, nmatch_barrel_1=0, nmatch_barrel_2=0,nmatch=0,ntofup=0,ntofdown=0;
  double sum_EstimeMdc=0,sum_EstimeMdcMC=0;
  int   nbunch=0,tEstFlag=0, runNo=0; 
  double helpath[88]={0.},helz[88]={0.},abmom2[88]={0.};
  double mcTestime=0,trigtiming=0;
  double mean_tdc_btof[2][88]={0.}, mean_tdc_etof[3][48]={0.}, mean_tdc_etf[3][36][12]={0.};
  int  optCosmic=m_optCosmic;
  int  m_userawtime=m_userawtime_opt;
  double Testime_fzisan= -999.;//yzhang add
  int    TestimeFlag_fzisan= -999;//yzhang add
  double TestimeQuality_fzisan= -999.;//yzhang add
  double Tof_t0Arr[500]={-999.};

  bool useEtofScin = ( m_phase<3 );
  bool useEtofMRPC = ( m_phase>2 );

  // Part 1: Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "EsTimeAlg: retrieved event: " << eventHeader->eventNumber()  << "  run: " <<  eventHeader->runNumber() << endreq;
  int eventNo=eventHeader->eventNumber();
  runNo=eventHeader->runNumber();

  if(m_ntupleflag && m_tuple2){g_runNo=runNo;  g_eventNo=eventNo;}
  if(runNo<0) {
    offset_dt=0;
    offset_dt_end=0;
    toffset_rawtime=0;
    toffset_rawtime_e=0;
    if( useEtofMRPC ) { toffset_rawtime_e = -20.0; }
  }
  if( runNo>0 && useEtofMRPC ) { toffset_rawtime_e = 1.7; }

  if(runNo>0 && runNo<5336)  offset_dt=5.8;
  if(runNo>5462 && runNo<5466){ offset_dt=1.6; offset_dt_end=0.4;}
  if(runNo>5538 && runNo<5920){ offset_dt=-0.2; offset_dt_end=-1.2;}
  if(runNo>5924 && runNo<6322){ offset_dt=-0.2; offset_dt_end=-0.1;}
  if(runNo>6400 && runNo<6423){ offset_dt=-2.4; offset_dt_end=-1.9;}
  if(runNo>6514 && runNo<6668){ offset_dt=0; offset_dt_end=3.4;}
  if(runNo>6667 && runNo<6715){ offset_dt=4; offset_dt_end=7.2;}
  if(runNo>6715 && runNo<6720){ offset_dt=8; offset_dt_end=7.5;}
  if(runNo>6879 && runNo<6909){ offset_dt=0.2; offset_dt_end=-0.1;}
  if(m_ntupleflag && m_tuple3){
    g_bunchtime=8;
    g_t0offset_b=2.0;
    g_t0offset_e=2.0;
  }

  m_pass[0]++;
  if(m_evtNo==1 && m_pass[0]%1000 ==0){
    cout<<"------------------- Events-----------------: "<<m_pass[0]<<endl;
  }
  if(m_debug==4) cout<<"m_userawtime: "<<m_userawtime<<endl;
  if(m_debug==4) cout<<"EstTofCalibSvc est flag: "<<tofCaliSvc->ValidInfo()<<endl;
  if(tofCaliSvc->ValidInfo()==0&&m_userawtime_opt==0) 
  {
    log << MSG::ERROR << "EstTof Calibration Const is Invalid! " << endreq;
    return StatusCode::FAILURE;
  }
  if(tofCaliSvc->ValidInfo()==0||m_userawtime_opt==1) m_userawtime=1;
  else m_userawtime=0;

  SmartDataPtr<RecEsTimeCol> aRecestimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
  if (!aRecestimeCol || aRecestimeCol->size()==0) {
    if(m_debug==4) log << MSG::INFO << "Could not find RecEsTimeCol from fzsian" << endreq;
  }else{
    RecEsTimeCol::iterator it_evt = aRecestimeCol->begin();
    for(; it_evt!=aRecestimeCol->end(); it_evt++){
      Testime_fzisan = (*it_evt)->getTest();//yzhang add
      TestimeFlag_fzisan = (*it_evt)->getStat(); //yzhang add
      TestimeQuality_fzisan = (*it_evt)->getQuality(); //yzhang add

      log << MSG::INFO << "fzisan : Test = "<<(*it_evt)->getTest()
	<< ", Status = "<<(*it_evt)->getStat() <<endreq;

      if(m_ntupleflag && m_tuple2) g_Testime_fzisan = Testime_fzisan;
    }}


    std::string fullPath = "/Calib/EsTimeCal";
    SmartDataPtr<CalibData::EsTimeCalibData> TEst(m_pCalibDataSvc, fullPath);
    if(!TEst){ cout<<"ERROR EsTimeCalibData"<<endl;}
    else{
      int no = TEst->getTestCalibConstNo();
      // std::cout<<"no==========="<<no<<std::endl;
      // for(int i=0;i<no;i++){
      //   std::cout<<"i=  "<<i<<"  value="<< TEst->getTEstCalibConst(i)<<std::endl;
      // }
      log<<MSG::INFO<<"offset barrel t0="<< TEst->getToffsetb()
	<<", offset endcap t0="<< TEst->getToffsete()
	<<", bunch time ="<<TEst->getBunchTime()
	<<endreq;
      tOffset_b=TEst->getToffsetb();
      tOffset_e=TEst->getToffsete();
      bunchtime=TEst->getBunchTime();
    }
    if(m_userawtime) { 
      tOffset_b=0; 
      tOffset_e=0;
    } 
    else
    {
      toffset_rawtime=0;
      toffset_rawtime_e=0;
      offset_dt=0; 
      offset_dt_end=0;
    }

    if(runNo>0&&m_useTimeFactor)
    {
      bunchtime=RawDataUtil::TofTime(8*1024)*bunchtime/24.0;
    }

    if(runNo<0) bunchtime=m_bunchtime_MC;

    //Part 2: Retrieve MC truth 
    int digiId;
    if(runNo<0){
      SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
      if (!mcParticleCol) {
	log << MSG::INFO<< "Could not find McParticle" << endreq;
      }
      else{ 
	McParticleCol::iterator iter_mc = mcParticleCol->begin();
	digiId = 0;
	ntrkMC = 0;
	int charge = 0;

	for (;iter_mc != mcParticleCol->end(); iter_mc++, digiId++) {
	  int  statusFlags = (*iter_mc)->statusFlags();
	  int  pid = (*iter_mc)->particleProperty();
	  log << MSG::INFO 
	    << " MC ParticleId = " << pid
	    << " statusFlags = " << statusFlags
	    << " PrimaryParticle = " <<(*iter_mc)->primaryParticle()
	    << endreq;
	  if(m_ntupleflag && m_tuple2){
	    g_theta0MC[ntrkMC] = (*iter_mc)->initialFourMomentum().theta();
	    g_phi0MC[ntrkMC] = (*iter_mc)->initialFourMomentum().phi();
	    g_pxMC[ntrkMC] = (*iter_mc)->initialFourMomentum().px()/1000;
	    g_pyMC[ntrkMC] = (*iter_mc)->initialFourMomentum().py()/1000;
	    g_pzMC[ntrkMC] = (*iter_mc)->initialFourMomentum().pz()/1000;
	    g_ptMC[ntrkMC] = sqrt(((*iter_mc)->initialFourMomentum().px())*((*iter_mc)->initialFourMomentum().px())+((*iter_mc)->initialFourMomentum().py())*((*iter_mc)->initialFourMomentum().py()))/1000;
	  }
	  if( pid >0 ) { 
	    if(m_particleTable->particle( pid ))  charge = m_particleTable->particle( pid )->charge();
	  } else if ( pid <0 ) {
	    if(m_particleTable->particle( -pid )) {
	      charge = m_particleTable->particle( -pid )->charge();
	      charge *= -1;
	    }
	  } else {
	    log << MSG::WARNING << "wrong particle id, please check data" <<endreq;
	  }
	  log << MSG::DEBUG
	    << "MC ParticleId = " << pid << " charge = " << charge
	    << endreq;
	  if(charge !=0 && abs(cos((*iter_mc)->initialFourMomentum().theta()))<0.93){ 
	    ntrkMC++; 
	  }
	  if(((*iter_mc)->primaryParticle())&& m_ntupleflag && m_tuple2){
	    g_mcTestime=(*iter_mc)->initialPosition().t();
	    mcTestime=(*iter_mc)->initialPosition().t();
	  }
	}
	if(m_ntupleflag && m_tuple2)  g_ntrkMC = ntrkMC;
      } 
    }//close if(runno<0)
    if (m_debug)  cout<<"bunchtime: "<<bunchtime<<endl;

    SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
    if (!newtrkCol || newtrkCol->size()==0) { 
      log << MSG::INFO<< "Could not find RecMdcTrackCol" << endreq;
    } else{
      log << MSG::INFO << "Begin to check RecMdcTrackCol"<<endreq; 
      RecMdcTrackCol::iterator iter_trk = newtrkCol->begin();
      for( ; iter_trk != newtrkCol->end(); iter_trk++){
	log << MSG::DEBUG << "retrieved MDC track:"
	  << " Track Id: " << (*iter_trk)->trackId()
	  << " Phi0: " << (*iter_trk)->helix(0)
	  << " kappa: " << (*iter_trk)->helix(2) 
	  << " Tanl: " << (*iter_trk)->helix(4)
	  << " Phi terminal: "<< (*iter_trk)->getFiTerm()
	  << endreq
	  << "Number of hits: "<< (*iter_trk)->getNhits()
	  << "   Number of stereo hits " << (*iter_trk)->nster()
	  << endreq;
	double kappa = (*iter_trk)->helix(2);
	double tanl = (*iter_trk)->helix(4);
	momentum[ntot] = 1./fabs(kappa) * sqrt(1.+tanl*tanl);
	if((*iter_trk)->helix(3)>50.0)  continue;
	ntot++;
	if (ntot>15) break;
      }
    }

    //get EmcRec results
    int emctrk=0;
    SmartDataPtr<RecEmcShowerCol> aShowerCol(eventSvc(),"/Event/Recon/RecEmcShowerCol");
    if (!aShowerCol || aShowerCol->size()==0) { 
      log << MSG::WARNING << "Could not find RecEmcShowerCol" << endreq;
    } else{
      RecEmcShowerCol::iterator iShowerCol=aShowerCol->begin();
      for(;iShowerCol!=aShowerCol->end();iShowerCol++,emctrk++){
	if(emctrk>20) break;
	phiemc_rec[emctrk]=(*iShowerCol)->position().phi();
	thetaemc_rec[emctrk]=(*iShowerCol)->position().theta();
	energy_rec[emctrk]=(*iShowerCol)->energy();
	xemc_rec[emctrk]=(*iShowerCol)->x();
	yemc_rec[emctrk]=(*iShowerCol)->y();
	zemc_rec[emctrk]=(*iShowerCol)->z();
	module[emctrk]=(*iShowerCol)->module();
      }
    }
    for(int i=0; i<2; i++){
      double fi_endtof = atan2(yemc_rec[i],xemc_rec[i] );    // atna2 from -pi to pi
      if( fi_endtof<0 ) { fi_endtof=2*3.141593+fi_endtof; }
      if( module[i]==1 ) {
	int Tofid = (int)(fi_endtof/(3.141593/44));
	if(Tofid>87) Tofid=Tofid-88;
	tofid_emc[i]=Tofid;
	idmatch_emc[1][Tofid]=1;
      }
      else{
	if( useEtofScin ) {
	  int  Tofid =(int)(fi_endtof/(3.141593/24));
	  if( Tofid>47) Tofid=Tofid-48;
	  tofid_emc[i]= Tofid;
	  if(module[i]==2) idmatch_emc[2][Tofid]=1;
	  if(module[i]==0) idmatch_emc[0][Tofid]=1;
	}
	if( useEtofMRPC ) {
	  int Tofid = (int)(fi_endtof/(3.141593/18));
	  if( Tofid>35) Tofid=Tofid-36;
	  etfid_emc[i]= Tofid;
	  if(module[i]==2) idmatch_etf_emc[2][Tofid]=1;
	  if(module[i]==0) idmatch_etf_emc[0][Tofid]=1;
	}
      }
    }

    ntrk=0;
    if (ntot > 0) {    
      RecMdcTrackCol::iterator iter_trk = newtrkCol->begin();
      for( int idfztrk=0; iter_trk != newtrkCol->end(); iter_trk++,idfztrk++){
	double mdcftrk[5];
	mdcftrk[0] = (*iter_trk)->helix(0);
	mdcftrk[1] = (*iter_trk)->helix(1);
	mdcftrk[2] =-(*iter_trk)->helix(2);
	mdcftrk[3] = (*iter_trk)->helix(3);
	mdcftrk[4] = (*iter_trk)->helix(4);

	if(optCosmic==0){ 
	  Emc_helix EmcHit;
	  // EMC acceptance
	  EmcHit.pathlCut(Estparam.pathlCut()); 
	  // Set max. path length(cm)
	  // MDC --> EMC match

	  if (EmcHit.Emc_Get(sqrt(RCEMC2),idfztrk,mdcftrk) > 0){ 
	    double   z_emc    = EmcHit.Z_emc;
	    double   thetaemc_ext= EmcHit.theta_emc;
	    double   phiemc_ext  = EmcHit.phi_emc;

	    for(int t=0; t<emctrk;t++){
	      if((thetaemc_ext>=(thetaemc_rec[t]-0.1)) && (thetaemc_ext<=(thetaemc_rec[t]+0.1)) && (phiemc_ext>=(phiemc_rec[t]-0.1)) && (phiemc_ext<=(phiemc_rec[t]+0.1))){
		if((energy_rec[t])>=(0.85*momentum[idfztrk])) 
		  particleId[idfztrk]=1;
	      }
	    }
	  }
	  //get dE/dx results
	  if(particleId[idfztrk]!=1){

	    SmartDataPtr<RecMdcDedxCol> newdedxCol(eventSvc(),"/Event/Recon/RecMdcDedxCol");
	    if (!newdedxCol || newdedxCol->size()==0) { 
	      log << MSG::WARNING<< "Could not find RecMdcDedxCol" << endreq;
	    }
	    else{
	      RecMdcDedxCol::iterator it_dedx = newdedxCol->begin();
	      for( int npid=0; it_dedx != newdedxCol->end(); it_dedx++,npid++) {
		log << MSG::INFO << "retrieved MDC dE/dx: "
		  << "dEdx Id: " << (*it_dedx)->trackId()
		  << "   particle Id:      "<< (*it_dedx)->particleType() <<endreq;
		if((*it_dedx)->particleType() == proton){
		  particleId[npid]= 5;
		}
		if(m_debug==4) cout<<"dedx pid: "<<particleId[npid]<<endl;
	      }
	    }
	  }  
	}//if(optCosmic==0)

	idtof = -100;
	idetf = -100;
	TofFz_helix TofHit;

	// TOF acceptance
	TofHit.pathlCut(Estparam.pathlCut()); 
	// Set max. path length(cm)

	TofHit.ztofCut(Estparam.ztofCutmin(),Estparam.ztofCutmax()); // Set Ztof cut window (cm)
	// MDC --> TOF match
	int tofpart = TofHit.TofFz_Get(sqrt(RCTOF2),idfztrk,mdcftrk);
	if(tofpart < 0) continue; 
	//        if (TofHit.TofFz_Get(sqrt(RCTOF2),idfztrk,mdcftrk) < 0) continue;

	bool useBarrelScin = ( tofpart==1 );   // barrel
	bool useEndcapScin = ( ( tofpart==0 || tofpart==2 ) && useEtofScin ); // endcap for 96Scin and 92Scin+2MRPC;
	bool useEndcapMRPC = ( ( tofpart==0 || tofpart==2 ) && useEtofMRPC ); // endcap for 72MRPC and 92Scin+2MRPC

	if( useBarrelScin || useEndcapScin ) {
	  idtof  = TofHit.Tofid;
	  tofid_helix[idfztrk] = TofHit.Tofid;
	}
	if( useEndcapMRPC ) {
	  idetf  = TofHit.Etfid;
	  etfid_helix[idfztrk] = TofHit.Etfid;
	}

	log << MSG::INFO << "helix to tof hit part: "<<tofpart<<" tof id: "<< idtof << " etf id:" << idetf << endreq;
	if( m_debug==4 ) cout << "helix to tof hit part, Id: "<<tofpart<<" , "<< idtof <<endl;
	if( ( useBarrelScin && idtof>=0 && idtof<=87 ) || ( useEndcapScin && idtof>=0 && idtof<=47 ) || ( useEndcapMRPC && idetf>=0 && idetf<=35 ) ) { // barrel part: idtof:0~87; endcap part: idtof:0~47 (scintillator),idetf:0~35 (mrpc)

	  double abmom = 0.0;
	  if( useEndcapMRPC ) {
	    idetfmatch[tofpart][idetf]= 1; 
	    helpathetf[idetf]= TofHit.Path_etf;
	    helz[idetf]      = TofHit.Z_etf;
	    abmom  = 1./fabs(TofHit.Kappa) * sqrt(1.+TofHit.Tanl*TofHit.Tanl);
	    if(abmom<0.1) continue;
	    abmom2etf[idetf] = abmom*abmom; 
	    r_endetf[idfztrk]= TofHit.r_etf;
	    helzetf[idfztrk] =  helz[idetf];
	  }

	  if( useBarrelScin || useEndcapScin ) {
	    idmatch[tofpart][idtof] = 1; 
	    helpath[idtof] = TofHit.Pathl;
	    helz[idtof]    = TofHit.Z_tof;
	    abmom  = 1./fabs(TofHit.Kappa) * sqrt(1.+TofHit.Tanl*TofHit.Tanl);
	    if(abmom<0.1) continue;
	    abmom2[idtof] = abmom*abmom; 
	    r_endtof[idfztrk]= TofHit.r_endtof;
	    helztof[idfztrk] =  helz[idtof];
	  }

	  if( m_debug==4 ) {
	    cout << "Scintillator info   trk number=" << idfztrk << " tofpart=" << tofpart << " idtof=" << idtof << " helpath=" << helpath[idtof] << " helz=" << helz[idtof] << " abmom=" << abmom2[idtof] << " r=" << r_endtof[idfztrk] << " helztof=" << helz[idtof] << endl;
	    cout << "MRPC         info   trk number=" << idfztrk << " tofpart=" << tofpart << " idetf=" << idetf << " helpath=" << helpathetf[idetf] << " helz=" << helzetf[idetf] << " abmom=" << abmom2etf[idetf] << " r=" << r_endetf[idfztrk] << " helztof=" << helzetf[idetf] << endl;
	  }

	  double vel = 1.0e-6;
	  if( optCosmic==0 ) {

	    if( useEndcapMRPC ) {
	      if( particleId[idfztrk] == 1 ) {
		tetf[idfztrk] = sqrt(ELMAS2/abmom2etf[idetf]+1)* helpathetf[idetf]/VLIGHT;
		vel = VLIGHT/sqrt(ELMAS2/abmom2etf[idetf]+1);
	      }
	      else if( particleId[idfztrk] == 5 ) {
		tetf[idfztrk] = sqrt(PROTONMAS2/abmom2etf[idetf]+1)* helpathetf[idetf]/VLIGHT;
		vel = VLIGHT/sqrt(PROTONMAS2/abmom2etf[idtof]+1);
	      }
	      else {
		tetf[idfztrk] =  sqrt(PIMAS2/abmom2etf[idetf]+1)* helpathetf[idetf]/VLIGHT;
		vel = VLIGHT/sqrt(PIMAS2/abmom2etf[idtof]+1);
	      }
	    }

	    if( useBarrelScin || useEndcapScin ) {
	      if( particleId[idfztrk] == 1 ) {
		ttof[idfztrk] = sqrt(ELMAS2/abmom2[idtof]+1)* helpath[idtof]/VLIGHT;
		vel = VLIGHT/sqrt(ELMAS2/abmom2[idtof]+1);
	      }
	      else if( particleId[idfztrk] == 5 ) {
		ttof[idfztrk] = sqrt(PROTONMAS2/abmom2[idtof]+1)* helpath[idtof]/VLIGHT;
		vel = VLIGHT/sqrt(PROTONMAS2/abmom2[idtof]+1);
	      }
	      else {
		ttof[idfztrk] = sqrt(PIMAS2/abmom2[idtof]+1)* helpath[idtof]/VLIGHT; 
		vel = VLIGHT/sqrt(PIMAS2/abmom2[idtof]+1);
	      }
	    }

	  }
	  else{

	    if( useEndcapMRPC ) {
	      tetf[idfztrk] =  sqrt(MUMAS2/abmom2etf[idetf]+1)* helpathetf[idetf]/VLIGHT;
	      vel = VLIGHT/sqrt(MUMAS2/abmom2etf[idtof]+1);
	    }

	    if( useBarrelScin || useEndcapMRPC ) {
	      ttof[idfztrk] =  sqrt(MUMAS2/abmom2[idtof]+1)* helpath[idtof]/VLIGHT; 
	      vel = VLIGHT/sqrt(MUMAS2/abmom2[idtof]+1);
	    }
	  }

	  if(m_ntupleflag && m_tuple2){
	    g_vel[idfztrk] = vel;
	    g_abmom[idfztrk] = abmom;
	    if( useEndcapMRPC ) {
	      g_ttof[idfztrk] = tetf[idfztrk];
	    }
	    if( useBarrelScin || useEndcapScin ) {
	      g_ttof[idfztrk] = ttof[idfztrk];
	    }
	    g_pid[idfztrk] = particleId[idfztrk];
	  }
	}
	ntrk++;
      }
      if(m_ntupleflag && m_tuple2)  g_ntrk= ntrk;
    }

    //  C a l c u l a t e   E v t i m e
    // Retrieve TofMCHit truth 
    if(runNo<0){ 
      SmartDataPtr<TofMcHitCol> tofmcHitCol(eventSvc(),"/Event/MC/TofMcHitCol");
      if (!tofmcHitCol) {
	log << MSG::ERROR<< "Could not find McParticle" << endreq;
	//	return StatusCode::FAILURE;
      }
      else{
	TofMcHitCol::iterator iter_mctof = tofmcHitCol->begin();

	for (;iter_mctof !=tofmcHitCol->end(); iter_mctof++, digiId++) {
	  log << MSG::INFO 
	    << " TofMcHit Flight Time = " << (*iter_mctof)->getFlightTime()
	    << " zPosition = " << ((*iter_mctof)->getPositionZ())/10
	    << " xPosition = " <<((*iter_mctof)->getPositionX())/10
	    << " yPosition = " <<((*iter_mctof)->getPositionY())/10
	    << endreq;
	}
      }
    }

    ////choose cosmic
    TofDataVector tofDigiVec = m_rawDataProviderSvc->tofDataVectorEstime();
    //if(tofDigiVec.size()==0)  return StatusCode::SUCCESS;
    for(TofDataVector::iterator iter2 = tofDigiVec.begin();iter2 != tofDigiVec.end(); iter2++) {
      int barrelid;
      int layerid;
      int tofid;
      int strip;

      if( !( (*iter2)->is_mrpc() ) ) { // Scintillator
	if( (*iter2)->barrel() ) {     // Scintillator Barrel
	  barrelid = 1;
	  tofid = (*iter2)->tofId();
	  layerid = (*iter2)->layer();
	  if(layerid==1)  tofid=tofid-88;
	  if( ((*iter2)->quality() & 0x5)==0x5 && (*iter2)->times()==1 ) {
	    double ftdc = (*iter2)->tdc1();
	    double btdc = (*iter2)->tdc2();
	    mean_tdc_btof[layerid][tofid]=(ftdc+btdc)/2;
	  }
	  else if( ((*iter2)->quality() & 0x5)==0x5 && (*iter2)->times()>1 ) {
	    double ftdc = (*iter2)->tdc1();
	    double btdc = (*iter2)->tdc2();
	    mean_tdc_btof[layerid][tofid]=(ftdc+btdc)/2;
	  }
	}
	else{ // Scintillator Endcap
	  tofid = (*iter2)->tofId();
	  if(tofid<48) barrelid=0;
	  if(tofid>47) barrelid=2;
	  if(barrelid==2) tofid=tofid-48;
	  
	  if((*iter2)->times()==1){
	    double ftdc= (*iter2)->tdc();
	    mean_tdc_etof[barrelid][tofid]=ftdc;
	  }
	  else if(((*iter2)->times()>1) && ((*iter2)->times()<5)){
	    double ftdc= (*iter2)->tdc();
	    mean_tdc_etof[barrelid][tofid]=ftdc;
	  }
	}
      }
      else { // MRPC Endcap
	tofid = (*iter2)->tofId();
	strip = (*iter2)->strip();
	if( tofid<36 ) barrelid=0;
	if( tofid>35 ) barrelid=2;
	if(barrelid==2) tofid=tofid-36;
	if( ((*iter2)->quality() & 0x5)==0x5 && (*iter2)->times()==1 ) {
	  double ftdc = (*iter2)->tdc1();
	  double btdc = (*iter2)->tdc2();
	  mean_tdc_etf[barrelid][tofid][strip]=(ftdc+btdc)/2;
	}
	else if( ((*iter2)->quality()&0x5)==0x5 && (*iter2)->times()>1 ) {
	  double ftdc = (*iter2)->tdc1();
	  double btdc = (*iter2)->tdc2();
	  mean_tdc_etf[barrelid][tofid][strip]=(ftdc+btdc)/2;
	}
      }
    }

    double  difftof_b=100, difftof_e=100;
    int tofid1=tofid_emc[0];
    int tofid2=tofid_emc[1];
    if( module[0]==1 && module[1]==1 ) {
      for(int i=0; i<2; i++){
	for(int m=0; m<2; m++){
	  for(int j=-2; j<3; j++){
	    for(int k=-2; k<3; k++){
	      int p=tofid1+j;
	      int q=tofid2+k;
	      if(p<0) p=p+88;
	      if(p>87) p=p-88;
	      if(q<0) q=q+88;
	      if(q>87) q=q+88;
	      if(mean_tdc_btof[i][p]==0 || mean_tdc_btof[m][q]==0) continue;
	      double difftof_b_temp = mean_tdc_btof[i][p]-mean_tdc_btof[m][q];
	      if(abs(difftof_b_temp)<abs(difftof_b ))  difftof_b =difftof_b_temp;
	      if(m_ntupleflag && m_tuple2) g_difftof_b=difftof_b;
	    }
	  }
	}
      }
    }

    if( useEtofMRPC ) {
      if( module[0]!=1 && module[1]!=1 ) {
	tofid1 = etfid_emc[0];
	tofid2 = etfid_emc[1];
	for(int i=-1; i<2; i++){
	  for(int j=-1; j<2; j++){
	    int m=tofid1+i;
	    int n=tofid2+j;
	    if(m<0)  m=36+m;
	    if(m>35) m=m-36;
	    if(n<0)  n=36+n;
	    if(n>35) n=n-36;
	    if( mean_tdc_etf[0][m] && mean_tdc_etf[2][n]){
	      double difftof_e_temp= mean_tdc_etf[0][m]-mean_tdc_etf[2][n];
	      if(abs(difftof_e_temp) < abs(difftof_e))  difftof_e= difftof_e_temp; 
	      if(m_ntupleflag && m_tuple2) g_difftof_e=difftof_e;
	    } 
	  }
	}
      }
    }

    if( useEtofScin ) {
      if( module[0]!=1 && module[1]!=1 ) {
	for(int i=-1; i<2; i++){
	  for(int j=-1; j<2; j++){
	    int m=tofid1+i;
	    int n=tofid2+j;
	    if(m<0)  m=48+m;
	    if(m>47) m=m-48;
	    if(n<0)  n=48+n;
	    if(n>47) n=n-48;
	    if( mean_tdc_etof[0][m] && mean_tdc_etof[2][n]){
	      double difftof_e_temp= mean_tdc_etof[0][m]-mean_tdc_etof[2][n];
	      if(abs(difftof_e_temp) < abs(difftof_e))  difftof_e= difftof_e_temp; 
	      if(m_ntupleflag && m_tuple2) g_difftof_e=difftof_e;
	    } 
	  }
	}
      }
    }

    if(m_optCosmic==1) optCosmic=1;
    else optCosmic=0;

    digiId = 0; 
    unsigned int tofid;
    unsigned int barrelid;
    unsigned int layerid;
    t0forward_add = 0; 
    t0backward_add = 0;
    TofDataVector::iterator iter2 = tofDigiVec.begin();
    for (;iter2 != tofDigiVec.end(); iter2++, digiId++){
      log << MSG::INFO << "TOF digit No: " << digiId << endreq;
      barrelid=(*iter2)->barrel();
      if((*iter2)->barrel()==0) continue; 
      if( ((*iter2)->quality() & 0x5)==0x5 && (*iter2)->times()==1 ) {   // tof_flag=1
	tofid = (*iter2)->tofId();
	layerid = (*iter2)->layer();
	if(layerid==1)  tofid=tofid-88;
	log<< MSG::INFO
	   <<" TofId = "<<tofid
	   <<" barrelid = "<<barrelid
	   <<" layerid = "<<layerid
	   <<" ForwordADC =  "<<(*iter2)->adc1()
	   <<" ForwordTDC =  "<<(*iter2)->tdc1() 
	   <<" BackwordADC =  "<<(*iter2)->adc2()
	   <<" BackwordTDC =  "<<(*iter2)->tdc2()
	   << endreq; 
	//forb=0/1 for forward(z>0, east) and backward(z<0, west)
	double ftdc = (*iter2)->tdc1();
	double btdc = (*iter2)->tdc2();
	if(m_debug==4) cout<<"barrel 1 ::layer, barrel, tofid, ftdc, btdc: "<<layerid<<" , "<<barrelid<<" , "<<tofid<<" , "<<ftdc<<" , "<<btdc<<endl;
	double fadc = (*iter2)->adc1(); 
	double badc = (*iter2)->adc2();
	int idptof = ((tofid-1) == -1) ? 87 : tofid-1;
	int idntof = ((tofid+1) == 88) ? 0 : tofid+1;
	double ztof = fabs((ftdc-btdc)/2)*17.7 ,      ztof2 = ztof*ztof;    
	double mean_tdc = 0.5*(btdc+ftdc);
	double cor_path = sqrt(RCTOF2+ztof2)/VLIGHT;
	
	if(idmatch[barrelid][tofid]==1||idmatch[barrelid][idptof]==1||idmatch[barrelid][idntof]==1){
	  for(int i=0;i<=ntot;i++){
	    if(ttof[i]!=0 && ftdc>0){
	      if((tofid_helix[i] == tofid) || (tofid_helix[i] == idntof) ||(tofid_helix[i] == idptof)) {
		if(barrelid==1 && helztof[i]<117 && helztof[i]>-117 ){
		  if (optCosmic && tofid<44) {
		    backevtime =  -ttof[i] + (115 + helztof[i])*0.0566 + 12.;
		    forevtime  =  -ttof[i] + (115 - helztof[i])*0.0566 + 12.;
		    meantevup[ntofup]=(backevtime+forevtime)/2;
		    ntofup++;
		  }
		  else{
		    backevtime =  ttof[i] + (115 + helztof[i])*0.0566 + 12.;
		    forevtime  =  ttof[i] + (115 - helztof[i])*0.0566 + 12.; 
		    meantevdown[ntofdown]=(backevtime+forevtime)/2;
		    ntofdown++;
		  }
		  if( (*iter2)->adc1()<0.0 || (*iter2)->adc2()<0.0 || m_userawtime){
		    t0forward_trk  = ftdc - forevtime ;
		    t0backward_trk = btdc - backevtime ;
		  }
		  else{
		    t0forward_trk = tofCaliSvc->BTime1((*iter2)->adc1(), (*iter2)->tdc1(),helztof[i], (*iter2)->tofId())-ttof[i];
		    t0backward_trk = tofCaliSvc->BTime2((*iter2)->adc2(), (*iter2)->tdc2(),helztof[i], (*iter2)->tofId())-ttof[i];
		    if (optCosmic && tofid<44) {
		      t0forward_trk = tofCaliSvc->BTime1((*iter2)->adc1(), (*iter2)->tdc1(),helztof[i], (*iter2)->tofId())+ttof[i];
		      t0backward_trk = tofCaliSvc->BTime2((*iter2)->adc2(), (*iter2)->tdc2(),helztof[i], (*iter2)->tofId())+ttof[i];
		    }
		  }

		  if(t0forward_trk<-3 || t0backward_trk<-3 || fabs(t0forward_trk-t0backward_trk)>15.0) continue;
		  if(!m_TofOpt&& nmatch_barrel!=0 && fabs((t0forward_trk+t0backward_trk)/2-(t0backward_add+t0forward_add)/2/nmatch_barrel)>11) continue;
		  if(m_debug ==4 ) cout<<" t0forward_trk, t0backward_trk: "<<t0forward_trk<<" , "<<t0backward_trk<<endl; 
		  if(m_ntupleflag && m_tuple2){	
		    g_t0for[nmatch1] = t0forward_trk ; 
		    g_t0back[nmatch2] = t0backward_trk ;
		    g_meantdc=(ftdc+btdc)/2;
		    if(tofid<44) g_ntofup1++;
		    else  g_ntofdown1++;	
		  }
		  meantev[nmatch]=(backevtime+forevtime)/2;
		  t0forward_add += t0forward_trk;
		  t0backward_add += t0backward_trk;
		  if(nmatch>499) break;
		  Tof_t0Arr[nmatch]=(t0forward_trk+t0backward_trk)/2.0;
		  nmatch++;
		  nmatch1=nmatch1+1;
		  nmatch2=nmatch2+1;
		  nmatch_barrel++;
		}
	      }
	    }
	  }
	}
      }
    }
    
    if(nmatch_barrel != 0  ){
      if(m_ntupleflag && m_tuple2){
	g_t0barrelTof=( t0forward_add/nmatch_barrel + t0backward_add/nmatch_barrel)/2;
      }
      tof_flag = 1; 
      t_quality=1;
    }  

    if(nmatch_barrel==0){
      digiId = 0;
      t0forward_add = 0;
      t0backward_add = 0;
      for (TofDataVector::iterator iter2 = tofDigiVec.begin();iter2 != tofDigiVec.end(); iter2++, digiId++) {
	log << MSG::INFO << "TOF digit No: " << digiId << endreq;
	barrelid=(*iter2)->barrel();
	if((*iter2)->barrel()==0) continue;
	if( ((*iter2)->quality() & 0x5)==0x5 && (*iter2)->times()>1 ) {   // tof_flag=2
	  tofid = (*iter2)->tofId();
	  layerid = (*iter2)->layer();
	  if(layerid==1)  tofid=tofid-88;
	  log<< MSG::INFO
	     <<" TofId = "<<tofid
	     <<" barrelid = "<<barrelid
	     <<" layerid = "<<layerid
	     <<" ForwordADC =  "<<(*iter2)->adc1()
	     <<" ForwordTDC =  "<<(*iter2)->tdc1()
	     <<endreq;
	  double ftdc=  (*iter2)->tdc1();
	  double btdc=  (*iter2)->tdc2();
	  double fadc=  (*iter2)->adc1();
	  double badc=  (*iter2)->adc2();
	  if(m_debug==4) cout<<"barrel 2 ::layer, barrel, tofid, ftdc, btdc: "<<layerid<<" , "<<barrelid<<" , "<<tofid<<" , "<<ftdc<<" , "<<btdc<<endl;
	  int idptof = ((tofid-1) == -1) ? 87 : tofid-1;
	  int idntof = ((tofid+1) == 88) ? 0 : tofid+1;
	  if(idmatch[barrelid][tofid]==1||idmatch[barrelid][idptof]==1||idmatch[barrelid][idntof]==1){
	    for(int i=0;i<=ntot;i++){
	      if(ttof[i]!=0 && ftdc>0){
		if(tofid_helix[i] == tofid ||(tofid_helix[i] == idptof)||(tofid_helix[i] == idntof)){
		  if(barrelid==1 && helztof[i]<117 && helztof[i]>-117 ){
		    if (optCosmic && tofid<44) {
		      backevtime =  -ttof[i] + (115 + helztof[i])*0.0566 + 12.;
		      forevtime  =  -ttof[i] + (115 - helztof[i])*0.0566 + 12.;
		      meantevup[ntofup]=(backevtime+forevtime)/2;
		      ntofup++;
		    }
		    else{
		      backevtime =  ttof[i] + (115 + helztof[i])*0.0566 + 12.;
		      forevtime  =  ttof[i] + (115 - helztof[i])*0.0566 + 12.;
		      meantevdown[ntofdown]=(backevtime+forevtime)/2;
		      ntofdown++;
		    }
		    if( (*iter2)->adc1()<0.0 || (*iter2)->adc2()<0.0 || m_userawtime){
		      t0forward_trk  = ftdc - forevtime ;
		      t0backward_trk = btdc - backevtime ;
		    }
		    else{
		      t0forward_trk = tofCaliSvc->BTime1((*iter2)->adc1(), (*iter2)->tdc1(),helztof[i], (*iter2)->tofId())-ttof[i];
		      t0backward_trk = tofCaliSvc->BTime2((*iter2)->adc2(), (*iter2)->tdc2(),helztof[i], (*iter2)->tofId())-ttof[i];
		      if (optCosmic && tofid<44) {
			t0forward_trk = tofCaliSvc->BTime1((*iter2)->adc1(), (*iter2)->tdc1(),helztof[i], (*iter2)->tofId())+ttof[i];
			t0backward_trk = tofCaliSvc->BTime2((*iter2)->adc2(), (*iter2)->tdc2(),helztof[i], (*iter2)->tofId())+ttof[i];
		      }
		    }

		    if(t0forward_trk<-3 || t0backward_trk<-3 || fabs(t0forward_trk-t0backward_trk)>15.0) continue;
		    if(!m_TofOpt&&nmatch_barrel!=0 && fabs((t0forward_trk+t0backward_trk)/2-(t0backward_add+t0forward_add)/2/nmatch_barrel)>11) continue;
		    if(m_debug == 4) cout<<"t0forward_trk, t0backward_trk: "<<t0forward_trk<<" , "<<t0backward_trk<<endl;
		    if(m_ntupleflag && m_tuple2){
		      g_t0for[nmatch1] = t0forward_trk ;
		      g_t0back[nmatch2] = t0backward_trk ;
		      g_meantdc=(ftdc+btdc)/2;
		      if(tofid<44) g_ntofup1++;
		      else  g_ntofdown1++;
		    }
		    meantev[nmatch]=(backevtime+forevtime)/2;
		    t0forward_add += t0forward_trk;
		    t0backward_add += t0backward_trk;
		    if(nmatch>499) break;
		    Tof_t0Arr[nmatch]=(t0forward_trk+t0backward_trk)/2.0;
		    nmatch++;
		    nmatch1=nmatch1+1;
		    nmatch2=nmatch2+1;
		    nmatch_barrel++;
		  }
		}
	      }
	    }
	  }
	}
      }//close 2nd for loop -> only barrel part
      if(nmatch_barrel)  tof_flag=2;
    }

    if(ntot==0 || nmatch_barrel==0) {
      for (TofDataVector::iterator iter2 = tofDigiVec.begin();iter2 != tofDigiVec.end(); iter2++, digiId++) {
	log << MSG::INFO << "TOF digit No: " << digiId << endreq;
	barrelid=(*iter2)->barrel();
	if((*iter2)->barrel()==0) continue;
	if( ((*iter2)->quality() & 0x5)==0x5 && (*iter2)->times()==1 ) {   // tof_flag=3
	  tofid = (*iter2)->tofId();
	  layerid = (*iter2)->layer();
	  if(layerid==1)  tofid=tofid-88;
	  log<< MSG::INFO
	     <<" TofId = "<<tofid
	     <<" barrelid = "<<barrelid
	     <<" layerid = "<<layerid
	     <<" ForwordADC =  "<<(*iter2)->adc1()
	     <<" ForwordTDC =  "<<(*iter2)->tdc1()
	     <<endreq;
	  double ftdc=  (*iter2)->tdc1();
	  double btdc=  (*iter2)->tdc2();
	  double fadc=  (*iter2)->adc1();
	  double badc=  (*iter2)->adc2();
	  if(m_debug==4) cout<<"barrel 3 ::layer, barrel, tofid, ftdc, btdc: "<<layerid<<" , "<<barrelid<<" , "<<tofid<<" , "<<ftdc<<" , "<<btdc<<endl;
	  int idptof = ((tofid-1) == -1) ? 87 : tofid-1;
	  int idntof = ((tofid+1) == 88) ? 0 : tofid+1;
	  for(int i=0; i<2; i++){
	    if(tofid_emc[i] == tofid || tofid_emc[i] == idptof || tofid_emc[i] == idntof){
	      if(zemc_rec[0]||zemc_rec[1]){
		if(tofid ==tofid_emc[i] || tofid_emc[i]==idntof || tofid_emc[i]==idptof){
		  if(ftdc>2000.|| module[i]!=1) continue;   
		  ttof[i]= sqrt(ELMAS2/(m_ebeam*m_ebeam)+1)* sqrt(xemc_rec[i]*xemc_rec[i]+yemc_rec[i]*yemc_rec[i]+zemc_rec[i]*zemc_rec[i])/VLIGHT;
		  if(optCosmic==1 && tofid<44){
		    backevtime =  -ttof[i] + (115 + zemc_rec[i])*0.0566 + 12.;
		    forevtime  =  -ttof[i] + (115 - zemc_rec[i])*0.0566 + 12.; 
		    meantevup[ntofup]=(backevtime+forevtime)/2;
		    ntofup++;
		  }
		  else{
		    backevtime =  ttof[i] + (115 + zemc_rec[i])*0.0566 + 12.;
		    forevtime  =  ttof[i] + (115 - zemc_rec[i])*0.0566 + 12.;
		    meantevdown[ntofdown]=(backevtime+forevtime)/2;
		    ntofdown++;
		  }
		  if( (*iter2)->adc1()<0.0 || (*iter2)->adc2()<0.0 || m_userawtime){
		    t0forward_trk=ftdc-forevtime;
		    t0backward_trk=btdc-backevtime;
		  }
		  else{
		    t0forward_trk = tofCaliSvc->BTime1((*iter2)->adc1(), (*iter2)->tdc1(),helztof[i], (*iter2)->tofId())-ttof[i];
		    t0backward_trk = tofCaliSvc->BTime2((*iter2)->adc2(), (*iter2)->tdc2(),helztof[i], (*iter2)->tofId())-ttof[i];
		    if (optCosmic && tofid<44) {
		      t0forward_trk = tofCaliSvc->BTime1((*iter2)->adc1(), (*iter2)->tdc1(),helztof[i], (*iter2)->tofId())+ttof[i];
		      t0backward_trk = tofCaliSvc->BTime2((*iter2)->adc2(), (*iter2)->tdc2(),helztof[i], (*iter2)->tofId())+ttof[i];
		    }
		  }

		  if(t0forward_trk<-1 || t0backward_trk<-1 || fabs(t0forward_trk-t0backward_trk)>15.0) continue;
		  if(!m_TofOpt&&nmatch_barrel!=0 && fabs((t0forward_trk+t0backward_trk)/2-(t0backward_add+t0forward_add)/2/nmatch_barrel)>11) continue;
		  if(m_debug == 4) cout<<"t0forward_trk, t0backward_trk: "<<t0forward_trk<<" , "<<t0backward_trk<<endl;
		  meantev[nmatch]=(backevtime+forevtime)/2;
		  t0forward_add += t0forward_trk;
		  t0backward_add += t0backward_trk;
		  if(nmatch>499) break;
		  Tof_t0Arr[nmatch]=(t0forward_trk+t0backward_trk)/2.0;
		  nmatch++;
		  nmatch_barrel++;
		  emcflag1=1;
		}
	      }
	    }
	  }
	}
      }//3rd for loop only barrel part
      if(nmatch_barrel)  tof_flag=3;
    }

    if( nmatch_barrel != 0 ) { //only matched barrel tracks
      t0forward = t0forward_add/nmatch_barrel;
      t0backward = t0backward_add/nmatch_barrel;
      if(optCosmic==0){
	if(m_TofOpt)
	  {
	    t_Est=EST_Trimmer(Opt_new(Tof_t0Arr,nmatch,m_TofOpt_Cut),tOffset_b,toffset_rawtime,offset_dt,bunchtime);
	  }
	else t_Est=EST_Trimmer((t0forward+t0backward)/2,tOffset_b,toffset_rawtime,offset_dt,bunchtime);
	if(t_Est<0) t_Est=0;
	if(tof_flag==1) tEstFlag=111;
	else if(tof_flag==2) tEstFlag=121;
	else if(tof_flag==3) tEstFlag=131;
      }
      if(optCosmic){
	t_Est=(t0forward+t0backward)/2;//3. yzhang add tOffset_b for barrel cosmic
	if(tof_flag==1) tEstFlag=211;
	else if(tof_flag==2) tEstFlag=221;
	else if(tof_flag==3) tEstFlag=231;
      }
      if(m_ntupleflag && m_tuple2)  g_meant0=(t0forward+t0backward)/2;
    }//close if(nmatch_barrel !=0)  


    digiId = 0; 
    t0forward_add = 0; 
    t0backward_add = 0;
    
    if(nmatch_barrel==0){
      for (TofDataVector::iterator iter2 = tofDigiVec.begin();iter2 != tofDigiVec.end(); iter2++, digiId++) {
	log << MSG::INFO << "TOF digit No: " << digiId << endreq;
	barrelid=(*iter2)->barrel();
	if((*iter2)->barrel()==0) continue; 
	if(((*iter2)->quality() & 0x5) ==0x4){   // tEstFlag=241
	  tofid = (*iter2)->tofId();
	  layerid = (*iter2)->layer();
	  if(layerid==1)  tofid=tofid-88;
	  log<< MSG::INFO
	     <<" TofId = "<<tofid
	     <<" barrelid = "<<barrelid
	     <<" layerid = "<<layerid
	     <<" ForwordADC =  "<<(*iter2)->adc1()
	     <<" ForwordTDC =  "<<(*iter2)->tdc1() 
	     <<endreq; 
	  //forb=0/1 for forward(z>0, east) and backward(z<0, west)
	  double ftdc=  (*iter2)->tdc1();
	  double fadc=  (*iter2)->adc1(); 
	  if(m_debug==4) cout<<"barrel 4 ::layer, barrel, tofid, ftdc: "<<layerid<<" , "<<barrelid<<" , "<<tofid<<" , "<<ftdc<<endl;
	  int idptof = ((tofid-1) == -1) ? 87 : tofid-1;
	  int idntof = ((tofid+1) == 88) ? 0 : tofid+1;
	  if(idmatch[barrelid][tofid]==1||idmatch[barrelid][idptof]==1||idmatch[barrelid][idntof]==1){
	    for(int i=0;i<=ntot;i++){
	      if(ttof[i]!=0 && ftdc>0){
		if(tofid_helix[i] == tofid ||(tofid_helix[i] == idptof) || (tofid_helix[i] == idntof)){
		  if(barrelid==1 && helztof[i]<117 && helztof[i]>-117 ){
		    if (optCosmic && tofid<44) {
		      forevtime  =  -ttof[i] + (115 - helztof[i])*0.0566 + 12.;
		      meantevup[ntofup]=forevtime;
		      ntofup++;
		    }
		    else{
		      forevtime  =  ttof[i] + (115 - helztof[i])*0.0566 + 12.;  
		      meantevdown[ntofdown]=forevtime;
		      ntofdown++;
		    }  
		    if( (*iter2)->adc1()<0.0 || m_userawtime){
		      t0forward_trk  = ftdc - forevtime ;
		    }
		    else{
		      t0forward_trk = tofCaliSvc->BTime1((*iter2)->adc1(), (*iter2)->tdc1(),helztof[i], (*iter2)->tofId())-ttof[i];
		    }

		    if(t0forward_trk<-1) continue;
		    if(!m_TofOpt&&nmatch_barrel_1!=0 && fabs((t0forward_trk)-(t0forward_add)/nmatch_barrel_1)>11) continue;
		    if(m_debug == 4) cout<<"t0forward_trk: "<<t0forward_trk<<endl;
		    if(m_ntupleflag && m_tuple2){	
		      g_t0for[nmatch1] = t0forward_trk ; 
		      g_meantdc=ftdc;
		      if(tofid<44) g_ntofup1++;
		      else  g_ntofdown1++;	
		    }
		    meantev[nmatch]=forevtime;
		    t0forward_add += t0forward_trk;
		    //t0v.push_back(t0forward_trk);
		    if(nmatch>499) break;
		    Tof_t0Arr[nmatch]=t0forward_trk;
		    nmatch++;
		    nmatch1++;
		    nmatch_barrel_1++;
		  }
		}
	      }
	    }
	  }
	}
	else if(((*iter2)->quality() & 0x5) ==0x1){   // tEstFlag=241
	  tofid = (*iter2)->tofId();
	  layerid = (*iter2)->layer();
	  if(layerid==1)  tofid=tofid-88;
	  log<< MSG::INFO
	     <<" TofId = "<<tofid
	     <<" barrelid = "<<barrelid
	     <<" layerid = "<<layerid
	     <<" BackwordADC =  "<<(*iter2)->adc2()
	     <<" BackwordTDC =  "<<(*iter2)->tdc2()
	     << endreq; 
	  //forb=0/1 for forward(z>0, east) and backward(z<0, west)
	  double btdc=  (*iter2)->tdc2();
	  if(m_debug==4) cout<<"barrel 5 ::layer, barrel, tofid, btdc: "<<layerid<<" , "<<barrelid<<" , "<<tofid<<" , "<<btdc<<endl;
	  double badc=  (*iter2)->adc2();
	  int idptof = ((tofid-1) == -1) ? 87 : tofid-1;
	  int idntof = ((tofid+1) == 88) ? 0 : tofid+1;   
	  if(idmatch[barrelid][tofid]==1||idmatch[barrelid][idptof]==1||idmatch[barrelid][idntof]==1){
	    for(int i=0;i<=ntot;i++){
	      if(ttof[i]!=0 && btdc>0){
		if((tofid_helix[i] == tofid) || (tofid_helix[i] == idntof) ||(tofid_helix[i] == idptof)){
		  if(barrelid==1 && helztof[i]<117 && helztof[i]>-117 ){
		    if (optCosmic && tofid<44) {
		      backevtime =  -ttof[i] + (115 + helztof[i])*0.0566 + 12.;
		      meantevup[ntofup]=backevtime;
		      ntofup++;
		    }
		    else{
		      backevtime =  ttof[i] + (115 + helztof[i])*0.0566 + 12.;
		      meantevdown[ntofdown]=backevtime;
		      ntofdown++;
		    }  
		    
		    if( (*iter2)->adc2()<0.0 || m_userawtime){
		      t0backward_trk = btdc - backevtime ;
		    }
		    else{
		      t0backward_trk = tofCaliSvc->BTime2((*iter2)->adc2(), (*iter2)->tdc2(),helztof[i], (*iter2)->tofId())-ttof[i];
		    }

		    if(t0backward_trk<-1) continue;
		    if(!m_TofOpt&&nmatch_barrel_2!=0 && fabs((t0backward_trk)-(t0backward_add)/nmatch_barrel_2)>11) continue;
		    if(m_debug == 4) cout<<"t0backward_trk: "<<t0backward_trk<<endl;
		    if(m_ntupleflag && m_tuple2){	
		      g_t0back[nmatch2] = t0backward_trk ;
		      g_meantdc=btdc;
		      if(tofid<44) g_ntofup1++;
		      else  g_ntofdown1++;	
		    }
		    meantev[nmatch]=backevtime;
		    t0backward_add += t0backward_trk;
		    if(nmatch>499) break;
		    Tof_t0Arr[nmatch]=t0backward_trk;
		    nmatch++;
		    nmatch2++;
		    nmatch_barrel_2++;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    if(nmatch_barrel_1 != 0 ){
      t0forward = t0forward_add/nmatch_barrel_1;
      if(optCosmic==0){
	if(m_TofOpt)
	  {
	    t_Est=EST_Trimmer(Opt_new(Tof_t0Arr,nmatch,m_TofOpt_Cut),tOffset_b,toffset_rawtime,offset_dt,bunchtime);
	  }
	else t_Est=EST_Trimmer(t0forward,tOffset_b,toffset_rawtime,offset_dt,bunchtime);
	if(t_Est<0) t_Est=0;
	tEstFlag=141;
      }
      else{
	t_Est=t0forward;//5. yzhang add for nmatch_barrel_1 cosmic 
	tEstFlag=241;
      }
      if(m_ntupleflag && m_tuple2)  g_meant0=t0forward;
    }  
    if(nmatch_barrel_2 != 0  ){
      t0backward = t0backward_add/nmatch_barrel_2;
      if(optCosmic==0){
	if(m_TofOpt)
	  {
	    t_Est=EST_Trimmer(Opt_new(Tof_t0Arr,nmatch,m_TofOpt_Cut),tOffset_b,toffset_rawtime,offset_dt,bunchtime);
	  }
	else t_Est=EST_Trimmer(t0backward,tOffset_b,toffset_rawtime,offset_dt,bunchtime);
	if(t_Est<0) t_Est=0;
	tEstFlag=141;
      }
      else{
	t_Est=t0backward;//7. yzhang add tOffset_b for nmatch_barrel_2 cosmic
	tEstFlag=241;
      }
      if(m_ntupleflag && m_tuple2)  g_meant0=t0backward;
    }  

    digiId = 0; 
    t0forward_add = 0; 
    t0backward_add = 0;
    if(nmatch_barrel==0){
      for (TofDataVector::iterator iter2 = tofDigiVec.begin();iter2 != tofDigiVec.end(); iter2++, digiId++) {
	log << MSG::INFO << "TOF digit No: " << digiId << endreq;
	barrelid=(*iter2)->barrel();
	if((*iter2)->barrel()!=0) continue; 
	if((*iter2)->times()!=1) continue;
	tofid = (*iter2)->tofId();
	// Scintillator Endcap TOF
	if( !( (*iter2)->is_mrpc() ) ) {
	  if( tofid<48 ) { barrelid=0; }
	  if( tofid>47 ) { barrelid=2; }
	  if( barrelid==2 ) { tofid=tofid-48; }
	}
	// MRPC Endcap TOF
	else if( (*iter2)->is_mrpc() ) {
	  if( ( TofID::endcap( Identifier( (*iter2)->identify() ) ) ) == 0 ) { barrelid=0; }
	  else if( ( TofID::endcap( Identifier( (*iter2)->identify() ) ) ) == 1 ) { barrelid=2; }
	  if( barrelid==2 ) { tofid=tofid-36; }
	}
	  
	log<< MSG::INFO
	   <<" is_mrpc = " << (*iter2)->is_mrpc()
	   <<" TofId = "<< tofid
	   <<" barrelid = "<< barrelid
	   <<endreq
	   <<" ForwordADC =  "<< (*iter2)->adc()
	   <<" ForwordTDC =  "<< (*iter2)->tdc()
	   << endreq;
	double ftdc=  (*iter2)->tdc();
	double fadc=  (*iter2)->adc();
	if(m_debug ==4)  cout<<"endcap::single hit,barrelid,tofid,tdc: "<<barrelid<<" , "<<tofid<<" , "<<ftdc<<endl;

	// Scintillator Endcap TOF
	if( !( (*iter2)->is_mrpc() ) && useEtofScin ) {
	  int idptof = ((tofid-1) == -1) ? 47 : tofid-1;
	  int idntof = ((tofid+1) == 48) ? 0 : tofid+1;
	  // Collision Case
	  if(idmatch[barrelid][tofid]==1||idmatch[barrelid][idptof]==1||idmatch[barrelid][idntof]==1){
	    for(int i=0;i<=ntot;i++){
	      if(ttof[i]!=0 && ftdc>0 && ftdc<2000.){
		if((tofid_helix[i] == tofid) ||(tofid_helix[i] == idntof) ||(tofid_helix[i] == idptof)){
		  if( barrelid==0 || barrelid==2 ){
		    if( r_endtof[i]>=41 && r_endtof[i]<=90 ) {
		      if( optCosmic && ( tofid<24 || ( tofid>48 && tofid<71 ) ) ) { 
			forevtime = -ttof[i] + r_endtof[i]*0.09 + 12.2;
			meantevup[ntofup] = forevtime;
			ntofup++;
		      }
		      else {
			forevtime = ttof[i] + r_endtof[i]*0.09 + 12.2;
			meantevdown[ntofdown] = forevtime;
			ntofdown++;
		      }
		      if( (*iter2)->adc()<0.0 || m_userawtime){
			t0forward_trk = ftdc - forevtime;
		      }
		      else{
			t0forward_trk = tofCaliSvc->ETime((*iter2)->adc(), (*iter2)->tdc(),r_endtof[i], (*iter2)->tofId())-ttof[i];
		      }

		      if(t0forward_trk<-1.) continue;
		      if( !m_TofOpt && nmatch_end!=0 && fabs( t0forward_trk - t0forward_add/nmatch_end)>9 ) continue;
		      t0forward_add  += t0forward_trk;
		      if(nmatch>499) break;
		      Tof_t0Arr[nmatch]=t0forward_trk;
		      meantev[nmatch]=forevtime/2;
		      nmatch++;
		      nmatch_end++; 
		    }
		  }
		}
		if( m_debug==4 ) { cout << "t0forward_trk:" << t0forward_trk << endl; }
	      }
	    }
	  }
	}
	// MRPC Endcap TOF
	if( (*iter2)->is_mrpc() && useEtofMRPC ) {
	  if( ((*iter2)->quality() & 0x5)!=0x5 ) continue;
	  double btdc=  (*iter2)->tdc2();
	  double badc=  (*iter2)->adc2();
	  int idptof = ((tofid-1) == -1) ? 35 : tofid-1;
	  int idntof = ((tofid+1) == 36) ? 0 : tofid+1;	  
	  // B e a m   d a t a   c a s e    <--- Implement for test!
	  if( idetfmatch[barrelid][tofid]==1 || idetfmatch[barrelid][idptof]==1 || idetfmatch[barrelid][idntof]==1 ) {
	    for( int i=0; i<=ntot; i++ ) {
	      if( tetf[i]!=0 && ftdc>0 && ftdc<2000.) {
		if( etfid_helix[i]==tofid || etfid_helix[i]==idntof || etfid_helix[i] == idptof ) {
		  if( barrelid==0 || barrelid==2 ) {
		    if( r_endetf[i]>=41 && r_endetf[i]<=90 ) {
		      if( optCosmic && ( tofid<18 || ( tofid>35 && tofid<54 ) ) ) {
			forevtime  = -tetf[i] + 17.7;
			meantevup[ntofup] = forevtime;
			ntofup++;
		      }
		      else {
			forevtime  = tetf[i] + 17.7;
			meantevdown[ntofdown] = forevtime;
			ntofdown++;
		      }
		      if( m_userawtime ) {
			double fbtdc = ( ftdc + btdc )/2.0;
			if( ftdc>0 && btdc<0 )      { fbtdc = ftdc; }
			else if( ftdc<0 && btdc>0 ) { fbtdc = btdc; }
			else if( ftdc<0 && btdc<0 ) continue;
			t0forward_trk  = fbtdc - forevtime;
		      }
		      else {
			t0forward_trk = tofCaliSvc->EtfTime( (*iter2)->tdc1(), (*iter2)->tdc2(), (*iter2)->tofId(), (*iter2)->strip() )-tetf[i];
		      }

		      if( t0forward_trk<-1 ) continue;
		      if( m_TofOpt && nmatch_end!=0 && fabs(t0forward_trk-t0forward_add/nmatch_end)>9 ) continue;
		      if( m_debug == 4 ) { cout << "t0forward_trk:" << t0forward_trk << endl; }
		      t0forward_add  += t0forward_trk;
		      if(nmatch>499) break;
		      Tof_t0Arr[nmatch] = t0forward_trk;
		      meantev[nmatch] = forevtime;
		      nmatch++;
		      nmatch_end++;
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
      if( nmatch_end ) { tof_flag=5; }
    }
    
    if( nmatch_barrel==0 && nmatch_end==0 ) {
      for( TofDataVector::iterator iter2 = tofDigiVec.begin(); iter2 != tofDigiVec.end(); iter2++, digiId++ ) {
	barrelid=(*iter2)->barrel();
	if( (*iter2)->barrel()!=0 ) continue;
	if( (*iter2)->times()!=1 ) continue;
	tofid = (*iter2)->tofId();
	// Scintillator Endcap TOF
	if( !( (*iter2)->is_mrpc() ) ) {
	  if( tofid<48 ) { barrelid=0; }
	  if( tofid>47 ) { barrelid=2; }
	  if( barrelid==2 ) { tofid=tofid-48; }
	}
	// MRPC Endcap TOF
	else if( (*iter2)->is_mrpc() ) {
	  if( ( TofID::endcap( Identifier( (*iter2)->identify() ) ) ) == 0 ) { barrelid=0; }
	  else if( ( TofID::endcap( Identifier( (*iter2)->identify() ) ) ) == 1 ) { barrelid=2; }
	  if( barrelid==2 ) { tofid=tofid-36; }
	}
	
	log<< MSG::INFO
	   <<" is_mrpc = " << (*iter2)->is_mrpc()
	   <<" TofId = "<< tofid
	   <<" barrelid = "<< barrelid
	   <<endreq
	   <<" ForwordADC =  "<< (*iter2)->adc()
	   <<" ForwordTDC =  "<< (*iter2)->tdc()
	   << endreq;
	double ftdc=  (*iter2)->tdc();
	double fadc=  (*iter2)->adc();
	if(m_debug ==4)  cout<<"endcap::single hit,barrelid,tofid,tdc: "<<barrelid<<" , "<<tofid<<" , "<<ftdc<<endl;

	// Scintillator Endcap TOF
	if( !( (*iter2)->is_mrpc() ) && useEtofScin ) {
	  int idptof = ((tofid-1) == -1) ? 47 : tofid-1;
	  int idntof = ((tofid+1) == 48) ? 0 : tofid+1;
	  for( int i=0; i<2; i++ ) {
	    if( zemc_rec[0] || zemc_rec[1] ) {
	      if( tofid==tofid_emc[i] || tofid_emc[i]==idntof || tofid_emc[i]==idptof ) {
		if( ftdc>2000. || module[i]==1 ) continue;  // module[i]==1   barrel
		ttof[i]= sqrt(ELMAS2/(m_ebeam*m_ebeam)+1)*sqrt(xemc_rec[i]*xemc_rec[i]+yemc_rec[i]*yemc_rec[i]+133*133)/VLIGHT;
		r_endtof[i]=sqrt(xemc_rec[i]*xemc_rec[i]+yemc_rec[i]*yemc_rec[i]);
		if( optCosmic && ( tofid<24 || ( tofid>48 && tofid<71 ) ) ) { 
		  forevtime = -ttof[i] + r_endtof[i]*0.09 + 12.2;
		  meantevup[ntofup] = forevtime;
		  ntofup++;
		}
		else {
		  forevtime = ttof[i] + r_endtof[i]*0.09 + 12.2;
		  meantevdown[ntofdown] = forevtime;
		  ntofdown++;
		}
		if( (*iter2)->adc()<0.0 || m_userawtime){
		  t0forward_trk = ftdc - forevtime;
		}
		else{
		  t0forward_trk = tofCaliSvc->ETime((*iter2)->adc(), (*iter2)->tdc(),r_endtof[i], (*iter2)->tofId())-ttof[i];
		  if(m_debug ==4) cout<<"emc flag t0forward_trk: "<<t0forward_trk<<endl;
		}

		if( t0forward_trk<-1.) continue;
		if( !m_TofOpt && nmatch_end!=0 && fabs( t0forward_trk - t0forward_add/nmatch_end)>9 ) continue;
		meantev[nmatch] = forevtime;
		t0forward_add  += t0forward_trk;
		if(nmatch>499) break;
		Tof_t0Arr[nmatch] = t0forward_trk;
		nmatch++;
		nmatch_end++;
		emcflag2=1;
	      }
	    }
	  }
	}
	// MRPC Endcap TOF
	if( (*iter2)->is_mrpc() && useEtofMRPC ) {
	  double btdc=  (*iter2)->tdc2();
	  double badc=  (*iter2)->adc2();
	  int idptof = ((tofid-1) == -1) ? 35 : tofid-1;
	  int idntof = ((tofid+1) == 36) ? 0 : tofid+1;
	  for( int i=0; i<2; i++ ) {
	    if( zemc_rec[0] || zemc_rec[1] ) {
	      if( tofid==etfid_emc[i] || etfid_emc[i]==idntof || etfid_emc[i]==idptof ) {

		if( ftdc>2000.|| module[i]==1 ) continue;
		tetf[i]= sqrt(ELMAS2/(m_ebeam*m_ebeam)+1)* sqrt(xemc_rec[i]*xemc_rec[i]+yemc_rec[i]*yemc_rec[i]+133*133)/VLIGHT;
		r_endetf[i] = sqrt(xemc_rec[i]*xemc_rec[i]+yemc_rec[i]*yemc_rec[i]);
		if( optCosmic && ( tofid<18 || ( tofid>35 && tofid<54 ) ) ) {
		  forevtime  = -tetf[i] + 17.7;
		  meantevup[ntofup] = forevtime;
		  ntofup++;
		}
		else {
		  forevtime  = tetf[i] + 17.7;
		  meantevdown[ntofdown] = forevtime;
		  ntofdown++;
		}
		
		if( m_userawtime ) {
		  double fbtdc = ( ftdc + btdc )/2.0;
		  if( ftdc>0 && btdc<0 )      { fbtdc = ftdc; }
		  else if( ftdc<0 && btdc>0 ) { fbtdc = btdc; }
		  else if( ftdc<0 && btdc<0 ) continue;
		  t0forward_trk  = fbtdc - forevtime;
		}
		else {
		  t0forward_trk = tofCaliSvc->EtfTime( (*iter2)->tdc1(), (*iter2)->tdc2(), (*iter2)->tofId(), (*iter2)->strip() )-tetf[i];
		}

		if( t0forward_trk<-1 ) continue;
		if( !m_TofOpt && nmatch_end!=0 && fabs( t0forward_trk - t0forward_add/nmatch_end)>9 ) continue;
		if( m_debug==4 ) { cout << "t0forward_trk:" << t0forward_trk << endl; }
		t0forward_add  += t0forward_trk;
		if(nmatch>499) break;
		Tof_t0Arr[nmatch]=t0forward_trk;
		nmatch++;
		nmatch_end++;
		emcflag2=1;
	      }
	    }
	  }
	} 
      }
      if( nmatch_end ) { tof_flag=5; }
    }

    if( nmatch_barrel==0 && nmatch_end==0 ) {
      for( TofDataVector::iterator iter2 = tofDigiVec.begin(); iter2 != tofDigiVec.end(); iter2++, digiId++ ) {
	log << MSG::INFO << "TOF digit No: " << digiId << endreq;
	barrelid = (*iter2)->barrel();
	if( (*iter2)->barrel()!=0 ) continue; 
	if( (*iter2)->times()>1 && (*iter2)->times()<5 ) {
	  tofid = (*iter2)->tofId();
	  // Scintillator Endcap TOF
	  if( !( (*iter2)->is_mrpc() ) ) {
	    if( tofid<48 ) { barrelid=0; }
	    if( tofid>47 ) { barrelid=2; }
	    if( barrelid==2 ) { tofid=tofid-48; }
	  }
	  // MRPC Endcap TOF
	  else if( (*iter2)->is_mrpc() ) {
	    if( ( TofID::endcap( Identifier( (*iter2)->identify() ) ) ) == 0 ) { barrelid=0; }
	    else if( ( TofID::endcap( Identifier( (*iter2)->identify() ) ) ) == 1 ) { barrelid=2; }
	    if( barrelid==2 ) { tofid=tofid-36; }
	  }
	  log<< MSG::INFO
	     <<" TofId = "<<tofid
	     <<" barrelid = "<<barrelid
	     <<endreq
	     <<" ForwordADC =  "<< (*iter2)->adc()
	     <<" ForwordTDC =  "<< (*iter2)->tdc()
	     << endreq;
	  double ftdc = (*iter2)->tdc();
	  double fadc = (*iter2)->adc();
	  if( m_debug==4 ) { cout << "endcap::multi hit,barrelid,tofid,tdc: " << barrelid << " , " << tofid << " , " << ftdc << endl; }

	  // Scintillator Endcap TOF
	  if( !( (*iter2)->is_mrpc() ) && useEtofScin ) {
	    int idptof = ((tofid-1) == -1) ? 47 : tofid-1;
	    int idntof = ((tofid+1) == 48) ? 0 : tofid+1;
	    // B e a m   d a t a   c a s e
	    if( idmatch[barrelid][tofid]==1 || idmatch[barrelid][idptof]==1 || idmatch[barrelid][idntof]==1 ) {
	      for( int i=0; i<=ntot; i++ ) {
		if( ttof[i]!=0 && ftdc>0 ) {
		  if( (tofid_helix[i]==tofid) || (tofid_helix[i]==idntof) || (tofid_helix[i]==idptof) ) {
		    if( barrelid==0 || barrelid==2 ) {
		      if( r_endtof[i]>=41 && r_endtof[i]<=90 ) {
			if( optCosmic && ( tofid<24 || ( tofid>48 && tofid<71 ) ) ) { 
			  forevtime = -ttof[i] + r_endtof[i]*0.09 + 12.2;
			  meantevup[ntofup]=forevtime;
			  ntofup++;
			}
			else{
			  forevtime = ttof[i] + r_endtof[i]*0.09 + 12.2;
			  meantevdown[ntofdown]=forevtime;
			  ntofdown++;
			}
			if( (*iter2)->adc()<0.0 || m_userawtime){ 
			  t0forward_trk=ftdc-forevtime;
			}
			else{
			  t0forward_trk = tofCaliSvc->ETime((*iter2)->adc(), (*iter2)->tdc(),r_endtof[i], (*iter2)->tofId())-ttof[i];
			}

			if( t0forward_trk<-1.) continue;
			if( !m_TofOpt && nmatch_end!=0 && fabs( t0forward_trk - t0forward_add/nmatch_end)>9 ) continue;
			meantev[nmatch] = forevtime;
			t0forward_add  += t0forward_trk;     
			if( nmatch>499 ) break;
			Tof_t0Arr[nmatch] = t0forward_trk;
			nmatch++;
			nmatch_end++;
		      }
		    }
		    if( m_debug==4 ) { cout << "t0forward_trk:" << t0forward_trk << endl; }
		  }
		}
	      }
	    }
	  }
	  // MRPC Endcap TOF
	  if( (*iter2)->is_mrpc() && useEtofMRPC ) {
	    double btdc=  (*iter2)->tdc2();
	    double badc=  (*iter2)->adc2();
	    int idptof = ((tofid-1) == -1) ? 35 : tofid-1;
	    int idntof = ((tofid+1) == 36) ? 0 : tofid+1;
	    // B e a m   d a t a   c a s e    <--- Implement for test!
	    if( idetfmatch[barrelid][tofid]==1 || idetfmatch[barrelid][idptof]==1 || idetfmatch[barrelid][idntof]==1 ) {
	      for( int i=0; i<=ntot; i++ ) {
		if( tetf[i]!=0 && ftdc>0 && ftdc<2000.) {
		  if( etfid_helix[i]==tofid || etfid_helix[i]==idntof || etfid_helix[i] == idptof ) {
		    if( barrelid==0 || barrelid==2 ) {
		      if( r_endetf[i]>=41 && r_endetf[i]<=90 ) {
			if( optCosmic && ( tofid<18 || ( tofid>35 && tofid<54 ) ) ) {
			  forevtime  = -tetf[i] + 17.7;
			  meantevup[ntofup] = forevtime;
			  ntofup++;
			}
			else {
			  forevtime  = tetf[i] + 17.7;
			  meantevdown[ntofdown] = forevtime;
			  ntofdown++;
			}
			if( m_userawtime ) {
			  double fbtdc = ( ftdc + btdc )/2.0;
			  if( ftdc>0 && btdc<0 )      { fbtdc = ftdc; }
			  else if( ftdc<0 && btdc>0 ) { fbtdc = btdc; }
			  else if( ftdc<0 && btdc<0 ) continue;
			  t0forward_trk  = fbtdc - forevtime;
			}
			else {
			  t0forward_trk = tofCaliSvc->EtfTime( (*iter2)->tdc1(), (*iter2)->tdc2(), (*iter2)->tofId(), (*iter2)->strip() )-tetf[i];
			}

			if( t0forward_trk<-1 ) continue;
			if( !m_TofOpt && nmatch_end!=0 && fabs( t0forward_trk - t0forward_add/nmatch_end )>9 ) continue;
			if( m_debug == 4 ) { cout << "t0forward_trk:" << t0forward_trk << endl; }
			t0forward_add  += t0forward_trk;
			if(nmatch>499) break;
			Tof_t0Arr[nmatch] = t0forward_trk;
			meantev[nmatch] = forevtime;
			nmatch++;
			nmatch_end++; 
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
      if( nmatch_end ) { tof_flag=7; }
    }
    
    if(m_ntupleflag && m_tuple2){ 
      g_nmatchbarrel = nmatch_barrel;
      g_nmatchbarrel_1 = nmatch_barrel_1;
      g_nmatchbarrel_2 = nmatch_barrel_2;
      g_nmatchend = nmatch_end;
    }

    if( nmatch_end !=0 ) {
      t0forward = t0forward_add/nmatch_end;
      if( optCosmic==0 ) {
	if( m_TofOpt ) {
	  t_Est=EST_Trimmer(Opt_new(Tof_t0Arr,nmatch,m_TofOpt_Cut),tOffset_e,toffset_rawtime_e,offset_dt_end,bunchtime);
	}
	else { t_Est=EST_Trimmer(t0forward,tOffset_e,toffset_rawtime_e,offset_dt_end,bunchtime); }
	/*
	  cout << "EsTimeAlg: Determine t_est:" << endl;
	  cout << "    tOffset_b         " << tOffset_b << endl;
	  cout << "    toffset_rawtime   " <<toffset_rawtime<< endl;      
	  cout << "     offset_dt        "<< offset_dt << endl;
	  cout << "     Opt_new          "<< Opt_new(Tof_t0Arr,nmatch,m_TofOpt_Cut) << endl;
	  cout << "     Tof_t0Arr        "<< *Tof_t0Arr << endl;
	  cout << "     nmatch           "<< nmatch << endl;
	  cout << "    t_Est         " << t_Est << endl;
	  cout << "    t_0forward    " << t0forward  << endl;
	  cout << "    tOffset_e     " << tOffset_e << endl;
	  cout << "    toffset_raw_e " << toffset_rawtime_e << endl;
	  cout << "    offset_dt_end " << offset_dt_end << endl;
	  cout << "    bunchtime     " << bunchtime  << endl;
	  cout << "    m_TofOpt      " << m_TofOpt << endl;
	  cout << "--------------------------" << endl;
	*/
	if(t_Est<0) t_Est=0;
	if(tof_flag==5) tEstFlag=151;
	else if(tof_flag==7) tEstFlag=171;
	if(emcflag2==1) tEstFlag=161;
	/* if(m_nbunch==6){
	   nbunch=(int)(t0forward-offset)/4;
	   if(((int)(t0forward-offset))%4>2 || ((int)(t0forward-offset))%4==2) nbunch=nbunch+1;
	   // if(((int)(t0forward-offset))%4>2)  nbunch=nbunch+1;
	   t_Est=nbunch*4+offset;
	   if(t_Est<0) t_Est=0;
	   tEstFlag=151;
	   }*/
      }
      if( optCosmic ) {
	t_Est=t0forward;//10. yzhang add for nmatch_end cosmic event
	if(tof_flag==5) tEstFlag=251;
	else if(tof_flag==7) tEstFlag=271;
	if(emcflag2==1) tEstFlag=261;
      }
      if(m_ntupleflag && m_tuple2) g_meant0=t0forward;
    }
    
	      double t0_comp=-999;
	      double T0=-999;

	      if(nmatch_barrel==0 && nmatch_end==0 && m_flag==1){
		double mhit[43][300]={0.};
		SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc(),"/Event/Digi/MdcDigiCol");
		if (!mdcDigiCol) {
		  log << MSG::INFO<< "Could not find MDC digi" << endreq;
		  return StatusCode::FAILURE;
		}

		IMdcGeomSvc* mdcGeomSvc;
		StatusCode sc = service("MdcGeomSvc", mdcGeomSvc);
		if (sc !=  StatusCode::SUCCESS) {
		  return StatusCode::FAILURE;
		}

		MdcDigiCol::iterator iter1 = mdcDigiCol->begin();
		digiId = 0;
		Identifier mdcId;
		int layerId;
		int wireId;

		for (;iter1 != mdcDigiCol->end(); iter1++, digiId++) {
		  mdcId = (*iter1)->identify();
		  layerId = MdcID::layer(mdcId);
		  wireId = MdcID::wire(mdcId);

		  mhit[layerId][wireId]=RawDataUtil::MdcTime((*iter1)->getTimeChannel());
		  mhit[layerId][wireId]-=1.28*(mdcGeomSvc->Layer(layerId)->Radius())/299.8;

		  mdcGeomSvc->Wire(layerId,wireId);
		  // Apply crude TOF, Belle: tof=1.074*radius/c, here we use 1.28 instead of 1.074. 
		  double tof;
		  //      tof = 1.28 * mhit.geo->Lyr()->Radius()/299.8;  //unit of Radius is mm.  
		}

		int Iused[43][300]={0},tmp=0;
		bool Lpat,Lpat11,Lpat12,Lpat2,Lpat31,Lpat32;
		double t0_all=0,t0_mean=0;
		double r[4]={0.};
		double chi2=999.0;
		double phi[4]={0.},corr[4]={0.},driftt[4]={0.};
		double ambig=1;
		double mchisq=50000;

		//for(int i=2;i<10;i++){
		for(int i=5;i<10;i++){

		  double T1=0.5*0.1*(mdcGeomSvc->Layer(4*i+0)->PCSiz())/0.004;
		  double T2=0.5*0.1*(mdcGeomSvc->Layer(4*i+1)->PCSiz())/0.004;
		  double T3=0.5*0.1*(mdcGeomSvc->Layer(4*i+2)->PCSiz())/0.004;
		  double T4=0.5*0.1*(mdcGeomSvc->Layer(4*i+3)->PCSiz())/0.004;
		  r[0]=(mdcGeomSvc->Layer(4*i+0)->Radius())*0.1;
		  r[1]=(mdcGeomSvc->Layer(4*i+1)->Radius())*0.1;
		  r[2]=(mdcGeomSvc->Layer(4*i+2)->Radius())*0.1;
		  r[3]=(mdcGeomSvc->Layer(4*i+3)->Radius())*0.1;
		  double  r0=r[0]-r[1]-(r[2]-r[1])/2;
		  double  r1=-(r[2]-r[1])/2;
		  double  r2=(r[2]-r[1])/2;
		  double  r3=r[3]-r[2]+(r[2]-r[1])/2;

		  for(int j=0;j<mdcGeomSvc->Layer(i*4+3)->NCell();j++){

		    int Icp=0;
		    Icp=j-1;
		    if(Icp<0) Icp=mdcGeomSvc->Layer(i*4+3)->NCell();

		    Lpat=(mhit[4*i][j]!=0) && (mhit[4*i][Icp]==0) &&( mhit[4*i][j+1]==0) && (Iused[4*i][j]==0);
		    if(Lpat==1){ 

		    }   
		    if(Lpat){
		      Lpat11=(mhit[4*i+1][Icp]==0)&&(Iused[4*i+1][j]==0)&&(mhit[4*i+1][j]!=0)&&(mhit[4*i+1][j+1]==0);
		      Lpat12=(mhit[4*i+1][j]==0)&&(Iused[4*i+1][j+1]==0)&&(mhit[4*i+1][j+1]!=0)&&(mhit[4*i+1][j+2]==0);
		      Lpat2=(mhit[4*i+2][Icp]==0)&&(Iused[4*i+2][j]==0)&&(mhit[4*i+2][j]!=0)&&(mhit[4*i+2][j+1]==0);
		      Lpat31=(mhit[4*i+3][Icp]==0)&&(Iused[4*i+3][j]==0)&&(mhit[4*i+3][j]!=0)&&(mhit[4*i+3][j+1]==0);
		      Lpat32=(mhit[4*i+3][j]==0)&&(Iused[4*i+3][j+1]==0)&&(mhit[4*i+3][j+1]!=0)&&(mhit[4*i+3][j+2]==0);

		      if(Lpat11 && Lpat2 && Lpat31 ){

			Iused[4*i+0][j]=1;
			Iused[4*i+1][j]=1;
			Iused[4*i+2][j]=1;
			Iused[4*i+3][j]=1;
			double  t_i = mhit[4*i+0][j]+mhit[4*i+2][j];
			double  t_j = mhit[4*i+1][j]+mhit[4*i+3][j];
			double  l_j = T2+T4;
			double  r_i = r0+r2;
			double  r_j = r1+r3;
			double  r_2k= r0*r0+r1*r1+r2*r2+r3*r3;
			double  rt_i = r0*mhit[4*i+0][j]+r2*mhit[4*i+2][j];
			double  rt_j = r1*mhit[4*i+1][j]+r3*mhit[4*i+3][j];
			double  rl_j = r1*T2+r3*T4;

			double  deno= 4*r_2k-2*(r_i*r_i+r_j*r_j);

			if (deno!=0){
			  double  Pa=(4*(rt_i-rt_j+rl_j)-(t_i+t_j-l_j)*(r_i-r_j)-(t_i-t_j+l_j)*(r_i+r_j))/deno;
			  double  Pb= 0.25*(t_i-t_j+l_j-(r_i+r_j) * Pa);
			  double  Ang=fabs(90.-fabs(atan(Pa)*180./3.141593));

			  t0_all+= (-0.25*((r_i-r_j)*Pa-t_i-t_j+l_j));

			  double chi2_tmp;
			  for(int t0c=0;t0c<17;t0c+=8){
			    chi2_tmp=(mhit[4*i+0][j]-t0c-r0 * Pa-Pb)*(mhit[4*i+0][j]-t0c-r0 * Pa-Pb)+(T2-mhit[4*i+1][j]+t0c-r1 * Pa-Pb)*(T2-mhit[4*i+1][j]+t0c-r1 * Pa-Pb)+(mhit[4*i+2][j]-t0c-r2 * Pa-Pb)*(mhit[4*i+2][j]-t0c-r2 * Pa-Pb) + (T4-mhit[4*i+3][j]+t0c-r3 * Pa-Pb)*(T4-mhit[4*i+3][j]+t0c-r3 * Pa-Pb);
			    if(chi2_tmp<chi2){
			      chi2=chi2_tmp;
			      t0_comp=t0c;
			    }  
			  }
			  tmp+=1;
			}
			//use  squareleas t0 

			for(int tmpT0=0;tmpT0<17;tmpT0+=8){
			  driftt[0]=mhit[4*i+0][j]-tmpT0;
			  driftt[1]=mhit[4*i+1][j]-tmpT0;
			  driftt[2]=mhit[4*i+2][j]-tmpT0;
			  driftt[3]=mhit[4*i+3][j]-tmpT0;

			  phi[0]=j*2*3.14159265/(mdcGeomSvc->Layer(i*4)->NCell())+2*3.14159265/(mdcGeomSvc->Layer(i*4+1)->NCell())/2;  
			  phi[1]=j*2*3.14159265/(mdcGeomSvc->Layer(i*4+1)->NCell());
			  phi[2]=j*2*3.14159265/(mdcGeomSvc->Layer(i*4+2)->NCell())+2*3.14159265/(mdcGeomSvc->Layer(i*4+1)->NCell())/2;
			  phi[3]=j*2*3.14159265/(mdcGeomSvc->Layer(i*4+3)->NCell());
			  phi[0]-=ambig*driftt[0]*0.004/r[0];
			  phi[1]+=ambig*driftt[1]*0.004/r[1];
			  phi[2]-=ambig*driftt[2]*0.004/r[2];
			  phi[3]+=ambig*driftt[3]*0.004/r[3];
			  double s1, sx, sy, sxx, sxy;  // The various sums. 
			  double delinv, denom;
			  double weight;     // weight for hits, calculated from sigmas. 
			  double sigma;
			  double x[4];
			  x[0]=r0;
			  x[1]=r1;
			  x[2]=r2;
			  x[3]=r3;
			  sigma=0.12;
			  s1 = sx = sy = sxx = sxy = 0.0;
			  double chisq = 0.0;
			  for (int ihit = 0; ihit < 4; ihit++) {
			    weight = 1. / (sigma * sigma);//NEED sigma of MdcHits
			    s1 += weight;
			    sx += x[ihit] * weight;
			    sy += phi[ihit] * weight;
			    sxx += x[ihit] * (x[ihit] * weight);
			    sxy += phi[ihit] * (x[ihit] * weight);
			  }
			  double resid[4]={0.};
			  /* Calculate parameters. */
			  denom = s1 * sxx - sx * sx;
			  delinv = (denom == 0.0) ? 1.e20 : 1. / denom;
			  double intercept = (sy * sxx - sx * sxy) * delinv;
			  double slope = (s1 * sxy - sx * sy) * delinv;

			  /* Calculate residuals. */
			  for (int ihit = 0; ihit < 4; ihit++) {
			    resid[ihit] = ( phi[ihit] - intercept - slope * x[ihit] );
			    chisq += resid[ihit] * resid[ihit]/(sigma*sigma) ;
			  }
			  if(chisq<mchisq){
			    mchisq=chisq;
			    T0=tmpT0;
			  }
			}
		      } 
		      if(Lpat12 && Lpat2 && Lpat32){
			Iused[4*i+0][j]=1;
			Iused[4*i+1][j+1]=1;
			Iused[4*i+2][j]=1;
			Iused[4*i+3][j+1]=1;

			double  t_i = mhit[4*i+0][j]+mhit[4*i+2][j];
			double  t_j = mhit[4*i+1][j+1]+mhit[4*i+3][j+1];
			double  l_j = T2+T4;
			double  r_i = r0+r2;
			double  r_j = r1+r3;
			double  r_2k= r0*r0+r1*r1+r2*r2+r3*r3;
			double  rt_i = r0*mhit[4*i+0][j]+r2*mhit[4*i+2][j];
			double  rt_j = r1*mhit[4*i+1][j+1]+r3*mhit[4*i+3][j+1];
			double  rl_j = r1*T2+r3*T4;
			double  deno= 4*r_2k-2*(r_i*r_i+r_j*r_j);

			if (deno!=0){
			  double  Pa=(4*(rt_i-rt_j+rl_j)-(t_i+t_j-l_j)*(r_i-r_j)-(t_i-t_j+l_j)*(r_i+r_j))/deno;
			  double  Pb= 0.25*(t_i-t_j+l_j-(r_i+r_j) * Pa);
			  double  Ang=fabs(90.-fabs(atan(Pa)*180./3.141593));
			  t0_all+= (-0.25*((r_i-r_j)*Pa-t_i-t_j+l_j));
			  tmp+=1;
			  double chi2_tmp;

			  for(int t0c=0;t0c<17;t0c+=8){
			    chi2_tmp=(mhit[4*i+0][j]-t0c-r0 * Pa-Pb)*(mhit[4*i+0][j]-t0c-r0 * Pa-Pb)+(T2-mhit[4*i+1][j+1]+t0c-r1 * Pa-Pb)*(T2-mhit[4*i+1][j+1]+t0c-r1 * Pa-Pb)+(mhit[4*i+2][j]-t0c-r2 * Pa-Pb)*(mhit[4*i+2][j]-t0c-r2 * Pa-Pb) + (T4-mhit[4*i+3][j+1]+t0c-r3 * Pa-Pb)*(T4-mhit[4*i+3][j+1]+t0c-r3 * Pa-Pb);

			    if(chi2_tmp<chi2){
			      chi2=chi2_tmp;
			      t0_comp=t0c;
			    }  
			  }
			}

			//use  squareleast  

			for(int tmpT0=0;tmpT0<17;tmpT0+=8){
			  driftt[0]=mhit[4*i+0][j]-tmpT0;
			  driftt[1]=mhit[4*i+1][j+1]-tmpT0;
			  driftt[2]=mhit[4*i+2][j]-tmpT0;
			  driftt[3]=mhit[4*i+3][j+1]-tmpT0;

			  phi[0]=j*2*3.14159265/(mdcGeomSvc->Layer(i*4)->NCell())+2*3.14159265/(mdcGeomSvc->Layer(i*4+1)->NCell())/2;
			  phi[1]=j*2*3.14159265/(mdcGeomSvc->Layer(i*4+1)->NCell());
			  phi[2]=j*2*3.14159265/(mdcGeomSvc->Layer(i*4+2)->NCell())+2*3.14159265/(mdcGeomSvc->Layer(i*4+1)->NCell())/2;
			  phi[3]=j*2*3.14159265/(mdcGeomSvc->Layer(i*4+3)->NCell());
			  phi[0]-=ambig*driftt[0]*0.004/r[0];
			  phi[1]+=ambig*driftt[1]*0.004/r[1];
			  phi[2]-=ambig*driftt[2]*0.004/r[2];
			  phi[3]+=ambig*driftt[3]*0.004/r[3];
			  double s1, sx, sy, sxx, sxy;  // The various sums. 
			  double delinv, denom;
			  double weight;     // weight for hits, calculated from sigmas. 
			  double sigma;
			  double x[4];
			  x[0]=r0;
			  x[1]=r1;
			  x[2]=r2;
			  x[3]=r3;
			  sigma=0.12;
			  s1 = sx = sy = sxx = sxy = 0.0;
			  double chisq = 0.0;
			  for (int ihit = 0; ihit < 4; ihit++) {
			    weight = 1. / (sigma * sigma);//NEED sigma of MdcHits
			    s1 += weight;
			    sx += x[ihit] * weight;
			    sy += phi[ihit] * weight;
			    sxx += x[ihit] * (x[ihit] * weight);
			    sxy += phi[ihit] * (x[ihit] * weight);
			  }
			  double resid[4]={0.};
			  /* Calculate parameters. */
			  denom = s1 * sxx - sx * sx;
			  delinv = (denom == 0.0) ? 1.e20 : 1. / denom;
			  double intercept = (sy * sxx - sx * sxy) * delinv;
			  double slope = (s1 * sxy - sx * sy) * delinv;

			  /* Calculate residuals. */
			  for (int ihit = 0; ihit < 4; ihit++) {
			    resid[ihit] = ( phi[ihit] - intercept - slope * x[ihit] );
			    chisq += resid[ihit] * resid[ihit]/(sigma*sigma) ;
			  }

			  if(chisq<mchisq){
			    mchisq=chisq;
			    T0=tmpT0;
			  }
			}
		      }
		    }
		  }
		} 

		if(tmp!=0){
		  t0_mean=t0_all/tmp;
		}
		if(m_ntupleflag && m_tuple2) g_t0mean=t0_mean;

		t_Est=T0 + tOffset_b;//11.yzhang add tOffset_b, MDC method calc. Tes
		tEstFlag=2;
	      } 
	      if(m_ntupleflag && m_tuple2){
		g_t0=t0_comp;
		g_T0=T0; 
	      }   
	      if(nmatch_barrel==0 && nmatch_end==0 && nmatch_barrel_1==0&&nmatch_barrel_2==0&&m_mdcCalibFunSvc&&m_flag==2){

		log << MSG::INFO << " mdc " << endreq;

#define MXWIRE  6860
#define MXTKHIT 6860
#define MXTRK  15

		// C o n s t a n t s
		double VELPROP=33.33;

		// L o c a l   v a r i a b l e s
		int nhits_ax = 0;
		int nhits_ax2 = 0;
		int nhits_st = 0;
		int nhits_st2 = 0;

		double tev_ax[MXTKHIT];
		double tev_st[MXTKHIT];
		double tevv[MXTKHIT];
		double toft;
		double prop;
		double t0_minus_TDC[MXWIRE];
		// double adc[MXWIRE];
		double T0_cal=-230;
		double Mdc_t0Arr[500];

		int   expmc=1;
		double scale=1.;
		int   expno, runno;
		ndriftt=0;

		// A l l   t r a c k s
		//Check if Fzisan track exist
		//  if(ntot<=0) return 0;  // it was "if(ntot<=0) return 0;"
		if(ntot > MXTRK) {
		  ntot=MXTRK;
		}

		SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
		if (!newtrkCol || newtrkCol->size()==0) { 
		  log << MSG::INFO<< "Could not find RecMdcTrackCol" << endreq;
		  return StatusCode::SUCCESS;
		}
		log << MSG::INFO << "Begin to check RecMdcTrackCol"<<endreq; 

		RecMdcTrackCol::iterator iter_trk = newtrkCol->begin();

		for( int tempntrack=0; iter_trk != newtrkCol->end(); iter_trk++,tempntrack++) {
		  log << MSG::DEBUG << "retrieved MDC track:"
		    << " Track Id: " << (*iter_trk)->trackId()
		    << " Dr: " <<(*iter_trk)->helix(0)
		    << " Phi0: " << (*iter_trk)->helix(1)
		    << " kappa: " << (*iter_trk)->helix(2)
		    << " Dz: " << (*iter_trk)->helix(3) 
		    << " Tanl: " << (*iter_trk)->helix(4)
		    << "   Phi terminal: "<< (*iter_trk)->getFiTerm()
		    << endreq
		    << "Number of hits: "<< (*iter_trk)->getNhits()
		    << "   Number of stereo hits " << (*iter_trk)->nster()
		    << endreq;

		  // Track variables
		  const HepPoint3D pivot0(0.,0.,0.);
		  HepVector  a(5,0);

		  a[0] = (*iter_trk)->helix(0);
		  a[1] = (*iter_trk)->helix(1);
		  a[2] = (*iter_trk)->helix(2);
		  a[3] = (*iter_trk)->helix(3);
		  a[4] = (*iter_trk)->helix(4);

		  // Ill-fitted (dz=tanl=-9999.) or off-IP track in fzisan
		  if (abs(a[0])>Estparam.MDC_drCut() || abs(a[3])>Estparam.MDC_dzCut() || abs(a[4])>500.) continue;

		  double phi0 = a[1];
		  double kappa = abs(a[2]);
		  double dirmag = sqrt(1. + a[4]*a[4]);
		  // double unit_s = abs(rho * dirmag);
		  double mom = abs(dirmag/kappa);
		  double beta=mom/sqrt(mom*mom+PIMAS2);
		  if (particleId[tempntrack]== 1) { beta=mom/sqrt(mom*mom+ELMAS2);}
		  if(particleId[tempntrack]== 5) { beta=mom/sqrt(mom*mom+PROTONMAS2);}

		  // D e f i n e   h e l i x
		  Helix helix0(pivot0,a);
		  double rho = helix0.radius();
		  double unit_s = abs(rho * dirmag);

		  helix0.ignoreErrorMatrix();
		  HepPoint3D hcen  = helix0.center();
		  double xc= hcen(0);
		  double yc= hcen(1);

		  if( xc==0.0 && yc==0.0 )                                continue;

		  double direction =1.;
		  if(optCosmic!=0) {
		    double phi = atan2(helix0.momentum(0.).y(),helix0.momentum(0.).x());
		    if(phi> 0. && phi <= M_PI) direction=-1.; 
		  }

		  IMdcGeomSvc* mdcGeomSvc;
		  StatusCode sc = service("MdcGeomSvc", mdcGeomSvc);
		  double zeast;
		  double zwest;
		  double m_vp[43]={0.}, m_zst[43]={0.};
		  for(int lay=0; lay<43; lay++){
		    zwest = mdcGeomSvc->Wire(lay, 0)->Forward().z();
		    zeast = mdcGeomSvc->Wire(lay, 0)->Backward().z();
		    //  m_zwid[lay] = (zeast - zwest) / (double)m_nzbin;

		    if(lay < 8) m_vp[lay] = 220.0; // *10^9 mm/s
		    else m_vp[lay] = 240.0; // *10^9 mm/s

		    if( 0 == (lay % 2) ){	// west end
		      m_zst[lay] = zwest;
		    } else{		// east end
		      m_zst[lay] = zeast;
		    }
		  }

		  //Hits
		  log << MSG::DEBUG << "hitList of this  track:" << endreq;
		  HitRefVec gothits = (*iter_trk)->getVecHits();
		  HitRefVec::iterator it_gothit = gothits.begin();
		  for( ; it_gothit != gothits.end(); it_gothit++){

		    log << MSG::DEBUG << "hits Id: "<<(*it_gothit)->getId()
		      << "   hits MDC layerId wireId " <<MdcID::layer((*it_gothit)->getMdcId())
		      << "  "<<MdcID::wire((*it_gothit)->getMdcId())
		      << endreq	 
		      << "   hits TDC " <<(*it_gothit)->getTdc()  
		      << endreq;

		    int layer=MdcID::layer((*it_gothit)->getMdcId());
		    int wid=MdcID::wire((*it_gothit)->getMdcId());
		    double tdc=(*it_gothit)->getTdc() ;
		    //cout<<"-----------mdc layer,wireid,tdc--------------: "<<layer<<","<<wid<<","<<tdc<<endl;
		    double trkchi2=(*iter_trk)->chi2();
		    if(trkchi2>100)continue;
		    double hitChi2=(*it_gothit)->getChisqAdd();
		    HepVector helix_par = (*iter_trk)->helix();
		    HepSymMatrix helixErr=(*iter_trk)->err();
		    // *** A x i a l   s e g m e n t s
		    if((layer>=8&&layer<=19) ||(layer>=36&&layer<=42)){
		      // H i t s 
		      ////Geomdc_wire &GeoRef = GeoMgr[wid-1];
		      //  MdcGeoWire & GeoRef = Wire[wid-1];

		      const MdcGeoWire* GeoRef = mdcGeomSvc->Wire(layer,wid);

		      ////int layer =  GeoRef->Layer();
		      ////	int local =  GeoRef->Cell();

		      // int layer = mdcGeomSvc->Wire(wid-1)->Layer();
		      // int local = mdcGeomSvc->Wire(wid-1)->Cell();
		      //	double fadc = adc[wid];
		      ////	if(mdc_adc_cut_(&expmc, &expno, &runno, &fadc, &layer, &local)==0) continue; 

		      //Use or not to use inner 4 layers (layers with high bg.)
		      if(Estparam.MDC_Inner()==0 && layer <=3) continue;

		      double xw = GeoRef->Forward().x()/10;                    // wire x position at z=0
		      double yw = GeoRef->Forward().y()/10;                    // wire y position at z=0
		      // move pivot to the wire (slant angle ignored)
		      HepPoint3D pivot1(xw,yw,0.);
		      helix0.pivot(pivot1);
		      double zw=helix0.a()[3];                      // z position

		      // T o F   c o r r e c t i o n
		      double dphi = (-xc*(xw-xc)-yc*(yw-yc)) /
			sqrt((xc*xc+yc*yc)*((xw-xc)*(xw-xc)+(yw-yc)*(yw-yc)));
		      dphi = acos(dphi);
		      double pathtof = abs(unit_s * dphi);
		      if (kappa!=0) {
			toft = pathtof/VLIGHT/beta;
		      } else {
			toft = pathtof/VLIGHT;
		      }

		      // P r o p a g a t i o n   d e l a y   c o r r e c t i o n
		      ////if (zw < GeoRef.zwb()) zw = GeoRef.zwb();
		      ////if (zw > GeoRef.zwf()) zw = GeoRef.zwf();

		      if (zw >(GeoRef->Backward().z())/10) zw =(GeoRef->Backward().z())/10;
		      if (zw <(GeoRef->Forward().z())/10)  zw =(GeoRef->Forward().z())/10;

		      double slant = GeoRef ->Slant();
		      prop = zw / cos(slant) / VELPROP;
		      //Time walk correction 
		      double Tw = 0;
		      //if(expmc==1) {
		      //  Calmdc_const3 &TwRef = Cal3Mgr[layer];
		      // if(adc[wid]>0.) Tw = TwRef.tw(0) + TwRef.tw(1)/sqrt(adc[wid]);
		      //}

		      double driftt;
		      double dummy;
		      int lr=2;
		      //if((xw*helix0.x(0.).y()-yw*helix0.x(0.).x())<0) lr=-1;
		      double p[3];
		      p[0]=helix0.momentum(0.).x();
		      p[1]=helix0.momentum(0.).y();
		      double pos[2];
		      pos[0]=xw; pos[1]=yw;
		      double alpha;
		      ////	calmdc_getalpha_( p, pos, &alpha);
		      //	double dist = wir.distance(); this is wrong
		      //double dist = abs(helix0.a()[0]); this if wrong 
		      double dist = 0.;

		      dist=(m_mdcUtilitySvc->doca(layer, wid, helix_par, helixErr))*10.0;

		      if(dist<0.) lr=1; 
		      else lr=0;
		      dist=fabs(dist);
		      if(dist> 0.4*(mdcGeomSvc->Layer(layer))->PCSiz()) continue;

		      //* drift distance cut 
		      //	int ia;
		      //	ia = (int) ((alpha+90.)/10.);
		      //	double da = alpha+90. - ia*10.;
		      //	if (ia==0) ia=18;

		      //	Calmdc_const &BndRef = Cal1Mgr[layer];
		      //	if(lr==-1) {
		      //if(dist < BndRef.bndl(ia,0)) continue;
		      //if(dist > BndRef.bndl(ia,3)) continue;
		      //}
		      //if(lr== 1) {
		      // if(dist < BndRef.bndr(ia,0)) continue;
		      //  if(dist > BndRef.bndr(ia,3)) continue;
		      //	}

		      int idummy;

		      if(!m_useXT) {driftt = dist/Estparam.vdrift();}
		      else  {
			double entrance=(*it_gothit)->getEntra();
			driftt = m_mdcCalibFunSvc->distToDriftTime(dist, layer, wid,lr,entrance);
		      }
		      if(m_useT0cal)
		      {
			T0_cal=m_mdcCalibFunSvc->getT0(layer, wid)+m_mdcCalibFunSvc->getTimeWalk(layer,tdc);
		      }

		      double  zprop = fabs(zw - m_zst[layer]);
		      double  tp = zprop / m_vp[layer];
		      //cout<<"proptation time --tp ax: "<<tp<<endl;
		      if(driftt>tdc) continue;
		      double difft=tdc-driftt-toft-tp-T0_cal;
		      if(ndriftt>=500) break;
		      if(difft<-10) continue;
		      Mdc_t0Arr[ndriftt]=difft;
		      //cout<<"ax: tdc, driftt, toft, tp: "<<tdc<<" , "<<driftt<<" , "<<toft<<" , "<<tp<<" , "<<difft<<endl;
		      sum_EstimeMdc=sum_EstimeMdc+difft;
		      ndriftt++;
		      /*if(Estparam.MDC_Xt()==2) {
			calmdc_d2t_bfld_( &driftt, &dist, &dummy, &dummy, &lr, 
			&idummy, &layer, &alpha, &dummy, &dummy, &dummy);
			} else if(Estparam.MDC_Xt()==1) {
			driftt = dist/Estparam.vdrift();

			}*/
		      //	htf[1]->accumulate( t0_minus_TDC[wid] );	

		      double tev= -t0_minus_TDC[wid]+ driftt;
		      if(Estparam.MDC_Tof() !=0) tev += direction*toft; 
		      if(Estparam.MDC_Prop()!=0) tev += prop;
		      ////	if(Estparam.MDC_Walk()!=0) tev += Tw;

		      //	sum_tev_ax += tev;
		      //	htf[3+hid]->accumulate( tev );	      
		      nhits_ax++;
		      tev_ax[nhits_ax-1]=tev;

		      if(Estparam.MDC_Debug()!=0) log << MSG::INFO << "*** tev ***" <<tev <<endreq; 
		      double driftt_mea = t0_minus_TDC[wid];
		      //	if(abs(driftt-t0_minus_TDC[wid])<75.) 
		      if(abs(driftt - driftt_mea)<75.) {
			//	  sum_tev_ax2 += tev;
			nhits_ax2++;
			if(Estparam.MDC_Debug()!=0) log << MSG::INFO << "*** tev2 ***" <<tev <<endreq; 
		      }
		    }  // End axial hits    

		    // S t e r e o   s e g m e n t s
		    else if(((layer>=4&&layer<=7)||(layer>=20&&layer<=35))&&m_useSw){

		      IMdcGeomSvc* mdcGeomSvc;
		      StatusCode sc = service("MdcGeomSvc", mdcGeomSvc);
		      const MdcGeoWire* GeoRef = mdcGeomSvc->Wire(layer,wid);

		      //double fadc=adc[wid];
		      ////	if(mdc_adc_cut_(&expmc, &expno, &runno, &fadc, &layer, &local)==0) continue; 

		      double bx= GeoRef->Backward().x()/10;
		      double by= GeoRef->Backward().y()/10;
		      double bz= GeoRef->Backward().z()/10;
		      double fx= GeoRef->Forward().x()/10;
		      double fy= GeoRef->Forward().y()/10;
		      double fz= GeoRef->Forward().z()/10;

		      ////        HepPoint3D fwd(GeoRef->Forward().x(),GeoRef->Forward().y(),GeoRef->Forward().z());
		      ////        HepPoint3D bck(GeoRef->Backward().x(),GeoRef->Backward().y(),GeoRef->Backward().z());
		      HepPoint3D fwd(fx,fy,fz);
		      HepPoint3D bck(bx,by,bz);

		      Hep3Vector wire = (CLHEP::Hep3Vector)bck - (CLHEP::Hep3Vector)fwd;
		      HepPoint3D try1=(fwd + bck) * .5; 
		      helix0.pivot(try1);
		      HepPoint3D try2 = (helix0.x(0).z() - bck.z())/ wire.z() * wire + bck;
		      helix0.pivot(try2);
		      HepPoint3D try3 = (helix0.x(0).z() - bck.z())/ wire.z() * wire + bck;
		      helix0.pivot(try3);         

		      double xh = helix0.x(0.).x();
		      double yh = helix0.x(0.).y();
		      double z  = helix0.x(0.).z();

		      // T o F   c o r r e c t i o n
		      double dphi = (-xc*(xh-xc)-yc*(yh-yc)) /
			sqrt((xc*xc+yc*yc)*((xh-xc)*(xh-xc)+(yh-yc)*(yh-yc)));
		      dphi = acos(dphi);
		      double pathtof = abs(unit_s * dphi);
		      if (kappa!=0) {
			toft = pathtof/VLIGHT/beta;
		      } else {
			toft = pathtof/VLIGHT;
		      }

		      // P r o p a g a t i o n   d e l a y   c o r r e c t i o n

		      if (z != 9999.) {
			//  if (z < GeoRef->Forward().z()/10) z = GeoRef->Forward().z()/10;
			if(z < fz ) z = fz;
			// if (z >GeoRef->Backward().z()/10) z =GeoRef->Backward().z()/10;
			if(z > bz ) z = bz;
			double slant = GeoRef->Slant();
			prop = z / cos(slant) / VELPROP;
		      } else {
			prop = 0.;
		      }

		      //Time walk correction 
		      double Tw = 0;
		      //if(expmc==1) {
		      //  Calmdc_const3 &TwRef = Cal3Mgr[layer];
		      //  if(adc[wid]>0.) Tw = TwRef.tw(0) + TwRef.tw(1)/sqrt(adc[wid]);
		      //	}

		      double driftt=0;
		      double dummy;

		      double xw = fx + (bx-fx)/(bz-fz)*(z-fz);
		      double yw = fy + (by-fy)/(bz-fz)*(z-fz);
		      // move pivot to the wire (slant angle ignored)
		      HepPoint3D pivot1(xw,yw,z);
		      helix0.pivot(pivot1);

		      double zw=helix0.a()[3];                      // z position

		      int lr=2;
		      //if((xw*helix0.x(0.).y()-yw*helix0.x(0.).x())<0) lr=-1;
		      double p[3];
		      p[0]=helix0.momentum(0.).x(); 
		      p[1]=helix0.momentum(0.).y();
		      double pos[2]; 
		      pos[0]=xw; pos[1]=yw;
		      double alpha;
		      ////	calmdc_getalpha_( p, pos, &alpha);
		      //	double dist = wir.distance(); this is wrong
		      //double dist = abs(helix0.a()[0]); this is wrong 
		      double dist=0.;

		      dist=(m_mdcUtilitySvc->doca(layer, wid, helix_par, helixErr))*10.0;

		      if(dist<0.) lr=1;
		      else lr=0;
		      dist=fabs(dist);
		      if(dist> 0.4*(mdcGeomSvc->Layer(layer))->PCSiz()) continue;

		      //* drift distance cut 
		      //	int ia;
		      //	ia = (int) ((alpha+90.)/10.);
		      //	double da = alpha+90. - ia*10.;
		      //	if (ia==0) ia=18;

		      //	Calmdc_const &BndRef = Cal1Mgr[layer];
		      //	if(lr==-1) {
		      //  if(dist < BndRef.bndl(ia,0)) continue;
		      //  if(dist > BndRef.bndl(ia,3)) continue;
		      //	}
		      //if(lr== 1) {
		      //  if(dist < BndRef.bndr(ia,0)) continue;
		      //  if(dist > BndRef.bndr(ia,3)) continue;
		      //	}

		      if(!m_useXT){driftt = dist/(Estparam.vdrift());}
		      else {
			double entrance=(*it_gothit)->getEntra();
			driftt = m_mdcCalibFunSvc->distToDriftTime(dist, layer, wid,lr,entrance);
		      }
		      if(m_useT0cal)
		      {
			T0_cal=m_mdcCalibFunSvc->getT0(layer, wid)+m_mdcCalibFunSvc->getTimeWalk(layer,tdc);
		      }

		      double  zprop = fabs(zw - m_zst[layer]);
		      double  tp = zprop / m_vp[layer];
		      //cout<<"proptation time --tp st: "<<tp<<endl;
		      if(driftt>tdc) continue;
		      double difft=tdc-driftt-toft-tp-T0_cal;
		      if(difft<-10) continue;
		      if(ndriftt>=500) break;
		      Mdc_t0Arr[ndriftt]=difft;
		      //if(difft>-2 && difft<22)
		      // if(fabs(hitChi2)<0.2)
		      //if(difft>-20 && difft<30)
		      sum_EstimeMdc=sum_EstimeMdc+difft;
		      ndriftt++;
		      //}

		      //	htf[2]->accumulate( t0_minus_TDC[wid] );	

		      double tev= -t0_minus_TDC[wid]+ driftt;
		      if(Estparam.MDC_Tof() !=0) tev += direction*toft; 
		      if(Estparam.MDC_Prop()!=0) tev += prop;
		      ////	if(Estparam.MDC_Walk()!=0) tev += Tw;

		      //	sum_tev_st += tev;

		      //	htf[3+hid]->accumulate( tev );

		      nhits_st++;
		      tev_st[nhits_st-1]= tev;

		      if(Estparam.MDC_Debug()!=0) log << MSG::INFO << "*** tev_st ***" <<tev <<endreq; 
		      double driftt_mea = t0_minus_TDC[wid];
		      //	if(abs(driftt-t0_minus_TDC[wid]) <75.) 
		      if(abs(driftt - driftt_mea) <75.) {
			//	  sum_tev_st2 += tev;
			nhits_st2++;
			if(Estparam.MDC_Debug()!=0) log << MSG::INFO << "*** tev_st2 ***" <<tev <<endreq; 
		      }
		  }    //* End stereo hits    

		}// End hits
		nmatch_mdc++;
	      }    //* End tracks 


	      if(m_ntupleflag && m_tuple2)  g_nmatchmdc = nmatch_mdc;
	      if(ndriftt!=0){
		if(m_mdcopt) {
		  sum_EstimeMdc=Opt_new(Mdc_t0Arr,ndriftt,400.0);
		}
		else { sum_EstimeMdc= sum_EstimeMdc/ndriftt;}
		if(m_ntupleflag && m_tuple2) g_EstimeMdc=sum_EstimeMdc;
		t_Est=sum_EstimeMdc + tOffset_b;//12.yzhang add tOffset_b, calc. Tes after rec for ?
		if(t_Est<0)  t_Est=0;
		if(optCosmic==0){
		  tEstFlag=102;
		  nbunch=((int)(t_Est-offset))/bunchtime;
		  //if(((int)(t_Est-offset))%bunchtime>bunchtime/2) nbunch=nbunch+1;
		  if((t_Est-offset-nbunch*bunchtime)>(bunchtime/2)) nbunch=nbunch+1;
		  t_Est=nbunch*bunchtime+offset + tOffset_b;
		  //tEstFlag=2;
		  //	}
		  /*	if(m_nbunch==6){
			nbunch=((int)(t_Est-offset))/4;
			if(((int)(t_Est-offset))%4>2 ||((int)(t_Est-offset))%4==2 ) nbunch=nbunch+1;
			t_Est=nbunch*4+offset;
			tEstFlag=2;
			}*/
	      }
	      if(optCosmic){
		t_Est=sum_EstimeMdc;
		tEstFlag=202;
	      }
	      }
	      if(m_ntupleflag && m_tuple2)  g_ndriftt=ndriftt;
	    }
	    //yzhang add, Store to TDS
	    if(t_Est!=-999){
	      //changed event start time flag after rec 
	      if((!m_beforrec) && (Testime_fzisan != t_Est) ){
		if(tEstFlag==211) tEstFlag=213;
		if(tEstFlag==212) tEstFlag=216;
		if(tEstFlag==111) tEstFlag=113;
		if(tEstFlag==112) tEstFlag=116;
	      }

	      if( optCosmic /*&& (nmatch_barrel || nmatch_end)*/){
		StatusCode scStoreTds = storeTDS(t_Est,tEstFlag,t_quality);
		if (scStoreTds!=StatusCode::SUCCESS){ return scStoreTds; }
	      }else if(!optCosmic){
		StatusCode scStoreTds = storeTDS(t_Est,tEstFlag,t_quality);
		if (scStoreTds!=StatusCode::SUCCESS){ return scStoreTds; }
	      }
	    }else{
	      // no t_Est calc.
	      if(m_beforrec){
		//store event start time from segment fitting method 
		//FIXME add tOffset_b or tOffset_e ???
		double segTest = Testime_fzisan + tOffset_b;
		int segFlag = TestimeFlag_fzisan;
		double segQuality = TestimeQuality_fzisan;
		StatusCode scStoreTds = storeTDS(segTest,segFlag,segQuality);
		if (scStoreTds!=StatusCode::SUCCESS){ return scStoreTds; }
	      }
	    }
	    //zhangy add, end of Store to TDS

	    // check RecEsTimeCol registered
	    SmartDataPtr<RecEsTimeCol> arecestimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
	    if (!arecestimeCol) { 
	      if(m_debug==4) log << MSG::WARNING << "Could not find RecEsTimeCol" << endreq;
	      return( StatusCode::SUCCESS);
	    }
	    RecEsTimeCol::iterator iter_evt = arecestimeCol->begin();
	    for(; iter_evt!=arecestimeCol->end(); iter_evt++){
	      log << MSG::INFO
		<< "Test = "<<(*iter_evt)->getTest()
		<< ", Status = "<<(*iter_evt)->getStat()
		<<endreq;
	      if(m_ntupleflag && m_tuple2){ 
		g_Testime=(*iter_evt)->getTest();
	      }
	      //  cout<<"register to TDS: "<<(*iter_evt)->getTest()<<", "<<(*iter_evt)->getStat()<<endl;
	    }

	    if(m_ntupleflag){
	      if(m_tuple2){
		for(g_ntofdown=0;g_ntofdown<ntofdown;g_ntofdown++){ g_meantevdown[g_ntofdown]=meantevdown[g_ntofdown];}
		for(g_ntofup=0;g_ntofup<ntofup;g_ntofup++){ g_meantevup[g_ntofup]=meantevup[g_ntofup];}
		g_nmatch_tot=nmatch;
		m_estFlag=tEstFlag;/////20100427  guanyh add
		m_estTime=t_Est;
		StatusCode status = m_tuple2->write();
		if (!status.isSuccess()) {
		  log << MSG::ERROR << "Can't fill ntuple!" << endreq;
		}
	      }
	      if(m_tuple9){
		for(g_nmatch=0;g_nmatch<nmatch;g_nmatch++)
		{
		  g_meantev[g_nmatch]=meantev[g_nmatch];
		}
		StatusCode status = m_tuple9->write();
		if (!status.isSuccess()) {
		  log << MSG::ERROR << "Can't fill ntuple!" << endreq;
		}
	      }
	    }
	    return StatusCode::SUCCESS;
	  }

	  StatusCode EsTimeAlg::finalize() {

	    MsgStream log(msgSvc(), name());
	    log << MSG::INFO << "in finalize()" << endreq;
	    if(m_ntupleflag && m_tuple3){
	      StatusCode status = m_tuple3->write();
	      if (!status.isSuccess()) {
		log << MSG::ERROR << "Can't fill ntuple!" << endreq;
	      }
	    }
	    cout<<"EsTimeAlg::finalize(),total events in this run: "<<m_pass[0]<<endl;
	    return StatusCode::SUCCESS;
	  }

	  //make TDS
	  StatusCode EsTimeAlg::storeTDS(double tEst, int tEstFlag, double quality){
	    StatusCode sc;
	    MsgStream log(msgSvc(), name());

	    //check whether Recon already registered
	    DataObject *aReconEvent;
	    eventSvc()->findObject("/Event/Recon",aReconEvent);
	    if(aReconEvent==NULL) {
	      //then register Recon
	      aReconEvent = new ReconEvent();
	      sc = eventSvc()->registerObject("/Event/Recon",aReconEvent);
	      if(sc!=StatusCode::SUCCESS) {
		log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
		return StatusCode::FAILURE;
	      }
	    }

	    //clear MdcFastTrk
	    SmartIF<IDataManagerSvc> dataManagerSvc(eventSvc());
	    DataObject *aRecMdcTrack;
	    eventSvc()->findObject("/Event/Recon/RecMdcTrackCol",aRecMdcTrack);
	    if(aRecMdcTrack!=NULL){
	      dataManagerSvc->clearSubTree("/Event/Recon/RecMdcTrackCol");
	      eventSvc()->unregisterObject("/Event/Recon/RecMdcTrackCol");
	    }

	    if(tEst<0){
	      return StatusCode::SUCCESS;
	    }

	    //clear RecEsTimeCol
	    SmartIF<IDataManagerSvc> dataManSvc(eventSvc());
	    DataObject *aRecEsTime;
	    eventSvc()->findObject("/Event/Recon/RecEsTimeCol",aRecEsTime);
	    if(aRecEsTime!=NULL){
	      dataManSvc->clearSubTree("/Event/Recon/RecEsTimeCol");
	      eventSvc()->unregisterObject("/Event/Recon/RecEsTimeCol");
	    }

	    // register event start time to TDS
	    RecEsTimeCol *aRecEsTimeCol = new RecEsTimeCol;
	    sc = eventSvc()->registerObject("/Event/Recon/RecEsTimeCol", aRecEsTimeCol);
	    if(sc!=StatusCode::SUCCESS) {
	      log << MSG::ERROR << "Could not register RecEsTimeCol" << endreq;
	      return StatusCode::FAILURE;
	    }

	    RecEsTime *arecestime = new RecEsTime;
	    arecestime->setTest(tEst);
	    arecestime->setStat(tEstFlag);
	    arecestime->setQuality(quality);
	    aRecEsTimeCol->push_back(arecestime);

	    return StatusCode::SUCCESS;
	  }

	  double EsTimeAlg::Opt_new(const double *arr,const int size,const double sigma_cut)
	  {
	    Vdouble t0v_mdc;
	    t0v_mdc.clear();
	    double mean=-999;
	    double sigma=9999;
	    for(int i=0;i<size;i++){t0v_mdc.push_back(arr[i]);}
	    if(size==0)  mean=-999;
	    if(size==1) mean=t0v_mdc[0];
	    if(size==2) mean=0.5*(t0v_mdc[0]+t0v_mdc[1]);
	    if(size>=3)
	    {
	      for(int n=0;n<size;n++){
		mean=0.0;
		sigma=0.0;
		for(int i=0;i<t0v_mdc.size();i++){mean+=t0v_mdc[i];}
		mean=mean/t0v_mdc.size();
		for(int i=0;i<t0v_mdc.size();i++){sigma+=(t0v_mdc[i]-mean)*(t0v_mdc[i]-mean);}
		sigma=sigma/t0v_mdc.size();
		if(sigma<sigma_cut) break;
		double tmp=fabs(mean-t0v_mdc[0]);
		int no=0;
		for(int j=0;j<t0v_mdc.size();j++)
		{
		  if(fabs(mean-t0v_mdc[j])>=tmp){no=j;tmp=fabs(mean-t0v_mdc[j]);}
		}
		t0v_mdc.erase(t0v_mdc.begin()+no);
		if(t0v_mdc.size()<=2) break;
	      }
	      mean=0.0;            for(int i=0;i<t0v_mdc.size();i++){mean+=t0v_mdc[i];}
	      mean=mean/t0v_mdc.size();
	    }
	    return mean;
	  }

	  double EsTimeAlg::EST_Trimmer(double t0_original,double t0_offset,double raw_offset,double t0_offset_dt,double bunchTime)
	  {
	    int Nbunch = (int)( t0_original - t0_offset - raw_offset )/bunchTime;
	    if( (t0_original-t0_offset-raw_offset-bunchTime*Nbunch)>(bunchTime/2.) ) { Nbunch=Nbunch+1; }
	    double t_Est = Nbunch * bunchTime + t0_offset + t0_offset_dt;

	    return t_Est;
	  }
