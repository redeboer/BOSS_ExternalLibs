#include "MdcCalibAlg/QtMdcCalib.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <math.h>

#include "TF1.h"
#include "TMinuit.h"

using namespace std;

QtMdcCalib::QtMdcCalib(){
     m_vdr = 0.03;

     m_nlayer = MdcCalNLayer;
     m_qtorder = MdcCalQtOrd;
     m_nbin = MdcCalNQBin;
     m_innNLay = MdcCalInnNLay;
}

QtMdcCalib::~QtMdcCalib(){
}

void QtMdcCalib::clear(){
     int bin;
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  delete m_hqhit[lay];
	  delete m_grqt[lay];
	  delete m_grqdt[lay];
	  for(bin=0; bin<MdcCalQtOrd; bin++){
	       delete m_hqt[lay][bin];
	  }
     }
     delete m_fdQt;
     delete m_fdQ_T;

     MdcCalib::clear();
}

void QtMdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			    IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "QtMdcCalib");
     log << MSG::INFO << "QtMdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     MdcCalib::initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

     int bin;
     int lay;
     double qbinw;
     char hname[200];

     for(lay=0; lay<m_nlayer; lay++){
	  m_qmin[lay] = m_param.qmin[lay];
	  m_qmax[lay] = m_param.qmax[lay];
	  m_qbinw[lay] = (m_qmax[lay] - m_qmin[lay]) / (double)m_nbin;
     }

     m_fdQt = new TFolder("fdQt", "fdQt");
     m_fdQ_T = new TFolder("QtPlot", "QtPlot");
     m_hlist -> Add(m_fdQt);
     m_hlist -> Add(m_fdQ_T);

     for(lay=0; lay<m_nlayer; lay++){
	  sprintf(hname, "HQ_Layer%02d", lay);
	  m_hqhit[lay] = new TH1F(hname, "", 1500, 0, 3000);
	  m_fdQt -> Add(m_hqhit[lay]);

	  sprintf(hname, "HQT_Plot_lay%02d", lay);
	  m_grqt[lay] = new TGraphErrors();
	  m_grqt[lay]->SetName(hname);
	  m_grqt[lay]->SetMarkerStyle(20);
	  m_grqt[lay]->SetMarkerColor(1);
	  m_fdQ_T->Add(m_grqt[lay]);

	  sprintf(hname, "HQdelT_Plot_lay%02d", lay);
	  m_grqdt[lay] = new TGraphErrors();
	  m_grqdt[lay]->SetName(hname);
	  m_grqdt[lay]->SetMarkerStyle(10);
	  m_grqdt[lay]->SetMarkerColor(1);
	  m_fdQ_T->Add(m_grqdt[lay]);
        
	  for(bin=0; bin<m_nbin; bin++){
	       sprintf(hname, "HQT_Lay%02d_Bin%02d", lay, bin);
	       m_hqt[lay][bin] = new TH1F(hname, "", 200, -1, 1);
	       m_fdQt -> Add(m_hqt[lay][bin]);
	  }
     }
}

int QtMdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "QtMdcCalib");
     log << MSG::DEBUG << "QtMdcCalib::fillHist()" << endreq;

     MdcCalib::fillHist(event);

     // get EsTimeCol
     bool esCutFg = event->getEsCutFlag();
     if( ! esCutFg ) return -1;

     int i;
     int k;
     int bin;
     int lay;

     double doca;
     double dmeas;

     int ntrk;
     int nhit;

     bool fgHitLay[MdcCalNLayer];

     MdcCalRecTrk* rectrk;
     MdcCalRecHit* rechit;

     ntrk = event -> getNTrk();
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

	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       doca = rechit -> getDocaInc();
	       dmeas = rechit -> getDmeas();
	       m_resi = rechit -> getResiInc();
	       m_qhit = rechit -> getQhit();

	       m_hqhit[lay] -> Fill(m_qhit);

	       bin = (int)((m_qhit - m_qmin[lay]) / m_qbinw[lay]);
	       if( (bin >= 0) && (bin < m_nbin) ){
		    m_hqt[lay][bin] -> Fill( m_resi );
	       }
	  }
     }
     return 1;
}

int QtMdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "QtMdcCalib");
     log << MSG::INFO << "QtMdcCalib::updateConst()" << endreq;

     MdcCalib::updateConst(calconst);

     int lay;
     int bin;
     int ord;

     Stat_t entry;
     double qtpar;
     double qbcen;
     double tw;
     double deltw;
     double qterr;

     TF1* funQt = new TF1("funQt", qtFun, 200, 2000, 2);

     ofstream fqtlog("qtlog");
     for(lay=0; lay<m_nlayer; lay++){
	  if(0 == m_param.fgCalib[lay]) continue;

	  fqtlog << "Layer" << lay << endl;

	  for(ord=0; ord<m_qtorder; ord++){
	       m_qtpar[lay][ord] = calconst -> getQtpar(lay, ord);
	  }

	  for(bin=0; bin<m_nbin; bin++){
	       entry = m_hqt[lay][bin] -> GetEntries();

	       if(entry > 300){
		    deltw = m_hqt[lay][bin] -> GetMean();
		    qterr = ( m_hqt[lay][bin]->GetRMS() ) / sqrt((double)entry);
		    deltw /= m_vdr;
		    qterr /= m_vdr;
	       } else{
		    continue;
	       }

	       qbcen = ( (double)bin + 0.5 ) * m_qbinw[lay] + m_qmin[lay];
// 	       tw = qtFun(qbcen, m_qtpar[lay]) + deltw;
	       tw = (m_mdcFunSvc->getTimeWalk(lay, qbcen)) + deltw;

	       m_grqt[lay]->SetPoint(bin, qbcen, tw);
	       m_grqt[lay]->SetPointError(bin, 0, qterr);

	       m_grqdt[lay]->SetPoint(bin, qbcen, deltw);
	       m_grqdt[lay]->SetPointError(bin, 0, qterr);

	       fqtlog << setw(3) << bin
		      << setw(12) << deltw
		      << setw(12) << tw
		      << setw(12) << qbcen
		      << setw(12) << qterr
		      << endl;
	  }

	  m_grqt[lay]->Fit("funQt", "Q+", "", m_qmin[lay], m_qmax[lay]);

	  fqtlog << "Qtpar:  ";
	  for(ord=0; ord<m_qtorder; ord++){
	       qtpar = funQt->GetParameter(ord);
	       qterr = funQt->GetParError(ord);
	       calconst -> resetQtpar(lay, ord, qtpar);

	       fqtlog << setw(12) << qtpar
		      << setw(12) << qterr
		      << endl;
	  }

// 	  if( (0 == ierflg) && (3 == istat) ){
// 	       for(ord=0; ord<m_qtorder; ord++){
// 		    gmqt -> GetParameter(ord, qtpar, qterr);
// 		    calconst -> resetQtpar(lay, ord, qtpar);

// 		    fqtlog << setw(12) << qtpar
// 			   << setw(12) << qterr
// 			   << endl;
// 	       }
// 	  } else{
// 	       fqtlog << setw(12) << m_qtpar[lay][0] 
// 		      << setw(12) << "0"
// 		      << setw(12) << m_qtpar[lay][1]
// 		      << setw(12) << "0"
// 		      << endl;
// 	  }

     } // end of layer loop

     fqtlog.close();
     delete funQt;
     return 1;
}

Double_t QtMdcCalib::qtFun(Double_t *x, Double_t *par){
     double tw = par[1] / sqrt(x[0]) + par[0];
     return tw;
}

