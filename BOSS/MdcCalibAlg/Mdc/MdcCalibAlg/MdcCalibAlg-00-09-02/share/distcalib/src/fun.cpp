#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"

#include "include/MdcCalibConst.h"
#include "include/fun.h"

using namespace std;

vector<double> XMEAS;
vector<double> TBINCEN;
vector<double> ERR;
double Tmax;
double Dmax;
vector<double> XMEASED;
vector<double> TBINCENED;
vector<double> ERRED;

int gNEntr[43];

/* from calib config file */
double gTimeShift = 0.0;	/* if T<0 after subtracting Tes, use this */
double gTesMin = 0.0;		/* minimun Tes for calibration */
double gTesMax = 9999.0;	/* maximun Tes for calibration */
int    gFgIniCalConst = 2; 	/* effective for IniMdcCalib */
bool   gPreT0SetTm = true;	/* flag for updating Tm in PreT0Calib */
double gInitT0 = 50.0;		/* initial value of T0 fit */
double gT0Shift = 0.0;	        /* t0 shift based on leading edge fitting */
double gTminFitChindf = 20.0;	/* chisquare cut for Tmin fit */
double gTmaxFitChindf = 20.0;	/* chisquare cut for Tmax fit */
int    gResiType = 0;	        /* 0: including measurement point; 1: excluding */
int    gCalSigma = 1;		/* flag for update sigma */
int    gFixXtC0 = 0;	        /* 1: fix c0 at 0 */
int    gFgCalib[NLAYER];
double gTminFitRange[NLAYER][2];
double gTmaxFitRange[NLAYER][2];
double gInitTm[NLAYER];	/* initial value of Tm fit */
double gQmin[NLAYER];		/* QT fit range */
double gQmax[NLAYER];		/* QT fit range */

double xtFun(double t, double xtpar[]){
     int ord;
     double dist = 0.0;
     double tm = xtpar[6];

     if(t < tm ){
	  for(ord=0; ord<=5; ord++){
	       dist += xtpar[ord] * pow(t, ord);
	  }
     }else{
	  for(ord=0; ord<=5; ord++){
	       dist += xtpar[ord] * pow(tm, ord);
	  }
	  dist += xtpar[7] * (t - tm);
     }

     return dist;
}

void fcnXT(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
     unsigned int i;
     int ord;
     Double_t deta;
     Double_t chisq = 0.0;
     Double_t dfit;

     for(i=0; i<TBINCEN.size(); i++){
	  dfit = 0;
	  for(ord=0; ord<=5; ord++){
	       dfit += par[ord] * pow(TBINCEN[i], ord);
	  }
	  deta = (dfit - XMEAS[i]) / ERR[i];
	  chisq += deta * deta;
     }

     f = chisq;
}

void fcnXtEdge(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
     unsigned int i;
     Double_t deta;
     Double_t chisq = 0.0;
     Double_t dfit;

     for(i=0; i<TBINCENED.size(); i++){
          dfit = par[0] * (TBINCENED[i] - Tmax) + Dmax;
          deta = (dfit - XMEASED[i]) / ERRED[i];
          chisq += deta * deta;
     }

     f = chisq;
}

Double_t xtFitFun(Double_t *x, Double_t par[]){
     Double_t val = 0.0;
     for(Int_t ord=0; ord<6; ord++){
	  val += par[ord] * pow(x[0], ord);
     }
     return val;
}

Double_t xtFitEdge(Double_t *x, Double_t par[]){
     double val = Dmax + (x[0] - Tmax) * par[0];
     return val;
}

void writeConst(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     TFile fout("MdcCalibConst_new.root", "recreate");

     int key;
     double xtpar;
     TTree *xttree = new TTree("XtTree", "XtTree");
     xttree -> Branch("xtkey", &key, "key/I");
     xttree -> Branch("xtpar", &xtpar, "xtpar/D");
     for(int lay=0; lay<43; lay++){
	  for(int entr=0; entr<18; entr++){
	       for(int lr=0; lr<3; lr++){
		    for(int ord=0; ord<8; ord++){
			 key = calconst->getXtKey(lay, entr, lr, ord);
			 xtpar = calconst->getXtpar(lay, entr, lr, ord);
			 xttree -> Fill();
		    }
	       }
	  }
     }

     double t0;
     double delt0;
     TTree *t0tree = new TTree("T0Tree", "T0Tree");
     t0tree -> Branch("t0", &t0, "t0/D");
     t0tree -> Branch("delt0", &delt0, "delt0/D");
     for(int wid=0; wid<6796; wid++){
	  t0 = calconst->getT0(wid);
	  delt0 = calconst->getDelT0(wid);
	  t0tree -> Fill();
     }

     double qtval[2];
     TTree *qttree = new TTree("QtTree", "QtTree");
     qttree -> Branch("qtpar0", &(qtval[0]), "qtpar0/D");
     qttree -> Branch("qtpar1", &(qtval[1]), "qtpar1/D");
     for(int lay=0; lay<43; lay++){
	  qtval[0] = calconst->getQtpar0(lay);
	  qtval[1] = calconst->getQtpar1(lay);
	  qttree -> Fill();
     }

     double sdpar;
     TTree *sdtree = new TTree("SdTree", "SdTree");
     sdtree -> Branch("sdkey", &key, "key/I");
     sdtree -> Branch("sdpar", &sdpar, "sdpar/D");
     for(int lay=0; lay<43; lay++){
	  for(int entr=0; entr<6; entr++){
	       for(int lr=0; lr<2; lr++){
		    for(int bin=0; bin<24; bin++){
			 key = calconst->getSdKey(lay, entr, lr, bin);
			 sdpar = calconst->getSdpar(lay, entr, lr, bin);
			 sdtree -> Fill();
		    }
	       }
	  }
     }

     fout.cd();
     xttree -> Write();
     t0tree -> Write();
     qttree -> Write();
     sdtree -> Write();
     if((newXtList->GetEntries()) > 0) newXtList -> Write();
     if((r2tList->GetEntries()) > 0) r2tList -> Write();
     fout.Close();
}

vector<string> getHistList()
{
   vector<string> fnames;

   string command(
	 "JobOutputDir=`/bin/ls -dt1 joboutput-* 2>/dev/null | head -1`\n"
	 "if [ -d \"${JobOutputDir}\" ]; then\n"
	 "  find ${JobOutputDir} -name hist.root\n"
	 "fi\n"
	 );

   stringstream fnstream;

   char* fnbuf   = new char[1024];
   FILE* fstream = popen(command.c_str(), "r");

   while ( fgets(fnbuf, 1024, fstream) != NULL ) {
      fnstream << fnbuf;
   }

   string fname;
   while ( ! (fnstream>>fname).eof() ) {
      fnames.push_back(fname);
   }

   pclose(fstream);
   delete [] fnbuf;

   if ( fnames.empty() ) {
      cout << "WARNING: Failed to retrieve hist files in the current directory!" << endl;
//       exit(1);
   }
   return fnames;
}

vector<string> getHistList(string path)
{
   vector<string> fnames;
   string newpath = path;
   string::size_type strl = newpath.length();
   if((strl>1) && ('/'==newpath[strl-1])) newpath.erase(strl-1);

   char com1[500];
   sprintf(com1, "JobOutputDir=`/bin/ls -dt1 %s/joboutput-* 2>/dev/null | head -1`\n", newpath.c_str());
   string command1(com1);
   string command2(
	 "if [ -d \"${JobOutputDir}\" ]; then\n"
	 "  find ${JobOutputDir} -name hist.root\n"
	 "fi\n"
	 );
   string command = command1 + command2;
   stringstream fnstream;

   char* fnbuf   = new char[1024];
   FILE* fstream = popen(command.c_str(), "r");

   while ( fgets(fnbuf, 1024, fstream) != NULL ) {
      fnstream << fnbuf;
   }

   string fname;
   while ( ! (fnstream>>fname).eof() ) {
      fnames.push_back(fname);
   }

   pclose(fstream);
   delete [] fnbuf;

   if ( fnames.empty() ) {
      cout << "ERROR: Failed to retrieve hist files!" << endl;
      exit(1);
   }
   return fnames;
}
