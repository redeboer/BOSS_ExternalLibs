#include "MdcCalibAlg/XtInteMdcCalib.h"

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

XtInteMdcCalib::XtInteMdcCalib(){
     m_fgIni = false;
     m_nMaxGrPoint = 5000;
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  m_tbinWid[lay][0] = 5.0;
	  m_tbinWid[lay][1] = 10.0;
	  m_tbinWid[lay][2] = 20.0;

	  m_tbinLim[lay][0] = -10.0;
	  m_tbinLim[lay][1] = 30.0;
	  if(lay < 8) m_tbinLim[lay][2] = 210.0;
	  else m_tbinLim[lay][2] = 350.0;
	  m_tbinLim[lay][3] = 990.0;
     }
}

XtInteMdcCalib::~XtInteMdcCalib(){
}

void XtInteMdcCalib::clear(){
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  for(int iEntr=0; iEntr<NENTR; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    delete m_pfNear[lay][iEntr][lr];
		    delete m_pfMid[lay][iEntr][lr];
		    delete m_pfFar[lay][iEntr][lr];
		    delete m_grXt[lay][iEntr][lr];
	       }
	  }
     }
     delete m_fdPf;
     MdcCalib::clear();
}

void XtInteMdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
				IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "XtInteMdcCalib");
     log << MSG::INFO << "XtInteMdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     MdcCalib::initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

     m_fdPf = new TFolder("fdProfile", "fdProfile");
     m_hlist -> Add(m_fdPf);

     char hname[200];
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  for(int iEntr=0; iEntr<NENTR; iEntr++){
	       for(int lr=0; lr<2; lr++){
                    sprintf(hname, "xt%02d_%02d_%d_gr", lay, iEntr, lr);
		    m_grXt[lay][iEntr][lr] = new TGraph();
		    m_grXt[lay][iEntr][lr]->SetName(hname);
		    m_fdPf->Add(m_grXt[lay][iEntr][lr]);

		    int xbinN = (int)((m_tbinLim[lay][1] - m_tbinLim[lay][0])/m_tbinWid[lay][0] + 0.5);
		    sprintf(hname, "xt%02d_%02d_%d_near", lay, iEntr, lr);
		    m_pfNear[lay][iEntr][lr] = new TProfile(hname, hname, xbinN, m_tbinLim[lay][0], m_tbinLim[lay][1]);
		    m_fdPf->Add(m_pfNear[lay][iEntr][lr]);

		    int xbinM = (int)((m_tbinLim[lay][2] - m_tbinLim[lay][1])/m_tbinWid[lay][1] + 0.5);
		    sprintf(hname, "xt%02d_%02d_%d_mid", lay, iEntr, lr);
		    m_pfMid[lay][iEntr][lr] = new TProfile(hname, hname, xbinM, m_tbinLim[lay][1], m_tbinLim[lay][2]);
		    m_fdPf->Add(m_pfMid[lay][iEntr][lr]);

		    int xbinF = (int)((m_tbinLim[lay][3] - m_tbinLim[lay][2])/m_tbinWid[lay][2] + 0.5);
		    sprintf(hname, "xt%02d_%02d_%d_far", lay, iEntr, lr);
		    m_pfFar[lay][iEntr][lr] = new TProfile(hname, hname, xbinF, m_tbinLim[lay][2], m_tbinLim[lay][3]);
		    m_fdPf->Add(m_pfFar[lay][iEntr][lr]);

// 		    if((0==iEntr)&&(0==lr))
// 		    cout << setw(5) << lay
// 			 << setw(5) << xbinN << setw(10) << m_tbinLim[lay][0] << setw(10) << m_tbinLim[lay][1]
// 			 << setw(5) << xbinM << setw(10) << m_tbinLim[lay][1] << setw(10) << m_tbinLim[lay][2]
// 			 << setw(5) << xbinF << setw(10) << m_tbinLim[lay][2] << setw(10) << m_tbinLim[lay][3] << endl;
	       }
	  }
     }
}

int XtInteMdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "XtInteMdcCalib");
     log << MSG::DEBUG << "XtInteMdcCalib::fillHist()" << endreq;

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
	       int npoint = m_grXt[lay][iEntr][iLR]->GetN();
	       if(npoint < m_nMaxGrPoint) m_grXt[lay][iEntr][iLR]->SetPoint(npoint, tdr, fabs(doca));

	       if(tdr<m_tbinLim[lay][0]) continue;
	       else if(tdr<m_tbinLim[lay][1])  m_pfNear[lay][iEntr][iLR]->Fill(tdr, fabs(doca));
	       else if(tdr<m_tbinLim[lay][2])  m_pfMid[lay][iEntr][iLR]->Fill(tdr, fabs(doca));
	       else if(tdr<m_tbinLim[lay][3]) m_pfFar[lay][iEntr][iLR]->Fill(tdr, fabs(doca));
	  } // hit loop
     } // track loop
     return 1;
}

int XtInteMdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "XtInteMdcCalib");
     log << MSG::INFO << "XtInteMdcCalib::updateConst()" << endreq;

     MdcCalib::updateConst(calconst);
}

