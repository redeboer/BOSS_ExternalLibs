#ifndef _TrkTables_h_
#define _TrkTables_h_

#include <string>
#include <iostream>   
#include <vector>           

#include "MdcTables/MdcTables.h"

using namespace std;

//
//*** Info. on tracks reconstructed in KalFitter. For each track 5 slots 
//*** are allocated to handle different mass  hypotheses.
//
class MdcTrk_global;  // what is the purpose of this class ???
class MdcTrk_localz;
class MdcTrk_localf;
class MdcTrk_locall;
class MdcRec_trk;
class MdcTrk {
public:
  MdcTrk() :prekal(0) {
   for(int i =0; i<5; i++) {
      glob[i]=0;
      zero[i]=0;
      first[i]=0;
      last[i]=0;
   }
  }
   
  MdcTrk(const MdcTrk& rhs) {
    for( int i=0; i<5; i++) {
      glob[i] = rhs.glob[i];
      zero[i] = rhs.zero[i];
      first[i] = rhs.first[i];
      last[i] = rhs.last[i];
      }
    prekal = rhs.prekal;
  }
   
   MdcTrk& operator =(const MdcTrk& rhs) {

    if(this == &rhs)
	return *this;
    for( int i=0; i<5; i++) {
      glob[i] = rhs.glob[i];
      zero[i] = rhs.zero[i];
      first[i] = rhs.first[i];
      last[i] = rhs.last[i];
      }
    prekal = rhs.prekal;

    return *this;
   }   
      

public:
  int id;
  MdcTrk_global * glob[5];
  MdcTrk_localz * zero[5];
  MdcTrk_localf * first[5];
  MdcTrk_locall * last[5];
  MdcRec_trk * prekal;
private:
  

};

class MdcTrkCol {
public:
   static vector<MdcTrk>* getMdcTrkCol(void);
private:
   static vector<MdcTrk>* s_col;
};

//
//*** Track fit results indep. of position. ***
//
class MdcTrk_global {
public:
  MdcTrk_global() {
     id = 0;
     mass = 0.0;
     pathl = 0.0;
     tof = 0.0;
     pathl_rd = 0.0;
     pathl_ab = 0.0;

     for ( int i=0; i<5; i++ ) {
	if( i<2 ) stat[i] = 0;
	if( i<3 ) {
	   chisq[i] = 999;
	   ndf[i] = 0;	
           hit[i] = 0;
	}
        nhits[i] = 0;
     }
  }
   
  MdcTrk_global(const MdcTrk_global& rhs) {  

     id = rhs.id;
     mass = rhs.mass;
     pathl = rhs.pathl;
     tof = rhs.tof;
     pathl_rd = rhs.pathl_rd;
     pathl_ab = rhs.pathl_ab;

     for ( int i=0; i<5; i++ ) {
	if( i<2 ) stat[i] = rhs.stat[i];	
	if( i<3 ) {
	   chisq[i] = rhs.chisq[i];
	   ndf[i] = rhs.ndf[i];	
           hit[i] = rhs.hit[i];
	}
        nhits[i] = rhs.nhits[i];
     }
  }   
         
  MdcTrk_global& operator =(const MdcTrk_global& rhs) {

    if(this == &rhs)
	return *this;
     id = rhs.id;
     mass = rhs.mass;
     pathl = rhs.pathl;
     tof = rhs.tof;
     pathl_rd = rhs.pathl_rd;
     pathl_ab = rhs.pathl_ab;

     for ( int i=0; i<5; i++ ) {
	if( i<2 ) stat[i] = rhs.stat[i];	
	if( i<3 ) {
	   chisq[i] = rhs.chisq[i];
	   ndf[i] = rhs.ndf[i];	
           hit[i] = rhs.hit[i];
	}
        nhits[i] = rhs.nhits[i];
     }

  return *this;
   }   
         
public:
  int id;
  float mass;
  float chisq[3];
  int ndf[3];
  float pathl;
  float tof;
  float pathl_rd;
  float pathl_ab;
  int stat[2];
  int nhits[5];
  int hit[3];
};

class MdcTrkGlobalCol {
public:
   static vector<MdcTrk_global>* getMdcTrkGlobalCol(void);
private:
   static vector<MdcTrk_global>* s_col;
};
 
//
//*** Track fit results at near the coord. origin. ***
//
class MdcTrk_localz {
public:
  MdcTrk_localz() {};
  MdcTrk_localz(const MdcTrk_localz& rhs) {
     id = rhs.id;
     for ( int i=0; i<15; i++ ) {
	if( i<3 ) pivot[i] = rhs.pivot[i];	
	if( i<5 ) helix[i] = rhs.helix[i];
        error[i] = rhs.error[i];
     }
  }
   
  MdcTrk_localz& operator =(const MdcTrk_localz& rhs) {

    if(this == &rhs)
	return *this;
     id = rhs.id;
     for ( int i=0; i<15; i++ ) {
	if( i<3 ) pivot[i] = rhs.pivot[i];	
	if( i<5 ) helix[i] = rhs.helix[i];
        error[i] = rhs.error[i];
     }

  return *this;
   }      
public:
  int id;
  float pivot[3];
  float helix[5];
  float error[15];
};

class MdcTrkLocalzCol {
public:
   static vector<MdcTrk_localz>* getMdcTrkLocalzCol(void);
private:
   static vector<MdcTrk_localz>* s_col;
};
 
//
//*** Track fit results at the first hit point. ***
//
class MdcTrk_localf {
public:
  MdcTrk_localf() {};
  MdcTrk_localf(const MdcTrk_localf& rhs) {
     id = rhs.id;
     for ( int i=0; i<15; i++ ) {
	if( i<3 ) pivot[i] = rhs.pivot[i];	
	if( i<5 ) helix[i] = rhs.helix[i];
        error[i] = rhs.error[i];
     }
  }   
   
  MdcTrk_localf& operator =(const MdcTrk_localf& rhs) {

    if(this == &rhs)
	return *this;
     id = rhs.id;
     for ( int i=0; i<15; i++ ) {
	if( i<3 ) pivot[i] = rhs.pivot[i];	
	if( i<5 ) helix[i] = rhs.helix[i];
        error[i] = rhs.error[i];
     }

  return *this;
   }         
public:
  int id;
  float pivot[3];
  float helix[5];
  float error[15];
};

class MdcTrkLocalfCol {
public:
   static vector<MdcTrk_localf>* getMdcTrkLocalfCol(void);
private:
   static vector<MdcTrk_localf>* s_col;
};
 
//
//*** Track fit results at the last hit point. ***
//
class MdcTrk_locall {
public:
  MdcTrk_locall() {};
  MdcTrk_locall(const MdcTrk_locall& rhs) {
     id = rhs.id;
     for ( int i=0; i<15; i++ ) {
	if( i<3 ) pivot[i] = rhs.pivot[i];	
	if( i<5 ) helix[i] = rhs.helix[i];
        error[i] = rhs.error[i];
     }
  }   
  MdcTrk_locall& operator =(const MdcTrk_locall& rhs) {

    if(this == &rhs)
	return *this;
     id = rhs.id;
     for ( int i=0; i<15; i++ ) {
	if( i<3 ) pivot[i] = rhs.pivot[i];	
	if( i<5 ) helix[i] = rhs.helix[i];
        error[i] = rhs.error[i];
     }

  return *this;
   }         
public:
  int id;
  float pivot[3];
  float helix[5];
  float error[15];
};

class MdcTrkLocallCol {
public:
   static vector<MdcTrk_locall>* getMdcTrkLocallCol(void);
private:
   static vector<MdcTrk_locall>* s_col;
};
 
//
//*** V0 candidates. ***
//
class MdcTrk_v0 {
public:
  MdcTrk_v0() {};
public:
  int id;
  int kind;
  MdcTrk * dau[2];
  float vtx[3];
  float P[5];
  float fl;
  float dz;
  int type;
};

class MdcTrkV0Col {
public:
   static vector<MdcTrk_v0>* getMdcTrkV0Col(void);
private:
   static vector<MdcTrk_v0>* s_col;
};
 
//
//*** V0 candidates; V0 info. after vtx-constrained fit***
//
class MdcTrk_v0_daughters;
class MdcTrk_v02 {
public:
  MdcTrk_v02() {};
public:
  int id;
  int kind;
  MdcTrk * daut[2];
  float P[4];
  float vtx[3];
  float dz;
  float chisq;
  int type;
  MdcTrk_v0_daughters * dau;
};

class MdcTrkV02Col {
public:
   static vector<MdcTrk_v02>* getMdcTrkV02Col(void);
private:
   static vector<MdcTrk_v02>* s_col;
};
 
//
//*** Track params. and error matrix of V0 daughters before the    ***
//*** vtx-constraint fit. They are defined at the V0 vtx-position. ***
//
class MdcTrk_v0_daughters {
public:
  MdcTrk_v0_daughters() {};
public:
  int id;
  float helix_p[5];
  float helix_m[5];
  float error_p[15];
  float error_m[15];
};

class MdcTrkV0DaughtersCol {
public:
   static vector<MdcTrk_v0_daughters>* getMdcTrkV0DaughtersCol(void);
private:
   static vector<MdcTrk_v0_daughters>* s_col;
};
 
//
//*** Path length in MDC for dEdx pid. ***
//
class MdcTrk_pathl {
public:
  MdcTrk_pathl():trk(0) {};
   MdcTrk_pathl& operator=( MdcTrk_pathl& rhs) {
     id = rhs.id;
     layer = rhs.layer;
     trk = rhs.trk;      
     xin = rhs.xin;
     yin = rhs.yin;
     zin = rhs.zin;
     xout = rhs.xout;      
     yout = rhs.yout;
     zout = rhs.zout;
     pathl = rhs.pathl; 
     px = rhs.px;
     py = rhs.py;
     pz = rhs.pz;
     status = rhs.status;
     return *this;      
   };
  
//   MdcTrk_pathl(const MdcTrk_pathl& rhs){
//      this = rhs;
  // }
public:
  int id;
  int layer;
  MdcTrk * trk;
  float xin;
  float yin;
  float zin;
  float xout;
  float yout;
  float zout;
  float pathl;
  float px;
  float py;
  float pz;
  int status;
 private:
   

};

class MdcTrkPathlCol {
public:
   static vector<MdcTrk_pathl>* getMdcTrkPathlCol(void);
private:
   static vector<MdcTrk_pathl>* s_col;
};
 
//
//*** Info. on tracks found either in MDC+SVD combined system or in SVD only. ***
//
class MdcRec_trk_add;
class MdcTrk_svd {
public:
  MdcTrk_svd() {};
public:
  int id;
  float Helix[5];
  MdcRec_trk_add * mdc_trk;
  //   svd_cluster[24];
  int Status;
};

class MdcTrkSvdCol {
public:
   static vector<MdcTrk_svd>* getMdcTrkSvdCol(void);
private:
   static vector<MdcTrk_svd>* s_col;
};
 
//
//*** Tof results for diff mass assumptions ***
//
class MdcTrk_tof {
public:
  MdcTrk_tof() : rectrk(0) {};
  MdcTrk_tof& operator=(MdcTrk_tof& rhs) {
    id = rhs.id ;
    rectrk = rhs.rectrk;
    tof_k = rhs.tof_k;
    tof_p = rhs.tof_p;
    return *this;     
     
  };
public:
  int id;
  MdcTrk * rectrk;
  float tof;
  float tof_k;
  float tof_p;
private:
//  MdcTrk_tof(MdcTrk_tof &rhs);

   
};

class MdcTrkTofCol {
public:
   static vector<MdcTrk_tof>* getMdcTrkTofCol(void);
private:
   static vector<MdcTrk_tof>* s_col;
};

#endif // _TrkTables_h_

