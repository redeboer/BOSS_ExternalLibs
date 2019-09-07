#include "MdcCalibAlg/IniMdcCalib.h"
 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/EventHeader.h"
#include "EvTimeEvent/RecEsTime.h"
#include "EventModel/Event.h"
#include "MdcRawEvent/MdcDigi.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "TStyle.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cmath>

#include "TF1.h"

using namespace Event;

IniMdcCalib::IniMdcCalib(){
}

IniMdcCalib::~IniMdcCalib(){
}

void IniMdcCalib::clear(){
     int iEs;
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  delete m_hlaymapT[lay];
          delete m_htdc[lay];
          delete m_htraw[lay];
	  for(iEs=0; iEs<m_param.nEsFlag; iEs++){
	       delete m_htdcTes[lay][iEs];
	       delete m_htrawTes[lay][iEs];
	  }

	  delete m_hlaymapQ[lay];
          delete m_hqraw[lay];
     }

     for(int wir=0; wir<MdcCalTotCell; wir++){
	  delete m_htrawCel[wir];
	  delete m_hqrawCel[wir];
     }

     delete m_hLayerHitmapT;
     delete m_hWireHitMapT;

     delete m_hLayerHitmapQ;
     delete m_hWireHitMapQ;
     for(iEs=0; iEs<m_param.nEsFlag; iEs++) delete m_hTes[iEs];
     delete m_hTesAllFlag;
     delete m_hTesAll;
     delete m_hTesCal;
     delete m_hTesFlag;

     delete m_fdcom;
     delete m_fdTmap;
     delete m_fdTraw;
     delete m_fdTrawCel;
     delete m_fdTrawTes;
     delete m_fdQmap;
     delete m_fdQraw;
     delete m_fdQrawCel;
}

void IniMdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "IniMdcCalib");
     log << MSG::INFO << "IniMdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     int lay;
     int cel;
     int wir;
     int ncel;
     char hname[200];

     m_nWire = m_mdcGeomSvc -> getWireSize();
     m_nLayer = m_mdcGeomSvc -> getLayerSize();

     m_fdcom = new TFolder("Common", "Common");
     m_hlist->Add(m_fdcom);

     m_fdTmap = new TFolder("Thitmap", "Thitmap");
     m_hlist->Add(m_fdTmap);

     m_fdTraw = new TFolder("Traw", "Traw");
     m_hlist->Add(m_fdTraw);

     m_fdTrawCel = new TFolder("TrawCell", "TrawCell");
     m_hlist->Add(m_fdTrawCel);

     m_fdTrawTes = new TFolder("TrawTes", "TrawTes");
     m_hlist->Add(m_fdTrawTes);

     m_fdQmap = new TFolder("Qhitmap", "Qhitmap");
     m_hlist->Add(m_fdQmap);

     m_fdQraw = new TFolder("Qraw", "Qraw");
     m_hlist->Add(m_fdQraw);

     m_fdQrawCel = new TFolder("QrawCell", "QrawCell");
     m_hlist->Add(m_fdQrawCel);

     m_hLayerHitmapT = new TH1F("T_Hitmap_Layer", "", 43, -0.5, 42.5);
     m_fdcom->Add(m_hLayerHitmapT);
                
     m_hWireHitMapT = new TH1F("T_Hitmap_Wire", "", 6796, -0.5, 6795.5);
     m_fdcom->Add(m_hWireHitMapT);

     m_hLayerHitmapQ = new TH1F("Q_Hitmap_Layer", "", 43, -0.5, 42.5);
     m_fdcom->Add(m_hLayerHitmapQ);
                
     m_hWireHitMapQ = new TH1F("Q_Hitmap_Wire", "", 6796, -0.5, 6795.5);
     m_fdcom->Add(m_hWireHitMapQ);

     int iEs;
     for(iEs=0; iEs<m_param.nEsFlag; iEs++){
	  sprintf(hname, "Tes_%d", m_param.esFlag[iEs]);
	  m_hTes[iEs] = new TH1F(hname, "", 750, 0, 1500);
	  m_fdcom->Add(m_hTes[iEs]);
     }

     m_hTesAllFlag = new TH1F("TesAllFlag", "", 300, -0.5, 299.5);
     m_fdcom -> Add(m_hTesAllFlag);

     m_hTesAll = new TH1F("TesAll", "", 750, 0, 1500);
     m_fdcom->Add(m_hTesAll);

     m_hTesCal = new TH1F("TesCal", "", 750, 0, 1500);
     m_fdcom->Add(m_hTesCal);

     m_hTesFlag = new TH1F("Tes_Flag", "", 300, -0.5, 299.5);
     m_fdcom->Add(m_hTesFlag);

     for(lay=0; lay<m_nLayer; lay++){
	  ncel = m_mdcGeomSvc->Layer(lay)->NCell();

	  sprintf(hname, "T_hitmap_Lay%02d", lay);
	  m_hlaymapT[lay] = new TH1F(hname, "", ncel, -0.5, (float)ncel-0.5);
	  m_fdTmap -> Add(m_hlaymapT[lay]);

	  sprintf(hname, "TDC_Lay%02d", lay);
          m_htdc[lay] = new TH1F(hname, "", 800, 0, 20000);
          m_fdTraw -> Add(m_htdc[lay]);

	  sprintf(hname, "Traw_Lay%02d", lay);
          m_htraw[lay] = new TH1F(hname, "", 500, 0, 1000);
          m_fdTraw -> Add(m_htraw[lay]);

	  for(iEs=0; iEs<m_param.nEsFlag; iEs++){
	       sprintf(hname, "TDC_Lay%02d_Tes%d", lay, m_param.esFlag[iEs]);
	       m_htdcTes[lay][iEs] = new TH1F(hname, "", 800, 0, 20000);
	       m_fdTrawTes -> Add(m_htdcTes[lay][iEs]);

	       sprintf(hname, "Traw_Lay%02d_Tes%d", lay, m_param.esFlag[iEs]);
	       m_htrawTes[lay][iEs] = new TH1F(hname, "", 300, 0, 600);
	       m_fdTrawTes -> Add(m_htrawTes[lay][iEs]);
	  }

	  sprintf(hname, "Q_hitmap_Lay%02d", lay);
	  m_hlaymapQ[lay] = new TH1F(hname, "", ncel, -0.5, (float)ncel-0.5);
	  m_fdQmap -> Add(m_hlaymapQ[lay]);

	  sprintf(hname, "Qraw_Lay%02d", lay);
          m_hqraw[lay] = new TH1F(hname, "", 2000, 0, 4000);
          m_fdQraw -> Add(m_hqraw[lay]);
     }

     for(wir=0; wir<MdcCalTotCell; wir++){
	  lay = m_mdcGeomSvc -> Wire(wir) -> Layer();
	  cel = m_mdcGeomSvc -> Wire(wir) -> Cell();

	  sprintf(hname, "Traw_%02d_%03d_%04d", lay, cel, wir);
	  m_htrawCel[wir] = new TH1F(hname, "", 300, 0, 600);
	  m_fdTrawCel -> Add(m_htrawCel[wir]);

	  sprintf(hname, "Qraw_%02d_%03d_%04d", lay, cel, wir);
	  m_hqrawCel[wir] = new TH1F(hname, "", 2000, 0, 4000);
	  m_fdQrawCel -> Add(m_hqrawCel[wir]);
     }
}

int IniMdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "IniMdcCalib");
     log << MSG::DEBUG << "IniMdcCalib::fillHist()" << endreq;

     int lay;
     int cel;
     int wir;
     int digiId;
     unsigned fgOverFlow;
     double tdc;
     double traw;
     double adc;
     double qraw;
     Identifier id; 

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");
     if (!eventHeader) {
	  log << MSG::FATAL << "Could not find Event Header" << endreq;
	  return( StatusCode::FAILURE);
     }
     int iRun = eventHeader->runNumber();
     int iEvt = eventHeader->eventNumber();

     // get EsTimeCol
     double tes = -9999.0;
     int esTimeflag = -1;
     SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc,"/Event/Recon/RecEsTimeCol");
     if( (!aevtimeCol) || (aevtimeCol->size()==0) ){
	  tes = -9999.0;
	  esTimeflag = -1;
     }else{
	  RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
	  for(; iter_evt!=aevtimeCol->end(); iter_evt++){
	       tes = (*iter_evt)->getTest();
	       esTimeflag = (*iter_evt)->getStat();
// 	       cout << "tes " << tes << endl;
	  }
     }
     m_hTesAllFlag->Fill(esTimeflag);
     m_hTesAll->Fill(tes);
     m_hTesFlag->Fill(esTimeflag);
     int nTesFlag = -1;
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++){
	  if(esTimeflag == m_param.esFlag[iEs]){
	       m_hTes[iEs]->Fill(tes);
	       nTesFlag = iEs;
	       break;
	  }
     }
     bool fgFillTes = false;
     if( (nTesFlag > -1) && (tes > m_param.tesMin) && (tes < m_param.tesMax) ){
	  m_hTesCal->Fill(tes);
	  fgFillTes = true;
     }

//      cout << setw(10) << iRun << setw(10) << iEvt << setw(10) << tes << endl;

     // retrieve Mdc digi
     SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc,"/Event/Digi/MdcDigiCol");
     if (!mdcDigiCol) {
          log << MSG::FATAL << "Could not find event" << endreq;
     }

     MdcDigiCol::iterator iter = mdcDigiCol->begin();
     digiId = 0;
     for(; iter != mdcDigiCol->end(); iter++, digiId++) {
          MdcDigi *aDigi = (*iter);
          id = (aDigi)->identify();

          lay = MdcID::layer(id);
          cel = MdcID::wire(id);
          wir = m_mdcGeomSvc->Wire(lay, cel)->Id();

          tdc = (aDigi) -> getTimeChannel();
          adc = (aDigi) -> getChargeChannel();
	  fgOverFlow = (aDigi) -> getOverflow();

	  if ( ((fgOverFlow & 1) !=0 ) || ((fgOverFlow & 12) != 0) ||
	       (aDigi->getTimeChannel() == 0x7FFFFFFF) ||
	       (aDigi->getChargeChannel() == 0x7FFFFFFF) ) continue;

          traw = tdc * MdcCalTdcCnv;
	  qraw = adc * MdcCalAdcCnv;

          m_hLayerHitmapT -> Fill(lay);
          m_hWireHitMapT -> Fill(wir);
          m_hlaymapT[lay] -> Fill(cel);

          m_hLayerHitmapQ -> Fill(lay);
          m_hWireHitMapQ -> Fill(wir);
          m_hlaymapQ[lay] -> Fill(cel);

	  if(fgFillTes){
	       traw -= tes;
	       traw += m_param.timeShift;

	       m_htdc[lay] -> Fill(tdc);
	       m_htraw[lay] -> Fill(traw);
	       m_hqraw[lay] -> Fill(qraw);

	       m_htdcTes[lay][nTesFlag] -> Fill(tdc);
	       m_htrawTes[lay][nTesFlag] -> Fill(traw);

	       m_htrawCel[wir] -> Fill(traw);
	       m_hqrawCel[wir] -> Fill(qraw);
          }
     }
     return 1;
}

int IniMdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "IniMdcCalib");
     log << MSG::DEBUG << "IniMdcCalib::updateConst()" << endreq;

     int lay;
     int wir;
     double t0Fit[MdcCalNLayer];
     double t0Cal[MdcCalNLayer];
     double tmax[MdcCalNLayer];
     double initT0 = m_param.initT0;

     int fitTminFg[MdcCalNLayer];
     int fitTmaxFg[MdcCalNLayer];
     double chisq;
     double ndf;
     double chindfTmin[MdcCalNLayer];
     double chindfTmax[MdcCalNLayer];
     char funname[200];

     // fit Tmin
     TF1* ftmin[MdcCalNLayer];
//      sprintf(funname, "ftmin", lay);
//      TF1* ftmin = new TF1(funname, funTmin, 0, 150, 6);
     for(lay=0; lay<m_nLayer; lay++){
	  fitTminFg[lay] = 0;
	  chindfTmin[lay] = -1;
	  sprintf(funname, "ftmin%02d", lay);
	  ftmin[lay] = new TF1(funname, funTmin, 0, 150, 6);

	  if(1 == m_param.fgCalib[lay]){
	       Stat_t nEntryTot = 0;
	       for(int ibin=1; ibin<=25; ibin++){
		    Stat_t entry = m_htraw[lay]->GetBinContent(ibin);
		    nEntryTot += entry;
	       }
	       double c0Ini = (double)nEntryTot / 25.0;
	       double c1Ini = (m_htraw[lay]->GetMaximum()) - c0Ini;

	       ftmin[lay] -> SetParameter(0, c0Ini);
	       ftmin[lay] -> SetParameter(1, c1Ini);
	       ftmin[lay] -> SetParameter(2, 0);
	       ftmin[lay] -> SetParameter(4, initT0);
	       ftmin[lay] -> SetParameter(5, 2);

	       m_htraw[lay] -> Fit(funname, "Q", "",
				   m_param.tminFitRange[lay][0],
				   m_param.tminFitRange[lay][1]);
	       chisq = ftmin[lay]->GetChisquare();
	       gStyle -> SetOptFit(11);
	       ndf = ftmin[lay]->GetNDF();
	       chindfTmin[lay] = chisq / ndf;
	       if(chindfTmin[lay] < m_param.tminFitChindf){
		    fitTminFg[lay] = 1;
		    t0Fit[lay] = ftmin[lay]->GetParameter(4);
		    t0Fit[lay] += m_param.t0Shift;
		    t0Cal[lay] = t0Fit[lay] - m_param.timeShift;
	       }
	  }

	  if(0 == fitTminFg[lay]){
	       wir = m_mdcGeomSvc->Wire(lay, 0)->Id();
	       t0Cal[lay] = calconst->getT0(wir);
	       t0Fit[lay] = t0Cal[lay] + m_param.timeShift;
	  }
     }

     // fit Tmax
     TF1* ftmax[MdcCalNLayer];
     for(lay=0; lay<m_nLayer; lay++){
	  fitTmaxFg[lay] = 0;
	  chindfTmax[lay] = -1;
	  sprintf(funname, "ftmax%02d", lay);
	  ftmax[lay] = new TF1(funname, funTmax, 250, 500, 4);

	  if(1 == m_param.fgCalib[lay]){
	       ftmax[lay] -> SetParameter(2, m_param.initTm[lay]);
	       ftmax[lay] -> SetParameter(3, 10);

	       m_htraw[lay] -> Fit(funname, "Q+", "",
				   m_param.tmaxFitRange[lay][0],
				   m_param.tmaxFitRange[lay][1]);
	       gStyle -> SetOptFit(11);
	       chisq = ftmax[lay]->GetChisquare();
	       ndf = ftmax[lay]->GetNDF();
	       chindfTmax[lay] = chisq / ndf;
	       if(chindfTmax[lay] < m_param.tmaxFitChindf){
		    fitTmaxFg[lay] = 1;
		    tmax[lay] = ftmax[lay]->GetParameter(2);
	       }
	  }

	  if(0 == fitTmaxFg[lay]){
	       tmax[lay] = (calconst->getXtpar(lay, 0, 0, 6)) + t0Fit[lay];
	  }
     }

     // output for check
     ofstream ft0("iniT0.dat");
     for(lay=0; lay<m_nLayer; lay++){
	  ft0 << setw(5) << lay << setw(3) << fitTminFg[lay]
	      << setw(12) << t0Cal[lay] << setw(12) << t0Fit[lay]
	      << setw(12) << chindfTmin[lay] << setw(5) << fitTmaxFg[lay]
	      << setw(12) << tmax[lay] << setw(12) << tmax[lay] - t0Fit[lay]
	      << setw(12) << chindfTmax[lay] << endl;
     }
     ft0.close();
     cout << "iniT0.dat was written." << endl;

     // set T0
     int i;
     int nwire = m_mdcGeomSvc -> getWireSize();
     for(i=0; i<nwire; i++){
	  lay = m_mdcGeomSvc -> Wire(i) -> Layer();
	  if(1 == m_param.fgCalib[lay]){
	       calconst -> resetT0(i, t0Cal[lay]);
	       calconst -> resetDelT0(i, 0.0);
	  }
     }

     if(0 == m_param.fgIniCalConst){
	  // set X-T
	  int lr;
	  int iEntr;
	  int ord;
	  double xtpar;
	  double xtini[8] = {0, 0.03, 0, 0, 0, 0, 999.9, 0};
	  for(lay=0; lay<m_nLayer; lay++){
	       if(1 != m_param.fgCalib[lay]) continue;

	       for(iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
		    for(lr=0; lr<MdcCalLR; lr++){
			 for(ord=0; ord<MdcCalXtNPars; ord++){
			      if(6 == ord){
				   xtpar = tmax[lay] - t0Fit[lay];
			      } else{
				   xtpar = xtini[ord];
			      }
			      calconst -> resetXtpar(lay, iEntr, lr, ord, xtpar);
			 }
		    }
	       }
	  }

	  // set Q-T
	  for(lay=0; lay<m_nLayer; lay++){
	       calconst -> resetQtpar0(lay, 0.0);
	       calconst -> resetQtpar1(lay, 0.0);
	  }

	  // set S-D
	  int bin;
	  double sdpar = m_param.initSigma;	// mm
	  for(lay=0; lay<m_nLayer; lay++){
	       for(iEntr=0; iEntr<MdcCalNENTRSD; iEntr++){
		    for(lr=0; lr<2; lr++){
			 for(bin=0; bin<MdcCalSdNBIN; bin++){
			      calconst -> resetSdpar(lay, iEntr, lr, bin, sdpar);
			 }
		    }
	       }
	  }
     } else if(2 == m_param.fgIniCalConst){
	  int lr;
	  int iEntr;
	  double xtpar;
	  for(lay=0; lay<m_nLayer; lay++){
	       for(iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
		    for(lr=0; lr<MdcCalLR; lr++){
			 xtpar = tmax[lay] - t0Fit[lay];
			 calconst -> resetXtpar(lay, iEntr, lr, 6, xtpar);
		    }
	       }
	  }
     }

//      delete ftmin;
     for(lay=0; lay<MdcCalNLayer; lay++){
	  delete ftmin[lay];
	  delete ftmax[lay];
     }
     return 1;
}

Double_t IniMdcCalib::funTmin(Double_t* x, Double_t* par){
     Double_t fitval;
     fitval = par[0] + par[1]*exp( -par[2]*(x[0]-par[3]) ) /
          ( 1 + exp( -(x[0]-par[4])/par[5] ));
     return fitval;
}

Double_t IniMdcCalib::funTmax(Double_t* x, Double_t* par){
     Double_t fitval;
     fitval = par[0] + par[1] / (1 + exp((x[0]-par[2])/par[3]));
     return fitval;
}

