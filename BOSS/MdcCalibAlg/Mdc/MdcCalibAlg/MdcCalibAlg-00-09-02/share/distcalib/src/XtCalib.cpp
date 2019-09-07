#include "include/XtCalib.h"
#include "include/fun.h"

#include "TMinuit.h"

XtCalib::XtCalib(){
     cout << "Calibration type: XtCalib" << endl;
}

XtCalib::~XtCalib(){
}

void XtCalib::init(TObjArray* hlist, MdcCosGeom* pGeom){
     CalibBase::init(hlist, pGeom);

     m_fdXt = new TFolder("mfdxt","fdxt");
     hlist->Add(m_fdXt);

     char hname[200];
     for(int lay=0; lay<43; lay++){
	  for(int iEntr=0; iEntr<gNEntr[lay]; iEntr++){
	       for(int lr=0; lr<NLR; lr++){
		    for(int bin=0; bin<=NXTBIN; bin++){
			 sprintf(hname, "mHxt%02d_E%02d_LR%01d_B%02d", lay, iEntr, lr, bin);
			 m_hxt[lay][iEntr][lr][bin] = new TH1D(hname, "", 300, -1.5, 1.5);
			 m_fdXt->Add(m_hxt[lay][iEntr][lr][bin]);
		    }
	       }
	  }
     }
}

void XtCalib::mergeHist(TFile* fhist){
     CalibBase::mergeHist(fhist);

     char hname[200];
     TFolder* fd = (TFolder*)fhist->Get("fdXt");
     for(int lay=0; lay<43; lay++){
	  for(int iEntr=0; iEntr<gNEntr[lay]; iEntr++){
	       for(int lr=0; lr<NLR; lr++){
		    for(int bin=0; bin<=NXTBIN; bin++){
			 sprintf(hname, "Hxt%02d_E%02d_LR%01d_B%02d", lay, iEntr, lr, bin);
			 TH1F* hist = (TH1F*)fd->FindObjectAny(hname);
			 m_hxt[lay][iEntr][lr][bin]->Add(hist);
// 			 if((0==lay)&&(0==iEntr)&&(0==lr)&&(50==bin)){
// 			      cout << setw(15) << hist->GetEntries()
// 				   << setw(15) << m_hxt[lay][iEntr][lr][bin]->GetEntries() << endl;
// 			 }
		    }
	       }
	  }
     }
}

void XtCalib::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     CalibBase::calib(calconst, newXtList, r2tList);

     Int_t ierflg;
     Int_t istat;
     Int_t nvpar;
     Int_t nparx;
     Double_t fmin;
     Double_t edm;
     Double_t errdef;
     Double_t arglist[10];

     TMinuit* gmxt = new TMinuit(6);
     gmxt -> SetPrintLevel(-1);
     gmxt -> SetFCN(fcnXT);
     gmxt -> SetErrorDef(1.0);
     gmxt -> mnparm(0, "xtpar0", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(1, "xtpar1", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(2, "xtpar2", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(3, "xtpar3", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(4, "xtpar4", 0, 0.1, 0, 0, ierflg);
     gmxt -> mnparm(5, "xtpar5", 0, 0.1, 0, 0, ierflg);
     arglist[0] = 0;
     gmxt -> mnexcm("SET NOW", arglist, 0, ierflg);

     TMinuit* gmxtEd = new TMinuit(1);
     gmxtEd -> SetPrintLevel(-1);
     gmxtEd -> SetFCN(fcnXtEdge);
     gmxtEd -> SetErrorDef(1.0);
     gmxtEd -> mnparm(0, "xtpar0", 0, 0.1, 0, 0, ierflg);
     arglist[0] = 0;
     gmxtEd -> mnexcm("SET NOW", arglist, 0, ierflg);

//      double xtpar;
     int i;
     Stat_t histEntry;
     double xtpar;
     double xterr;
     double tbcen;
     double deltx;
     double xcor;
     double xerr;
     double xtini[8];
     double xtfit[8];
     ofstream fxtlog("xtlog");
     for(int lay=0; lay<43; lay++){
	  if(0 == gFgCalib[lay]) continue;
	  for(int iEntr=0; iEntr<gNEntr[lay]; iEntr++){
	       for(int iLR=0; iLR<NLR; iLR++){
		    fxtlog << "Layer " << setw(3) << lay << setw(3) << iEntr
			   << setw(3) << iLR << endl;
		    for(int ord=0; ord<NXTPAR; ord++){
			 if(0 == iEntr) xtpar = calconst -> getXtpar(lay, 8, iLR, ord);
			 else if(1 == iEntr) xtpar = calconst -> getXtpar(lay, 9, iLR, ord);
			 xtini[ord] = xtpar;
			 xtfit[ord] = xtpar;
		    }
		    Tmax = xtini[6];

		    for(int bin=0; bin<=NXTBIN; bin++){
			 histEntry = (int)(m_hxt[lay][iEntr][iLR][bin] -> GetEntries());
			 if(histEntry > 100){
			      deltx = m_hxt[lay][iEntr][iLR][bin] -> GetMean();
			      xerr = m_hxt[lay][iEntr][iLR][bin]->GetRMS();
			 } else{
			      continue;
			 }

			 if(bin < NXTBIN)
			      tbcen = ( (double)bin + 0.5 ) * gTbinw;
			 else tbcen = xtini[6]; // m_tm[lay][iEntr][iLR];
			 xcor = xtFun(tbcen, xtini) - deltx;

			 if((tbcen <= Tmax) || (bin == NXTBIN)){
			      TBINCEN.push_back( tbcen );
			      XMEAS.push_back( xcor );
			      ERR.push_back( xerr );
			 } else{
			      TBINCENED.push_back( tbcen );
			      XMEASED.push_back( xcor );
			      ERRED.push_back( xerr );                       
			 }
			 fxtlog << setw(3) << bin
				<< setw(15) << deltx
				<< setw(15) << xcor
				<< setw(15) << tbcen
				<< setw(15) << xerr
				<< endl; 
		    } // end of bin loop

		    if( XMEAS.size() < 12 ){
			 TBINCEN.clear();
			 XMEAS.clear();
			 ERR.clear();

			 TBINCENED.clear();
			 XMEASED.clear();
			 ERRED.clear();

			 continue;
		    }               

		    for(int ord=0; ord<=5; ord++){
			 arglist[0] = ord + 1;
			 arglist[1] = xtini[ord];
			 gmxt -> mnexcm("SET PARameter", arglist, 2, ierflg);
		    }

		    // fix the xtpar[0] at 0
		    if(1 == gfixXtC0){
			 arglist[0] = 1;
			 arglist[1] = 0.0;
			 gmxt -> mnexcm("SET PARameter", arglist, 2, ierflg);
			 gmxt -> mnexcm("FIX", arglist, 1, ierflg);
		    }

		    arglist[0] = 1000;
		    arglist[1] = 0.1;
		    gmxt -> mnexcm("MIGRAD", arglist, 2, ierflg);
		    gmxt -> mnstat(fmin, edm, errdef, nvpar, nparx, istat);

		    fxtlog << "Xtpar: " << endl;
		    if( (0 == ierflg) && (istat >= 2) ){
			 for(int ord=0; ord<=5; ord++){
			      gmxt -> GetParameter(ord, xtpar, xterr);
// 			      calconst -> resetXtpar(lay, iEntr, iLR, ord, xtpar);
			      xtfit[ord] = xtpar;

			      if(1 == gNEntr[lay]){
				   for(i=0; i<18; i++)
					calconst -> resetXtpar(lay, i, iLR, ord, xtpar);
			      } else if(2 == gNEntr[lay]){
				   if(0 == iEntr){
					for(i=0; i<9; i++) // entr<0
					     calconst->resetXtpar(lay, i, iLR, ord, xtpar);
				   } else{
					for(i=9; i<18; i++) // entr>0
					     calconst->resetXtpar(lay, i, iLR, ord, xtpar);
				   }
			      }
			      fxtlog << setw(15) << xtpar << setw(15) << xterr << endl;
			 }
		    } else{
			 for(int ord=0; ord<=5; ord++){
			      fxtlog << setw(15) << xtini[ord] << setw(15) << "0" << endl;
			 } 
		    }
		    fxtlog << setw(15) << Tmax << setw(15) << "0" << endl;

		    //   release the first parameter
		    if(1 == gfixXtC0){
			 arglist[0] = 1;
			 gmxt -> mnexcm("REL", arglist, 1, ierflg);
		    }

		    Dmax = xtFun(Tmax, xtfit);

		    if( XMEASED.size() >= 3 ){
			 // fit xt in the edge area
			 arglist[0] = 1;
			 arglist[1] = xtini[7];
			 gmxtEd -> mnexcm("SET PARameter", arglist, 2, ierflg);

			 arglist[0] = 1000;
			 arglist[1] = 0.1;
			 gmxtEd -> mnexcm("MIGRAD", arglist, 2, ierflg);
			 gmxtEd -> mnstat(fmin, edm, errdef, nvpar, nparx, istat);

			 if( (0 == ierflg) && (istat >=2) ){
			      gmxtEd -> GetParameter(0, xtpar, xterr);
			      if(xtpar < 0.0) xtpar = 0.0;
// 			      calconst -> resetXtpar(lay, iEntr, iLR, 7, xtpar);

			      if(1 == gNEntr[lay]){
				   for(i=0; i<18; i++)
					calconst -> resetXtpar(lay, i, iLR, 7, xtpar);
			      } else if(2 == gNEntr[lay]){
				   if(0 == iEntr){
					for(i=0; i<9; i++)
					     calconst->resetXtpar(lay, i, iLR, 7, xtpar);
				   } else{
					for(i=9; i<18; i++)
					     calconst->resetXtpar(lay, i, iLR, 7, xtpar);
				   }
			      }
			      fxtlog << setw(15) << xtpar << setw(15) << xterr << endl;				     
			 } else {
			      fxtlog << setw(15) << xtini[7] << setw(15) << "0" << endl;
			 }
		    } else {
			 fxtlog << setw(15) << xtini[7] << setw(15) << "0" << endl;
		    }    
		    fxtlog << "Tm " << setw(15) << Tmax
			   << "  Dmax " << setw(15) << Dmax << endl;

		    TBINCEN.clear();
		    XMEAS.clear();
		    ERR.clear();
		    TBINCENED.clear();
		    XMEASED.clear();
		    ERRED.clear();
	       } // lr loop
	  } // entrance loop
     } // layer loop
     fxtlog.close();

     renameHist();
     delete gmxt;
     delete gmxtEd;
}

void XtCalib::renameHist(){
     char hname[200];
     m_fdXt->SetName("fdXt");
     for(int lay=0; lay<43; lay++){
	  for(int iEntr=0; iEntr<gNEntr[lay]; iEntr++){
	       for(int lr=0; lr<NLR; lr++){
		    for(int bin=0; bin<=NXTBIN; bin++){
			 sprintf(hname, "Hxt%02d_E%02d_LR%01d_B%02d", lay, iEntr, lr, bin);
			 m_hxt[lay][iEntr][lr][bin]->SetName(hname);
		    }
	       }
	  }
     }
}
