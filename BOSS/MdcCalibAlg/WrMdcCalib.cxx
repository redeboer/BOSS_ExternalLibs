#include "MdcCalibAlg/WrMdcCalib.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include <iostream>
#include <iomanip>
#include <cstring>

WrMdcCalib::WrMdcCalib(){
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

WrMdcCalib::~WrMdcCalib(){
}

void WrMdcCalib::clear(){
     for(int i=0; i<MdcCalTotCell; i++){
	  delete m_hleft[i];
	  delete m_hright[i];
     }
     delete m_hdwxtot;
     delete m_hddwx;
     delete m_hdwytot;
     delete m_hddwy;
     delete m_hLrResiSum;
     delete m_hLrResiSub;
     delete m_fdWire;
     delete m_fdResiWire;
     MdcCalib::clear();
}

void WrMdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			    IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "WrMdcCalib");
     log << MSG::INFO << "WrMdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     MdcCalib::initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

     int i;
     int nwire;
     int lay;
     int cel;
     char hname[200];

     m_fdWire = new TFolder("WireCor", "WireCor");
     m_hlist->Add(m_fdWire);

     m_fdResiWire = new TFolder("ResiWire", "ResiWire");
     m_hlist->Add(m_fdResiWire);

     nwire = m_mdcGeomSvc -> getWireSize();
     for(i=0; i<nwire; i++){
	  lay = m_mdcGeomSvc -> Wire(i) -> Layer();
	  cel = m_mdcGeomSvc -> Wire(i) -> Cell();

	  sprintf(hname, "h%04d_L%02d_%03d_Left", i, lay, cel);
	  m_hleft[i] = new TH1F(hname, hname, 300, -1.5, 1.5);
	  m_fdWire->Add(m_hleft[i]);

	  sprintf(hname, "h%04d_L%02d_%03d_Right", i, lay, cel);
	  m_hright[i] = new TH1F(hname, hname, 300, -1.5, 1.5);
	  m_fdWire->Add(m_hright[i]);
     }

     m_hdwxtot = new TH1F("dwXtot", "", 100, -0.5, 0.5);
     m_fdResiWire->Add(m_hdwxtot);

     m_hddwx = new TH1F("ddwX", "", 100, -0.5, 0.5);
     m_fdResiWire->Add(m_hddwx);

     m_hdwytot = new TH1F("dwYtot", "", 100, -0.5, 0.5);
     m_fdResiWire->Add(m_hdwytot);

     m_hddwy = new TH1F("ddwY", "", 100, -0.5, 0.5);
     m_fdResiWire->Add(m_hddwy);

     m_hLrResiSum = new TH1F("LrResiSum", "", 200, -0.5, 0.5);
     m_fdResiWire->Add(m_hLrResiSum);

     m_hLrResiSub = new TH1F("LrResiSub", "", 200, -0.5, 0.5);
     m_fdResiWire->Add(m_hLrResiSub);
}

int WrMdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "WrMdcCalib");
     log << MSG::DEBUG << "WrMdcCalib::fillHist()" << endreq;

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

     ntrk = event->getNTrk();
     log << MSG::DEBUG << "number of tracks: " << ntrk << endreq;

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

	  log << MSG::DEBUG << "number of hits: " << nhit << endreq;
	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       cel = rechit -> getCellid();
	       wir = m_mdcGeomSvc ->Wire(lay, cel)->Id();
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
		    std::cout << "wir: " << wir << std::endl;
	       }
	  }
     }
     return 1;
}

int WrMdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "WrMdcCalib");
     log << MSG::INFO << "WrMdcCalib::updateConst()" << endreq;

     MdcCalib::updateConst(calconst);

     int i;
     int lay;
     int cel;
     int nwire = m_mdcGeomSvc -> getWireSize();
     double dwphi;		// wire derivation in phi direction
     double resiLrSum;		// to fill histogram
     double wpos[6];
     double xx;
     double yy;
     double rr;
     double dx;
     double dy;
     double wphi;

     Stat_t entry_l;
     double mean_l;
     Stat_t entry_r;
     double mean_r;
     const MdcGeoWire* pWire;

     double ddw[MdcCalTotCell][6];
     double dwinput[MdcCalTotCell][6];
     string strtmp;
     ifstream fwpinput(m_param.wpcFile.c_str());
     for(i=0; i<7; i++) fwpinput >> strtmp;
     for(i=0; i<nwire; i++){
	  fwpinput >> strtmp >> dwinput[i][0] >> dwinput[i][1] >> dwinput[i][2]
		   >> dwinput[i][3] >> dwinput[i][4] >> dwinput[i][5];
     }
     fwpinput.close();

     std::cout << "totwire: " << nwire << std::endl;
     for(i=0; i<nwire; i++){
	  pWire = m_mdcGeomSvc -> Wire(i);
	  lay = pWire -> Layer();
	  cel = pWire -> Cell();

	  if(1 == m_param.fgCalib[lay]){
	       entry_l = m_hleft[i] -> GetEntries();
	       mean_l = m_hleft[i] -> GetMean();

	       entry_r = m_hright[i] -> GetEntries();
	       mean_r = m_hright[i] -> GetMean();

	       dwphi = 0.5 * (mean_l - mean_r);
	  } else{
	       dwphi = 0.0;
	  }

	  resiLrSum = 0.5 * (mean_l + mean_r);
	  m_hLrResiSum->Fill(resiLrSum);
	  m_hLrResiSub->Fill(dwphi);

	  wpos[0] = pWire -> Backward().x(); // east end
	  wpos[1] = pWire -> Backward().y();
	  wpos[3] = pWire -> Forward().x(); // west end
	  wpos[4] = pWire -> Forward().y();

	  xx = 0.5 * (wpos[0] + wpos[3]);
	  yy = 0.5 * (wpos[1] + wpos[4]);
	  rr = sqrt( (xx * xx) + (yy * yy) );

	  if( yy >= 0 )  wphi = acos(xx / rr);
	  else           wphi = PI2 - acos(xx / rr);

	  dx = -1.0 * dwphi * sin(wphi);
	  dy = dwphi * cos(wphi);

	  ddw[i][0] = dx;
	  ddw[i][1] = dy;
	  ddw[i][2] = 0.0;
	  ddw[i][3] = dx;
	  ddw[i][4] = dy;
	  ddw[i][5] = 0.0;
     }

     ofstream fwpc("WirePosCalib_new.txt");
     fwpc << setw(5) << "wireId" << setw(15) << "dx_East(mm)"
	  << setw(15) << "dy_East(mm)" << setw(15) << "dz_East(mm)"
	  << setw(15) << "dx_West(mm)" << setw(15) << "dy_West(mm)"
	  << setw(15) << "dz_West(mm)" << endl;

     ofstream fdw("dw.txt");
     fdw << setw(5) << "wireId" << setw(15) << "ddx_East(mm)"
	 << setw(15) << "ddy_East(mm)" << setw(15) << "ddz_East(mm)"
	 << setw(15) << "ddx_West(mm)" << setw(15) << "ddy_West(mm)"
	 << setw(15) << "ddz_West(mm)" << endl;

     int k;
     double dwtot[6];
     for(i=0; i<nwire; i++){
	  for(k=0; k<6; k++) dwtot[k] = dwinput[i][k] + ddw[i][k];
	  fwpc << setw(5) << i;
	  for(k=0; k<6; k++) fwpc << setw(15) << dwtot[k];
	  fwpc << endl;

	  fdw << setw(5) << i;
	  for(k=0; k<6; k++) fdw << setw(15) << ddw[i][k];
	  fdw << endl;
	  m_hdwxtot->Fill(dwtot[0]);
	  m_hddwx->Fill(ddw[i][0]);
	  m_hdwytot->Fill(dwtot[1]);
	  m_hddwy->Fill(ddw[i][1]);
     }
     fwpc.close();
     fdw.close();
     return 1;
}

