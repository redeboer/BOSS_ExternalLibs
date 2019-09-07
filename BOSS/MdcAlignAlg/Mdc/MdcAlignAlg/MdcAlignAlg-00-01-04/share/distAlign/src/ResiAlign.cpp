#include "include/ResiAlign.h"
#include "include/fun.h"

#include <cmath>

#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"

ResiAlign::ResiAlign(){
     cout << "Alignment type: ResiAlign" << endl;
}

ResiAlign::~ResiAlign(){
}

void ResiAlign::init(TObjArray* hlist, MdcCosGeom* pGeom){
     m_pGeom = pGeom;
     char hname[200];
     m_hnTrk = new TH1F("mHNtrack", "", 10, -0.5, 9.5);
     hlist->Add(m_hnTrk);

     m_hnHit = new TH1F("mHNhit", "", 100, -0.5, 99.5);
     hlist->Add(m_hnHit);

     m_hlayHitmap = new TH1F("mHitmap", "", 43, -0.5, 42.5);
     hlist->Add(m_hnHit);

     m_hresAll = new TH1F("mHResAllInc", "", 200, -1.0, 1.0);
     hlist->Add(m_hresAll);

     m_hresInn = new TH1F("mHResInnInc", "", 200, -1.0, 1.0);
     hlist->Add(m_hresInn);

     m_hresStp = new TH1F("mHResStpInc", "", 200, -1.0, 1.0);
     hlist->Add(m_hresStp);

     m_hresOut = new TH1F("mHResOutInc", "", 200, -1.0, 1.0);
     hlist->Add(m_hresOut);

     for(int lay=0; lay<LAYERNMAX; lay++){
	  sprintf(hname, "mRes_Layer%02d", lay);
	  m_hresLay[lay] = new TH1F(hname, "", 200, -1.0, 1.0);
	  hlist->Add(m_hresLay[lay]);
     }

     for(int iEP=0; iEP<NEP; iEP++){
	  m_gr[iEP] = new TGraph();
	  sprintf(hname, "mgrResi%02d", iEP);
	  m_gr[iEP]->SetName(hname);
	  hlist->Add(m_gr[iEP]);

	  m_grSinPhi[iEP] = new TGraph();
	  sprintf(hname, "mgrResi_sinPhi%02d", iEP);
	  m_grSinPhi[iEP]->SetName(hname);
	  hlist->Add(m_grSinPhi[iEP]);

	  m_grCosPhi[iEP] = new TGraph();
	  sprintf(hname, "mgrResi_cosPhi%02d", iEP);
	  m_grCosPhi[iEP]->SetName(hname);
	  hlist->Add(m_grCosPhi[iEP]);

	  m_npoint[iEP] = 0;
     }
}

void ResiAlign::mergeHist(TFile* fhist){
     char hname[200];
     TH1F* hist;
     hist = (TH1F*)fhist->Get("HNtrack");
     m_hnTrk->Add(hist);

     hist = (TH1F*)fhist->Get("HNhit");
     m_hnHit->Add(hist);

     hist = (TH1F*)fhist->Get("Hitmap");
     m_hlayHitmap->Add(hist);

     hist = (TH1F*)fhist->Get("HResAllInc");
     m_hresAll->Add(hist);

     hist = (TH1F*)fhist->Get("HResInnInc");
     m_hresInn->Add(hist);

     hist = (TH1F*)fhist->Get("HResStpInc");
     m_hresStp->Add(hist);

     hist = (TH1F*)fhist->Get("HResOutInc");
     m_hresOut->Add(hist);

     for(int lay=0; lay<LAYERNMAX; lay++){
	  sprintf(hname, "Res_Layer%02d", lay);
	  hist = (TH1F*)fhist->Get(hname);
	  m_hresLay[lay]->Add(hist);
     }

     for(int iEP=0; iEP<NEP; iEP++){
	  sprintf(hname, "grResi%02d", iEP);
	  TGraph* gr = (TGraph*)fhist->Get(hname);
	  int np = gr->GetN();
	  double xx;
	  double yy;
	  for(int i=0; i<np; i++){
	       gr->GetPoint(i, xx, yy);
	       m_gr[iEP]->SetPoint(m_npoint[iEP], xx, yy);
	       m_grSinPhi[iEP]->SetPoint(m_npoint[iEP], sin(xx), yy);
	       m_grCosPhi[iEP]->SetPoint(m_npoint[iEP], cos(xx), yy);
	       m_npoint[iEP]++;
	  }
     }
}

void ResiAlign::align(MdcAlignPar* alignPar){
     int iEP;
     double par[3];
     double err[3];
     double dx;
     double dy;
     double rz;
     double rLayer[] = { 120.225, 205.0, 237.55, 270.175, 302.625, 334.775, 366.65, 500.0,
			 120.225, 205.0, 237.55, 270.175, 302.625, 334.775, 366.65, 500.0 };

     TCanvas c1("c1", "c1", 10, 10, 700, 500);
     c1.SetFillColor(10);

     TF1* fResPhi = new TF1("fResPhi", funResi, 0, PI2, 3);
     fResPhi->SetParameter(0, 0.0);
     fResPhi->SetParameter(1, 0.0);
     fResPhi->SetParameter(2, 0.0);

     for(iEP=0; iEP<NEP; iEP++){
	  if((m_gr[iEP]->GetN()) > 500){
	       // align dx, dy, rz
	       m_gr[iEP]->Fit("fResPhi", "V");
	       par[0] = fResPhi->GetParameter(0);
	       par[1] = fResPhi->GetParameter(1);
	       par[2] = fResPhi->GetParameter(2);
	       err[0] = fResPhi->GetParError(0);
	       err[1] = fResPhi->GetParError(1);
	       err[2] = fResPhi->GetParError(2);

	       // align dx and rz
// 	       m_grSinPhi[iEP]->Fit("pol1");
// 	       par[0] = m_grSinPhi[iEP]->GetFunction("pol1")->GetParameter(0);
// 	       par[1] = m_grSinPhi[iEP]->GetFunction("pol1")->GetParameter(1);
// 	       par[2] = 0.0;
// 	       err[0] = m_grSinPhi[iEP]->GetFunction("pol1")->GetParError(0);
// 	       err[1] = m_grSinPhi[iEP]->GetFunction("pol1")->GetParError(1);
// 	       err[2] = 0.0;

	       // align dy
// 	       m_grCosPhi[iEP]->Fit("pol1");
// 	       par[0] = 0.0;
// 	       par[1] = 0.0;
// 	       par[2] = m_grCosPhi[iEP]->GetFunction("pol1")->GetParameter(1);
// 	       err[0] = 0.0;
// 	       err[1] = 0.0;
// 	       err[2] = m_grCosPhi[iEP]->GetFunction("pol1")->GetParError(1);

	       dx = -1.0 * par[1];
	       dy = par[2];
	       rz = par[0] / rLayer[iEP];

	       if (7==iEP || 15==iEP) {
		    dx = 0.0;
		    dy = 0.0;
		    rz = 0.0;
		    par[0] = 0.0;
		    par[1] = 0.0;
		    par[2] = 0.0;
	       }
	       alignPar->setDelDx(iEP, dx);
	       alignPar->setDelDy(iEP, dy);
	       alignPar->setDelRz(iEP, rz);

	       alignPar->setErrDx(iEP, err[1]);
	       alignPar->setErrDy(iEP, err[2]);
	       alignPar->setErrRz(iEP, err[0]/rLayer[iEP]);
	  }
     }
     renameHist();
     delete fResPhi;
}

void ResiAlign::renameHist(){
     char hname[200];
     m_hnTrk->SetName("HNtrack");
     m_hnHit->SetName("HNhit");
     m_hlayHitmap->SetName("Hitmap");
     m_hresAll->SetName("HResAllInc");
     m_hresInn->SetName("HResInnInc");
     m_hresStp->SetName("HResStpInc");
     m_hresOut->SetName("HResOutInc");
     for(int lay=0; lay<LAYERNMAX; lay++){
	  sprintf(hname, "Res_Layer%02d", lay);
	  m_hresLay[lay]->SetName(hname);
     }
     for(int iEP=0; iEP<NEP; iEP++){
	  sprintf(hname, "grResi%02d", iEP);
	  m_gr[iEP]->SetName(hname);

	  sprintf(hname, "grResi_sinPhi%02d", iEP);
	  m_grSinPhi[iEP]->SetName(hname);

	  sprintf(hname, "grResi_cosPhi%02d", iEP);
	  m_grCosPhi[iEP]->SetName(hname);
     }
}

Double_t ResiAlign::funResi(Double_t* x, Double_t* par){
     Double_t val;
     val = par[0] + par[1]*sin(x[0]) + par[2]*cos(x[0]);
     return val;
}
