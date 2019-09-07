#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "TVacuumPol.h"

using namespace std;

TVacuumPol::TVacuumPol(){
  using namespace std;
  fsvtr = NULL;
  fsvsr = NULL;
  fsvsi = NULL;
  fNoVP = false;
}

void TVacuumPol::Init(std::string vpold, std::string vpolf){
/*  char *vpd = getenv("VACUUM_POL_DIR");
  char *vpf = getenv("VACUUM_POL_FNAME");
  string vpolfname;
  if(vpf == NULL){
    if(vpd == NULL){
      vpolfname = "vpol.dat";
    }else{
      ostringstream temp;
      temp<<vpd<<"/"<<"vpol.dat";
      vpolfname = temp.str();
    }
  } else {
    vpolfname = vpf;
  }
*/
  string vpolfname;
  if(vpolf.empty()) vpolfname = "vpol.dat";
  else vpolfname = vpolf;

  if(! vpold.empty()) { 
        cout<<"Directory is \""<<vpold<<"\"."<<endl<<flush;
	vpolfname = vpold + "/" + vpolfname;
  }

//  if(vpd != NULL) cout<<"Directory is \""<<vpd<<"\"."<<endl<<flush;
  cout<<"File name is \""<<vpolfname<<"\"."<<endl<<flush;

  ReadVacuumPolData(vpolfname);
}

TVacuumPol::~TVacuumPol(){
  delete fsvtr;
  delete fsvsr;
  delete fsvsi;
}

void TVacuumPol::ReadVacuumPolData(std::string fname){
  ifstream IN(fname.c_str());
  if(IN.is_open() == false ){
    std::cout<<"Can't read \""<<fname<<"\". Vacuum polarization will be zero."
	     <<std::endl;
    std::cout<<"Probably you should tune VACUUM_POL_DIR or VACUUM_POL_FNAME variables."<<std::endl;
    fReadSuccessfullyVP = false;
    return;
  }

  const int MAX_NP = 20000;
  double *s   = new double[MAX_NP];
  double *vtr = new double[MAX_NP];
  double *vsr = new double[MAX_NP];
  double *vsi = new double[MAX_NP];
  double *vp  = new double[MAX_NP];

  int np = 0;
  while(!IN.eof()){
    IN>>s[np]>>vtr[np]>>vsr[np]>>vsi[np]>>vp[np];
    string str;
    getline(IN,str);
    s[np] *= 1e6;
    np++;
  }
  np--;
  
  double *re_vs = new double[np];
  double *im_vs = new double[np];
  double *re_vt = new double[np];

  for(int i=0;i<np;i++){
    std::complex<double> Pi_s(vsr[i],vsi[i]);
    std::complex<double> vs = 1./(1.-Pi_s);
    re_vs[i] = std::real(vs);
    im_vs[i] = std::imag(vs);

    std::complex<double> Pi_t(vtr[i],0.);
    std::complex<double> vt = 1./(1.-Pi_t);
    re_vt[i] = std::real(vt);
  }

  if(fsvsr) delete fsvsr;
  fsvsr = new TRadSpline3("vsr",s,re_vs,np);

  if(fsvsi) delete fsvsi;
  fsvsi = new TRadSpline3("vsi",s,im_vs,np);

  if(fsvtr) delete fsvtr;
  fsvtr = new TRadSpline3("vtr",s,re_vt,np);

  delete [] re_vs;
  delete [] im_vs;
  delete [] re_vt;

  delete [] s; 
  delete [] vtr; 
  delete [] vsr; 
  delete [] vsi; 
  delete [] vp; 

  fReadSuccessfullyVP = true;
}
