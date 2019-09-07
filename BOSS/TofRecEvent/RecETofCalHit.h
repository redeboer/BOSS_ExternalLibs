/* 
 * Class : RecETofCalHit
 * This class is a Event data model for Reconstrcted input for Endcap Tof calibration
 * Created  by  Lin li Jiang (USTC)     2005/12/12  17:42:22 
 * 
 */
#ifndef RECETOFCALHIT_H
#define RECETOFCALHIT_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRef.h"
#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"

extern const  CLID &CLID_RecETofCalHit;


class RecETofCalHit : virtual public ContainedObject {

public:
  RecETofCalHit();//{} 
  virtual ~RecETofCalHit(){} 

  virtual const CLID& clID()  const { return  RecETofCalHit::classID();}
  static  const CLID& classID() { return CLID_RecETofCalHit; }

public:
  int         run()      const {  return  m_run;   }
  int         event()    const {  return  m_event; }
  int         mod()      const {  return  m_imod;  }
  int         qual()     const {  return  m_qual;  }
  double      tpred()    const {  return  m_tpred; }
  double      tdc()      const {  return  m_tdc;   }
  double      adc()      const {  return  m_adc;   }
  double      rHit()     const {  return  m_rHit;  }
  double      drHit()    const {  return  m_drHit; }  
  double      deltaPhi() const {  return  m_deltaPhi; }
  double      cosTheta() const {  return  m_costheta; }
  double      p()        const {  return  m_p;     }
  double      Q()        const {  return  m_Q;     }
  double      path()     const {  return  m_path;  }
//add 9/23/06
public:
  double  tpred(int idx) const { if(idx<0||idx>5) idx=0;return m_texp[idx];}  
  void    setTpred(int idx,double t) { if(idx<0||idx>5) idx=0;m_texp[idx]=t;}
private:
  double m_texp[5];
//end add
public: 
  void  setRun(int run )             { m_run      = run;      }
  void  setEvent(int event )         { m_event    = event;    }
  void  setMod(int  imod )           { m_imod     = imod;     }
  void  setQual(int qual)            { m_qual     = qual;     }
  void  setTpred(double tpred)       { m_tpred    = tpred;    }
  void  setTdc(double tdc)           { m_tdc      = tdc;      }
  void  setAdc(double adc)           { m_adc      = adc;      }
  void  setRHit(double rHit)         { m_rHit     = rHit;     }
  void  setdRHit(double drHit)       { m_drHit    = drHit;    } 
  void  setDeltaPhi(double deltaPhi) { m_deltaPhi = deltaPhi; } 
  void  setcosTheta(double cost)     { m_costheta = cost;     }
  void  setP(double p)               { m_p        = p;        }
  void  setQ(double q)               { m_Q        = q;        }
  void  setPath(double l)            { m_path     = l;        }
private:
  int    m_run;
  int    m_event;
  int    m_imod;   
  int    m_qual;
  double m_tpred;
  double m_tdc;
  double m_adc;
  double m_rHit;
  double m_drHit;
  double m_deltaPhi;
  double m_costheta;
  double m_p;
  double m_Q;
  double m_path;
};

typedef ObjectVector<RecETofCalHit> RecETofCalHitCol;

#endif //RECETOFCalHit_H


