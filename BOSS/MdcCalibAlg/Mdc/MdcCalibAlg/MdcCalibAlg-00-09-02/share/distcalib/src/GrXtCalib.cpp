#include "include/GrXtCalib.h"
#include "include/fun.h"

#include <cmath>

#include "TMinuit.h"
#include "TF1.h"

GrXtCalib::GrXtCalib(){
     m_maxNhit = 5000;
     m_nMaxEd = 1000;
     for(int lay=0; lay<NLAYER; lay++){
	  if(lay<8) m_tEd[lay] = 200.0;
	  else m_tEd[lay] = 300.0;
     }
     cout << "Calibration type: GrXtCalib" << endl;
}

GrXtCalib::~GrXtCalib(){
}

void GrXtCalib::init(TObjArray* hlist, MdcCosGeom* pGeom){
     CalibBase::init(hlist, pGeom);

     m_fdXt = new TFolder("mfdxt","fdxt");
     hlist->Add(m_fdXt);

     m_haxis = new TH2F("axis", "", 50, 0, 300, 50, 0, 9);
     m_haxis -> SetStats(0);
     m_fdXt -> Add(m_haxis);

     char hname[200];
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<NLR; lr++){
		    m_nhitIn[lay][iEntr][lr] = 0;
		    m_nhitEd[lay][iEntr][lr] = 0;

		    sprintf(hname, "mgrXt%02d_%02d_lr%01d", lay, iEntr, lr);
		    m_grxt[lay][iEntr][lr] = new TGraph();
		    m_grxt[lay][iEntr][lr] -> SetName(hname);
		    m_grxt[lay][iEntr][lr] -> SetMarkerStyle(10);
		    m_grxt[lay][iEntr][lr] -> SetLineColor(10);
		    m_fdXt -> Add(m_grxt[lay][iEntr][lr]);
	       }
	  }
     }
}

void GrXtCalib::mergeHist(TFile* fhist){
     CalibBase::mergeHist(fhist);

     double tdr, doca;
     char hname[200];
     TFolder* fd = (TFolder*)fhist->Get("fdXtGr");
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<NLR; lr++){
		    if((m_nhitIn[lay][iEntr][lr] > m_maxNhit) && (m_nhitEd[lay][iEntr][lr] > m_nMaxEd)) continue;

		    sprintf(hname, "grXt%02d_%02d_lr%01d", lay, iEntr, lr);
		    TGraph* gr = (TGraph*)fd->FindObjectAny(hname);
		    int nPoint = gr->GetN();
		    for(int i=0; i<nPoint; i++){
			 gr->GetPoint(i, tdr, doca);
			 if((tdr < m_tEd[lay]) && (m_nhitIn[lay][iEntr][lr] <= m_maxNhit)){
			      int np = m_grxt[lay][iEntr][lr]->GetN();
			      m_grxt[lay][iEntr][lr]->SetPoint(np, tdr, doca);
			      m_nhitIn[lay][iEntr][lr]++;
			 } else if((tdr >= m_tEd[lay]) && (m_nhitEd[lay][iEntr][lr] <= m_nMaxEd)){
			      int np = m_grxt[lay][iEntr][lr]->GetN();
			      m_grxt[lay][iEntr][lr]->SetPoint(np, tdr, doca);
			      m_nhitEd[lay][iEntr][lr]++;
			 }
		    }
	       }
	  }
     }
}

void GrXtCalib::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     CalibBase::calib(calconst, newXtList, r2tList);

     int ord;
     double xtpar[NLAYER][NENTRXT][NLR][8];
     TF1* fxtDr = new TF1("fxtDr", xtFitFun, 0, 300, 6);
     TF1* fxtEd = new TF1("fxtEd", xtFitEdge, 150, 500, 1);
     if(1 == gfixXtC0) fxtDr -> FixParameter(0, 0);

     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<NLR; lr++){
		    m_fgFit[lay][iEntr][lr] = false;
		    if(0 == gFgCalib[lay]) continue;

		    if(m_nhitIn[lay][iEntr][lr] > 1000){
			 Tmax = calconst -> getXtpar(lay, iEntr, lr, 6);

			 m_grxt[lay][iEntr][lr] -> Fit("fxtDr", "Q+", "", 0, Tmax);
			 for(ord=0; ord<6; ord++){
			      xtpar[lay][iEntr][lr][ord] = fxtDr->GetParameter(ord);
			 }
			 xtpar[lay][iEntr][lr][6] = Tmax;

			 Dmax = 0.0;
			 for(ord=0; ord<6; ord++) Dmax += xtpar[lay][iEntr][lr][ord] * pow(Tmax, ord);

			 if(m_nhitEd[lay][iEntr][lr] > 300){
			      m_grxt[lay][iEntr][lr] -> Fit("fxtEd", "Q+", "", Tmax, Tmax+300);
			      xtpar[lay][iEntr][lr][7] = fxtEd->GetParameter(0);
			      if(xtpar[lay][iEntr][lr][7] < 0.0) xtpar[lay][iEntr][lr][7] = 0.0;
			 } else{
			      xtpar[lay][iEntr][lr][7] = 0.0;
			 }

			 m_fgFit[lay][iEntr][lr] = true;
		    }

	       } // end of lr loop
	  } // end of entrance angle loop
     } // end of layer loop

     ofstream fxtlog("xtlog");
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<NLR; lr++){
		    fxtlog << setw(3) << lay << setw(3) << iEntr << setw(3) << lr;

		    int fgUpdate = -1;
		    if(m_fgFit[lay][iEntr][lr]){
			 fgUpdate = 1;
			 for(ord=0; ord<8; ord++) calconst->resetXtpar(lay, iEntr, lr, ord, xtpar[lay][iEntr][lr][ord]);
		    } else{
			 int iEntrNew = findXtEntr(lay, iEntr, lr);
			 if(-1 != iEntrNew){
			      fgUpdate = 2;
			      for(ord=0; ord<8; ord++){
				   calconst->resetXtpar(lay, iEntr, lr, ord, xtpar[lay][iEntrNew][lr][ord]);
			      }
			 }
		    }
		    fxtlog << setw(3) << fgUpdate;
		    for(ord=0; ord<8; ord++){
			 double par = calconst -> getXtpar(lay, iEntr, lr, ord);
			 if(6==ord) fxtlog << setw(9) << par;
			 else fxtlog << setw(14) << par;
		    }
		    fxtlog << endl;
	       }
	  }
     }
     fxtlog.close();

     cout << "Xt update finished. File xtlog was written." << endl;

     renameHist();
     delete fxtDr;
     delete fxtEd;
}

void GrXtCalib::renameHist(){
     char hname[200];
     m_fdXt->SetName("fdXtGr");
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<NLR; lr++){
		    sprintf(hname, "grXt%02d_%02d_lr%01d", lay, iEntr, lr);
		    m_grxt[lay][iEntr][lr] -> SetName(hname);
	       }
	  }
     }
}

int GrXtCalib::findXtEntr(int lay, int iEntr, int lr) const {
     int id0 = 8;
     int id1 = 9;
     int idmax = 17;
     int entrId = -1;
     if(iEntr <= id0){
	  int id = -1;
	  for(int i=iEntr; i<=id0; i++){
	       if(m_fgFit[lay][i][lr]){
		    id = i;
		    break;
	       }
	  }
	  if(-1 != id) entrId = id;
	  else{
	       for(int i=iEntr; i>=0; i--){
		    if(m_fgFit[lay][i][lr]){
			 id = i;
			 break;
		    }
	       }
	       entrId = id;
	  }
     } else{
	  int id = -1;
	  for(int i=iEntr; i>=id1; i--){
	       if(m_fgFit[lay][i][lr]){
		    id = i;
		    break;
	       }
	  }
	  if(-1 != id) entrId = id;
	  else{
	       for(int i=iEntr; i<idmax; i++){
		    if(m_fgFit[lay][i][lr]){
			 id = i;
			 break;
		    }
	       }
	       entrId = id;
	  }
     }
     if(-1 == entrId){
	  cout << "find EntrId error " << "layer " << lay << "  iEntr " << iEntr << "  lr " << lr << endl;
     }

     return entrId;
}
