#include "MdcCalibAlg/PreT0MdcCalib.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EvTimeEvent/RecEsTime.h"
#include "EventModel/Event.h"
#include "MdcRawEvent/MdcDigi.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "TStyle.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

#include <math.h>
#include "TH2F.h"     
#include "TH1F.h"
#include "TFolder.h"
#include "TF1.h"

PreT0MdcCalib::PreT0MdcCalib(){
     m_nzbin = 11;
}

PreT0MdcCalib::~PreT0MdcCalib(){
}

void PreT0MdcCalib::clear(){
     int lay;
     int lr;
     int bin;
     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(lr=0; lr<MdcCalLR; lr++){
	       delete m_hTrec[lay][lr];
	       for(bin=0; bin<m_nzbin; bin++){
		    delete m_hTrecZ[lay][lr][bin];
	       }
	  }
     }
     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(bin=0; bin<2; bin++) delete m_hTrecCosm[lay][bin];
     }
     delete m_fdTrec;
     delete m_fdTrecZ;

     MdcCalib::clear();
}

void PreT0MdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			       IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "PreT0MdcCalib");
     log << MSG::INFO << "PreT0MdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

//      MdcCalib::initialize(m_hlist, m_mdcGeomSvc, m_mdcFunSvc, m_mdcUtilitySvc);

     int lay;
     int lr;
     int bin;
     char hname[200];

     m_fdTrec = new TFolder("Trec", "Trec");
     m_hlist->Add(m_fdTrec);

     m_fdTrecZ = new TFolder("TrecZ", "TrecZ");
     m_hlist->Add(m_fdTrecZ);

     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(lr=0; lr<MdcCalLR; lr++){
	       if(0 == lr) sprintf(hname, "Trec%02d_L", lay);
	       else if(1 == lr) sprintf(hname, "Trec%02d_R", lay);
	       else sprintf(hname, "Trec%02d_C", lay);

	       if(lay < 8) m_hTrec[lay][lr] = new TH1F(hname, "", 100, 0, 400);
	       else m_hTrec[lay][lr] = new TH1F(hname, "", 125, 0, 500);
	       m_fdTrec -> Add(m_hTrec[lay][lr]);
	  }
     }

     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(int iud=0; iud<2; iud++){
	       if(0 == iud) sprintf(hname, "TrecCosm%02d_Up", lay);
	       else sprintf(hname, "TrecCosm%02d_Dw", lay);
	       if(lay < 8) m_hTrecCosm[lay][iud] = new TH1F(hname, "", 100, 0, 400);
	       else m_hTrecCosm[lay][iud] = new TH1F(hname, "", 125, 0, 500);
	       m_fdTrec -> Add(m_hTrecCosm[lay][iud]);
	  }
     }

     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(lr=0; lr<MdcCalLR; lr++){
	       for(bin=0; bin<m_nzbin; bin++){
		    if(0 == lr) sprintf(hname, "Trec%02d_z%02d_L", lay, bin);
		    else if(1 == lr) sprintf(hname, "Trec%02d_z%02d_R", lay, bin);
		    else sprintf(hname, "Trec%02d_z%02d_C", lay, bin);

		    if(lay < 8) m_hTrecZ[lay][lr][bin] = new TH1F(hname, "", 100, 0, 400);
		    else m_hTrecZ[lay][lr][bin] = new TH1F(hname, "", 125, 0, 500);
		    m_fdTrecZ -> Add(m_hTrecZ[lay][lr][bin]);
	       }
	  }
     }

     double zeast;
     double zwest;
     for(lay=0; lay<MdcCalNLayer; lay++){
	  zwest = m_mdcGeomSvc->Wire(lay, 0)->Forward().z();
	  zeast = m_mdcGeomSvc->Wire(lay, 0)->Backward().z();
	  m_zwid[lay] = (zeast - zwest) / (double)m_nzbin;

	  if(lay < 8) m_vp[lay] = 220.0; // *10^9 mm/s
	  else m_vp[lay] = 240.0; // *10^9 mm/s

	  if( 0 == (lay % 2) ){	// west end
	       m_zst[lay] = zwest;
	  } else{		// east end
	       m_zst[lay] = zeast;
	  }
     }
}

int PreT0MdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "PreT0MdcCalib");
     log << MSG::DEBUG << "PreT0MdcCalib::fillHist()" << endreq;

//      MdcCalib::fillHist(event);

     int i;
     int k;
     int lay;
     int cel;
     int lr;
     int bin;

     double tdc;
     double traw;
     double trec;
     double tdr;
     double t0;
     double tp = 0.0;
     double tof = 0.0;
     double zhit;
     double zprop;

     MdcCalRecTrk* rectrk;
     MdcCalRecHit* rechit;

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     // get EsTimeCol
     double tes = event->getTes();
  //    cout << "tes " << tes << endl;
     bool esCutFg = event->getEsCutFlag();
     if( ! esCutFg ) return -1;

     int nhit;
     int ntrk = event -> getNTrk();
     for(i=0; i<ntrk; i++){
	  rectrk = event->getRecTrk(i);
	  nhit = rectrk -> getNHits();
	  for(k=0; k<nhit; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       cel = rechit -> getCellid();
	       lr = rechit -> getLR();
	       zhit = rechit -> getZhit();
	       tdc = rechit -> getTdc();
	       tdr = rechit -> getTdrift();

	       traw = tdc * MdcCalTdcCnv;
// 	       traw = tdc;	// ns

	       zprop = fabs(zhit - m_zst[lay]);
	       bin = (int)(zprop / m_zwid[lay]);
	       tp = zprop / m_vp[lay];
	       t0 = m_mdcFunSvc -> getT0(lay, cel);
	       tof = rechit -> getTof();

	       double adc = rechit -> getQhit();
	       double tw = m_mdcFunSvc->getTimeWalk(lay, adc);
// 	       cout << setw(15) << adc << setw(15) << tw << endl;

	       const MdcGeoWire* pWire = m_mdcGeomSvc -> Wire(lay, cel);
	       double y1 = pWire -> Backward().y();
	       double y2 = pWire -> Forward().y();
	       double ymid = (y1+y2)*0.5;
// 	       if(5==m_param.particle){	// cosmic-ray
// 		    if(ymid > 0) trec = traw - tp + tof - tes + m_param.timeShift - tw;
// 		    else trec = traw - tp - tof - tes + m_param.timeShift - tw;
// 	       } else{
// 		    trec = traw - tp - tof - tes + m_param.timeShift - tw;
// 	       }
	       trec = traw - tp - tof - tes + m_param.timeShift - tw;

	       m_hTrec[lay][lr] -> Fill(trec);
	       m_hTrec[lay][2] -> Fill(trec); // l-r in common
	       if(ymid > 0) m_hTrecCosm[lay][0] -> Fill(trec);
	       else m_hTrecCosm[lay][1] -> Fill(trec);

	       if(bin < m_nzbin){
		    m_hTrecZ[lay][lr][bin] -> Fill(trec);
		    m_hTrecZ[lay][2][bin] -> Fill(trec);
	       }
	  }
     }
     return 1;
}

int PreT0MdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "PreT0MdcCalib");
     log << MSG::DEBUG << "PreT0MdcCalib::updateConst()" << endreq;

//      MdcCalib::updateConst(calconst);

     // fit Tmin
     int lay;
     int wir;
     int lr;
     double t0Fit[MdcCalNLayer][MdcCalLR];
     double t0Cal[MdcCalNLayer][MdcCalLR];
     double tmax[MdcCalNLayer][MdcCalLR];
     double initT0 = m_param.initT0;

     int fitTminFg[MdcCalNLayer][MdcCalLR];
     int fitTmaxFg[MdcCalNLayer][MdcCalLR];
     double chisq;
     double ndf;
     double chindfTmin[MdcCalNLayer][MdcCalLR];
     double chindfTmax[MdcCalNLayer][MdcCalLR];
     char funname[200];

     // add for cosmic-ray
     TF1* ftminCosm[MdcCalNLayer][2];
     double t0FitCosm[MdcCalNLayer][2];

     TF1* ftmin[MdcCalNLayer][MdcCalLR];
//      sprintf(funname, "ftmin");
//      TF1* ftmin = new TF1(funname, funTmin, 0, 150, 6);
     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(lr=0; lr<MdcCalLR; lr++){
	       fitTminFg[lay][lr] = 0;
	       chindfTmin[lay][lr] = -1;
	       sprintf(funname, "ftmin%02d_%d", lay, lr);
	       ftmin[lay][lr] = new TF1(funname, funTmin, 0, 150, 6);

	       if(1 == m_param.fgCalib[lay]){
		    Stat_t nEntryTot = 0;
		    for(int ibin=1; ibin<=25; ibin++){
			 Stat_t entry = m_hTrec[lay][lr]->GetBinContent(ibin);
			 nEntryTot += entry;
		    }
		    double c0Ini = (double)nEntryTot / 25.0;
		    double c1Ini = (m_hTrec[lay][lr]->GetMaximum()) - c0Ini;

		    ftmin[lay][lr] -> SetParameter(0, c0Ini);
		    ftmin[lay][lr] -> SetParameter(1, c1Ini);
		    ftmin[lay][lr] -> SetParameter(2, 0);
		    ftmin[lay][lr] -> SetParameter(4, initT0);
		    ftmin[lay][lr] -> SetParameter(5, 2);

		    m_hTrec[lay][lr] -> Fit(funname, "Q", "",
					    m_param.tminFitRange[lay][0],
					    m_param.tminFitRange[lay][1]);
		    gStyle -> SetOptFit(11);
// 		    chisq = ftmin[lay][lr]->GetChisquare();
// 		    ndf = ftmin[lay][lr]->GetNDF();
		    chisq = ftmin[lay][lr]->GetChisquare();
		    ndf = ftmin[lay][lr]->GetNDF();
		    chindfTmin[lay][lr] = chisq / ndf;

		    if(chindfTmin[lay][lr] < m_param.tminFitChindf){
			 fitTminFg[lay][lr] = 1;
			 t0Fit[lay][lr] = ftmin[lay][lr]->GetParameter(4);
// 			 t0Fit[lay][lr] = ftmin->GetParameter(4);
			 t0Fit[lay][lr] += m_param.t0Shift;
			 t0Cal[lay][lr] = t0Fit[lay][lr] - m_param.timeShift;
		    }
	       }

	       if(0 == fitTminFg[lay][lr]){
		    wir = m_mdcGeomSvc->Wire(lay, 0)->Id();
		    t0Cal[lay][lr] = calconst->getT0(wir);
		    t0Fit[lay][lr] = t0Cal[lay][lr] + m_param.timeShift;
	       }
	  }

	  for(int iud=0; iud<2; iud++){
	       sprintf(funname, "ftminCosm_%02d_%d", lay, iud);
	       ftminCosm[lay][iud] = new TF1(funname, funTmin, 0, 150, 6);
	       ftminCosm[lay][iud] -> SetParameter(0, 0);
	       ftminCosm[lay][iud] -> SetParameter(4, initT0);
	       ftminCosm[lay][iud] -> SetParameter(5, 1);
	       m_hTrecCosm[lay][iud] -> Fit(funname, "Q", "",
					    m_param.tminFitRange[lay][0],
					    m_param.tminFitRange[lay][1]);
	       gStyle -> SetOptFit(11);
	       t0FitCosm[lay][iud] += m_param.t0Shift;
	       t0FitCosm[lay][iud] = ftminCosm[lay][iud]->GetParameter(4);
	  }
     }

     // fit Tmax
     TF1* ftmax[MdcCalNLayer][MdcCalLR];
     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(lr=0; lr<MdcCalLR; lr++){
	       fitTmaxFg[lay][lr] = 0;
	       chindfTmax[lay][lr] = -1;
	       sprintf(funname, "ftmax%02d_%d", lay, lr);
	       ftmax[lay][lr] = new TF1(funname, funTmax, 250, 500, 4);

	       if(1 == m_param.fgCalib[lay]){
		    ftmax[lay][lr] -> SetParameter(2, m_param.initTm[lay]);
		    ftmax[lay][lr] -> SetParameter(3, 10);
		    m_hTrec[lay][lr] -> Fit(funname, "Q+", "",
					    m_param.tmaxFitRange[lay][0],
					    m_param.tmaxFitRange[lay][1]);
		    gStyle -> SetOptFit(11);
		    chisq = ftmax[lay][lr]->GetChisquare();
		    ndf = ftmax[lay][lr]->GetNDF();
		    chindfTmax[lay][lr] = chisq / ndf;
		    if(chindfTmax[lay][lr] < m_param.tmaxFitChindf){
			 fitTmaxFg[lay][lr] = 1;
			 tmax[lay][lr] = ftmax[lay][lr]->GetParameter(2);
		    }
	       }

	       if(0 == fitTmaxFg[lay][lr]){
		    tmax[lay][lr] = (calconst->getXtpar(lay, 0, lr, 6)) + t0Fit[lay][2];
	       }
	  }
     }

     // output for check
     ofstream ft0("preT0.dat");
     for(lay=0; lay<MdcCalNLayer; lay++){
	  ft0 << setw(5) << lay << setw(3) << fitTminFg[lay][2]
	      << setw(15) << t0Cal[lay][2] << setw(15) << t0Fit[lay][2]
	      << setw(15) << chindfTmin[lay][2] << endl;
     }
     ft0 << endl;
     for(lay=0; lay<MdcCalNLayer; lay++){
	  ft0 << setw(5) << lay
	      << setw(3) << fitTmaxFg[lay][0] << setw(10) << tmax[lay][0]
	      << setw(10) << chindfTmax[lay][0]
	      << setw(3) << fitTmaxFg[lay][1] << setw(10) << tmax[lay][1]
	      << setw(10) << chindfTmax[lay][1]
	      << setw(3) << fitTmaxFg[lay][2] << setw(10) << tmax[lay][2]
	      << setw(10) << chindfTmax[lay][2]
	      << setw(10) << tmax[lay][0] - t0Fit[lay][2]
	      << setw(10) << tmax[lay][1] - t0Fit[lay][2]
	      << setw(10) << tmax[lay][2] - t0Fit[lay][2]
	      << endl;
     }
     ft0.close();
     cout << "preT0.dat was written." << endl;

     // output for cosmic T0
     ofstream ft0cosm("cosmicT0.dat");
     for(lay=0; lay<MdcCalNLayer; lay++){
	  ft0cosm << setw(5) << lay << setw(15) << t0Fit[lay][2]
		  << setw(15) << t0FitCosm[lay][0] << setw(15) << t0FitCosm[lay][1] << endl;
     }
     ft0cosm.close();

     // set T0
     int i;
     int nwire = m_mdcGeomSvc -> getWireSize();
     for(i=0; i<nwire; i++){
	  lay = m_mdcGeomSvc -> Wire(i) -> Layer();
	  if(1 == m_param.fgCalib[lay]){
	       calconst -> resetT0(i, t0Cal[lay][2]);
	       calconst -> resetDelT0(i, 0.0);
	  }
     }

     // set tm of X-T
     if(m_param.preT0SetTm){
	  int iEntr;
	  double tm;
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       if(1 != m_param.fgCalib[lay]) continue;

	       for(iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
		    for(lr=0; lr<MdcCalLR; lr++){
			 tm = tmax[lay][lr] - t0Fit[lay][2];
			 if( (tmax[lay][lr] > m_param.tmaxFitRange[lay][0]) &&
			     (tmax[lay][lr] < m_param.tmaxFitRange[lay][1]) ){
			      calconst -> resetXtpar(lay, iEntr, lr, 6, tm);
			 }
		    }
	       }
	  }
     }

     // set sigma
     int bin;
     double sdpar = m_param.initSigma;	// mm
     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(int iEntr=0; iEntr<MdcCalNENTRSD; iEntr++){
	       for(lr=0; lr<2; lr++){
		    for(bin=0; bin<MdcCalSdNBIN; bin++){
			 calconst -> resetSdpar(lay, iEntr, lr, bin, sdpar);
		    }
	       }
	  }
     }

//      delete ftmin;
     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(lr=0; lr<MdcCalLR; lr++){     
	       delete ftmin[lay][lr];
	       delete ftmax[lay][lr];
	  }
     }
     return 1;
}
Double_t PreT0MdcCalib::funTmin(Double_t* x, Double_t* par){
     Double_t fitval;
     fitval = par[0] + par[1]*exp( -par[2]*(x[0]-par[3]) ) /
          ( 1 + exp( -(x[0]-par[4])/par[5] ));
     return fitval;
}

Double_t PreT0MdcCalib::funTmax(Double_t* x, Double_t* par){
     Double_t fitval;
     fitval = par[0] + par[1] / (1 + exp((x[0]-par[2])/par[3]));
     return fitval;
}

