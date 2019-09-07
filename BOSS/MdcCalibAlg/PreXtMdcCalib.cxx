#include "MdcCalibAlg/PreXtMdcCalib.h"

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
#include <string>
#include <cstring>    
#include <cmath>

#include "TF1.h"

using namespace Event;

PreXtMdcCalib::PreXtMdcCalib(){
     m_fgIniTm = false;
}

PreXtMdcCalib::~PreXtMdcCalib(){
}

void PreXtMdcCalib::clear(){
     for(int lay=0; lay<MdcCalNLayer; lay++){
	  delete m_grXt[lay];
	  delete m_htrec[lay];
     }
     delete m_haxis;
     delete m_fdPreXt;
}

void PreXtMdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			       IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "PreXtMdcCalib");
     log << MSG::INFO << "PreXtMdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     int lay;

     m_nWire = m_mdcGeomSvc -> getWireSize();
     m_nLayer = m_mdcGeomSvc -> getLayerSize();

     m_fdPreXt = new TFolder("PreXt", "PreXt");
     m_hlist->Add(m_fdPreXt);

     m_fdNhit = new TFolder("XtNhit", "XtNhit");
     m_hlist->Add(m_fdNhit);

     m_haxis = new TH2F("axis", "", 50, 0, 300, 50, 0, 9);
     m_haxis -> SetStats(0);
     m_fdPreXt -> Add(m_haxis);

     char hname[200];
     for(lay=0; lay<MdcCalNLayer; lay++){
	  sprintf(hname, "trec%02d", lay);
	  m_htrec[lay] = new TH1F(hname, "", 310, -20, 600);
	  m_fdPreXt -> Add(m_htrec[lay]);
     }

     m_nhitTot = new TH1F("nhitTot", "", 43, -0.5, 42.5);
     m_fdNhit -> Add(m_nhitTot);

     for(lay=0; lay<MdcCalNLayer; lay++){
	  sprintf(hname, "nhitBin%02d", lay);
	  m_nhitBin[lay] = new TH1F(hname, "", 40, 5.0, 405.0);
	  m_fdNhit -> Add(m_nhitBin[lay]);
     }

     /* integrated time Spectrum for determination X-T */
     int bin;
     m_nXtBin = 40;
     double twid = 10.0;	// ns
     for(bin=0; bin<m_nXtBin; bin++)  m_tbin[bin] = (double)(bin+1) * twid;

     for(lay=0; lay<MdcCalNLayer; lay++){
	  m_nTot[lay] = 0;
	  for(bin=0; bin<m_nXtBin; bin++){
	       m_nEntries[lay][bin] = 0;
	  }
     }
}

int PreXtMdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "PreXtMdcCalib");
     log << MSG::DEBUG << "PreXtMdcCalib::fillHist()" << endreq;

     int bin;
     int lay;
     int digiId;
     unsigned fgOverFlow;
     double tdc;
     double adc;
     double traw;
     double trec;
     Identifier id; 

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     double xtpar[8];
     if(! m_fgIniTm){
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       m_t0[lay] = m_mdcFunSvc -> getT0(lay, 0);
	       m_mdcFunSvc->getXtpar(lay, 0, 0, xtpar);
	       m_tm[lay] = xtpar[6];
	  }
	  m_fgIniTm = true;
     }

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
	  }
     }
     bool flagTes = false;
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++){
	  if(esTimeflag == m_param.esFlag[iEs]){
	       flagTes = true;
	       break;
	  }
     }
     if( (!flagTes) || (tes < m_param.tesMin) || (tes > m_param.tesMax) ) return -1;

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

          tdc = (aDigi) -> getTimeChannel();
          adc = (aDigi) -> getChargeChannel();
	  fgOverFlow = (aDigi) -> getOverflow();
          traw = tdc * MdcCalTdcCnv;
	  trec = traw - tes - m_t0[lay];

// 	  if ( !((fgOverFlow == 0)||(fgOverFlow ==12)) ||
// 	       (aDigi->getTimeChannel() == 0x7FFFFFFF) ||
// 	       (aDigi->getChargeChannel() == 0x7FFFFFFF) ) continue;
	  if ( ((fgOverFlow & 1) !=0 ) || ((fgOverFlow & 12) != 0) ||
	       (aDigi->getTimeChannel() == 0x7FFFFFFF) ||
	       (aDigi->getChargeChannel() == 0x7FFFFFFF) ) continue;


	  /* integrated time Spectrum for determination X-T */
	  if(trec > 0){
	       if(trec < m_tm[lay]){
		    m_nTot[lay]++;
		    m_htrec[lay]->Fill(trec);
		    m_nhitTot->Fill(lay);
	       }
	       for(bin=0; bin<m_nXtBin; bin++){
		    if(trec < m_tbin[bin]){
			 m_nEntries[lay][bin]++;
			 m_nhitBin[lay]->Fill(m_tbin[bin]);
		    }
	       }
	  }
     }
     return 1;
}

int PreXtMdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "PreXtMdcCalib");
     log << MSG::DEBUG << "PreXtMdcCalib::updateConst()" << endreq;

     int lay;
     int bin;
     int iLR;
     int iEntr;
     int ord;

     double layRad;
     double ncel;
     double pi = 3.141592653;

     double dm;
     double dist[40];
     double xtpar[6];
     char hname[200];

     TF1* funXt = new TF1("funXt", xtfun, 0, 300, 6);
     funXt -> FixParameter(0, 0.0);
     funXt -> SetParameter(1, 0.03);
     funXt -> SetParameter(2, 0.0);
     funXt -> SetParameter(3, 0.0);
     funXt -> SetParameter(4, 0.0);
     funXt -> SetParameter(5, 0.0);

     ofstream fxtlog("preXtpar.dat");
     for(lay=0; lay<MdcCalNLayer; lay++){
	  sprintf(hname, "grPreXt%02d", lay);
	  m_grXt[lay] = new TGraph();
	  m_grXt[lay] -> SetName(hname);
	  m_grXt[lay] -> SetMarkerStyle(20);
	  m_fdPreXt -> Add(m_grXt[lay]);

	  layRad = m_mdcGeomSvc -> Layer(lay) -> Radius();
	  ncel = m_mdcGeomSvc -> Layer(lay) -> NCell();
	  dm = pi * layRad / ncel;

	  fxtlog << "layer " << lay << endl;
	  for(bin=0; bin<m_nXtBin; bin++){
	       dist[bin] = dm * m_nEntries[lay][bin] / m_nTot[lay];
	       m_grXt[lay] -> SetPoint(bin, m_tbin[bin], dist[bin]);
	       fxtlog << setw(4) << bin << setw(15) << m_tbin[bin]
		      << setw(15) << dist[bin] << setw(15) << dm
		      << setw(10) << m_nEntries[lay][bin]
		      << setw(10) << m_nTot[lay] << endl;

	       if(m_tbin[bin] >= m_tm[lay]) break;
	  }

	  if(1 == m_param.fgCalib[lay]){
	       m_grXt[lay] -> Fit(funXt, "Q", "", 0.0, m_tm[lay]);
	       for(ord=0; ord<6; ord++){
		    xtpar[ord] = funXt -> GetParameter(ord);
	       }

	       for(iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
		    for(iLR=0; iLR<MdcCalLR; iLR++){
			 for(ord=0; ord<6; ord++){
			      calconst -> resetXtpar(lay, iEntr, iLR, ord, xtpar[ord]);
			 }
		    }
	       }
	  } else{
	       for(ord=0; ord<6; ord++) xtpar[ord] = calconst->getXtpar(lay, 0, 0, ord);
	  }

	  for(ord=0; ord<6; ord++)  fxtlog << setw(14) << xtpar[ord];
	  fxtlog << setw(10) << m_tm[lay] << "  0" << endl;
     } // end of layer loop
     fxtlog.close();
     cout << "preXt.dat was written." << endl;

     delete funXt;
     return 1;
}

Double_t PreXtMdcCalib::xtfun(Double_t *x, Double_t *par){
     Double_t val = 0.0;
     for(Int_t ord=0; ord<6; ord++){
	  val += par[ord] * pow(x[0], ord);
     }
     return val;
}

