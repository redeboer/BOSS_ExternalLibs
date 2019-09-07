// MdcAlignAlg
// 2008/03/26   Wu Linghui / Luo Tao   IHEP

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "EventModel/EventHeader.h"

#include "MdcAlignAlg/MdcAlignAlg.h"
#include "MdcAlignAlg/ResiAlign.h"
#include "MdcAlignAlg/MilleAlign.h"

#include "TFile.h"

#include <iostream>

using namespace std;
// using namespace Event;

/////////////////////////////////////////////////////////////////////////////

MdcAlignAlg::MdcAlignAlg(const std::string& name, ISvcLocator* pSvcLocator) :
     Algorithm(name, pSvcLocator), m_alignMeth(0), m_flgKalFit(0){

     m_distAlign = false;
     m_configFile = "MdcCalibConfig.txt";
     m_fixMomLab = "fixMom_lab.txt";
     m_nEvt = 0;
     m_mag = 1.0;

     //declare properties
     declareProperty("DistAlign", m_distAlign);
     declareProperty("MdcAlignMeth", m_alignMeth);
     declareProperty("UseKalFit", m_flgKalFit);
     declareProperty("MagneticField", m_mag);
     declareProperty("AlignFile", m_alignFile);
     declareProperty("HistFile", m_histname);
     declareProperty("ConfigFile", m_configFile);
     declareProperty("FixMomLab", m_fixMomLab);
     declareProperty("NumEvtDisplay", m_nEvtDisp);           
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcAlignAlg::initialize() {
     MsgStream log( msgSvc(), name() );
     log << MSG::INFO << "MdcAlignAlg initialze() ..." << endreq;
     log << MSG::INFO << "MdcAlignFlg = " << m_alignMeth << endreq;

     StatusCode sc = service("MdcGeomSvc", m_mdcGeomSvc);
     if(sc != StatusCode::SUCCESS){
	  log << MSG::ERROR << "can not use MdcGeomSvc" << endreq;
     }

     sc = service("MdcCalibFunSvc", m_mdcFunSvc);
     if( sc != StatusCode::SUCCESS ){
	  log << MSG::FATAL << "can not use MdcCalibFunSvc" << endreq;
     }

     // initialize m_param
     initParam();

     // read mdc config parameters
     int i;
     std::string strconfig;
     std::string strcomment;
     std::string strTes;
     int fgTes[50];
     for(i=0; i<50; i++) fgTes[i] = -999;
     ifstream fconfig( m_configFile.c_str() );
     if( ! fconfig.is_open() ){
	  log << MSG::WARNING << "can not open config file " << m_configFile
	      << ". Use defalt config parameters" << endreq;
     } else {
	  log << MSG::INFO << "Open config file " << m_configFile << endreq;
	  while( fconfig >> strconfig ){
	       if('#' == strconfig[0]){
		    getline(fconfig, strcomment);
	       }  else if("EsTimeFlag" == strconfig){
		    getline(fconfig, strTes);
		    int n = sscanf(strTes.c_str(), "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
				   fgTes+0, fgTes+1, fgTes+2, fgTes+3, fgTes+4,
				   fgTes+5, fgTes+6, fgTes+7, fgTes+8, fgTes+9,
				   fgTes+10, fgTes+11, fgTes+12, fgTes+13, fgTes+14,
				   fgTes+15, fgTes+16, fgTes+17, fgTes+18, fgTes+19);
		    for(i=0; i<n; i++) m_param.esFlag[i] = fgTes[i];
		    m_param.nEsFlag = n;
	       }  else if("TesMin" == strconfig){
		    fconfig >> m_param.tesMin;
	       } else if("TesMax" == strconfig){
		    fconfig >> m_param.tesMax;
	       }  else if("ResidualType" == strconfig){
		    fconfig >> m_param.resiType;
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
	       } else if("ptCut" == strconfig){
		    fconfig >> m_param.ptCut[0] >> m_param.ptCut[1];
	       } else if("cosThetaCut" == strconfig){
		    fconfig >> m_param.costheCut[0] >> m_param.costheCut[1];
	       } else if("DrCut" == strconfig){
		    fconfig >> m_param.drCut;
	       } else if("DzCut" == strconfig){
		    fconfig >> m_param.dzCut;
	       } else if("MaximalDocaInner" == strconfig){
		    fconfig >> m_param.maxDocaInner;
	       }else if("MaximalDocaOuter" == strconfig){
		    fconfig >> m_param.maxDocaOuter;
	       }else if("MaximalResidual" == strconfig){
		    fconfig >> m_param.maxResi;
	       }
	  }
	  fconfig.close();
     }

     // set event type
     m_param.particle = m_evtType;

     cout << "EsFlag for Mdc Alignment: ";
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++) cout << setw(6) << m_param.esFlag[iEs];
     cout << endl;

     if(fabs(m_mag) > 0.01) Alignment::gFlagMag = true;
     else Alignment::gFlagMag = false;

     m_alignPar = new MdcAlignPar();
     m_alignPar -> rdAlignPar(m_alignFile);

     m_mdcevt = new MdcAliEvent();
     m_mdcevt -> setParam(m_param);

     m_hlist  = new TObjArray();

     if( 0 == m_alignMeth ){
	  m_pAlign = new ResiAlign();
     } else if( 1 == m_alignMeth ){
	  m_pAlign = new MilleAlign();
	  m_pAlign->fixMomLab = m_fixMomLab;
     }
     m_pAlign->setParam(m_param);
     m_pAlign -> initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc);

     return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcAlignAlg::execute() {
     MsgStream log(msgSvc(), name());
     log << MSG::DEBUG << "MdcAlignAlg execute()" << endreq;

     // display event number for debug
     SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
     if (!eventHeader) {
	  log << MSG::FATAL << "Could not find Event Header" << endreq;
	  return( StatusCode::FAILURE);
     }
     int iEvt = eventHeader->eventNumber();
     if(0 == (m_nEvt % m_nEvtDisp)) std::cout << "Event " << m_nEvt << ",   Event number online " << iEvt << endl;
     m_nEvt++;

     // get reconstruction result
     if(m_flgKalFit)  m_mdcevt -> setKalEvent();
     else             m_mdcevt -> setRecEvent();

     if (!(m_pAlign->fillHist(m_mdcevt))) {
	  m_mdcevt->clear();
	  return(StatusCode::SUCCESS);
     }
     m_mdcevt->clear();

     return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcAlignAlg::finalize() {
     MsgStream log(msgSvc(), name());
     log << MSG::INFO << "MdcAlignAlg finalize()" << endreq;

     if( ! m_distAlign ){
	  // execute calibration and write new calibration data file
	  m_pAlign -> updateConst(m_alignPar);
	  m_alignPar -> wrtAlignPar();
     }

     TFile* fhist = new TFile(m_histname.c_str(), "recreate");
     m_hlist -> Write();

     m_pAlign -> clear();
     delete m_pAlign;
     std::cout << "m_pAlign deleted" << std::endl;

     delete m_alignPar;
     std::cout << "m_alignPar deleted" << std::endl;

     delete m_mdcevt;
     std::cout << "m_mdcevt deleted" << std::endl;

     delete m_hlist;
     std::cout << "m_hlist deleted" << std::endl;

     fhist->Close();
     delete fhist;
     cout << m_histname << " was written" << endl;

     if( ! m_distAlign ){
	  std::ofstream fend("endalign.out");
	  fend << "MdcAlign end." << std::endl;
	  fend.close();
     }

     std::cout << "MdcAlignAlg End." << std::endl;

     return StatusCode::SUCCESS;
}

void MdcAlignAlg::initParam(){
     m_param.particle = 0;
     m_param.nEsFlag = 0;
     for(int i=0; i<50; i++) m_param.esFlag[i] = -999;
     m_param.tesMin = 0.0;
     m_param.tesMax = 9999.0;
     m_param.resiType = 0;
     m_param.fgAdjacLayerCut = 0;
     m_param.fgBoundLayerCut = 0;
     m_param.hitStatCut = 0;
     m_param.nTrkCut[0] = 2;
     m_param.nTrkCut[1] = 2;
     m_param.nHitLayCut = 35;
     m_param.nHitCut = 70;
     m_param.ptCut[0] = 0.0;
     m_param.ptCut[1] = 100.0;
     m_param.costheCut[0] = -1.0;
     m_param.costheCut[1] = 1.0;
     m_param.drCut = 50.0;	// mm
     m_param.dzCut = 300.0;	// mm
     m_param.maxDocaInner = 8.0;	// mm
     m_param.maxDocaOuter = 12.0;	// mm
     m_param.maxResi = 1.0;	// mm

}
