/* 
 * Class : RecBTofCalHit
 * This class is a Event data model for Reconstrcted input for Barrel Tof calibration
 * Created  by  Lin li Jiang (USTC)     2005/3/24   05:21 
 * Changed  by  Lin li Jiang (USTC)     2005/12/12  19:38 
 * 
 */
#ifndef RECBTOFCALHIT_H
#define RECBTOFCALHIT_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRef.h"
#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"

extern const  CLID &CLID_RecBTofCalHit;


class RecBTofCalHit : virtual public ContainedObject {

public:
  RecBTofCalHit();//{} 
  virtual ~RecBTofCalHit(){} 

  virtual const CLID& clID()  const { return  RecBTofCalHit::classID();}
  static  const CLID& classID() { return CLID_RecBTofCalHit; }

public:
  int         run()      const {  return  m_run;      }
  int         event()    const {  return  m_event;    }
  int         mod()      const {  return  m_imod;     }
  int         qual()     const {  return  m_qual;     }
  double      tpred()    const {  return  m_tpred;    }
  double      tdc1()     const {  return  m_tdc1;     }
  double      tdc2()     const {  return  m_tdc2;     }
  double      adc1()     const {  return  m_adc1;     }
  double      adc2()     const {  return  m_adc2;     }
  double      zHit()     const {  return  m_zHit;     }
  double      dzHit()    const {  return  m_dzHit;    }
  double      deltaPhi() const {  return  m_deltaPhi; }
  double      sinTheta() const {  return  m_sintheta; }
  double      p()        const {  return  m_p;        }
  double      Q()        const {  return  m_Q;        }
  double      path()     const {  return  m_path;     }
//add 9/23/06
public:
  double     tpred(int index) const { if(index<0||index>=5) index=0;return m_texp[index];}
  void       setTpred(int idx,double t) {if(idx<0||idx>=5) idx=0; m_texp[idx]=t; }
private:
  double m_texp[5];//0 e-,1 muon,2 Kaon,3 pion,4 proton
//end add
public: 
  void  setRun(int run)              { m_run      = run;      }
  void  setEvent(int event)          { m_event    = event;    }
  void  setMod(int imod)             { m_imod     = imod;     }
  void  setQual(int qual)            { m_qual     = qual;     }
  void  setTpred(double tpred)       { m_tpred    = tpred;    }
  void  setTdc1(double tdc1)         { m_tdc1     = tdc1;     }
  void  setTdc2(double tdc2)         { m_tdc2     = tdc2;     }
  void  setAdc1(double adc1)         { m_adc1     = adc1;     }
  void  setAdc2(double adc2)         { m_adc2     = adc2;     } 
  void  setZHit(double zHit)         { m_zHit     = zHit;     }
  void  setdZHit(double dzHit)       { m_dzHit    = dzHit;    }
  void  setDeltaPhi(double deltaPhi) { m_deltaPhi = deltaPhi; }
  void  setsinTheta(double sint)     { m_sintheta = sint;     }
  void  setP(double p)               { m_p        = p;        }
  void  setQ(double q)               { m_Q        = q;        }
  void  setPath(double l)            { m_path     = l;        }
private:
  int    m_run;
  int    m_event;
  int    m_imod;
  int    m_qual;
  double m_tpred;
  double m_tdc1;
  double m_tdc2;
  double m_adc1;
  double m_adc2;
  double m_zHit;
  double m_dzHit;
  double m_deltaPhi;
  double m_sintheta;
  double m_p;
  double m_Q;
  double m_path;
public:
  RecBTofCalHit* next() const { return m_bch;}
  void setnext(RecBTofCalHit* n) { m_bch=n;}
private:
  RecBTofCalHit* m_bch;
};

typedef ObjectVector<RecBTofCalHit> RecBTofCalHitCol;

#endif //RECBTOFCalHit_H


