#include "include/PreXtCalib.h"
#include "include/fun.h"
#include <math.h>

#include "TF1.h"
#include "TStyle.h"

PreXtCalib::PreXtCalib(){
     cout << "Calibration type: PreXtCalib" << endl;
}

PreXtCalib::~PreXtCalib(){
}

void PreXtCalib::init(TObjArray* hlist, MdcCosGeom* pGeom){
     m_pGeom = pGeom;

     double twid = 10.0;	// ns
     for(int bin=0; bin<NPreXtBin; bin++)  m_tbin[bin] = (double)(bin+1) * twid;

     m_fdPreXt = new TFolder("mPreXt", "PreXt");
     hlist->Add(m_fdPreXt);

     m_fdNhit = new TFolder("mXtNhit", "XtNhit");
     hlist->Add(m_fdNhit);

     m_haxis = new TH2F("maxis", "", 50, 0, 300, 50, 0, 9);
     m_haxis -> SetStats(0);
     m_fdPreXt -> Add(m_haxis);

     m_nhitTot = new TH1F("mnhitTot", "", 43, -0.5, 42.5);
     m_fdNhit -> Add(m_nhitTot);

     char hname[200];
     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "mtrec%02d", lay);
	  m_htrec[lay] = new TH1F(hname, "", 310, -20, 600);
	  m_fdPreXt -> Add(m_htrec[lay]);

	  sprintf(hname, "mnhitBin%02d", lay);
	  m_nhitBin[lay] = new TH1F(hname, "", 40, 5.0, 405.0);
	  m_fdNhit -> Add(m_nhitBin[lay]);
     }
}

void PreXtCalib::mergeHist(TFile* fhist){
     TFolder* fdPreXt = (TFolder*)fhist->Get("PreXt");
     TFolder* fdNhit = (TFolder*)fhist->Get("XtNhit");

     char hname[200];
     TH1F* hist;
     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "trec%02d", lay);
	  hist = (TH1F*)fdPreXt->FindObjectAny(hname);
	  m_htrec[lay]->Add(hist);

	  sprintf(hname, "nhitBin%02d", lay);
	  hist = (TH1F*)fdNhit->FindObjectAny(hname);
	  m_nhitBin[lay]->Add(hist);
     }

     hist = (TH1F*)fdNhit->FindObjectAny("nhitTot");
     m_nhitTot->Add(hist);
}

void PreXtCalib::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     double pi = 3.141592653;
     double dist[NPreXtBin];
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
     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "mgrPreXt%02d", lay);
	  m_grXt[lay] = new TGraph();
	  m_grXt[lay] -> SetName(hname);
	  m_grXt[lay] -> SetMarkerStyle(20);
	  m_fdPreXt -> Add(m_grXt[lay]);

	  double layRad = m_pGeom -> getLayer(lay) -> getLayerRad();
	  int ncel = m_pGeom -> getLayer(lay) -> getNcell();
	  double dm = pi * layRad / (double)ncel;
	  Double_t nTot = m_nhitTot->GetBinContent(lay+1);
	  double tm = calconst->getXtpar(lay, 0, 0, 6);

	  fxtlog << "layer " << lay << endl;
	  for(int bin=0; bin<NPreXtBin; bin++){
	       Double_t nhitBin = m_nhitBin[lay]->GetBinContent(bin+1);
	       dist[bin] = dm * nhitBin / nTot;
	       m_grXt[lay] -> SetPoint(bin, m_tbin[bin], dist[bin]);
	       fxtlog << setw(4) << bin << setw(15) << m_tbin[bin]
		      << setw(15) << dist[bin] << setw(15) << dm
		      << setw(10) << nhitBin
		      << setw(10) << nTot << endl;

	       if(m_tbin[bin] >= tm) break;
	  }

	  if(1 == gFgCalib[lay]){
	       m_grXt[lay] -> Fit(funXt, "Q", "", 0.0, tm);
	       for(int ord=0; ord<6; ord++) xtpar[ord] = funXt -> GetParameter(ord);

	       for(int iEntr=0; iEntr<NENTRXT; iEntr++){
		    for(int iLR=0; iLR<NLR; iLR++){
			 for(int ord=0; ord<6; ord++){
			      calconst -> resetXtpar(lay, iEntr, iLR, ord, xtpar[ord]);
			 }
		    }
	       }
	  } else{
	       for(int ord=0; ord<6; ord++) xtpar[ord] = calconst->getXtpar(lay, 0, 0, ord);
	  }

	  for(int ord=0; ord<6; ord++)  fxtlog << setw(14) << xtpar[ord];
	  fxtlog << setw(10) << tm << "  0" << endl;
     } // end of layer loop
     fxtlog.close();
     cout << "preXt.dat was written." << endl;

     renameHist();
     delete funXt;
}

Double_t PreXtCalib::xtfun(Double_t *x, Double_t *par){
     Double_t val = 0.0;
     for(Int_t ord=0; ord<6; ord++){
	  val += par[ord] * pow(x[0], ord);
     }
     return val;
}

void PreXtCalib::renameHist(){
     m_fdPreXt->SetName("PreXt");
     m_fdNhit->SetName("XtNhit");
     m_haxis->SetName("axis");
     m_nhitTot->SetName("nhitTot");

     char hname[200];
     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "trec%02d", lay);
	  m_htrec[lay]->SetName(hname);

	  sprintf(hname, "nhitBin%02d", lay);
	  m_nhitBin[lay]->SetName(hname);

	  sprintf(hname, "grPreXt%02d", lay);
	  m_grXt[lay] -> SetName(hname);
     }
}
