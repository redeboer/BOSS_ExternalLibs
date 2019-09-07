#include "include/QtCalib.h"
#include "include/fun.h"
#include <cmath>
#include "TF1.h"

QtCalib::QtCalib(){
     cout << "Calibration type: QtCalib" << endl;
}

QtCalib::~QtCalib(){
}

void QtCalib::init(TObjArray* hlist, MdcCosGeom* pGeom){
     CalibBase::init(hlist, pGeom);
     m_pGeom = pGeom;

     char hname[200];
     for(int lay=0; lay<NLAYER; lay++){
	  m_qmin[lay] = gQmin[lay];
	  m_qmax[lay] = gQmax[lay];
	  m_qbinw[lay] = (m_qmax[lay] - m_qmin[lay]) / (double)NQBin;
     }

     m_fdQt = new TFolder("mfdQt", "fdQt");
     m_fdQ_T = new TFolder("mQtPlot", "QtPlot");
     hlist -> Add(m_fdQt);
     hlist -> Add(m_fdQ_T);

     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "mHQ_Layer%02d", lay);
	  m_hqhit[lay] = new TH1F(hname, "", 1500, 0, 3000);
	  m_fdQt -> Add(m_hqhit[lay]);

	  sprintf(hname, "mHQT_Plot_lay%02d", lay);
	  m_grqt[lay] = new TGraphErrors();
	  m_grqt[lay]->SetName(hname);
	  m_grqt[lay]->SetMarkerStyle(20);
	  m_grqt[lay]->SetMarkerColor(1);
	  m_grqt[lay]->SetLineColor(10);
	  m_fdQ_T->Add(m_grqt[lay]);

	  sprintf(hname, "mHQdelT_Plot_lay%02d", lay);
	  m_grqdt[lay] = new TGraphErrors();
	  m_grqdt[lay]->SetName(hname);
	  m_grqdt[lay]->SetMarkerStyle(10);
	  m_grqdt[lay]->SetMarkerColor(1);
	  m_grqdt[lay]->SetLineColor(10);
	  m_fdQ_T->Add(m_grqdt[lay]);
        
	  for(int bin=0; bin<NQBin; bin++){
	       sprintf(hname, "mHQT_Lay%02d_Bin%02d", lay, bin);
	       m_hqt[lay][bin] = new TH1F(hname, "", 200, -1, 1);
	       m_fdQt -> Add(m_hqt[lay][bin]);
	  }
     }
}

void QtCalib::mergeHist(TFile* fhist){
     CalibBase::mergeHist(fhist);

     char hname[200];
     TH1F* hist;
     TFolder* fdQt = (TFolder*)fhist->Get("fdQt");
     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "HQ_Layer%02d", lay);
	  hist = (TH1F*)fdQt->FindObjectAny(hname);
	  m_hqhit[lay]->Add(hist);

	  for(int bin=0; bin<NQBin; bin++){
	       sprintf(hname, "HQT_Lay%02d_Bin%02d", lay, bin);
	       hist = (TH1F*)fdQt->FindObjectAny(hname);
	       m_hqt[lay][bin]->Add(hist);
	  }
     }
}

void QtCalib::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     CalibBase::calib(calconst, newXtList, r2tList);

     double vdr = 0.03;
     Stat_t entry;
     double qtpar;
     double qbcen;
     double tw;
     double deltw;
     double qterr;
     TF1* funQt = new TF1("funQt", qtFun, 200, 2000, 2);

     ofstream fqtlog("qtlog");
     for(int lay=0; lay<NLAYER; lay++){
	  if(0 == gFgCalib[lay]) continue;

	  fqtlog << "Layer" << lay << endl;
	  double qtini[2];
	  for(int ord=0; ord<QtOrd; ord++) qtini[ord] = calconst->getQtpar(lay, ord);
	  for(int bin=0; bin<NQBin; bin++){
	       entry = m_hqt[lay][bin]->GetEntries();
	       if(entry > 300){
		    deltw = m_hqt[lay][bin] -> GetMean();
		    qterr = ( m_hqt[lay][bin]->GetRMS() ) / sqrt((double)entry);
		    deltw /= vdr;
		    qterr /= vdr;
	       } else{
		    continue;
	       }

	       qbcen = ( (double)bin + 0.5 ) * m_qbinw[lay] + m_qmin[lay];
// 	       tw = qtFun(qbcen, m_qtpar[lay]) + deltw;
// 	       tw = (m_mdcFunSvc->getTimeWalk(lay, qbcen)) + deltw;
	       tw = qtini[1] / sqrt(qbcen) + qtini[0] + deltw;

	       m_grqt[lay]->SetPoint(bin, qbcen, tw);
	       m_grqt[lay]->SetPointError(bin, 0, qterr);

	       m_grqdt[lay]->SetPoint(bin, qbcen, deltw);
	       m_grqdt[lay]->SetPointError(bin, 0, qterr);

	       fqtlog << setw(3) << bin << setw(12) << deltw << setw(12) << tw
		      << setw(12) << qbcen << setw(12) << qterr << endl;
	  }

	  m_grqt[lay]->Fit("funQt", "Q+", "", m_qmin[lay], m_qmax[lay]);

	  fqtlog << "Qtpar:  ";
	  for(int ord=0; ord<QtOrd; ord++){
	       qtpar = funQt->GetParameter(ord);
	       qterr = funQt->GetParError(ord);
	       calconst -> resetQtpar(lay, ord, qtpar);

	       fqtlog << setw(12) << qtpar << setw(12) << qterr << endl;
	  }
     } // end of layer loop
     fqtlog.close();
     renameHist();
     delete funQt;
}

Double_t QtCalib::qtFun(Double_t *x, Double_t *par){
     Double_t tw = par[1] / sqrt(x[0]) + par[0];
     return tw;
}

void QtCalib::renameHist(){
     char hname[200];
     m_fdQt->SetName("fdQt");
     m_fdQ_T->SetName("QtPlot");
     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "HQ_Layer%02d", lay);
	  m_hqhit[lay]->SetName(hname);

	  sprintf(hname, "HQT_Plot_lay%02d", lay);
	  m_grqt[lay]->SetName(hname);

	  sprintf(hname, "HQdelT_Plot_lay%02d", lay);
	  m_grqdt[lay]->SetName(hname);

	  for(int bin=0; bin<NQBin; bin++){
	       sprintf(hname, "HQT_Lay%02d_Bin%02d", lay, bin);
	       m_hqt[lay][bin]->SetName(hname);
	  }
     }
}
