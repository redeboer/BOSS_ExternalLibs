#include "MdcCalibAlg/XtMdcCalib.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cmath>

#include "TF1.h"
#include "TMinuit.h"
#include "TRandom.h"

using namespace std;

typedef map<int, int>::value_type valType2;

vector<double> XtMdcCalib::XMEAS;
vector<double> XtMdcCalib::TBINCEN;
vector<double> XtMdcCalib::ERR;
double XtMdcCalib::Tmax;
double XtMdcCalib::Dmax;
vector<double> XtMdcCalib::XMEASED;
vector<double> XtMdcCalib::TBINCENED;
vector<double> XtMdcCalib::ERRED;


XtMdcCalib::XtMdcCalib(){
     m_nbin = 50;		// m_nbin=50 before 2011-11-16
     m_nLR = 3;
     m_nxtpar = 8;

     for(int lay=0; lay<43; lay++){
	  if(lay < 15) m_nEntr[lay] = 1;
	  else  m_nEntr[lay] = 2;		// for entr>0 and entr<0
     }

     m_tbinw = 10.0;
     m_fgIni = false;
}

XtMdcCalib::~XtMdcCalib(){
}

void XtMdcCalib::clear(){
     cout << "~XtMdcCalib" << endl;

     unsigned int i;
     for(i=0; i<m_hxt.size(); i++){
	  delete m_hxt[i];
          if(0 == (i % 1000)) cout << "delete hxt[" << i << "]" << endl;
     }
     delete m_fdXt;
     m_hxt.clear();
     m_hxtmap.clear();

     MdcCalib::clear();
}


void XtMdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			    IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "XtMdcCalib");
     log << MSG::INFO << "XtMdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     MdcCalib::initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

     int lay;
     int iLR;
     int iEntr;
     int bin;
     int hid;
     int key;
     char hname[200];
     TH1D* hist;

     m_fdXt = new TFolder("fdXt", "fdXt");
     m_hlist -> Add(m_fdXt);

     m_nlayer = m_mdcGeomSvc -> getLayerSize();

     hid = 0;
     for(lay=0; lay<m_nlayer; lay++){
	  for(iEntr=0; iEntr<m_nEntr[lay]; iEntr++){
	       for(iLR=0; iLR<m_nLR; iLR++){
		    for(bin=0; bin<=m_nbin; bin++){
			 sprintf(hname, "Hxt%02d_E%02d_LR%01d_B%02d", lay, iEntr, iLR, bin);
			 hist = new TH1D(hname, "", 300, -1.5, 1.5);
			 m_hxt.push_back( hist );
			 m_fdXt -> Add( hist );

			 key = getHxtKey(lay, iEntr, iLR, bin);
			 m_hxtmap.insert( valType2( key, hid ) );
			 hid++;
		    }
	       }
	  }
     }
}

int XtMdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "XtMdcCalib");
     log << MSG::DEBUG << "XtMdcCalib::fillHist()" << endreq;

     MdcCalib::fillHist(event);

     // get EsTimeCol
     bool esCutFg = event->getEsCutFlag();
     if( ! esCutFg ) return -1;

     int i;
     int k;
     int lay;
     int iLR;
     int iEntr;
     int bin;
     int key;
     int hid;

     double dr;
     double dz;
     double tdr;
     double doca;
     double resi;
     double entrance;
     int stat;

     double xtpar[MdcCalXtNPars];
     int nhitlay;
     bool fgHitLay[MdcCalNLayer];

     if(! m_fgIni){
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       if(lay < 8) m_docaMax[lay] = m_param.maxDocaInner;
	       else m_docaMax[lay] = m_param.maxDocaOuter;
	  }

	  for(lay=0; lay<MdcCalNLayer; lay++){
	       for(iEntr=0; iEntr<m_nEntr[lay]; iEntr++){
		    for(iLR=0; iLR<MdcCalLR; iLR++){
// 			 m_mdcFunSvc -> getXtpar(lay, iEntr, iLR, xtpar);
			 if(0 == iEntr) m_mdcFunSvc -> getXtpar(lay, 8, iLR, xtpar);
			 else if(1 == iEntr) m_mdcFunSvc -> getXtpar(lay, 9, iLR, xtpar);
			 m_tm[lay][iEntr][iLR] = xtpar[6];
		    }
	       }
	  }
	  m_fgIni = true;
     }

     MdcCalRecTrk* rectrk;
     MdcCalRecHit* rechit;

     int nhit;
     int ntrk = event -> getNTrk();
     if((ntrk < m_param.nTrkCut[0]) || (ntrk > m_param.nTrkCut[1])) return -1;
     for(i=0; i<ntrk; i++){
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
	       fgHitLay[lay] = true;
	  }

	  nhitlay = 0;
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       if(fgHitLay[lay]) nhitlay++;
	  }
	  if(nhitlay < m_param.nHitLayCut) continue;

// 	  bool fgNoise = rectrk->getFgNoiseRatio();
// 	  if(m_param.noiseCut && (!fgNoise)) continue;

	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       doca = rechit -> getDocaInc();
	       iLR = rechit -> getLR();
	       entrance = rechit -> getEntra();
	       resi = rechit -> getResiInc();
	       tdr = rechit -> getTdrift();
	       stat = rechit -> getStat();

	       if(1 != stat) continue;

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
	       if(1 == m_nEntr[lay]){
		    iEntr = 0;
	       } else if(2 == m_nEntr[lay]){
		    if(entrance > 0.0) iEntr = 1;
		    else iEntr = 0;
	       }

	       bin = (int)(tdr / m_tbinw);

	       // left-right separately
	       key = getHxtKey(lay, iEntr, iLR, bin);
	       if((bin < m_nbin) && (1 == m_hxtmap.count(key))){
		    hid = m_hxtmap[key];
		    m_hxt[hid] -> Fill( resi );
	       }

	       // left-right in common
	       key = getHxtKey(lay, iEntr, 2, bin);
	       if((bin < m_nbin) && (1 == m_hxtmap.count(key))){
		    hid = m_hxtmap[key];
		    m_hxt[hid] -> Fill( resi );
	       }

	       if(fabs(tdr - m_tm[lay][iEntr][iLR]) < 5){
		    key = getHxtKey(lay, iEntr, iLR, m_nbin);
		    if( 1 == m_hxtmap.count(key) ){
			 hid = m_hxtmap[key];
			 m_hxt[hid] -> Fill( resi );
		    }

		    key = getHxtKey(lay, iEntr, 2, m_nbin);
		    if( 1 == m_hxtmap.count(key) ){
			 hid = m_hxtmap[key];
			 m_hxt[hid] -> Fill( resi );
		    }
	       }

	  } // hit loop
     } // track loop
     return 1;
}

int XtMdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "XtMdcCalib");
     log << MSG::INFO << "XtMdcCalib::updateConst()" << endreq;

     MdcCalib::updateConst(calconst);

     int i;
     int hid;
     int lay;
     int iLR;
     int iEntr;
     int bin;
     int ord;
     int key;

     Stat_t entry;
     double xtpar;
     double xterr;
     double tbcen;
     double deltx;
     double xcor;
     double xerr;
     double xtini[8];
     double xtfit[8];

     Int_t ierflg;
     Int_t istat;
     Int_t nvpar;
     Int_t nparx;
     Double_t fmin;
     Double_t edm;
     Double_t errdef;
     Double_t arglist[10];

     TMinuit* gmxt = new TMinuit(6);
     gmxt -> SetPrintLevel(-1);
     gmxt -> SetFCN(fcnXT);
     gmxt -> SetErrorDef(1.0);
     gmxt -> mnparm(0, "xtpar0", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(1, "xtpar1", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(2, "xtpar2", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(3, "xtpar3", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(4, "xtpar4", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(5, "xtpar5", 0, 0.1, 0, 0, ierflg);
     arglist[0] = 0;
     gmxt -> mnexcm("SET NOW", arglist, 0, ierflg);

     TMinuit* gmxtEd = new TMinuit(1);
     gmxtEd -> SetPrintLevel(-1);
     gmxtEd -> SetFCN(fcnXtEdge);
     gmxtEd -> SetErrorDef(1.0);
     gmxtEd -> mnparm(0, "xtpar0", 0, 0.1, 0, 0, ierflg);
     arglist[0] = 0;
     gmxtEd -> mnexcm("SET NOW", arglist, 0, ierflg);

     ofstream fxtlog("xtlog");
     for(lay=0; lay<m_nlayer; lay++){
	  if(0 == m_param.fgCalib[lay]) continue;

	  for(iEntr=0; iEntr<m_nEntr[lay]; iEntr++){
	       for(iLR=0; iLR<m_nLR; iLR++){

		    fxtlog << "Layer " << setw(3) << lay << setw(3) << iEntr
			   << setw(3) << iLR << endl;

		    for(ord=0; ord<m_nxtpar; ord++){
// 			 xtpar = calconst -> getXtpar(lay, iEntr, iLR, ord);
			 if(0 == iEntr) xtpar = calconst -> getXtpar(lay, 8, iLR, ord);
			 else if(1 == iEntr) xtpar = calconst -> getXtpar(lay, 9, iLR, ord);

			 xtini[ord] = xtpar;
			 xtfit[ord] = xtpar;
		    }

		    Tmax = xtini[6];

		    for(bin=0; bin<=m_nbin; bin++){
			 key = getHxtKey(lay, iEntr, iLR, bin);
			 hid = m_hxtmap[key];

			 entry = m_hxt[hid] -> GetEntries();
			 if(entry > 100){
			      deltx = m_hxt[hid] -> GetMean();
			      xerr = m_hxt[hid]->GetRMS();
			 } else{
			      continue;
			 }

			 if(bin < m_nbin)
			      tbcen = ( (double)bin + 0.5 ) * m_tbinw;
			 else tbcen = m_tm[lay][iEntr][iLR];
			 xcor = xtFun(tbcen, xtini) - deltx;

			 if((tbcen <= Tmax) || (bin == m_nbin)){
			      TBINCEN.push_back( tbcen );
			      XMEAS.push_back( xcor );
			      ERR.push_back( xerr );
			 } else{
			      TBINCENED.push_back( tbcen );
			      XMEASED.push_back( xcor );
			      ERRED.push_back( xerr );                              
			 } 
			 fxtlog << setw(3) << bin
				<< setw(15) << deltx
				<< setw(15) << xcor
				<< setw(15) << tbcen
				<< setw(15) << xerr
				<< endl;
		    } // end of bin loop

		    if( XMEAS.size() < 12 ){
			 TBINCEN.clear();
			 XMEAS.clear();
			 ERR.clear();

			 TBINCENED.clear();
			 XMEASED.clear();
			 ERRED.clear();

			 continue;
		    }               

		    for(ord=0; ord<=5; ord++){
			 arglist[0] = ord + 1;
			 arglist[1] = xtini[ord];
			 gmxt -> mnexcm("SET PARameter", arglist, 2, ierflg);
		    }

		    // fix the xtpar[0] at 0
		    if(1 == m_param.fixXtC0){
			 arglist[0] = 1;
			 arglist[1] = 0.0;
			 gmxt -> mnexcm("SET PARameter", arglist, 2, ierflg);
			 gmxt -> mnexcm("FIX", arglist, 1, ierflg);
		    }

		    arglist[0] = 1000;
		    arglist[1] = 0.1;
		    gmxt -> mnexcm("MIGRAD", arglist, 2, ierflg);
		    gmxt -> mnstat(fmin, edm, errdef, nvpar, nparx, istat);

		    fxtlog << "Xtpar: " << endl;
		    if( (0 == ierflg) && (istat >= 2) ){
			 for(ord=0; ord<=5; ord++){
			      gmxt -> GetParameter(ord, xtpar, xterr);
// 			      calconst -> resetXtpar(lay, iEntr, iLR, ord, xtpar);
			      xtfit[ord] = xtpar;

			      if(1 == m_nEntr[lay]){
				   for(i=0; i<18; i++)
					calconst -> resetXtpar(lay, i, iLR, ord, xtpar);
			      } else if(2 == m_nEntr[lay]){
				   if(0 == iEntr){
					for(i=0; i<9; i++) // entr<0
					     calconst->resetXtpar(lay, i, iLR, ord, xtpar);
				   } else{
					for(i=9; i<18; i++) // entr>0
					     calconst->resetXtpar(lay, i, iLR, ord, xtpar);
				   }
			      }
			      fxtlog << setw(15) << xtpar << setw(15) << xterr << endl;
			 }
		    } else{
			 for(ord=0; ord<=5; ord++){
			      fxtlog << setw(15) << xtini[ord] << setw(15) << "0" << endl;
			 } 
		    }
		    fxtlog << setw(15) << Tmax << setw(15) << "0" << endl;

		    //   release the first parameter
		    if(1 == m_param.fixXtC0){
			 arglist[0] = 1;
			 gmxt -> mnexcm("REL", arglist, 1, ierflg);
		    }

		    Dmax = xtFun(Tmax, xtfit);

		    if( XMEASED.size() >= 3 ){
			 // fit xt in the edge area
			 arglist[0] = 1;
			 arglist[1] = xtini[7];
			 gmxtEd -> mnexcm("SET PARameter", arglist, 2, ierflg);

			 arglist[0] = 1000;
			 arglist[1] = 0.1;
			 gmxtEd -> mnexcm("MIGRAD", arglist, 2, ierflg);
			 gmxtEd -> mnstat(fmin, edm, errdef, nvpar, nparx, istat);

			 if( (0 == ierflg) && (istat >=2) ){
			      gmxtEd -> GetParameter(0, xtpar, xterr);
			      if(xtpar < 0.0) xtpar = 0.0;
			      if(m_param.fixXtEdge) xtpar = xtini[7];
// 			      calconst -> resetXtpar(lay, iEntr, iLR, 7, xtpar);

			      if(1 == m_nEntr[lay]){
				   for(i=0; i<18; i++)
					calconst -> resetXtpar(lay, i, iLR, 7, xtpar);
			      } else if(2 == m_nEntr[lay]){
				   if(0 == iEntr){
					for(i=0; i<9; i++)
					     calconst->resetXtpar(lay, i, iLR, 7, xtpar);
				   } else{
					for(i=9; i<18; i++)
					     calconst->resetXtpar(lay, i, iLR, 7, xtpar);
				   }
			      }
			      fxtlog << setw(15) << xtpar << setw(15) << xterr << endl;				     
			 } else {
			      fxtlog << setw(15) << xtini[7] << setw(15) << "0" << endl;
			 }
		    } else {
			 fxtlog << setw(15) << xtini[7] << setw(15) << "0" << endl;
		    }    
		    fxtlog << "Tm " << setw(15) << Tmax
			   << "  Dmax " << setw(15) << Dmax << endl;

		    TBINCEN.clear();
		    XMEAS.clear();
		    ERR.clear();
		    TBINCENED.clear();
		    XMEASED.clear();
		    ERRED.clear();                    
	       } // end of l-r loop
	  } // end of entrance angle loop
     } // end of layer loop

     fxtlog.close();
     delete gmxt;
     delete gmxtEd;
     cout << "Xt update finished" << endl;
     return 1;
}

int XtMdcCalib::getHxtKey(int lay, int entr, int lr, int bin) const {
     int key;

     key = ( (lay << HXTLAYER_INDEX) & HXTLAYER_MASK ) |
	  ( (entr << HXTENTRA_INDEX) & HXTENTRA_MASK ) |
	  ( (lr << HXTLR_INDEX) & HXTLR_MASK ) |
	  ( (bin << HXTBIN_INDEX) & HXTBIN_MASK );

     return key;
}

void XtMdcCalib::fcnXT(Int_t &npar, Double_t *gin, Double_t &f, 
		       Double_t *par, Int_t iflag){
     unsigned int i;
     int ord;
     Double_t deta;
     Double_t chisq = 0.0;
     Double_t dfit;

     for(i=0; i<TBINCEN.size(); i++){
	  dfit = 0;
	  for(ord=0; ord<=5; ord++){
	       dfit += par[ord] * pow(TBINCEN[i], ord);
	  }
	  deta = (dfit - XMEAS[i]) / ERR[i];
	  chisq += deta * deta;
     }

     f = chisq;
}

double XtMdcCalib::xtFun(double t, double xtpar[]){
     int ord;
     double dist = 0.0;
     double tmax = xtpar[6];

     if(t < tmax ){
	  for(ord=0; ord<=5; ord++){
	       dist += xtpar[ord] * pow(t, ord);
	  }
     }else{
	  for(ord=0; ord<=5; ord++){
	       dist += xtpar[ord] * pow(tmax, ord);
	  }
	  dist += xtpar[7] * (t - tmax);
     }

     return dist;
}
void XtMdcCalib::fcnXtEdge(Int_t &npar, Double_t *gin, Double_t &f,
                           Double_t *par, Int_t iflag){
     unsigned int i;
     Double_t deta;
     Double_t chisq = 0.0;
     Double_t dfit;

     for(i=0; i<TBINCENED.size(); i++){
          dfit = par[0] * (TBINCENED[i] - Tmax) + Dmax;
          deta = (dfit - XMEASED[i]) / ERRED[i];
          chisq += deta * deta;
     }

     f = chisq;
}

