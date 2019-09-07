// MdcCalibAlg
// 2006/01/09   Wu Linghui   IHEP

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "MdcCalibAlg/MdcCalibAlg.h"
#include "EventModel/EventHeader.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include "TStyle.h"

using namespace Event;

/////////////////////////////////////////////////////////////////////////////

MdcCalibAlg::MdcCalibAlg(const std::string& name, ISvcLocator* pSvcLocator) :
     Algorithm(name, pSvcLocator), m_mdcCalFlg(0), m_flgKalFit(0), m_evtType(0){

     m_histname = "NULL";
     m_configFile = "NULL";
     m_nEvtDisp = 1000;
     m_distCalib = false;
     m_nEvt = 0;

     // declare properties
     declareProperty("MdcCalFlg", m_mdcCalFlg);
     declareProperty("MdcKalmanFlg", m_flgKalFit);
     declareProperty("Event", m_evtType);
     declareProperty("HistOutput", m_histname);
     declareProperty("ConfigFile", m_configFile);
     declareProperty("WirePosCorFile", m_wpcFile);
     declareProperty("NumEvtDisplay", m_nEvtDisp);
     declareProperty("DistCalib", m_distCalib);
     declareProperty("Ecm", m_ecm = 3.686);

     m_initCalConstFlg = false;
}

MdcCalibAlg::~MdcCalibAlg(){
     delete m_constmgr;
     std::cout << "m_constmgr deleted" << std::endl;

     delete m_mdccalib;
     std::cout << "delete m_mdccalib" << std::endl;

     delete m_mdcevt;
     std::cout << "m_mdcevt deleted" << std::endl;

     delete m_hlist;
     std::cout << "m_hlist deleted" << std::endl;

     delete m_calconst;
     std::cout << "m_calconst deleted" << std::endl;

     if(!m_distCalib){
	  std::ofstream fend("endcal.out");
	  fend << "MdcCalib end." << std::endl;
	  fend.close();
     }

     std::cout << "MdcCalibAlg End." << std::endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcCalibAlg::initialize() {
     MsgStream log( msgSvc(), name() );
     log << MSG::INFO << "MdcCalibAlg initialze() ..." << endreq;
     log << MSG::INFO << "MdcCalFlg = " << m_mdcCalFlg << endreq;

     if("NULL"==m_histname){
	  log << MSG::ERROR << "not defined histogram file." << endreq;
	  return StatusCode::FAILURE;
     }
     if("NULL"==m_configFile){
	  log << MSG::ERROR << "not defined MdcCalibConfig file." << endreq;
	  return StatusCode::FAILURE;
     }

     StatusCode sc = service("MdcGeomSvc", m_mdcGeomSvc);
     if(sc != StatusCode::SUCCESS){
	  log << MSG::ERROR << "can not use MdcGeomSvc" << endreq;
     }

     sc = service("MdcCalibFunSvc", m_mdcFunSvc);
     if( sc != StatusCode::SUCCESS ){
	  log << MSG::FATAL << "can not use MdcCalibFunSvc" << endreq;
     }

     sc = service ("MdcUtilitySvc", m_mdcUtilitySvc);
     if ( sc.isFailure() ){
	  log << MSG::FATAL << "Could not load MdcUtilitySvc!" << endreq;
     }

     string estver = getenv("ESTIMEALGROOT");
     cout << "EsTimeAlg_ver: "<< estver << endl;

     // initialize m_param
     initParam();

     // read mdc config parameters
     int i;
     int lay;
     std::string strconfig;
     std::string strcomment;
     std::string strtmp;
     std::string strTes;
     int fgTes[50];
     for(i=0; i<50; i++) fgTes[i] = -999;
     ifstream fconfig( m_configFile.c_str() );
     if( ! fconfig.is_open() ){
	  log << MSG::ERROR << "can not open config file " << m_configFile
	      << ". Use defalt config parameters" << endreq;
	  return StatusCode::FAILURE;
     } else {
	  log << MSG::INFO << "Open config file " << m_configFile << endreq;
	  while( fconfig >> strconfig ){
	       if('#' == strconfig[0]){
		    getline(fconfig, strcomment);
	       } else if("FillNtuple" == strconfig){
		    fconfig >> m_param.fillNtuple;
	       } else if("nEvtNtuple" == strconfig){
		    fconfig >> m_param.nEvtNtuple;
	       }else if("FlagCalDetEffi" == strconfig){
		    fconfig >> m_param.fgCalDetEffi;
	       }else if("BoostPar" == strconfig){
		    fconfig >> m_param.boostPar[0] >> m_param.boostPar[1] >> m_param.boostPar[2];
	       } else if("EsTimeFlag" == strconfig){
		    getline(fconfig, strTes);
		    int n = sscanf(strTes.c_str(), "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
				   fgTes+0, fgTes+1, fgTes+2, fgTes+3, fgTes+4,
				   fgTes+5, fgTes+6, fgTes+7, fgTes+8, fgTes+9,
				   fgTes+10, fgTes+11, fgTes+12, fgTes+13, fgTes+14,
				   fgTes+15, fgTes+16, fgTes+17, fgTes+18, fgTes+19);
		    for(i=0; i<n; i++) m_param.esFlag[i] = fgTes[i];
		    m_param.nEsFlag = n;
	       } else if("TimeShift" == strconfig){
		    fconfig >> m_param.timeShift;
	       } else if("TesMin" == strconfig){
		    fconfig >> m_param.tesMin;
	       } else if("TesMax" == strconfig){
		    fconfig >> m_param.tesMax;
	       } else if("FlagIniCalConst" == strconfig){
		    fconfig >> m_param.fgIniCalConst;
	       } else if("FlagUpdateTmInPreT0" == strconfig){
		    fconfig >> m_param.preT0SetTm;
	       } else if("InitT0" == strconfig){
		    fconfig >> m_param.initT0;
	       } else if("T0Shift" == strconfig){
		    fconfig >> m_param.t0Shift;
	       } else if("TminFitChindf" == strconfig){
		    fconfig >> m_param.tminFitChindf;
	       } else if("TmaxFitChindf" == strconfig){
		    fconfig >> m_param.tmaxFitChindf;
	       } else if("InitSigma" == strconfig){
		    fconfig >> m_param.initSigma;
	       } else if("UpdateSigma" == strconfig){
		    fconfig >> m_param.calSigma;
	       } else if("ResidualType" == strconfig){
		    fconfig >> m_param.resiType;
	       } else if("FixXtC0" == strconfig){
		    fconfig >> m_param.fixXtC0;
	       } else if("FixXtEdge" == strconfig){
		    fconfig >> m_param.fixXtEdge;
	       } else if("FlagAdjacLayerCut" == strconfig){
		    fconfig >> m_param.fgAdjacLayerCut;
	       } else if("FlagBoundLayerCut" == strconfig){
		    fconfig >> m_param.fgBoundLayerCut;
	       } else if("hitStatCut" == strconfig){
		    fconfig >> m_param.hitStatCut;
	       } else if("nTrkCut" == strconfig){
		    fconfig >> m_param.nTrkCut[0] >> m_param.nTrkCut[1];
	       } else if("nHitLayCut" == strconfig){
		    fconfig >> m_param.nHitLayCut;
	       } else if("nHitCut" == strconfig){
		    fconfig >> m_param.nHitCut;
	       } else if("noiseCut" == strconfig){
		    fconfig >> m_param.noiseCut;
	       } else if("cosThetaCut" == strconfig){
		    fconfig >> m_param.costheCut[0] >> m_param.costheCut[1];
	       } else if("DrCut" == strconfig){
		    fconfig >> m_param.drCut;
	       } else if("DzCut" == strconfig){
		    fconfig >> m_param.dzCut;
	       }else if("MaximalDocaInner" == strconfig){
		    fconfig >> m_param.maxDocaInner;
	       }else if("MaximalDocaOuter" == strconfig){
		    fconfig >> m_param.maxDocaOuter;
	       } else if("RawTimeFitRange" == strconfig){
		    for(lay=0; lay<MdcCalNLayer; lay++){
			 fconfig >> strtmp
				 >> m_param.fgCalib[lay]
				 >> m_param.tminFitRange[lay][0]
				 >> m_param.tminFitRange[lay][1]
				 >> m_param.tmaxFitRange[lay][0]
				 >> m_param.tmaxFitRange[lay][1]
				 >> m_param.initTm[lay]
				 >> m_param.resiCut[lay]
				 >> m_param.qmin[lay]
				 >> m_param.qmax[lay];
		    }
	       }
	  }
	  fconfig.close();
     }
     // set single wire position calibration file
     m_param.wpcFile = m_wpcFile;

     // set event type
     m_param.particle = m_evtType;

     // set Ecm
     m_param.ecm = m_ecm;

     cout << "EsFlag for Mdc Calibration: ";
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++) cout << setw(6) << m_param.esFlag[iEs];
     cout << endl;

     m_hlist    = new TObjArray(0);
     m_constmgr = new MdcCalConstMgr();
     m_calconst = new MdcCalibConst();

     m_mdcevt   = new MdcCalEvent();
     m_mdcevt -> setParam(m_param);
     m_mdcevt -> setGeomSvc(m_mdcGeomSvc);
     m_mdcevt -> setUtilSvc(m_mdcUtilitySvc);

     if( 0 == m_mdcCalFlg ){
	  m_mdccalib  = new IniMdcCalib();
     }else if( 1 == m_mdcCalFlg ){
	  m_mdccalib  = new PreXtMdcCalib();
     }else if( 2 == m_mdcCalFlg ){
	  m_mdccalib  = new PreT0MdcCalib();
     }else if( 3 == m_mdcCalFlg ){
	  m_mdccalib  = new XtMdcCalib();
     }else if( 4 == m_mdcCalFlg ){
	  m_mdccalib  = new GrXtMdcCalib();
     }else if( 9 == m_mdcCalFlg ){
	  m_mdccalib  = new XtInteMdcCalib();
     }else if( 5 == m_mdcCalFlg ){
	  m_mdccalib  = new T0MdcCalib();
     }else if( 6 == m_mdcCalFlg ){
	  m_mdccalib  = new WrMdcCalib();
     }else if( 7 == m_mdcCalFlg ){
	  m_mdccalib  = new Wr2dMdcCalib();
     }else if( 8 == m_mdcCalFlg ){
	  m_mdccalib  = new QtMdcCalib();
     }
     // initialize and read the calibraion data from TCDS
     m_constmgr -> init(m_mdcGeomSvc, m_mdcFunSvc);

     m_mdccalib -> setParam(m_param);
     m_mdccalib -> initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

//      if(0 == m_mdcCalFlg) m_calconst->initCalibConst();

     return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcCalibAlg::execute() {
     MsgStream log(msgSvc(), name());
     log << MSG::DEBUG << "MdcCalibAlg execute()" << endreq;

     // display event number for debug
     SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
     if (!eventHeader) {
	  log << MSG::FATAL << "Could not find Event Header" << endreq;
	  return( StatusCode::FAILURE);
     }
     int iRun = eventHeader->runNumber();
     int iEvt = eventHeader->eventNumber();
     if(0 == (m_nEvt % m_nEvtDisp))
	  std::cout << "Run " << iRun << ", Event " << iEvt << ",   Total Event number " << m_nEvt << endl;

     m_mdcevt->setEvtNoOnline(iEvt);
     m_mdcevt->setEvtNoOffline(m_nEvt);

     if( ! m_initCalConstFlg ){
// 	  if((0 == m_mdcCalFlg) && (0 == m_param.fgIniCalConst)){
// 	       m_calconst->initCalibConst();
// 	  } else{
// 	       m_constmgr -> rdConstTcds(m_calconst);
// 	  }
	  m_constmgr -> rdConstTcds(m_calconst);
	  m_initCalConstFlg = true;
     }

     if(m_mdcCalFlg > 1){
	  if(m_flgKalFit)  m_mdcevt -> setKalEvent();
	  else             m_mdcevt -> setRecEvent();
     }

     // fill histograms
     m_mdccalib -> fillHist(m_mdcevt);
     m_mdcevt   -> clear();
     m_nEvt++;

     return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcCalibAlg::finalize() {
     MsgStream log(msgSvc(), name());
     log << MSG::INFO << "MdcCalibAlg finalize()" << endreq;

     gStyle -> SetCanvasBorderMode(0);
     gStyle -> SetCanvasColor(10);
     gStyle -> SetOptFit(0011);
     gStyle -> SetStatColor(10);
     gStyle -> SetStatBorderSize(1);
     gStyle -> SetStatFont(42);
     gStyle -> SetStatFontSize(0.04);
     gStyle -> SetStatX(0.9);
     gStyle -> SetStatY(0.9);
     gStyle -> SetPadColor(10);
     gStyle -> SetFuncColor(2);

     // execute calibration and write new calibration data file
     if(!m_distCalib){
	  m_mdccalib -> updateConst(m_calconst);
	  m_constmgr -> wrtConst( m_calconst );
     }

     // write histogram file
     TFile fhist(m_histname.c_str(), "recreate");
     m_hlist -> Write();
     fhist.Close();
     std::cout << m_histname << " was written" << std::endl;

     m_mdccalib->clear();

     return StatusCode::SUCCESS;
}

void MdcCalibAlg::initParam(){
     m_param.fillNtuple = 0;
     m_param.nEvtNtuple = 10000;
     m_param.fgCalDetEffi = 0;
     m_param.ecm = 3.686;
     m_param.boostPar[0] = 0.011;
     m_param.boostPar[1] = 0.0;
     m_param.boostPar[2] = 0.0;
     m_param.particle = 0;
     m_param.nEsFlag = 0;
     for(int i=0; i<50; i++) m_param.esFlag[i] = -999;
     m_param.timeShift = 0.0;
     m_param.tesMin = 0.0;
     m_param.tesMax = 9999.0;
     m_param.fgIniCalConst = 2;
     m_param.preT0SetTm = true;
     m_param.initT0 = 50.0;
     m_param.timeShift = 0.0;
     m_param.t0Shift = 0.0;
     m_param.tminFitChindf = 20.0;
     m_param.tmaxFitChindf = 20.0;
     m_param.initSigma = 0.16;	// mm
     m_param.resiType = 1;
     m_param.resiType = 0;
     m_param.fixXtC0 = 0;
     m_param.fixXtEdge = 0;
     m_param.fgAdjacLayerCut = 0;
     m_param.fgBoundLayerCut = 0;
     m_param.hitStatCut = 0;
     m_param.nTrkCut[0] = 2;
     m_param.nTrkCut[1] = 2;
     m_param.nHitLayCut = 35;
     m_param.nHitCut = 0;
     m_param.noiseCut = false;
     m_param.costheCut[0] = -1.0;
     m_param.costheCut[1] = 1.0;
     m_param.drCut = 50.0;	// mm
     m_param.dzCut = 300.0;	// mm
     m_param.maxDocaInner = 8.0;	// mm
     m_param.maxDocaOuter = 12.0;	// mm

     for(int lay=0; lay<MdcCalNLayer; lay++){
	  m_param.fgCalib[lay] = 1;
	  m_param.tminFitRange[lay][0] = 0.0;
	  m_param.tminFitRange[lay][1] = 100.0;
	  m_param.tmaxFitRange[lay][0] = 120.0;
	  m_param.tmaxFitRange[lay][1] = 400.0;
	  m_param.initTm[lay] = 300.0;
	  m_param.resiCut[lay] = 1.2;
	  m_param.qmin[lay] = 400;
	  m_param.qmax[lay] = 1000;
     }
}
