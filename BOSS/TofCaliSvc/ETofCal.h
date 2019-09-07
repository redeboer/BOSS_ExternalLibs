#ifndef ETOF_CAL_H
#define ETOF_CAL_H 

const unsigned int nEndAtten  = 5;
const unsigned int nEndSpeed  = 4;
const unsigned int nEndPar    = 8;
const unsigned int nEndSigma  = 5;

class ETofCal{
 public:
   ETofCal() {}
   ~ETofCal(){}
 public:
  double getAtten(int idx)      const{ return m_atten[idx];     }
  double getVeff(int idx)       const{ return m_veff[idx];      }
  double getP(int idx)          const{ return m_p[idx];         }
  double getFPCounter(int idx)  const{ return m_fpcounter[idx]; } 

 public:
  void   setAtten(int idx,double atten)     { m_atten[idx] = atten;  }
  void   setVeff(int idx,double veff)       { m_veff[idx]  = veff;   }
  void   setP(int idx,double p)             { m_p[idx]     = p;      }
  void   setFPCounter(int idx, double fp)   { m_fpcounter[idx] = fp; }

 private:
  double m_atten[nEndAtten];      // effective attenuation length in Tof (cm)
  double m_veff[nEndSpeed];       // effective velocity of light in Tof (cm/ns)
  double m_p[nEndPar];            // time calibration constant 
  double m_fpcounter[nEndSigma];  // sigma vs z polynomial function
};
#endif             
