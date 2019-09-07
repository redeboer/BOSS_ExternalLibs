#include "TofRecEvent/RecBTofHit.h"
/*
bool myless(RecBTofHit& a,RecBTofHit& b){
  return ((a.getQ1()+a.getQ2())<(b.getQ1()+b.getQ2()));
}
bool less_RecBTofHit(RecBTofHit* a,RecBTofHit* b){
  return((a->getQ1()+a->getQ2())<(b->getQ1()+b->getQ2()));
}
*/
RecBTofHit::RecBTofHit(){
 _tofID=0;
  _Id=_mod=_qual=_trk=-1;
   _zHit=_zTDC=_zADC=_dzHit=_phHit=_thTrk=_phTrk=_ph=_p=_path=_time=-1.;  
    _tof=_etof=-1.;
    _exptof[0]=-1.;
    _exptof[1]=-1.;_exptof[2]=-1.;_exptof[3]=-1.;_exptof[4]=-1.;

    _tt1=_tt2=_ett1=_ett2=-1.;
    _tm1=_tm2=_etm1=_etm2=-1.;
    _tr1=_tr2=_etr1=_etr2=-1.;
    _q1=_q2=_eq1=_eq2=-1.;
 
   m_next=NULL;
}
