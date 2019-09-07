//
//  BesIII Tof Reconstruction 
//  Class: RecETofHit 
//  (Endcap Tof Hit)   
//  Created by Linli Jiang (USTC) 2005/09/04
//
#ifndef RECETOFHIT_H
#define RECETOFHIT_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"


extern const  CLID &CLID_RecETofHit;

class RecETofHit : virtual public ContainedObject {
 public:
  RecETofHit();//{}
  virtual ~RecETofHit(){}
  virtual const CLID& clID()  const { return  RecETofHit::classID();}
  static  const CLID& classID() { return CLID_RecETofHit; }			
 public:
  int    getId()       const  { return _Id; } 
  int    getTrk()      const  { return _trk; }
  Identifier getTofId()const  { return _tofId; } 
  int    getMod()      const  { return _mod; } 
  int    getQual()     const  { return _qual; }
  double getRHit()     const  { return _rHit;}  
  double getdRHit()    const  { return _drHit;} 
  double getPhHit()    const  { return _phHit;} 
  double getThTrk()    const  { return _thTrk;} 
  double getPhTrk()    const  { return _phTrk;} 
  double getPh()       const  { return _ph;} 
  double getP()        const  { return _p;}
  double getPath()     const  { return _path; } 
  double getTime()     const  { return _time; } 
  double getTof()      const  { return _tof; }  
  double getETof()     const  { return _etof; } 
  double getExpTof(int im)   const  { return _exptof[im]; }
  double getTt()       const  { return _tt; }   
  double getETt()      const  { return _ett; }  
  double getTm()       const  { return _tm; }   
  double getETm()      const  { return _etm; }  
  double getTr()       const  { return _tr; }   
  double getEtr()      const  { return _etr; }  
  double getQ()        const  { return _q; }    
  double getEQ()      const   { return _eq; } 
  double getZTDC()     const  { return _zTDC;}
  double getZADC()     const  { return _zADC;}

 public:
  void  setZTDC(double zTDC){ _zTDC=zTDC; }
  void  setZADC(double zADC) { _zADC=zADC;}
  void  setId(int id){ _Id=id; }
  void  setP(double p){ _p=p; }
  void  setTrk(int trk){ _trk=trk; } 
  void  setTofID(Identifier tofId){ _tofId=tofId; }
  void  setMod(int mod){ _mod=mod; } 
  void  setQual(int qual){ _qual=qual; } 
  void  setRHit(double rHit){ _rHit=rHit; }
  void  setdRHit(double drHit){ _drHit=drHit; }
  void  setPhHit(double phHit){ _phHit=phHit; }
  void  setThTrk(double thTrk){ _thTrk=thTrk; }
  void  setPhTrk(double phTrk){ _phTrk=phTrk; }
  void  setPh(double ph){ _ph=ph; }
  void  setPath(double path){ _path=path; }
  void  setTime(double time){ _time=time; }
  void  setTof(double tof){ _tof=tof; }
  void  setETof(double etof){ _etof=etof; }
  void  setExpTof(double exptof,int im){ _exptof[im]=exptof; }
  void  setTt(double tt){ _tt=tt; }
  void  setETt(double ett){ _ett=ett; }
  void  setTm(double tm){ _tm=tm; }
  void  setETm(double etm){ _etm=etm; }
  void  setTr(double tr){ _tr=tr; }
  void  setETr(double etr){ _etr=etr; }
  void  setQ(double q){ _q=q; }
  void  setEQ(double eq){ _eq=eq; }

 private:
  Identifier _tofId;
  int    _Id,_trk,_mod,_qual;
  double _rHit,_drHit,_phHit,_thTrk,_phTrk,_ph,_path,_time;
  double _tof,_etof;
  double _exptof[5];
  double _tt,_ett,_tm,_etm;
  double _tr,_etr,_q,_eq;
  double _zTDC,_zADC;
  double _p;
};

typedef ObjectVector<RecETofHit> RecETofHitCol;

#endif 
