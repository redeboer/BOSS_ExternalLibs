#ifndef ETF_CAL_H
#define ETF_CAL_H 

const unsigned int nEtfSpeed  = 4;
const unsigned int nEtfPar    = 20;
  
class EtfCal{
 public:
  EtfCal() {}
  ~EtfCal(){}

 public:
  double getVeff(int idx)              const{ return m_veff[idx];  }
  double getP(int idx)                 const{ return m_p[idx];     }
  double getP1(int idx)                const{ return m_p1[idx];    }
  double getP2(int idx)                const{ return m_p2[idx];    }

 public:
  void   setVeff(int idx,double veff)       { m_veff[idx]  = veff; }
  void   setP(int idx,double p)             { m_p[idx]     = p;    }
  void   setP1(int idx,double p1)           { m_p1[idx]    = p1;   }
  void   setP2(int idx,double p2)           { m_p2[idx]    = p2;   }

 private:
  double m_veff[nEtfSpeed];    // effective velocity of light in Tof (cm/ns)
  double m_p[nEtfPar];         // combined time calibration constant 
  double m_p1[nEtfPar];        // left time calibration constant 
  double m_p2[nEtfPar];        // right time calibration constant 

};
#endif             
