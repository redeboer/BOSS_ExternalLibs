#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TFolder.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TSpline.h"
#include "TPostScript.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "include/MdcAlignPar.h"
#include "include/MdcCosGeom.h"
#include "include/fun.h"
#include "include/ResiAlign.h"

using namespace std;

int main(int argc, char* argv[]){
     char* jobname;
     if(argc>1){
	  jobname = argv[1];
     }else{
	  cout << "bad argument" << endl;
	  return -1;
     }

     string path = "";
     string strJob = jobname;
     cout << "strJob: " << strJob << endl;
     string::size_type ilast = strJob.find_last_of("/");
     if(string::npos != ilast){
	  path = strJob.substr(0, ilast);
     }

     int alignMeth;
     string alignFile;
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
	       } else if( str.find("MdcGeomSvc.alignFilePath", 0) != string::npos ){
		    string::size_type i1 = str.find_first_of("\"");
		    string::size_type i2 = str.find_last_of("\"");
		    alignFile = str.substr(i1+1, i2-i1-1);
	       } else if(str.find("MdcAlignAlg.ConfigFile", 0) != string::npos){
		    string::size_type i1 = str.find_first_of("\"");
		    string::size_type i2 = str.find_last_of("\"");
		    confname = str.substr(i1+1, i2-i1-1);
	       } else if(str.find("MdcAlignAlg.MdcAlignMeth", 0) != string::npos){
		    string::size_type i1 = str.find_first_of("=");
		    string::size_type i2 = str.find_last_of(";");
		    strtmp = str.substr(i1+1, i2-i1-1);
		    sscanf(strtmp.c_str(), "%d", &alignMeth);
	       }
	  }
     }

     MdcCosGeom* pGeom = 0;
     pGeom = new MdcCosGeom("/home/bes/wulh/document/wireconf.txt", "/home/bes/wulh/calibConst/MdcAlignPar_ini.txt" );
     pGeom -> initialize(0.0);

     TObjArray* hlist = new TObjArray(0);
     AlignBase* pAlign;
     if(0 == alignMeth) pAlign = new ResiAlign();
     else {cout << "Error AlignType" << endl; return 0;}
     pAlign->init(hlist, pGeom);

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
	       pAlign->mergeHist(fin);
	       fin->Close();
	  }
     }

     // read align par.
     MdcAlignPar* alignPar = new MdcAlignPar();
     alignPar->initAlignPar();
     alignPar->rdAlignPar(alignFile);

     // fit for alignment
     pAlign->align(alignPar);

     TFile fhist("histall.root", "recreate");
     fhist.cd();
     hlist->Write();
     fhist.Close();

     // output new alignment file
     alignPar->wrtAlignPar();

     return 0;
}

