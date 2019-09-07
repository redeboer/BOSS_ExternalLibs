#include "include/PreT0Calib.h"
#include "include/fun.h"
#include <cmath>
#include "TF1.h"
#include "TStyle.h"

PreT0Calib::PreT0Calib(){
     cout << "Calibration type: PreT0Calib" << endl;
     m_nzbin = 11;
}

PreT0Calib::~PreT0Calib(){
}

void PreT0Calib::init(TObjArray* hlist, MdcCosGeom* pGeom){
     m_pGeom = pGeom;
     char hname[200];

     m_fdTrec = new TFolder("mTrec", "Trec");
     hlist->Add(m_fdTrec);

     m_fdTrecZ = new TFolder("mTrecZ", "TrecZ");
     hlist->Add(m_fdTrecZ);

     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){
	       if(0 == lr) sprintf(hname, "mTrec%02d_L", lay);
	       else if(1 == lr) sprintf(hname, "mTrec%02d_R", lay);
	       else sprintf(hname, "mTrec%02d_C", lay);

	       if(lay < 8) m_hTrec[lay][lr] = new TH1F(hname, "", 100, 0, 400);
	       else m_hTrec[lay][lr] = new TH1F(hname, "", 125, 0, 500);
	       m_fdTrec -> Add(m_hTrec[lay][lr]);
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  for(int iud=0; iud<2; iud++){
	       if(0 == iud) sprintf(hname, "mTrecCosm%02d_Up", lay);
	       else sprintf(hname, "mTrecCosm%02d_Dw", lay);
	       if(lay < 8) m_hTrecCosm[lay][iud] = new TH1F(hname, "", 100, 0, 400);
	       else m_hTrecCosm[lay][iud] = new TH1F(hname, "", 125, 0, 500);
	       m_fdTrec -> Add(m_hTrecCosm[lay][iud]);
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){
	       for(int bin=0; bin<m_nzbin; bin++){
		    if(0 == lr) sprintf(hname, "mTrec%02d_z%02d_L", lay, bin);
		    else if(1 == lr) sprintf(hname, "mTrec%02d_z%02d_R", lay, bin);
		    else sprintf(hname, "mTrec%02d_z%02d_C", lay, bin);

		    if(lay < 8) m_hTrecZ[lay][lr][bin] = new TH1F(hname, "", 100, 0, 400);
		    else m_hTrecZ[lay][lr][bin] = new TH1F(hname, "", 125, 0, 500);
		    m_fdTrecZ -> Add(m_hTrecZ[lay][lr][bin]);
	       }
	  }
     }

}

void PreT0Calib::mergeHist(TFile* fhist){
     char hname[200];
     TFolder* fdTrec = (TFolder*)fhist->Get("Trec");
     TFolder* fdTrecZ = (TFolder*)fhist->Get("TrecZ");

     TH1F* hist;
     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){
	       if(0 == lr) sprintf(hname, "Trec%02d_L", lay);
	       else if(1 == lr) sprintf(hname, "Trec%02d_R", lay);
	       else sprintf(hname, "Trec%02d_C", lay);
	       hist = (TH1F*)fdTrec->FindObjectAny(hname);
	       m_hTrec[lay][lr]->Add(hist);
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  for(int iud=0; iud<2; iud++){
	       if(0 == iud) sprintf(hname, "TrecCosm%02d_Up", lay);
	       else sprintf(hname, "TrecCosm%02d_Dw", lay);
	       hist = (TH1F*)fdTrec->FindObjectAny(hname);
	       m_hTrecCosm[lay][iud]->Add(hist);
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){
	       for(int bin=0; bin<m_nzbin; bin++){
		    if(0 == lr) sprintf(hname, "Trec%02d_z%02d_L", lay, bin);
		    else if(1 == lr) sprintf(hname, "Trec%02d_z%02d_R", lay, bin);
		    else sprintf(hname, "Trec%02d_z%02d_C", lay, bin);
		    hist = (TH1F*)fdTrecZ->FindObjectAny(hname);
		    m_hTrecZ[lay][lr][bin]->Add(hist);
	       }
	  }
     }
}

void PreT0Calib::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     // fit Tmin int lay;
     double t0FitPar[NLAYER][NLR][6];
     double t0Fit[NLAYER][NLR];
     double t0Cal[NLAYER][NLR];
     double tmax[NLAYER][NLR];
     double initT0 = gInitT0;

     int fitTminFg[NLAYER][NLR];
     int fitTmaxFg[NLAYER][NLR];
     double chisq;
     double ndf;
     double chindfTmin[NLAYER][NLR];
     double chindfTmax[NLAYER][NLR];
     char funname[200];

     // add for cosmic-ray
     TF1* ftminCosm[NLAYER][2];
     double t0FitCosm[NLAYER][2];

     bool fgT0Ini = false;
     double t0ParIni[NLAYER][NLR][6];
     ifstream fparIni("fitT0_inival.txt");
     if(fparIni.is_open()){
	  string strtmp;
	  for(int lay=0; lay<NLAYER; lay++){
	       fparIni >> strtmp >> strtmp;
	       for(int i=0; i<6; i++) fparIni >> t0ParIni[lay][2][i];
	  }
	  fparIni.close();
	  fgT0Ini = true;
	  cout << "read initial values of T0 fit from fitT0_inival.txt" << endl;
     }
     if(!fgT0Ini) cout << "set initial values of T0 fit to default values" << endl;

     TF1* ftmin[NLAYER][NLR];
     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){
	       fitTminFg[lay][lr] = 0;
	       chindfTmin[lay][lr] = -1;
	       sprintf(funname, "ftmin%02d_%d", lay, lr);
	       ftmin[lay][lr] = new TF1(funname, funTmin, 0, 150, 6);
	       if(lr<2) continue;

	       if(1 == gFgCalib[lay]){
// 		    Stat_t nEntryTot = 0;
// 		    for(int ibin=1; ibin<=25; ibin++){
// 			 Stat_t entry = m_hTrec[lay][lr]->GetBinContent(ibin);
// 			 nEntryTot += entry;
// 		    }
// 		    double c0Ini = (double)nEntryTot / 25.0;
		    double c1Ini = (m_hTrec[lay][lr]->GetMaximum());

		    if(fgT0Ini){
			 for(int i=0; i<6; i++){
			      if(fabs(t0ParIni[lay][2][i] + 9999)<0.01) continue;
			      ftmin[lay][lr] -> SetParameter(i, t0ParIni[lay][2][i]);
			 }
		    } else{
			 ftmin[lay][lr] -> SetParameter(0, 0);
			 ftmin[lay][lr] -> SetParameter(1, c1Ini);
			 ftmin[lay][lr] -> SetParameter(2, 0);
			 ftmin[lay][lr] -> SetParameter(4, initT0);
			 if(lay<4) ftmin[lay][lr] -> SetParameter(5, 4);
			 else ftmin[lay][lr] -> SetParameter(5, 1.5);
		    }

		    if(lay<4) m_hTrec[lay][lr] -> Fit(funname, "Q", "", 0, 140);
		    else m_hTrec[lay][lr] -> Fit(funname, "Q", "", gTminFitRange[lay][0], gTminFitRange[lay][1]);
		    gStyle -> SetOptFit(11);
		    chisq = ftmin[lay][lr]->GetChisquare();
		    ndf = ftmin[lay][lr]->GetNDF();
		    chindfTmin[lay][lr] = chisq / ndf;

// 		    if(chindfTmin[lay][lr] > 100){
// 			 ftmin[lay][lr] -> SetParameter(1, c1Ini+2000);
// 			 m_hTrec[lay][lr] -> Fit(funname, "Q", "", gTminFitRange[lay][0], gTminFitRange[lay][1]);
// 			 chisq = ftmin[lay][lr]->GetChisquare();
// 			 ndf = ftmin[lay][lr]->GetNDF();
// 			 chindfTmin[lay][lr] = chisq / ndf;
// 		    }

		    if(chindfTmin[lay][lr] < gTminFitChindf){
			 fitTminFg[lay][lr] = 1;
			 t0Fit[lay][lr] = ftmin[lay][lr]->GetParameter(4);
			 t0Fit[lay][lr] += gT0Shift;
			 t0Cal[lay][lr] = t0Fit[lay][lr] - gTimeShift;
			 for(int i=0; i<6; i++) t0FitPar[lay][lr][i] = ftmin[lay][lr]->GetParameter(i);
		    }
	       }

	       if(0 == fitTminFg[lay][lr]){
		    int wir = m_pGeom->getWire(lay, 0)->getWireId();
		    t0Cal[lay][lr] = calconst->getT0(wir);
		    t0Fit[lay][lr] = t0Cal[lay][lr] + gTimeShift;
	       }
	  }

	  for(int iud=0; iud<2; iud++){
	       sprintf(funname, "ftminCosm_%02d_%d", lay, iud);
	       ftminCosm[lay][iud] = new TF1(funname, funTmin, 0, 150, 6);
	       ftminCosm[lay][iud] -> SetParameter(0, 0);
	       ftminCosm[lay][iud] -> SetParameter(4, initT0);
	       ftminCosm[lay][iud] -> SetParameter(5, 1);
	       m_hTrecCosm[lay][iud] -> Fit(funname, "Q", "", gTminFitRange[lay][0], gTminFitRange[lay][1]);
	       gStyle -> SetOptFit(11);
	       t0FitCosm[lay][iud] += gT0Shift;
	       t0FitCosm[lay][iud] = ftminCosm[lay][iud]->GetParameter(4);
	  }
     }

     // fit Tmax
     TF1* ftmax[NLAYER][NLR];
     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){
	       fitTmaxFg[lay][lr] = 0;
	       chindfTmax[lay][lr] = -1;
	       sprintf(funname, "ftmax%02d_%d", lay, lr);
	       ftmax[lay][lr] = new TF1(funname, funTmax, 250, 500, 4);

	       if(1 == gFgCalib[lay]){
		    ftmax[lay][lr] -> SetParameter(2, gInitTm[lay]);
		    ftmax[lay][lr] -> SetParameter(3, 10);
		    m_hTrec[lay][lr] -> Fit(funname, "Q+", "", gTmaxFitRange[lay][0], gTmaxFitRange[lay][1]);
		    gStyle -> SetOptFit(11);
		    chisq = ftmax[lay][lr]->GetChisquare();
		    ndf = ftmax[lay][lr]->GetNDF();
		    chindfTmax[lay][lr] = chisq / ndf;
		    if(chindfTmax[lay][lr] < gTmaxFitChindf){
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
     for(int lay=0; lay<NLAYER; lay++){
	  ft0 << setw(5) << lay << setw(3) << fitTminFg[lay][2]
	      << setw(15) << t0Cal[lay][2] << setw(15) << t0Fit[lay][2]
	      << setw(15) << chindfTmin[lay][2] << endl;
     }
     ft0 << endl;
     for(int lay=0; lay<NLAYER; lay++){
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
     ft0 << endl;
     for(int lay=0; lay<NLAYER; lay++){
	  ft0 << setw(5) << lay << setw(15) << chindfTmin[lay][2];
	  for(int i=0; i<6; i++) ft0 << setw(15) << t0FitPar[lay][2][i];
	  ft0 << endl;
     }
     ft0.close();
     cout << "preT0.dat was written." << endl;

     // output for cosmic T0
     ofstream ft0cosm("cosmicT0.dat");
     for(int lay=0; lay<NLAYER; lay++){
	  ft0cosm << setw(5) << lay << setw(15) << t0Fit[lay][2]
		  << setw(15) << t0FitCosm[lay][0] << setw(15) << t0FitCosm[lay][1] << endl;
     }
     ft0cosm.close();

     // set T0
     for(int i=0; i<NWIRE; i++){
	  int lay = m_pGeom -> getWire(i) -> getLayerId();
	  if(1 == gFgCalib[lay]){
	       calconst -> resetT0(i, t0Cal[lay][2]);
	       calconst -> resetDelT0(i, 0.0);
	  }
     }

     // set tm of X-T
     if(gPreT0SetTm){
	  double tm;
	  for(int lay=0; lay<NLAYER; lay++){
	       if(1 != gFgCalib[lay]) continue;

	       for(int iEntr=0; iEntr<NENTRXT; iEntr++){
		    for(int lr=0; lr<NLR; lr++){
			 tm = tmax[lay][lr] - t0Fit[lay][2];
			 if( (tmax[lay][lr] > gTmaxFitRange[lay][0]) &&
			     (tmax[lay][lr] < gTmaxFitRange[lay][1]) ){
			      calconst -> resetXtpar(lay, iEntr, lr, 6, tm);
			 }
		    }
	       }
	  }
     }

     renameHist();
     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){     
	       delete ftmin[lay][lr];
	       delete ftmax[lay][lr];
	  }
     }
}

void PreT0Calib::renameHist(){
     char hname[200];
     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){
	       if(0 == lr) sprintf(hname, "Trec%02d_L", lay);
	       else if(1 == lr) sprintf(hname, "Trec%02d_R", lay);
	       else sprintf(hname, "Trec%02d_C", lay);
	       m_hTrec[lay][lr]->SetName(hname);
	  }
     }
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iud=0; iud<2; iud++){
	       if(0 == iud) sprintf(hname, "TrecCosm%02d_Up", lay);
	       else sprintf(hname, "TrecCosm%02d_Dw", lay);
	       m_hTrecCosm[lay][iud]->SetName(hname);
	  }
     }
     for(int lay=0; lay<NLAYER; lay++){
	  for(int lr=0; lr<NLR; lr++){
	       for(int bin=0; bin<m_nzbin; bin++){
		    if(0 == lr) sprintf(hname, "Trec%02d_z%02d_L", lay, bin);
		    else if(1 == lr) sprintf(hname, "Trec%02d_z%02d_R", lay, bin);
		    else sprintf(hname, "Trec%02d_z%02d_C", lay, bin);
		    m_hTrecZ[lay][lr][bin]->SetName(hname);
	       }
	  }
     }
}

Double_t PreT0Calib::funTmin(Double_t* x, Double_t* par){
     Double_t fitval;
     fitval = par[0] + par[1]*exp( -par[2]*(x[0]-par[3]) ) /
          ( 1 + exp( -(x[0]-par[4])/par[5] ));
     return fitval;
}

Double_t PreT0Calib::funTmax(Double_t* x, Double_t* par){
     Double_t fitval;
     fitval = par[0] + par[1] / (1 + exp((x[0]-par[2])/par[3]));
     return fitval;
}
