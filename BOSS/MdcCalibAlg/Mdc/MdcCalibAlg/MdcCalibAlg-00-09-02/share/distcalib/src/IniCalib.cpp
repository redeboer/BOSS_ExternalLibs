#include "include/IniCalib.h"
#include "include/fun.h"
#include <cmath>

#include "TF1.h"
#include "TStyle.h"

IniCalib::IniCalib(){
     cout << "Calibration type: IniCalib" << endl;
}

IniCalib::~IniCalib(){
}

void IniCalib::init(TObjArray* hlist, MdcCosGeom* pGeom){
     m_pGeom = pGeom;
     char hname[200];
     m_fdcom = new TFolder("mCommon", "mCommon");
     hlist->Add(m_fdcom);

     m_fdTmap = new TFolder("mThitmap", "mThitmap");
     hlist->Add(m_fdTmap);

     m_fdTraw = new TFolder("mTraw", "mTraw");
     hlist->Add(m_fdTraw);

     m_fdTrawCel = new TFolder("mTrawCell", "mTrawCell");
     hlist->Add(m_fdTrawCel);

     m_fdQmap = new TFolder("mQhitmap", "mQhitmap");
     hlist->Add(m_fdQmap);

     m_fdQraw = new TFolder("mQraw", "mQraw");
     hlist->Add(m_fdQraw);

     m_fdQrawCel = new TFolder("mQrawCell", "mQrawCell");
     hlist->Add(m_fdQrawCel);

     m_hLayerHitmapT = new TH1F("mT_Hitmap_Layer", "", 43, -0.5, 42.5);
     m_fdcom->Add(m_hLayerHitmapT);
                
     m_hWireHitMapT = new TH1F("mT_Hitmap_Wire", "", 6796, -0.5, 6795.5);
     m_fdcom->Add(m_hWireHitMapT);

     m_hLayerHitmapQ = new TH1F("mQ_Hitmap_Layer", "", 43, -0.5, 42.5);
     m_fdcom->Add(m_hLayerHitmapQ);
                
     m_hWireHitMapQ = new TH1F("mQ_Hitmap_Wire", "", 6796, -0.5, 6795.5);
     m_fdcom->Add(m_hWireHitMapQ);

     m_hTesAll = new TH1F("mTesAll", "", 750, 0, 1500);
     m_fdcom->Add(m_hTesAll);

     m_hTesCal = new TH1F("mTesCal", "", 750, 0, 1500);
     m_fdcom->Add(m_hTesCal);

     m_hTesFlag = new TH1F("mTes_Flag", "", 300, -0.5, 299.5);
     m_fdcom->Add(m_hTesFlag);

     for(int lay=0; lay<NLAYER; lay++){
	  int ncel = pGeom->getLayer(lay)->getNcell();

	  sprintf(hname, "mT_hitmap_Lay%02d", lay);
	  m_hlaymapT[lay] = new TH1F(hname, "", ncel, -0.5, (float)ncel-0.5);
	  m_fdTmap -> Add(m_hlaymapT[lay]);

	  sprintf(hname, "mTDC_Lay%02d", lay);
          m_htdc[lay] = new TH1F(hname, "", 800, 0, 20000);
          m_fdTraw -> Add(m_htdc[lay]);

	  sprintf(hname, "mTraw_Lay%02d", lay);
          m_htraw[lay] = new TH1F(hname, "", 500, 0, 1000);
          m_fdTraw -> Add(m_htraw[lay]);

	  sprintf(hname, "mQ_hitmap_Lay%02d", lay);
	  m_hlaymapQ[lay] = new TH1F(hname, "", ncel, -0.5, (float)ncel-0.5);
	  m_fdQmap -> Add(m_hlaymapQ[lay]);

	  sprintf(hname, "mQraw_Lay%02d", lay);
          m_hqraw[lay] = new TH1F(hname, "", 2000, 0, 4000);
          m_fdQraw -> Add(m_hqraw[lay]);
     }

     for(int wir=0; wir<NWIRE; wir++){
	  int lay = m_pGeom -> getWire(wir) -> getLayerId();
	  int cel = m_pGeom -> getWire(wir) -> getCellId();

	  sprintf(hname, "mTraw_%02d_%03d_%04d", lay, cel, wir);
	  m_htrawCel[wir] = new TH1F(hname, "", 300, 0, 600);
	  m_fdTrawCel -> Add(m_htrawCel[wir]);

	  sprintf(hname, "mQraw_%02d_%03d_%04d", lay, cel, wir);
	  m_hqrawCel[wir] = new TH1F(hname, "", 2000, 0, 4000);
	  m_fdQrawCel -> Add(m_hqrawCel[wir]);
     }
}

void IniCalib::mergeHist(TFile* fhist){
     TFolder* fdcom = (TFolder*)fhist->Get("Common");
     TFolder* fdTmap = (TFolder*)fhist->Get("Thitmap");
     TFolder* fdTraw = (TFolder*)fhist->Get("Traw");
     TFolder* fdTrawCel = (TFolder*)fhist->Get("TrawCell");
     TFolder* fdQmap = (TFolder*)fhist->Get("Qhitmap");
     TFolder* fdQraw = (TFolder*)fhist->Get("Qraw");
     TFolder* fdQrawCel = (TFolder*)fhist->Get("QrawCell");

     char hname[200];
     TH1F* hist;
     hist = (TH1F*)fdcom->FindObjectAny("T_Hitmap_Layer");
     m_hLayerHitmapT->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("T_Hitmap_Wire");
     m_hWireHitMapT->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("Q_Hitmap_Layer");
     m_hLayerHitmapQ->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("Q_Hitmap_Wire");
     m_hWireHitMapQ->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("TesAll");
     m_hTesAll->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("TesCal");
     m_hTesCal->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("Tes_Flag");
     m_hTesFlag->Add(hist);

     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "T_hitmap_Lay%02d", lay);
	  hist = (TH1F*)fdTmap->FindObjectAny(hname);
	  m_hlaymapT[lay]->Add(hist);

	  sprintf(hname, "TDC_Lay%02d", lay);
	  hist = (TH1F*)fdTraw->FindObjectAny(hname);
	  m_htdc[lay]->Add(hist);

	  sprintf(hname, "Traw_Lay%02d", lay);
	  hist = (TH1F*)fdTraw->FindObjectAny(hname);
	  m_htraw[lay]->Add(hist);

	  sprintf(hname, "Q_hitmap_Lay%02d", lay);
	  hist = (TH1F*)fdQmap->FindObjectAny(hname);
	  m_hlaymapQ[lay]->Add(hist);

	  sprintf(hname, "Qraw_Lay%02d", lay);
	  hist = (TH1F*)fdQraw->FindObjectAny(hname);
	  m_hqraw[lay]->Add(hist);
     }

     for(int wir=0; wir<NWIRE; wir++){
	  int lay = m_pGeom -> getWire(wir) -> getLayerId();
	  int cel = m_pGeom -> getWire(wir) -> getCellId();

	  sprintf(hname, "Traw_%02d_%03d_%04d", lay, cel, wir);
	  hist = (TH1F*)fdTrawCel->FindObjectAny(hname);
	  m_htrawCel[wir]->Add(hist);

	  sprintf(hname, "Qraw_%02d_%03d_%04d", lay, cel, wir);
	  hist = (TH1F*)fdQrawCel->FindObjectAny(hname);
	  m_hqrawCel[wir]->Add(hist);
     }
}

void IniCalib::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     int lay;
     int wir;
     double t0Fit[NLAYER];
     double t0Cal[NLAYER];
     double tmax[NLAYER];
     double initT0 = gInitT0;

     int fitTminFg[NLAYER];
     int fitTmaxFg[NLAYER];
     double chisq;
     double ndf;
     double chindfTmin[NLAYER];
     double chindfTmax[NLAYER];
     char funname[200];

     // fit Tmin
     TF1* ftmin[NLAYER];
     for(lay=0; lay<NLAYER; lay++){
	  fitTminFg[lay] = 0;
	  chindfTmin[lay] = -1;
	  sprintf(funname, "ftmin%02d", lay);
	  ftmin[lay] = new TF1(funname, funTmin, 0, 150, 6);

	  if(1 == gFgCalib[lay]){
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
	       ftmin[lay] -> SetParameter(5, 1);

	       m_htraw[lay] -> Fit(funname, "Q", "", gTminFitRange[lay][0], gTminFitRange[lay][1]);
	       gStyle -> SetOptFit(11);
	       chisq = ftmin[lay]->GetChisquare();
	       ndf = ftmin[lay]->GetNDF();
	       chindfTmin[lay] = chisq / ndf;
	       if(chindfTmin[lay] < gTminFitChindf){
		    fitTminFg[lay] = 1;
		    t0Fit[lay] = ftmin[lay]->GetParameter(4);
		    t0Fit[lay] += gT0Shift;
		    t0Cal[lay] = t0Fit[lay] - gTimeShift;
	       }
	  }

	  if(0 == fitTminFg[lay]){
	       wir = m_pGeom->getWire(lay, 0)->getWireId();
	       t0Cal[lay] = calconst->getT0(wir);
	       t0Fit[lay] = t0Cal[lay] + gTimeShift;
	  }
     }

     // fit Tmax
     TF1* ftmax[NLAYER];
     for(lay=0; lay<NLAYER; lay++){
	  fitTmaxFg[lay] = 0;
	  chindfTmax[lay] = -1;
	  sprintf(funname, "ftmax%02d", lay);
	  ftmax[lay] = new TF1(funname, funTmax, 250, 500, 4);

	  if(1 == gFgCalib[lay]){
	       ftmax[lay] -> SetParameter(2, gInitTm[lay]);
	       ftmax[lay] -> SetParameter(3, 10);

	       m_htraw[lay] -> Fit(funname, "Q+", "", gTmaxFitRange[lay][0], gTmaxFitRange[lay][1]);
	       gStyle -> SetOptFit(11);
	       chisq = ftmax[lay]->GetChisquare();
	       ndf = ftmax[lay]->GetNDF();
	       chindfTmax[lay] = chisq / ndf;
	       if(chindfTmax[lay] < gTmaxFitChindf){
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
     for(lay=0; lay<NLAYER; lay++){
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
     int nwire = m_pGeom -> getWireSize();
     for(i=0; i<nwire; i++){
	  lay = m_pGeom -> getWire(i) -> getLayerId();
	  if(1 == gFgCalib[lay]){
	       calconst -> resetT0(i, t0Cal[lay]);
	       calconst -> resetDelT0(i, 0.0);
	  }
     }

     if(0 == gFgIniCalConst){
	  // set X-T
	  int lr;
	  int iEntr;
	  int ord;
	  double xtpar;
	  double xtini[8] = {0, 0.03, 0, 0, 0, 0, 999.9, 0};
	  for(lay=0; lay<NLAYER; lay++){
	       if(1 != gFgCalib[lay]) continue;

	       for(iEntr=0; iEntr<NENTRXT; iEntr++){
		    for(lr=0; lr<NLR; lr++){
			 for(ord=0; ord<NXTPAR; ord++){
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
	  for(lay=0; lay<NLAYER; lay++){
	       calconst -> resetQtpar0(lay, 0.0);
	       calconst -> resetQtpar1(lay, 0.0);
	  }

	  // set S-D
	  int bin;
	  double sdpar = 0.18;	// mm
	  for(lay=0; lay<NLAYER; lay++){
	       for(iEntr=0; iEntr<NENTRSD; iEntr++){
		    for(lr=0; lr<2; lr++){
			 for(bin=0; bin<NSDBIN; bin++){
			      calconst -> resetSdpar(lay, iEntr, lr, bin, sdpar);
			 }
		    }
	       }
	  }
     } else if(2 == gFgIniCalConst){
	  int lr;
	  int iEntr;
	  double xtpar;
	  for(lay=0; lay<NLAYER; lay++){
	       for(iEntr=0; iEntr<NENTRXT; iEntr++){
		    for(lr=0; lr<NLR; lr++){
			 xtpar = tmax[lay] - t0Fit[lay];
			 calconst -> resetXtpar(lay, iEntr, lr, 6, xtpar);
		    }
	       }
	  }
     }

     renameHist();
     for(lay=0; lay<NLAYER; lay++){
	  delete ftmin[lay];
	  delete ftmax[lay];
     }
}

Double_t IniCalib::funTmin(Double_t* x, Double_t* par){
     Double_t fitval;
     fitval = par[0] + par[1]*exp( -par[2]*(x[0]-par[3]) ) /
          ( 1 + exp( -(x[0]-par[4])/par[5] ));
     return fitval;
}

Double_t IniCalib::funTmax(Double_t* x, Double_t* par){
     Double_t fitval;
     fitval = par[0] + par[1] / (1 + exp((x[0]-par[2])/par[3]));
     return fitval;
}

void IniCalib::renameHist(){
     char hname[200];
     m_fdcom->SetName("Common");
     m_fdTmap->SetName("Thitmap");
     m_fdTraw->SetName("Traw");
     m_fdTrawCel->SetName("TrawCell");
     m_fdQmap->SetName("Qhitmap");
     m_fdQraw->SetName("Qraw");
     m_fdQrawCel->SetName("QrawCell");

     m_hLayerHitmapT->SetName("T_Hitmap_Layer");
     m_hWireHitMapT->SetName("T_Hitmap_Wire");
     m_hLayerHitmapQ->SetName("Q_Hitmap_Layer");
     m_hWireHitMapQ->SetName("Q_Hitmap_Wire");
     m_hTesAll->SetName("TesAll");
     m_hTesCal->SetName("TesCal");
     m_hTesFlag->SetName("Tes_Flag");

     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "T_hitmap_Lay%02d", lay);
	  m_hlaymapT[lay]->SetName(hname);

	  sprintf(hname, "TDC_Lay%02d", lay);
	  m_htdc[lay]->SetName(hname);

	  sprintf(hname, "Traw_Lay%02d", lay);
	  m_htraw[lay]->SetName(hname);

	  sprintf(hname, "Q_hitmap_Lay%02d", lay);
	  m_hlaymapQ[lay]->SetName(hname);

	  sprintf(hname, "Qraw_Lay%02d", lay);
	  m_hqraw[lay]->SetName(hname);
     }
     for(int wir=0; wir<NWIRE; wir++){
	  int lay = m_pGeom -> getWire(wir) -> getLayerId();
	  int cel = m_pGeom -> getWire(wir) -> getCellId();

	  sprintf(hname, "Traw_%02d_%03d_%04d", lay, cel, wir);
	  m_htrawCel[wir]->SetName(hname);

	  sprintf(hname, "Qraw_%02d_%03d_%04d", lay, cel, wir);
	  m_hqrawCel[wir]->SetName(hname);
     }
}
