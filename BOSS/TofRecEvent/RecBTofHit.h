//
//  BesIII Tof Reconstruction 
//  Class: RecBTofHit 
//  (Barrel Tof Hit)   
//  Created by Linli Jiang (USTC) 2005/09/04
//
#ifndef RECBTOFHIT_H
#define RECBTOFHIT_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"


extern const  CLID &CLID_RecBTofHit;

class RecBTofHit : virtual public ContainedObject {
 public:
    RecBTofHit();//{}
    virtual ~RecBTofHit(){}
    virtual const CLID& clID()  const { return  RecBTofHit::classID();}
    static  const CLID& classID() { return CLID_RecBTofHit; }			
 public:
  int    getId()       const  { return _Id; }
  int    getTrk()      const  { return _trk; }
  Identifier getTofID()const  { return _tofID; }
  int    getMod()      const  { return _mod; }
  int    getQual()     const  { return _qual; }  
  double getZHit()     const  { return _zHit;}
  double getdZHit()    const  { return _dzHit;}
  double getZTDC()     const  { return _zTDC;}
  double getZADC()     const  { return _zADC;}
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
  double getTt1()      const  { return _tt1; }
  double getTt2()      const  { return _tt2; }
  double getETt1()     const  { return _ett1; }
  double getETt2()     const  { return _ett2; }
  double getTm1()      const  { return _tm1; }
  double getTm2()      const  { return _tm2; }
  double getETm1()     const  { return _etm1; }
  double getETm2()     const  { return _etm2; }
  double getTr1()      const  { return _tr1; }
  double getTr2()      const  { return _tr2; }
  double getEtr1()     const  { return _etr1; }
  double getEtr2()     const  { return _etr2; }
  double getQ1()       const  { return _q1; } 
  double getQ2()       const  { return _q2; }
  double getEQ1()      const  { return _eq1; } 
  double getEQ2()      const  { return _eq2; }  

public:
  void  setId(int id){ _Id=id; }
  void  setTrk(int trk){ _trk=trk; }  
  void  setTofID(Identifier tofID){ _tofID=tofID; }
  void  setMod(int mod){ _mod=mod; }
  void  setQual(int qual){ _qual=qual; } 
  void  setZHit(double zHit){ _zHit=zHit; }
  void  setdZHit(double dzHit){ _dzHit=dzHit; }
  void  setZTDC(double zTDC){ _zTDC=zTDC; }
  void  setZADC(double zADC) { _zADC=zADC;}
  void  setPhHit(double phHit){ _phHit=phHit; }
  void  setThTrk(double thTrk){ _thTrk=thTrk; }
  void  setPhTrk(double phTrk){ _phTrk=phTrk; }
  void  setPh(double ph){ _ph=ph; }
  void  setP(double p){ _p=p; }
  void  setPath(double path){ _path=path; }
  void  setTime(double time){ _time=time; }
  void  setTof(double tof){ _tof=tof; }
  void  setETof(double etof){ _etof=etof; }
  void  setExpTof(double exptof,int im){ _exptof[im]=exptof; } 
  void  setTt1(double tt1){ _tt1=tt1; }
  void  setTt2(double tt2){ _tt2=tt2; }
  void  setETt1(double ett1){ _ett1=ett1; }
  void  setETt2(double ett2){ _ett2=ett2; }
  void  setTm1(double tm1){ _tm1=tm1; }
  void  setTm2(double tm2){ _tm2=tm2; }
  void  setETm1(double etm1){ _etm1=etm1; }
  void  setETm2(double etm2){ _etm2=etm2; }  
  void  setTr1(double tr1){ _tr1=tr1; }
  void  setTr2(double tr2){ _tr2=tr2; }
  void  setETr1(double etr1){ _etr1=etr1; }
  void  setETr2(double etr2){ _etr2=etr2; }
  void  setQ1(double q1){ _q1=q1; }
  void  setQ2(double q2){ _q2=q2; }
  void  setEQ1(double eq1){ _eq1=eq1; }
  void  setEQ2(double eq2){ _eq2=eq2; }  
//  friend bool operator<(RecBTofHit* a,RecBTofHit* b);
public://operator override
  bool operator<(const RecBTofHit& rhs) const {
       //std::cout<<"be called once"<<std::endl;
       return (_q1+_q2)<(rhs.getQ1()+rhs.getQ2());
  }
  bool operator>(const RecBTofHit& rhs) const {
       //std::cout<<"be called once"<<std::endl;
       return (_q1+_q2)>(rhs.getQ1()+rhs.getQ2());
  }

  bool operator<(RecBTofHit* rhs) const{
//      std::cout<<"be call once"<<std::endl;
       return (_q1+_q2)<(rhs->getQ1()+rhs->getQ2());
  }
   bool operator>(RecBTofHit* rhs) const{
      std::cout<<"be call once"<<std::endl;
       return (_q1+_q2)>(rhs->getQ1()+rhs->getQ2());
  }
public:
  const RecBTofHit* next() const { return m_next;}//if first layer,pointer to the second one,if second one,to the first one
  void  setnext(RecBTofHit* next) { m_next=next;}
private:
  RecBTofHit* m_next;
private:
  Identifier _tofID;
  int    _Id,_mod,_qual,_trk;
  double _zHit,_zTDC,_zADC,_dzHit,_phHit,_thTrk,_phTrk,_ph,_p,_path,_time;  
  double _tof,_etof;
  double _exptof[5];
  double _tt1,_tt2,_ett1,_ett2;
  double _tm1,_tm2,_etm1,_etm2;
  double _tr1,_tr2,_etr1,_etr2;
  double _q1,_q2,_eq1,_eq2;
};
typedef ObjectVector<RecBTofHit> RecBTofHitCol;

#endif 
