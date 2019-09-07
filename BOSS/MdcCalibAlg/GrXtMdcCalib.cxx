#include "MdcCalibAlg/GrXtMdcCalib.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>

#include "TF1.h"

using namespace std;

double GrXtMdcCalib::DMAX;
double GrXtMdcCalib::TMAX;

GrXtMdcCalib::GrXtMdcCalib(){
     m_maxNhit = 5000;
     m_fgIni = false;
}

GrXtMdcCalib::~GrXtMdcCalib(){
}

void GrXtMdcCalib::clear(){
     cout << "~GrXtMdcCalib" << endl;
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  for(int iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
	       for(int iLR=0; iLR<MdcCalLR; iLR++){
		    delete m_grxt[lay][iEntr][iLR];
	       }
	  }
     }
     delete m_haxis;
     delete m_fdXt;

     MdcCalib::clear();
}

void GrXtMdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			      IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "GrXtMdcCalib");
     log << MSG::INFO << "GrXtMdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     MdcCalib::initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

     int lay;
     int iLR;
     int iEntr;
     char hname[200];

     m_fdXt = new TFolder("fdXtGr", "fdXtGr");
     m_hlist -> Add(m_fdXt);

     m_haxis = new TH2F("axis", "", 50, 0, 300, 50, 0, 9);
     m_haxis -> SetStats(0);
     m_fdXt -> Add(m_haxis);

     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
	       for(iLR=0; iLR<MdcCalLR; iLR++){
		    m_nhit[lay][iEntr][iLR] = 0;

		    sprintf(hname, "grXt%02d_%02d_lr%01d", lay, iEntr, iLR);
		    m_grxt[lay][iEntr][iLR] = new TGraph();
		    m_grxt[lay][iEntr][iLR] -> SetName(hname);
		    m_grxt[lay][iEntr][iLR] -> SetMarkerStyle(10);
		    m_grxt[lay][iEntr][iLR] -> SetLineColor(10);
		    m_fdXt -> Add(m_grxt[lay][iEntr][iLR]);
	       }
	  }
     }
}

int GrXtMdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "GrXtMdcCalib");
     log << MSG::DEBUG << "GrXtMdcCalib::fillHist()" << endreq;

     MdcCalib::fillHist(event);

     // get EsTimeCol
     bool esCutFg = event->getEsCutFlag();
     if( ! esCutFg ) return -1;

     int i;
     int k;
     int lay;
     int iLR;
     int iEntr;

     double dr;
     double dz;
     double doca;
     double tdr;
     double resi;
     double entrance;

     int nhitlay;
     bool fgHitLay[MdcCalNLayer];
     bool fgTrk;
     
     if(! m_fgIni){
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       if(lay < 8) m_docaMax[lay] = m_param.maxDocaInner;
	       else m_docaMax[lay] = m_param.maxDocaOuter;
	  }
	  m_fgIni = true;
     }

     MdcCalRecTrk* rectrk;
     MdcCalRecHit* rechit;

     int nhit;
     int ntrk = event -> getNTrk();
     for(i=0; i<ntrk; i++){
	  fgTrk = true;
	  rectrk = event->getRecTrk(i);
	  nhit = rectrk -> getNHits();

	  // dr cut
	  dr = rectrk->getDr();
	  if(fabs(dr) > m_param.drCut) continue;
        
	  // dz cut
	  dz = rectrk->getDz();
	  if(fabs(dz) > m_param.dzCut) continue;

	  for(lay=0; lay<MdcCalNLayer; lay++){
	       fgHitLay[lay] = false;
	  }

	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       doca = rechit -> getDocaInc();
	       resi = rechit -> getResiInc();
	       fgHitLay[lay] = true;

// 	       if( (fabs(doca) > m_docaMax[lay]) || 
// 		   (fabs(resi) > m_param.resiCut[lay]) ){
// 		    fgTrk = false;
// 	       }
	  }
	  if(! fgTrk) continue;

	  nhitlay = 0;
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       if(fgHitLay[lay]) nhitlay++;
	  }
	  if(nhitlay < m_param.nHitLayCut) continue;

	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       doca = rechit -> getDocaInc();
	       resi = rechit -> getResiInc();
	       iLR = rechit -> getLR();
	       entrance = rechit -> getEntra();
	       tdr = rechit -> getTdrift();

	       if( (fabs(doca) > m_docaMax[lay]) || 
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

	       iEntr = m_mdcFunSvc -> getXtEntrIndex(entrance);

	       if(iLR < 2){
		    if(m_nhit[lay][iEntr][iLR] < m_maxNhit){
			 m_grxt[lay][iEntr][iLR] -> SetPoint(m_nhit[lay][iEntr][iLR],
							     tdr, fabs(doca));
			 m_nhit[lay][iEntr][iLR]++;
		    }
	       }

	       if(m_nhit[lay][iEntr][2] < m_maxNhit){
		    m_grxt[lay][iEntr][2] -> SetPoint(m_nhit[lay][iEntr][2],
						      tdr, fabs(doca));
		    m_nhit[lay][iEntr][2]++;
	       }
	  } // hit loop
     } // track loop
     return 1;
}

int GrXtMdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "GrXtMdcCalib");
     log << MSG::INFO << "GrXtMdcCalib::updateConst()" << endreq;

     MdcCalib::updateConst(calconst);

     int ord;
     double xtpar[MdcCalNLayer][MdcCalNENTRXT][MdcCalLR][8];
     TF1* fxtDr = new TF1("fxtDr", xtfun, 0, 300, 6);
     TF1* fxtEd = new TF1("fxtEd", xtedge, 150, 500, 1);
     if(1 == m_param.fixXtC0) fxtDr -> FixParameter(0, 0);

     for(int lay=0; lay<MdcCalNLayer; lay++){
	  for(int iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
	       for(int lr=0; lr<MdcCalLR; lr++){
		    m_fgFit[lay][iEntr][lr] = false;
		    if(0 == m_param.fgCalib[lay]) continue;

		    if(m_nhit[lay][iEntr][lr] > 1000){
			 TMAX = calconst -> getXtpar(lay, iEntr, lr, 6);

			 m_grxt[lay][iEntr][lr] -> Fit("fxtDr", "Q+", "", 0, TMAX);

			 for(ord=0; ord<6; ord++){
			      xtpar[lay][iEntr][lr][ord] = fxtDr->GetParameter(ord);
			 }
			 xtpar[lay][iEntr][lr][6] = TMAX;

			 DMAX = 0.0;
			 for(ord=0; ord<6; ord++) DMAX += xtpar[lay][iEntr][lr][ord] * pow(TMAX, ord);

			 m_grxt[lay][iEntr][lr] -> Fit("fxtEd", "Q+", "", TMAX, TMAX+300);
			 xtpar[lay][iEntr][lr][7] = fxtEd->GetParameter(0);
			 if(xtpar[lay][iEntr][lr][7] < 0.0) xtpar[lay][iEntr][lr][7] = 0.0;
			 m_fgFit[lay][iEntr][lr] = true;

// 			 for(ord=0; ord<8; ord++){
// 			      calconst -> resetXtpar(lay, iEntr, lr, ord, xtpar[ord]);
// 			 }
		    }

	       } // end of lr loop
	  } // end of entrance angle loop
     } // end of layer loop

     ofstream fxtlog("xtlog");
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  for(int iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
	       for(int lr=0; lr<MdcCalLR; lr++){
		    fxtlog << setw(3) << lay << setw(3) << iEntr << setw(3) << lr;

		    int fgUpdate = -1;
		    if(m_fgFit[lay][iEntr][lr]){
			 fgUpdate = 1;
			 for(ord=0; ord<8; ord++) calconst->resetXtpar(lay, iEntr, lr, ord, xtpar[lay][iEntr][lr][ord]);
		    } else{
			 int iEntrNew = findXtEntr(lay, iEntr, lr);
			 if(-1 != iEntrNew){
			      fgUpdate = 2;
			      for(ord=0; ord<8; ord++){
				   calconst->resetXtpar(lay, iEntr, lr, ord, xtpar[lay][iEntrNew][lr][ord]);
			      }
			 }
		    }
		    fxtlog << setw(3) << fgUpdate;
		    for(ord=0; ord<8; ord++){
			 double par = calconst -> getXtpar(lay, iEntr, lr, ord);
			 fxtlog << setw(14) << par;
		    }
		    fxtlog << endl;
	       }
	  }
     }
     fxtlog.close();

     cout << "Xt update finished. File xtlog was written." << endl;
     delete fxtDr;
     delete fxtEd;
}

Double_t GrXtMdcCalib::xtfun(Double_t *x, Double_t *par){
     Double_t val = 0.0;
     for(Int_t ord=0; ord<6; ord++){
	  val += par[ord] * pow(x[0], ord);
     }
     return val;
}

Double_t GrXtMdcCalib::xtedge(Double_t *x, Double_t *par){
     double val = DMAX + (x[0] - TMAX) * par[0];
     return val;
}

int GrXtMdcCalib::findXtEntr(int lay, int iEntr, int lr) const {
     int id0 = 8;
     int id1 = 9;
     int idmax = 17;
     int entrId = -1;
     if(iEntr <= id0){
	  int id = -1;
	  for(int i=iEntr; i<=id0; i++){
	       if(m_fgFit[lay][i][lr]){
		    id = i;
		    break;
	       }
	  }
	  if(-1 != id) entrId = id;
	  else{
	       for(int i=iEntr; i>=0; i--){
		    if(m_fgFit[lay][i][lr]){
			 id = i;
			 break;
		    }
	       }
	       entrId = id;
	  }
     } else{
	  int id = -1;
	  for(int i=iEntr; i>=id1; i--){
	       if(m_fgFit[lay][i][lr]){
		    id = i;
		    break;
	       }
	  }
	  if(-1 != id) entrId = id;
	  else{
	       for(int i=iEntr; i<idmax; i++){
		    if(m_fgFit[lay][i][lr]){
			 id = i;
			 break;
		    }
	       }
	       entrId = id;
	  }
     }
     if(-1 == entrId){
	  cout << "find EntrId error " << "layer " << lay << "  iEntr " << iEntr << "  lr " << lr << endl;
     }

     return entrId;
}
