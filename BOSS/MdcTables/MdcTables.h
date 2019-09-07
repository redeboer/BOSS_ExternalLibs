/* modified the Class MdcRec_wirhit for KalFit & Calib use
 *
 **********************************************************/


#ifndef _MdcTables_h_
#define _MdcTables_h_

#include <string>
#include <iostream>
#include <vector>

//#include "MdcRecGeo/MdcRecGeo.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MdcTables/HepevtTables.h"
#include "MdcRecEvent/RecMdcHit.h"
//#include "MdcTables/TrkTables.h"

using namespace std;

//
//*** raw data info. of MDC wire hits ***  
//
class MdcRaw_wirhit {
public:
  MdcRaw_wirhit() {};
public:
  int id;
  int crt;
  int slt;
  int chn;
  int pre;
  int sqt;
  int edges;
  int tdc1;
  int tdc2;
  int tdc3;
  int tdc4;
  int tdc5;
  int tdc6;
  int tdc7;
  int tdc8;
  int tdc9;
  int tdc10;
  int tdc11;
  int tdc12;
  int tdc13;
  int tdc14;
  int tdc15;
  int tdc16;
  int stat;
  int layer;
  int local;
  const MdcGeoWire * geo;
};

class MdcRawWirhitCol {
public:
   static vector<MdcRaw_wirhit>* getMdcRawWirhitCol(void);              
private:
   static vector<MdcRaw_wirhit>* s_col;            
};


//
//*** digitized info. of MDC wire hits ***
//
class MdcRec_wirhit;
class MdcDat_wirhit {
public:
  MdcDat_wirhit() {};
public:
  int id;
  float adc;
  float tdc;
  int stat;
  const MdcGeoWire * geo;
  MdcRec_wirhit * rec;
};

class MdcDatWirhitCol {
public:
   static vector<MdcDat_wirhit>* getMdcDatWirhitCol(void);              
private:
   static vector<MdcDat_wirhit>* s_col;            
};

//
//*** digitized info. of MDC wire hits ***
//
class MdcDat_olhit {
public:
  MdcDat_olhit() {};
public:
  int id;
  float adc;
  float tdc;
  int stat;
  const MdcGeoWire * geo;
};

class MdcDatOlhitCol {
public:
   static vector<MdcDat_olhit>* getMdcDatOlhitCol(void);              
private:
   static vector<MdcDat_olhit>* s_col;            
};

//
//*** const. param. to calculate drift distance ***
//
class MdcCal_tzero {
public:
  MdcCal_tzero() {};
public:
  int id;
  float tzero;
};

class MdcCalTzeroCol {
public:
   static vector<MdcCal_tzero>* getMdcCalTzeroCol(void);              
private:
   static vector<MdcCal_tzero>* s_col;            
};

//
//*** run by run t0 *** 
//
class MdcCal_tzero2 {
public:
  MdcCal_tzero2() {};
public:
  int id;
  float t0;
};

class MdcCalTzero2Col {
public:
   static vector<MdcCal_tzero2>* getMdcCalTzero2Col(void);              
private:
   static vector<MdcCal_tzero2>* s_col;            
};

//
//*** Defifinition of bad wires  *** 
//
class MdcCal_badwire {
public:
  MdcCal_badwire() {};
public:
  int id;
  int bad[3];
};

class MdcCalBadwireCol {
public:
   static vector<MdcCal_badwire>* getMdcCalBadwireCol(void);              
private:
   static vector<MdcCal_badwire>* s_col;            
};

//
//**** nition of tables for const. param. to calculate drift distance ***
//
class MdcCal_const {
public:
  MdcCal_const() {};
public:
  int id;
  float xtl[8][18];
  float xtr[8][18];
  float bndl[4][18];
  float bndr[4][18];
};

class MdcCalConstCol {
public:
   static vector<MdcCal_const>* getMdcCalConstCol(void);              
private:
   static vector<MdcCal_const>* s_col;            
};
 
//
//*** const. param. for error of drift distance ***
//
class MdcCal_const2 {
public:
  MdcCal_const2() {};
public:
  int id;
  float sgm[5];
};

class MdcCalConst2Col {
public:
   static vector<MdcCal_const2>* getMdcCalConst2Col(void);              
private:
   static vector<MdcCal_const2>* s_col;            
};
 
//
//*** const. param. for time walk *** 
//
class MdcCal_const3 {
public:
  MdcCal_const3() {};
public:
  int id;
  float tw[2];
};

class MdcCalConst3Col {
public:
   static vector<MdcCal_const3>* getMdcCalConst3Col(void);              
private:
   static vector<MdcCal_const3>* s_col;            
};
 
//
//*** fudge factor and tanL dep. ***
//
class MdcCal_const4 {
public:
  MdcCal_const4() {};
public:
  int id;
  float fudge;
  int npoint;
  float tanl[20];
  float ftanl[20];
  float a[20];
  float b[20];
};

class MdcCalConst4Col {
public:
   static vector<MdcCal_const4>* getMdcCalConst4Col(void);              
private:
   static vector<MdcCal_const4>* s_col;            
};
 
//
//*** miscellaneous *** 
//
class MdcCal_misc {
public:
  MdcCal_misc() {};
public:
  int id;
  int level;
  float inv_speed;
};

class MdcCalMiscCol {
public:
   static vector<MdcCal_misc>* getMdcCalMiscCol(void);              
private:
   static vector<MdcCal_misc>* s_col;            
};
 
//
//*** const. param. to calculate charge  *** 
//
class MdcCal_ped {
public:
  MdcCal_ped() {};
public:
  int id;
  float pmean;
  float psigma;
};

class MdcCalPedCol {
public:
   static vector<MdcCal_ped>* getMdcCalPedCol(void);              
private:
   static vector<MdcCal_ped>* s_col;            
};

//
//*** wire-hits of MDC *** 
//
class MdcRec_trk;
class MdcRec_wirhit {
public:
   MdcRec_wirhit() : geo(0), dat(0), trk(0),rechitptr(0){};
   ~MdcRec_wirhit( ); 
   MdcRec_wirhit(const MdcRec_wirhit& rhs) : geo(rhs.geo),
     dat(rhs.dat), trk(rhs.trk),rechitptr(rhs.rechitptr) {
     id = rhs.id;
     ddl = rhs.ddl;
     ddr = rhs.ddr;
     erddl = rhs.erddl;
     erddr = rhs.erddr;
     pChiSq = rhs.pChiSq;
     lr = rhs.lr;
     stat = rhs.stat; 
     tdc = rhs.tdc;
     adc = rhs.adc;
     timechannel=rhs.timechannel;
   }
   
   MdcRec_wirhit& operator =(const MdcRec_wirhit& rhs) {

    if(this == &rhs)
	return *this;
     id = rhs.id;
     ddl = rhs.ddl;
     ddr = rhs.ddr;
     erddl = rhs.erddl;
     erddr = rhs.erddr;
     pChiSq = rhs.pChiSq;
     lr = rhs.lr;
     stat = rhs.stat; 
     geo = rhs.geo;
     trk = rhs.trk;
     dat = rhs.dat;
     tdc = rhs.tdc;
     adc = rhs.adc;
     timechannel=rhs.timechannel;
     rechitptr = rhs.rechitptr;
     return *this;
   }
            
public:
  int id;
  float ddl;
  float ddr;
  float erddl;
  float erddr;
  float pChiSq;
  int lr;
  int stat;
  const MdcGeoWire * geo;
  MdcDat_wirhit * dat;
  MdcRec_trk * trk;
  RecMdcHit* rechitptr;
  float tdc;
  float adc;
  unsigned timechannel;// liucy for tsf
};

class MdcRecWirhitCol {
public:
   static vector<MdcRec_wirhit>* getMdcRecWirhitCol(void);              
private:
   static vector<MdcRec_wirhit>* s_col;            
};

//
//*** track of MDC *** 
//
class MdcRec_trk_add;
class MdcRec_trk {
public:
   MdcRec_trk():add(0) {};
   ~MdcRec_trk( );

   MdcRec_trk(const MdcRec_trk& rhs) : add(rhs.add) {
     id = rhs.id;
     chiSq = rhs.chiSq;
     ndf = rhs.ndf;
     fiTerm = rhs.fiTerm;
     nhits = rhs.nhits; 
     nster = rhs.nster;
     nclus = rhs.nclus; 
     stat = rhs.stat; 
     mass = rhs.mass;
     hitcol = rhs.hitcol;
     for ( int i=0; i<15; i++ ) {
	if( i<3 ) pivot[i] = rhs.pivot[i];	
	if( i<5 ) helix[i] = rhs.helix[i];
        error[i] = rhs.error[i];
     }      
   }
   
   MdcRec_trk& operator =(const MdcRec_trk& rhs) {

    if(this == &rhs)
	return *this;
     id = rhs.id;
     chiSq = rhs.chiSq;
     ndf = rhs.ndf;
     fiTerm = rhs.fiTerm;
     nhits = rhs.nhits; 
     nster = rhs.nster;
     nclus = rhs.nclus; 
     stat = rhs.stat; 
     add = rhs.add;
     mass = rhs.mass;
     hitcol = rhs.hitcol;
     for ( int i=0; i<15; i++ ) {
	if( i<3 ) pivot[i] = rhs.pivot[i];	
	if( i<5 ) helix[i] = rhs.helix[i];
        error[i] = rhs.error[i];
     }      
     return *this;
   }
               
public:
  int id;
  float helix[5];
  float pivot[3];
  float error[15];
  float chiSq;
  float ndf;
  float fiTerm;
  int nhits;
  int nster;
  int nclus;
  int stat;
  float mass;
  vector<MdcRec_wirhit*> hitcol;
  MdcRec_trk_add* add;
public:
  const float fi0(void)const{
          return helix[1];
  }
  const float cpa(void)const{
    	  return helix[2];
  }
  const float dz(void)const{
          return helix[3];
  }
  const float tanl(void)const{
          return helix[4];
  }


};

class MdcRecTrkCol {
public:
   static vector<MdcRec_trk>* getMdcRecTrkCol(void);              
private:
   static vector<MdcRec_trk>* s_col;            
};
 
//
//*** dEdx of MDC ***  
//
class MdcRec_dedx {
public:
  MdcRec_dedx() {};
public:
  int id;
  float dEdx;
  float dEdx_exp[5];
  float sigma_dEdx[5];
  float pid_prob[5];
  MdcRec_trk * trk;
  int stat;
};

class MdcRecDedxCol {
public:
   static vector<MdcRec_dedx>* getMdcRecDedxCol(void);              
private:
   static vector<MdcRec_dedx>* s_col;            
};
 
//
//*** MDC timing *** 
//
class MdcRec_timing {
public:
  MdcRec_timing() {};
public:
  int id;
  float time;
  int quality;
};

class MdcRecTimingCol {
public:
   static vector<MdcRec_timing>* getMdcRecTimingCol(void);              
private:
   static vector<MdcRec_timing>* s_col;            
};
 
//
//*** const. param. for dE/dx caliburation  *** 
//
class MdcRec_excalib {
public:
  MdcRec_excalib() {};
public:
  int id;
  float gain[8208];
  float ggs_f[10];
  float ggs_b[10];
  float erggs_f[10];
  float erggs_b[10];
  float localg[15][47];
  float ddg[10][47];
  float mdcgain;
};

class MdcRecExcalibCol {
public:
   static vector<MdcRec_excalib>* getMdcRecExcalibCol(void);              
private:
   static vector<MdcRec_excalib>* s_col;            
};
 
//
//*** const. param. to caliburate global gain ***   
//
class MdcRec_exglobal {
public:
  MdcRec_exglobal() {};
public:
  int id;
  float mdcgain;
  float mdcresol;
};

class MdcRecExglobalCol {
public:
   static vector<MdcRec_exglobal>* getMdcRecExglobalCol(void);              
private:
   static vector<MdcRec_exglobal>* s_col;            
};
 
//
//*** const. param. to caliburate dE/dx curve *** 
//
class MdcRec_excurve {
public:
  MdcRec_excurve() {};
public:
  int id;
  float f_bg[10];
  float f_erbg[12];
  float f_erbg2[12];
};

class MdcRecExcurveCol {
public:
   static vector<MdcRec_excurve>* getMdcRecExcurveCol(void);              
private:
   static vector<MdcRec_excurve>* s_col;            
};

//
//*** const. to calib. dE/dx curve for 5 particles *** 
//
class MdcRec_excurve_add {
public:
  MdcRec_excurve_add() {};
public:
  int id;
  int flag;
  float f_e[12];
  float f_mu[12];
  float f_pi[12];
  float f_k[12];
  float f_p[12];
};

class MdcRecExcurveAddCol {
public:
   static vector<MdcRec_excurve_add>* getMdcRecExcurveAddCol(void);              
private:
   static vector<MdcRec_excurve_add>* s_col;            
};
 
//
//*** re-reconst. dE/dx ( global calib )" 
//
class MdcRec_exnoggs {
public:
  MdcRec_exnoggs() {};
public:
  int id;
  float dEdx_r;
  float mdcR_old;
  MdcRec_trk * trk;
  MdcRec_dedx * rex;
};

class MdcRecExnoggsCol {
public:
   static vector<MdcRec_exnoggs>* getMdcRecExnoggsCol(void);              
private:
   static vector<MdcRec_exnoggs>* s_col;            
};

//
//*** Definition of current calib. function parameter (2nd POLY.)" 
//
class MdcRec_excurrentfit {
public:
  MdcRec_excurrentfit() {};
public:
  int id;
  float curr_slope;
  float curr_const;
  float f_slope[50];
  float f_const[50];
};

class MdcRecExcurrentfitCol {
public:
   static vector<MdcRec_excurrentfit>* getMdcRecExcurrentfitCol(void);              
private:
   static vector<MdcRec_excurrentfit>* s_col;            
};
 
//
//*** wire current from daqom" 
//
class MdcRec_daqom {
public:
  MdcRec_daqom() {};
public:
  int id;
  int Time;
  float MC[56];
  float GasPress;
  float MDCTemp[5];
  int RunState;
  float BeamCurrent[2];
  float LUMEFC;
  float LUMCSI;
  float VLT40;
  float TOFRATES;
};

class MdcRecDaqomCol {
public:
   static vector<MdcRec_daqom>* getMdcRecDaqomCol(void);              
private:
   static vector<MdcRec_daqom>* s_col;            
};
 
//
//*** digitized info. of MDC wire hits *** 
//
class MdcDat_mcwirhit {
public:
  MdcDat_mcwirhit() {};
public:
  int id;
  float xw;
  float yw;
  float zw;
  float dist;
  float chrg;
  float xin;
  float yin;
  float zin;
  float xout;
  float yout;
  float zout;
  int lr;
  int stat;
  const MdcGeoWire * geo;
  MdcDat_wirhit * dat;
  const Gen_hepevt * hep;
  MdcRec_trk * trk;
  float px;
  float py;
  float pz;
  float pid;
  float tof;
  float tdc;
};

class MdcDatMcwirhitCol {
public:
   static vector<MdcDat_mcwirhit>* getMdcDatMcwirhitCol(void);              
private:
   static vector<MdcDat_mcwirhit>* s_col;            
};

//
//*** MC information of MDC track *** 
//
class MdcRec_mctrk {
public:
  MdcRec_mctrk() {};
public:
  int id;
  const Gen_hepevt * hep;
  float wirFrac;
  float wirFracHep;
  int charge;
  float ptFrac;
  float pzFrac;
  int quality;
};

class MdcRecMctrkCol {
public:
   static vector<MdcRec_mctrk>* getMdcRecMctrkCol(void);              
private:
   static vector<MdcRec_mctrk>* s_col;            
};
 
//
//*** MDC track and generated track matching *** 
//
class MdcRec_mctrk2hep {
public:
  MdcRec_mctrk2hep() {};
public:
  int id;
  MdcRec_wirhit * wir;
  //  MdcRec_clust * clust;
  MdcRec_trk * trk;
  const Gen_hepevt * hep;
};

class MdcRecMctrk2hepCol {
public:
   static vector<MdcRec_mctrk2hep>* getMdcRecMctrk2hepCol(void);              
private:
   static vector<MdcRec_mctrk2hep>* s_col;            
};
 
//
//*** additional tables for track of MDC *** 
//
class MdcTrk;
class MdcRec_trk_add {
public:
   MdcRec_trk_add() : body(0),mother(0),
     daughter(0), rectrk(0) { };
   ~MdcRec_trk_add( );
   MdcRec_trk_add(const MdcRec_trk_add& rhs) : mother(rhs.mother), 
     body(rhs.body),daughter(rhs.daughter), rectrk(rhs.rectrk)  {
     id = rhs.id;
     quality = rhs.quality;
     kind = rhs.kind;
     decision = rhs.decision;
     stat = rhs.stat; 
     for ( int i=0; i<3; i++ ) {
	likelihood[i] = rhs.likelihood[i];
     }      
   }
   
   MdcRec_trk_add& operator =(const MdcRec_trk_add& rhs) {

    if(this == &rhs)
	return *this;
     id = rhs.id;
     quality = rhs.quality;
     kind = rhs.kind;
     decision = rhs.decision;
     stat = rhs.stat; 
     rectrk = rhs.rectrk;
     body = rhs.body;
     mother = rhs.mother;
     daughter = rhs.daughter; 
     for ( int i=0; i<3; i++ ) {
	likelihood[i] = rhs.likelihood[i];
     }      
     return *this;
   }
   
public:
  int id;
  int quality;
  int kind;
  MdcRec_trk* body;
  MdcRec_trk* mother;
  MdcRec_trk* daughter;
  int decision;
  float likelihood[3];
  int stat;
  MdcTrk* rectrk;
};

class MdcRecTrkAddCol {
public:
   static vector<MdcRec_trk_add>* getMdcRecTrkAddCol(void);              
private:
   static vector<MdcRec_trk_add>* s_col;            
};
 
//
//*** mdctime run-dependent offset  ***  
//
class MdcRec_timeofs {
public:
  MdcRec_timeofs() {};
public:
  int id;
  float offset;
};

class MdcRecTimeofsCol {
public:
   static vector<MdcRec_timeofs>* getMdcRecTimeofsCol(void);              
private:
   static vector<MdcRec_timeofs>* s_col;            
};
 
//
//*** Tof_xtime2 information with tighter cuts ***
//*** from Tof_mdctime                         ***
//
class Mdc_xtime2 {
public:
  Mdc_xtime2() {};
public:
  int id;
  float xtime;
  float tofhit;
  float trig32;
  float evtime;
  float tdcstop;
};

class MdcXtime2Col {
public:
   static vector<Mdc_xtime2>* getMdcXtime2Col(void);              
private:
   static vector<Mdc_xtime2>* s_col;            
};

#endif // _MdcTables_h_

