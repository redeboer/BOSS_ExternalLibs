#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TFolder.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TList.h"
#include "TSpline.h"
#include "TPostScript.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "include/MdcCalibConst.h"
#include "include/MdcCosGeom.h"
#include "include/fun.h"
#include "include/IniCalib.h"
#include "include/PreXtCalib.h"
#include "include/PreT0Calib.h"
#include "include/XtCalib.h"
#include "include/GrXtCalib.h"
#include "include/XtInteCalib.h"
#include "include/T0Calib.h"
#include "include/QtCalib.h"

using namespace std;

int main(int argc, char* argv[]){
     char* jobname;
     char* strcal;
     int fgCal = 1;
     if(argc>2){
	  jobname = argv[1];
	  strcal = argv[2];
	  sscanf(strcal, "%d", &fgCal);
     } else if(argc>1){
	  jobname = argv[1];
     }else{
	  cout << "bad argument" << endl;
	  return -1;
     }
     if(fgCal <= 0) cout << "do not calibrate " << endl;

     string path = "";
     string strJob = jobname;
     cout << "strJob: " << strJob << endl;
     string::size_type ilast = strJob.find_last_of("/");
     if(string::npos != ilast){
	  path = strJob.substr(0, ilast);
     }

     int calType;
     string constname;
     string confname;
     string str;
     string strtmp;
     ifstream fjob(jobname);
     if( ! fjob.is_open() ){
	  cout << "ERROR: can not read jobOption: " << jobname << endl;
	  return 0;
     } else{
	  cout << "Open jobOption: " << jobname << endl;
	  while( getline(fjob, str) ){
	       if(str.find("//", 0) != string::npos){
		    continue;
	       } else if( str.find("CalibRootCnvSvc.Mdcrootfile", 0) != string::npos ){
		    string::size_type i1 = str.find_first_of("\"");
		    string::size_type i2 = str.find_last_of("\"");
		    constname = str.substr(i1+1, i2-i1-1);
	       } else if(str.find("MdcCalibAlg.ConfigFile", 0) != string::npos){
		    string::size_type i1 = str.find_first_of("\"");
		    string::size_type i2 = str.find_last_of("\"");
		    confname = str.substr(i1+1, i2-i1-1);
	       } else if(str.find("MdcCalibAlg.MdcCalFlg", 0) != string::npos){
		    string::size_type i1 = str.find_first_of("=");
		    string::size_type i2 = str.find_last_of(";");
		    strtmp = str.substr(i1+1, i2-i1-1);
		    sscanf(strtmp.c_str(), "%d", &calType);
	       }
	  }
     }

     MdcCosGeom* pGeom = 0;
     pGeom = new MdcCosGeom("/home/bes/wulh/document/wireconf.txt", "/home/bes/wulh/calibConst/MdcAlignPar_ini.txt" );
     pGeom -> initialize(0.0);

     int lay;
     for(lay=0; lay<15; lay++) gNEntr[lay] = 1;
     for(lay=15; lay<43; lay++) gNEntr[lay] = 2;

     // read config file
     string strconfig;
     string strcomment;
     ifstream fconfig(confname.c_str());
     if( ! fconfig.is_open() ){
	  cout << "ERROR: can not read config file" << endl;
	  return 0;
     } else{
	  cout << "Open config file: " << confname << endl;
	  while( fconfig >> strconfig ){
	       if('#' == strconfig[0]){
		    getline(fconfig, strcomment);
	       } else if("TimeShift" == strconfig){
		    fconfig >> gTimeShift;
	       } else if("TesMin" == strconfig){
		    fconfig >> gTesMin;
	       } else if("TesMax" == strconfig){
		    fconfig >> gTesMax;
	       } else if("FlagIniCalConst" == strconfig){
		    fconfig >> gFgIniCalConst;
	       } else if("FlagUpdateTmInPreT0" == strconfig){
		    fconfig >> gPreT0SetTm;
	       } else if("InitT0" == strconfig){
		    fconfig >> gInitT0;
	       } else if("T0Shift" == strconfig){
		    fconfig >> gT0Shift;
	       } else if("TminFitChindf" == strconfig){
		    fconfig >> gTminFitChindf;
	       } else if("TmaxFitChindf" == strconfig){
		    fconfig >> gTmaxFitChindf;
	       } else if("ResidualType" == strconfig){
		    fconfig >> gResiType;
	       } else if("UpdateSigma" == strconfig){
		    fconfig >> gCalSigma;
	       } else if("FixXtC0" == strconfig){
		    fconfig >> gFixXtC0;
	       } else if("RawTimeFitRange" == strconfig){
		    for(lay=0; lay<NLAYER; lay++){
			 fconfig >> strtmp
				 >> gFgCalib[lay]
				 >> gTminFitRange[lay][0]
				 >> gTminFitRange[lay][1]
				 >> gTmaxFitRange[lay][0]
				 >> gTmaxFitRange[lay][1]
				 >> gInitTm[lay]
				 >> strtmp
				 >> gQmin[lay]
				 >> gQmax[lay];
		    }
	       }
	  }
     }
     fconfig.close();

     TObjArray* hlist = new TObjArray(0);
     CalibBase* pcal;
     if(0 == calType) pcal = new IniCalib();
     else if(1 == calType) pcal = new PreXtCalib();
     else if(2 == calType) pcal = new PreT0Calib();
     else if(3 == calType) pcal = new XtCalib();
     else if(4 == calType) pcal = new GrXtCalib();
     else if(9 == calType) pcal = new XtInteCalib();
     else if(5 == calType) pcal = new T0Calib();
     else if(8 == calType) pcal = new QtCalib();
     else {cout << "Error CalibType" << endl; return 0;}
     pcal->init(hlist, pGeom);

     vector<string> fhistname = getHistList();
     if(0==fhistname.size()){
	  cout << "hist file path: " << path << endl;
	  fhistname = getHistList(path);
     }
     for(unsigned nf=0; nf<fhistname.size(); nf++){
	  TFile* fin = new TFile(fhistname[nf].c_str());
	  if(!fin->IsOpen()){
	       continue;
	  } else{
	       cout << "merge hist file " << nf << ": " << fhistname[nf] << endl;
	       pcal->mergeHist(fin);
	       fin->Close();
	  }
     }

     // read calib const.
     MdcCalibConst* calconst = new MdcCalibConst();
//      if(fgCal <= 0){constname = "/home/bes/wulh/calibConst/MdcCalibConst_11397_psi3770_652b_v1.root";}
     if(constname==""){constname = "/home/bes/wulh/calibConst/MdcCalibConst_11397_psi3770_652b_v1.root";}
     TFile fconst(constname.c_str());
     if(!fconst.IsOpen()){
	  cout << "ERROR: " << constname << " does not exist" << endl;
	  return 0;
     }
     cout << "Open calib const file " << constname << endl;
     int key;
     double xtpar;
     int entry;
     TTree* xttree = (TTree*)fconst.Get("XtTree");
     xttree -> SetBranchAddress("xtkey", &key);
     xttree -> SetBranchAddress("xtpar", &xtpar);
     entry = (int)xttree -> GetEntries();
     for(int i=0; i<entry; i++){
	  xttree -> GetEntry(i);
	  calconst->fillXtpar(key, xtpar);
     }

     double t0;
     double delt0;
     TTree* t0tree = (TTree*)fconst.Get("T0Tree");
     t0tree -> SetBranchAddress("t0", &t0);
     t0tree -> SetBranchAddress("delt0", &delt0);
     entry = (int)t0tree -> GetEntries();
     for(int i=0; i<entry; i++){
	  t0tree -> GetEntry(i);
	  calconst->fillT0(t0);
	  calconst->fillDelT0(delt0);
     }

     double qtpar0;
     double qtpar1;
     TTree* qttree = (TTree*)fconst.Get("QtTree");
     qttree -> SetBranchAddress("qtpar0", &qtpar0);
     qttree -> SetBranchAddress("qtpar1", &qtpar1);
     entry = (int)qttree -> GetEntries();
     for(int i=0; i<entry; i++){
	  qttree -> GetEntry(i);
	  calconst->fillQtpar0(qtpar0);
	  calconst->fillQtpar1(qtpar1);
     }

     double sdpar;
     TTree* sdtree = (TTree*)fconst.Get("SdTree");
     sdtree -> SetBranchAddress("sdkey", &key);
     sdtree -> SetBranchAddress("sdpar", &sdpar);
     entry = sdtree -> GetEntries();
     for(int i=0; i<entry; i++){
	  sdtree -> GetEntry(i);
	  calconst->fillSdpar(key, sdpar);
     }

     TObjArray* newXtList = new TObjArray(0);
     TList* list = fconst.GetListOfKeys();
     int listSize = (int)(list->GetSize());
     cout << "Number of trees in old calib const file: " << listSize << endl;
     if(listSize > 4){
	  gROOT->cd();
	  for (int i = 0; i<listSize; i++) {
	       TTree* tree = (TTree*)fconst.Get(list->At(i)->GetName());
	       string strName(tree->GetName());
	       if(string::npos != strName.find("trNewXt")){
		    TTree* t2 = tree->CopyTree("");
		    newXtList->Add(t2);
	       }
	  }
     }
     fconst.Close();

     gStyle -> SetCanvasBorderMode(0);
     gStyle -> SetCanvasColor(10);
     gStyle -> SetOptFit(0011);
     gStyle -> SetStatColor(10);
     gStyle -> SetStatBorderSize(1);
     gStyle -> SetStatFont(42);
     gStyle -> SetStatFontSize(0.04);
     gStyle -> SetStatX(0.9);
     gStyle -> SetStatY(0.9);
     gStyle -> SetPadColor(10);
     gStyle -> SetFuncColor(2);

     TObjArray* r2tList = new TObjArray(0);

     // x-t calibration
     pcal->calib(calconst, newXtList, r2tList);

     TFile fhist("histall.root", "recreate");
     fhist.cd();
     hlist->Write();
     fhist.Close();

     // output new calib const file
     if(fgCal > 0) writeConst(calconst, newXtList, r2tList);

     return 0;
}

