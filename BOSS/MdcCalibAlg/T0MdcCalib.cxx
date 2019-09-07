#include "MdcCalibAlg/T0MdcCalib.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include <iostream>
#include <iomanip>
#include <cstring>

T0MdcCalib::T0MdcCalib(){
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  if(lay < 8){
	       m_docaMin[lay] = 1.2; // mm
	       m_docaMax[lay] = 4.8; // mm
	  } else{
	       m_docaMin[lay] = 1.6; // mm
	       m_docaMax[lay] = 6.4; // mm
	  }
     }
}

T0MdcCalib::~T0MdcCalib(){
}

void T0MdcCalib::clear(){
     for(int i=0; i<MdcCalTotCell; i++){
	  delete m_hleft[i];
	  delete m_hright[i];
     }
     delete m_hLrResiSum;
     delete m_hLrResiSub;
     delete m_fdT0;
     delete m_fdResiWire;

     MdcCalib::clear();
}

void T0MdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			    IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "T0MdcCalib");
     log << MSG::INFO << "T0MdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     m_vdr = 0.03;

     MdcCalib::initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

     int i;
     int nwire;
     int lay;
     int cel;
     char hname[200];

     m_fdT0 = new TFolder("fdT0", "fdT0");
     m_hlist -> Add(m_fdT0);

     m_fdResiWire = new TFolder("ResiWire", "ResiWire");
     m_hlist->Add(m_fdResiWire);

     nwire = m_mdcGeomSvc -> getWireSize();
     for(i=0; i<nwire; i++){
	  lay = m_mdcGeomSvc -> Wire(i) -> Layer();
	  cel = m_mdcGeomSvc -> Wire(i) -> Cell();

	  sprintf(hname, "Resi%04d_Lay%02d_Cell%03d_L", i, lay, cel);
	  m_hleft[i] = new TH1F(hname, "", 400, -2.0, 2.0);
	  m_fdT0 -> Add(m_hleft[i]);

	  sprintf(hname, "Resi%04d_Lay%02d_Cell%03d_R", i, lay, cel);
	  m_hright[i] = new TH1F(hname, "", 400, -2.0, 2.0);
	  m_fdT0 -> Add(m_hright[i]);
     }

     m_hLrResiSum = new TH1F("LrResiSum", "", 200, -0.5, 0.5);
     m_fdResiWire->Add(m_hLrResiSum);

     m_hLrResiSub = new TH1F("LrResiSub", "", 200, -0.5, 0.5);
     m_fdResiWire->Add(m_hLrResiSub);
}

int T0MdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "T0MdcCalib");
     log << MSG::DEBUG << "T0MdcCalib::fillHist()" << endreq;

     MdcCalib::fillHist(event);

     // get EsTimeCol
     bool esCutFg = event->getEsCutFlag();
     if( ! esCutFg ) return -1;

     int i;
     int k;
     int ntrk;
     int nhit;
     int stat;

     int lay;
     int cel;
     int wir;
     int lr;
     double dmeas;
     double doca;
     double resi;

     bool fgHitLay[MdcCalNLayer];

     MdcCalRecTrk* rectrk;
     MdcCalRecHit* rechit;

     ntrk = event -> getNTrk();
     if((ntrk < m_param.nTrkCut[0]) || (ntrk > m_param.nTrkCut[1])) return -1;

     for(i=0; i<ntrk; i++){
	  rectrk = event -> getRecTrk(i);
	  nhit = rectrk -> getNHits();

	  // dr cut
	  double dr = rectrk->getDr();
	  if(fabs(dr) > m_param.drCut) continue;

	  // dz cut
	  double dz = rectrk->getDz();
	  if(fabs(dz) > m_param.dzCut) continue;

	  for(lay=0; lay<MdcCalNLayer; lay++) fgHitLay[lay] = false;
	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       fgHitLay[lay] = true;
	  }

	  int nhitlay = 0;
	  for(lay=0; lay<MdcCalNLayer; lay++) if(fgHitLay[lay]) nhitlay++;
	  if(nhitlay < m_param.nHitLayCut) continue;

	  bool fgNoise = rectrk->getFgNoiseRatio();
	  if(m_param.noiseCut && (!fgNoise)) continue;

// 	  log << MSG::DEBUG << "nhits: " << nhit << endreq;
	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       cel = rechit -> getCellid();
	       wir = m_mdcGeomSvc -> Wire(lay, cel) -> Id();
	       lr = rechit -> getLR();
	       doca = rechit -> getDocaExc();
	       dmeas = rechit -> getDmeas();
	       resi = rechit -> getResiExc();
	       stat = rechit -> getStat();

	       if(1 != stat) continue;

	       if( (fabs(doca) < m_docaMin[lay]) ||
		   (fabs(doca) > m_docaMax[lay]) ||
		   (fabs(resi) > m_param.resiCut[lay]) ){
		    continue;
	       }

	       if(0 == lay){
		    if( ! fgHitLay[1] ) continue;
	       } else if(42 == lay){
		    if( ! fgHitLay[41] ) continue;
	       } else{
		    if( (!fgHitLay[lay-1]) && (!fgHitLay[lay+1]) ) continue;
	       }

	       if(wir < (m_mdcGeomSvc->getWireSize()) ){
		    if( 0 == lr ){
			 m_hleft[wir] -> Fill(resi);
		    }else if( 1 == lr ){
			 m_hright[wir] -> Fill(resi);
		    }
	       }else{
		    std::cout << "wireid: " << wir << std::endl;
	       }
	  }
     }
     return 1;
}

int T0MdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "T0MdcCalib");
     log << MSG::INFO << "T0MdcCalib::updateConst()" << endreq;

     MdcCalib::updateConst(calconst);

     int i;
     int lay;
     int nwire;
     double t0;
     double delt0;
     double resiLrSum;
     double resiLrSub;

     Stat_t entry_l;
     double mean_l;
//      double gmean_l;  // mean of fit with the gaussian distribution
//      double sigma_l;
//      double rms_l;

     Stat_t entry_r;
     double mean_r;
//      double gmean_r;
//      double sigma_r;
//      double rms_r;

     nwire = m_mdcGeomSvc -> getWireSize();
     std::cout << "totwire: " << nwire << std::endl;
     for(i=0; i<nwire; i++){
	  lay = m_mdcGeomSvc -> Wire(i) -> Layer();

	  if(1 == m_param.fgCalib[lay]){
	       entry_l = m_hleft[i] -> GetEntries();
	       mean_l = m_hleft[i] -> GetMean();
// 	       m_hleft[i] -> Fit("gaus", "Q");
// 	       gmean_l = m_hleft[i] -> GetFunction("gaus") -> GetParameter(1);

	       entry_r = m_hright[i] -> GetEntries();
	       mean_r = m_hright[i] -> GetMean();
// 	       m_hright[i] -> Fit("gaus", "Q");
// 	       gmean_r = m_hright[i] -> GetFunction("gaus") -> GetParameter(1);

	       delt0 = 0.5 * (mean_l + mean_r) / m_vdr;
	  } else{
	       delt0 = 0.0;
	  }

	  resiLrSum = 0.5 * (mean_l + mean_r);
	  resiLrSub = 0.5 * (mean_l - mean_r);
	  m_hLrResiSum->Fill(resiLrSum);
	  m_hLrResiSub->Fill(resiLrSub);

	  t0 = calconst -> getT0(i);
	  t0 += delt0;

	  calconst -> resetT0(i, t0);
	  calconst -> resetDelT0(i, delt0);
     }
     return 1;
}

