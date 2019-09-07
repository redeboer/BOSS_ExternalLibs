#include "TofRecEvent/RecETofHit.h"
RecETofHit::RecETofHit(){
  _tofId=0;
  _Id=_trk=_mod=_qual=-1;
  _rHit=_drHit=_phHit=_thTrk=_phTrk=_ph=_path=_time=-1.;
  _tof=_etof=-1.;
  _exptof[0]=_exptof[1]=_exptof[2]=_exptof[3]=_exptof[4]=-1.;
  _tt=_ett=_tm=_etm=-1.;
  _tr=_etr=_q=_eq=-1.;
  _zADC=_zTDC=_p=-1.;


}
