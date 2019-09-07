#include "include/XtInteCalib.h"
#include "include/fun.h"

#include <cmath>

#include "TMinuit.h"
#include "TF1.h"
#include "TTree.h"

XtInteCalib::XtInteCalib(){
     m_nMaxGrPoint = 5000;
     for(int lay=0; lay<NLAYER; lay++){
	  m_tbinWid[lay][0] = 5.0;
	  m_tbinWid[lay][1] = 10.0;
	  m_tbinWid[lay][2] = 20.0;

	  m_tbinLim[lay][0] = -10.0;
	  m_tbinLim[lay][1] = 30.0;
	  if(lay < 8) m_tbinLim[lay][2] = 210.0;
	  else m_tbinLim[lay][2] = 350.0;
	  m_tbinLim[lay][3] = 990.0;
     }
     cout << "Calibration type: XtInteCalib" << endl;
}

XtInteCalib::~XtInteCalib(){
}

void XtInteCalib::init(TObjArray* hlist, MdcCosGeom* pGeom){
     CalibBase::init(hlist, pGeom);

     m_fdPf = new TFolder("mfdProfile", "fdProfile");
     hlist -> Add(m_fdPf);

     m_haxis = new TH2F("axis", "", 200, -50, 1000, 50, 0, 10);
     m_haxis -> SetStats(0);
     m_fdPf -> Add(m_haxis);

     char hname[200];
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
                    sprintf(hname, "mxt%02d_%02d_%d_gr", lay, iEntr, lr);
		    m_grXt[lay][iEntr][lr] = new TGraph();
		    m_grXt[lay][iEntr][lr]->SetName(hname);
		    m_grXt[lay][iEntr][lr]->SetMarkerColor(2);
		    m_fdPf->Add(m_grXt[lay][iEntr][lr]);

		    int xbinN = (int)((m_tbinLim[lay][1] - m_tbinLim[lay][0])/m_tbinWid[lay][0] + 0.5);
		    sprintf(hname, "mxt%02d_%02d_%d_near", lay, iEntr, lr);
		    m_pfNear[lay][iEntr][lr] = new TProfile(hname, hname, xbinN, m_tbinLim[lay][0], m_tbinLim[lay][1]);
		    m_fdPf->Add(m_pfNear[lay][iEntr][lr]);

		    int xbinM = (int)((m_tbinLim[lay][2] - m_tbinLim[lay][1])/m_tbinWid[lay][1] + 0.5);
		    sprintf(hname, "mxt%02d_%02d_%d_mid", lay, iEntr, lr);
		    m_pfMid[lay][iEntr][lr] = new TProfile(hname, hname, xbinM, m_tbinLim[lay][1], m_tbinLim[lay][2]);
		    m_fdPf->Add(m_pfMid[lay][iEntr][lr]);

		    int xbinF = (int)((m_tbinLim[lay][3] - m_tbinLim[lay][2])/m_tbinWid[lay][2] + 0.5);
		    sprintf(hname, "mxt%02d_%02d_%d_far", lay, iEntr, lr);
		    m_pfFar[lay][iEntr][lr] = new TProfile(hname, hname, xbinF, m_tbinLim[lay][2], m_tbinLim[lay][3]);
		    m_fdPf->Add(m_pfFar[lay][iEntr][lr]);
	       }
	  }
     }
}

void XtInteCalib::mergeHist(TFile* fhist){
     CalibBase::mergeHist(fhist);

     double tdr, doca;
     char hname[200];
     TProfile* pr;
     TFolder* fd = (TFolder*)fhist->Get("fdProfile");
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    if((m_grXt[lay][iEntr][lr]->GetN()) < m_nMaxGrPoint){
			 sprintf(hname, "xt%02d_%02d_%d_gr", lay, iEntr, lr);
			 TGraph* gr = (TGraph*)fd->FindObjectAny(hname);
			 int nPoint = gr->GetN();
			 for(int i=0; i<nPoint; i++){
			      gr->GetPoint(i, tdr, doca);
			      int np = m_grXt[lay][iEntr][lr]->GetN();
			      m_grXt[lay][iEntr][lr]->SetPoint(np, tdr, doca);
			 }
		    }
		    sprintf(hname, "xt%02d_%02d_%d_near", lay, iEntr, lr);
		    pr = (TProfile*)fd->FindObjectAny(hname);
		    m_pfNear[lay][iEntr][lr]->Add(pr);

		    sprintf(hname, "xt%02d_%02d_%d_mid", lay, iEntr, lr);
		    pr = (TProfile*)fd->FindObjectAny(hname);
		    m_pfMid[lay][iEntr][lr]->Add(pr);

		    sprintf(hname, "xt%02d_%02d_%d_far", lay, iEntr, lr);
		    pr = (TProfile*)fd->FindObjectAny(hname);
		    m_pfFar[lay][iEntr][lr]->Add(pr);
	       }
	  }
     }
}

void XtInteCalib::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     CalibBase::calib(calconst, newXtList, r2tList);
     bool fgOldXt = saveOldXt(newXtList);
     newXtList->Clear();

     TGraph* grXtfit[NLAYER][NENTRXT][2];
     char hname[200];
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    m_vt.clear();
		    m_vd.clear();
		    m_entries.clear();
		    for(int iPf=0; iPf<3; iPf++){
			 TProfile* pro;
			 if(0==iPf) pro = m_pfNear[lay][iEntr][lr];
			 else if(1==iPf) pro = m_pfMid[lay][iEntr][lr];
			 else pro = m_pfFar[lay][iEntr][lr];

			 int nbin = pro->GetNbinsX();
			 for(int i=0; i<nbin; i++){
			      double tt = pro->GetBinCenter(i+1);
			      double dd = pro->GetBinContent(i+1);
			      double entries = pro->GetBinEntries(i+1);
			      if(entries > 10){
				   m_vt.push_back(tt);
				   m_vd.push_back(dd);
				   m_entries.push_back(entries);
			      }
			 }
		    }
		    unsigned vsize = m_vt.size();
		    if(vsize > 10){
			 for(int i=0; i<2; i++){
			      double slope = (m_vd[vsize-1]-m_vd[vsize-2])/(m_vt[vsize-1]-m_vt[vsize-2]);
			      if(fabs(slope)>0.04){ // 0.8mm/20ns
				   m_vt.pop_back();
				   m_vd.pop_back();
				   m_entries.pop_back();
				   vsize = m_vt.size();
			      }
			 }
		    }
		    sprintf(hname, "grXtFit%02d_%02d_%d", lay, iEntr, lr);
		    grXtfit[lay][iEntr][lr] = new TGraph();
		    grXtfit[lay][iEntr][lr]->SetName(hname);
		    grXtfit[lay][iEntr][lr]->SetMarkerStyle(20);
		    m_fgFit[lay][iEntr][lr] = getXt(lay, iEntr, lr, grXtfit[lay][iEntr][lr]);
	       }
	  }
     }

     double tdr, doca;
     for(int lay=0; lay<NLAYER; lay++){
	  double tCut = 500.0;
	  if(lay<8) tCut = 400.0;
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    if(!m_fgFit[lay][iEntr][lr]){
			 int iEntrNew = findXtEntr(lay, iEntr, lr);
			 if(-1 != iEntrNew){
			      int npoint = grXtfit[lay][iEntrNew][lr]->GetN();
			      for(int i=0; i<npoint; i++){
				   grXtfit[lay][iEntrNew][lr]->GetPoint(i, tdr, doca);
				   grXtfit[lay][iEntr][lr]->SetPoint(i, tdr, doca);
			      }
			 } else if(fgOldXt) {
			      cout << grXtfit[lay][iEntr][lr]->GetName() << " use old x-t" << endl;
			      int npoint = m_grXtOld[lay][iEntr][lr]->GetN();
			      for(int i=0; i<npoint; i++){
				   m_grXtOld[lay][iEntr][lr]->GetPoint(i, tdr, doca);
				   grXtfit[lay][iEntr][lr]->SetPoint(i, tdr, doca);
			      }
			 }
		    }
		    int nn = grXtfit[lay][iEntr][lr]->GetN();
		    double tmax, dmax;
		    grXtfit[lay][iEntr][lr]->GetPoint(nn-1, tmax, dmax);
		    if(tmax > tCut) m_fgEdge[lay][iEntr][lr] = true;
		    else m_fgEdge[lay][iEntr][lr] = false;
	       }
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    if(!m_fgEdge[lay][iEntr][lr]){
			 int iEntrNew = findXtEntrEdge(lay, iEntr, lr);
			 if(-1 != iEntrNew){
			      double t1, d1;
			      int n1 = grXtfit[lay][iEntr][lr]->GetN();
			      grXtfit[lay][iEntr][lr]->GetPoint(n1-1, t1, d1);
			      double t2, d2;
			      int n2 = grXtfit[lay][iEntrNew][lr]->GetN();
			      for(int i=0; i<n2; i++){
				   grXtfit[lay][iEntrNew][lr]->GetPoint(i, t2, d2);
				   if(t2 > t1){
					grXtfit[lay][iEntr][lr]->SetPoint(n1, t2, d2);
					n1++;
				   }
			      }
			 }
		    }
	       }
	  }
     }

     TTree* xttr[NLAYER][NENTRXT][2];
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    sprintf(hname, "trNewXt%02d_%02d_%d", lay, iEntr, lr);
		    xttr[lay][iEntr][lr] = new TTree(hname, hname);
		    xttr[lay][iEntr][lr]->Branch("t", &tdr, "t/D");
		    xttr[lay][iEntr][lr]->Branch("d", &doca, "d/D");
		    if(0 == gFgCalib[lay]){
			 int npoint = m_grXtOld[lay][iEntr][lr]->GetN();
			 for(int i=0; i<npoint; i++){
			      m_grXtOld[lay][iEntr][lr]->GetPoint(i, tdr, doca);
			      xttr[lay][iEntr][lr]->Fill();
			 }
		    } else{
			 int npoint = grXtfit[lay][iEntr][lr]->GetN();
			 for(int i=0; i<npoint; i++){
			      grXtfit[lay][iEntr][lr]->GetPoint(i, tdr, doca);
			      xttr[lay][iEntr][lr]->Fill();
			 }
		    }
		    newXtList->Add(xttr[lay][iEntr][lr]);
	       }
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    delete grXtfit[lay][iEntr][lr];
	       }
	  }
     }

//      fnewXt.Close();
     renameHist();
}

void XtInteCalib::renameHist(){
     char hname[200];
     m_fdPf->SetName("fdProfile");
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    sprintf(hname, "xt%02d_%02d_%d_gr", lay, iEntr, lr);
		    m_grXt[lay][iEntr][lr] -> SetName(hname);
		    sprintf(hname, "xt%02d_%02d_%d_near", lay, iEntr, lr);
		    m_pfNear[lay][iEntr][lr] -> SetName(hname);
		    sprintf(hname, "xt%02d_%02d_%d_mid", lay, iEntr, lr);
		    m_pfMid[lay][iEntr][lr] -> SetName(hname);
		    sprintf(hname, "xt%02d_%02d_%d_far", lay, iEntr, lr);
		    m_pfFar[lay][iEntr][lr] -> SetName(hname);
	       }
	  }
     }
}

bool XtInteCalib::saveOldXt(TObjArray* newXtList){
     char hname[200];
     Int_t entries = newXtList->GetEntries();
     cout << "entries of newXtList " << entries << endl;
     if(entries < 1548){
	  cout << "can not get old x-t" << endl;
	  return false;
     }
     for(int lay=0; lay<NLAYER; lay++){
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    sprintf(hname, "grXtOld%02d_%02d_%d", lay, iEntr, lr);
		    m_grXtOld[lay][iEntr][lr] = new TGraph();
		    m_grXtOld[lay][iEntr][lr]->SetName(hname);

		    double t, d;
		    sprintf(hname, "trNewXt%02d_%02d_%d", lay, iEntr, lr);
		    TTree* tr = (TTree*)(newXtList->FindObject(hname));
// 		    cout << setw(15) << tr->GetName() << setw(15) << tr->GetEntries() << endl;
		    tr -> SetBranchAddress("t", &t);
		    tr -> SetBranchAddress("d", &d);
		    int nPoint = tr -> GetEntries();
		    if(nPoint < 20){
			 cout << "can not get old x-t: " << hname << endl;
			 return false;
		    }
		    for(int i=0; i<nPoint; i++){
			 tr->GetEntry(i);
			 m_grXtOld[lay][iEntr][lr]->SetPoint(i, t, d);
		    }
	       }
	  }
     }
//      for(int lay=0; lay<NLAYER; lay++){
// 	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
// 	       for(int lr=0; lr<2; lr++){
// 		    sprintf(hname, "trNewXt%02d_%02d_%d", lay, iEntr, lr);
// 		    delete (newXtList->FindObject(hname));
// 	       }
// 	  }
//      }
     return true;
}

bool XtInteCalib::getXt(int lay, int iEntr, int lr, TGraph* gr){
     unsigned vsize = m_vt.size();
     if(vsize < 15) return false;

     double tmax = m_vt[vsize-1];
     double tm0 = 300.0;	// 280->300
     double tm1 = 500.0;
     double tm2 = 700.0;
     if(lay<8){
	  tm0 = 200.0;		// 180->200
	  tm1 = 300.0;
	  tm2 = 540.0;
     }
     int n0 = 0;
     for(unsigned i=0; i<vsize; i++){
	  if(m_vt[i] < tm0) n0++;
     }

     int nCut = 30;		// 25
     if(lay<8) nCut = 20;	// 15

     double entries1 = 0.0;
     double entries2 = 0.0;
     for(unsigned i=0; i<vsize; i++){
	  entries1 += m_entries[i];
	  if(m_vt[i] < 150.) entries2 += m_entries[i];
     }
     if((entries1*0.9) < entries2) return false;

     if(n0 < nCut) return false;
     if(tmax < tm1) return funXt0(lay,iEntr,lr, gr);
     else return funXt1(lay,iEntr,lr, gr);
}

bool XtInteCalib::funXt0(int lay, int iEntr, int lr, TGraph* gr){
     double tCut = 300.0;
     if(lay<8) tCut = 200.0;

     int npoint = 0;
     if(m_vt[0] > 0.0){
	  gr->SetPoint(0, 0, 0);
	  npoint++;
     }
     for(unsigned i=0; i<m_vt.size(); i++){
	  double delt = 0.0;
	  if(i>10) delt = m_vt[i] - m_vt[i-1];
	  if((i>10) && ((delt>100.) || ((delt>60.) && (m_vt[i-1]<tCut)))) break;

	  if(m_vt[i] < 0.0) gr->SetPoint(npoint, m_vt[i], 0.0);
	  else gr->SetPoint(npoint, m_vt[i], m_vd[i]);
	  npoint++;
     }
     return true;
}

bool XtInteCalib::funXt1(int lay, int iEntr, int lr, TGraph* gr){
     unsigned vsize = m_vt.size();
     double tmax = m_vt[vsize-1];
     double t1;
     if(lay<8){
	  if(tmax<540) t1 = 300.;
	  else t1 = 540.;
     }else{
	  if(tmax<700) t1 = 500.;
	  else t1 = 660.;
     }

     bool fgfit = false;
     int np = 0;
     double c0 = 0.0;
     double c1 = 0.0;
     TGraph* grPol1 = new TGraph();
     for(unsigned i=0; i<vsize; i++){
	  if(m_vt[i] >= t1){
	       grPol1->SetPoint(np, m_vt[i], m_vd[i]);
	       np++;
	  }
     }
     double t2;
     if(np<5){
	  t2 = t1;
     } else{
	  double x2;
	  grPol1->GetPoint(0,t2,x2);
	  grPol1->Fit("pol1","Q","",t2, m_vt[vsize-1]);
	  c0 = grPol1->GetFunction("pol1")->GetParameter(0);
	  c1 = grPol1->GetFunction("pol1")->GetParameter(1);

	  if(c1<0){
	       grPol1->Fit("pol0","Q","",t2, m_vt[vsize-1]);
	       c0 = grPol1->GetFunction("pol0")->GetParameter(0);
	       c1 = 0.0;
	  }
	  fgfit = true;
     }

     double tCut = 300.0;
     if(lay<8) tCut = 200.0;

     int npoint = 0;
     if(m_vt[0] > 0.0){
	  gr->SetPoint(0, 0, 0);
	  npoint++;
     }
     for(unsigned i=0; i<vsize; i++){
	  double delt = 0.0;
	  if(i>10) delt = m_vt[i] - m_vt[i-1];
	  if((i>10) && ((delt>100.) || ((delt>60.) && (m_vt[i-1]<tCut)))) break;

	  if(m_vt[i] < t2){
	       if(m_vt[i] < 0.0) gr->SetPoint(npoint, m_vt[i], 0.0);
	       else gr->SetPoint(npoint, m_vt[i], m_vd[i]);
	       npoint++;
	  } else{
	       double dist;
	       if(!fgfit) dist = m_vd[i];
	       else dist = c1*m_vt[i] + c0;
	       gr->SetPoint(npoint, m_vt[i], dist);
	       npoint++;
	  }
     }
     return true;
}


int XtInteCalib::findXtEntr(int lay, int iEntr, int lr) const {
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
	       if(-1 != id) entrId = id;
	       else{
		    for(int i=id1; i<=idmax; i++){
			 if(m_fgFit[lay][i][lr]){
			      id = i;
			      break;
			 }
		    }
		    entrId = id;
	       }
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
	       if(-1 != id) entrId = id;
	       else{
		    for(int i=id1; i>=0; i--){
			 if(m_fgFit[lay][i][lr]){
			      id = i;
			      break;
			 }
		    }
		    entrId = id;
	       }
	  }
     }
     if(-1 == entrId){
	  cout << "find EntrId error " << "layer " << lay << "  iEntr " << iEntr << "  lr " << lr << endl;
     }

     return entrId;
}

int XtInteCalib::findXtEntrEdge(int lay, int iEntr, int lr) const {
     int id0 = 8;
     int id1 = 9;
     int idmax = 17;
     int entrId = -1;
     if(iEntr <= id0){
	  int id = -1;
	  for(int i=iEntr; i<=id0; i++){
	       if(m_fgEdge[lay][i][lr]){
		    id = i;
		    break;
	       }
	  }
	  if(-1 != id) entrId = id;
	  else{
	       for(int i=iEntr; i>=0; i--){
		    if(m_fgEdge[lay][i][lr]){
			 id = i;
			 break;
		    }
	       }
	       if(-1 != id) entrId = id;
	       else{
		    for(int i=id1; i<=idmax; i++){
			 if(m_fgEdge[lay][i][lr]){
			      id = i;
			      break;
			 }
		    }
		    entrId = id;
	       }
	  }
     } else{
	  int id = -1;
	  for(int i=iEntr; i>=id1; i--){
	       if(m_fgEdge[lay][i][lr]){
		    id = i;
		    break;
	       }
	  }
	  if(-1 != id) entrId = id;
	  else{
	       for(int i=iEntr; i<idmax; i++){
		    if(m_fgEdge[lay][i][lr]){
			 id = i;
			 break;
		    }
	       }
	       if(-1 != id) entrId = id;
	       else{
		    for(int i=id1; i>=0; i--){
			 if(m_fgEdge[lay][i][lr]){
			      id = i;
			      break;
			 }
		    }
		    entrId = id;
	       }
	  }
     }
     if(-1 == entrId){
	  cout << "find EntrId error for cell edge " << "layer " << lay << "  iEntr " << iEntr << "  lr " << lr << endl;
     }

     return entrId;
}
