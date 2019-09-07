#ifndef BTOF_CAL_H
#define BTOF_CAL_H 

const unsigned int nBarAtten  = 8;
const unsigned int nBarSpeed  = 2;
const unsigned int nBarPar    = 10;
const unsigned int nBarParOff = 20;
const unsigned int nBarSigma  = 5;
const unsigned int nBarSigCnt = 10;
  
class BTofCal{
 public:
   BTofCal() {}
   ~BTofCal(){}

 public:
  double getAtten(int idx)       const{ return m_atten[idx]; }
  double getVeff(int idx)        const{ return m_veff[idx];  }
  double getP1(int idx)          const{ return m_p1[idx];    }
  double getP2(int idx)          const{ return m_p2[idx];    }
  double getPOff1_bunch0(int idx)       const{ return m_poff1_bunch0[idx]; }
  double getPOff2_bunch0(int idx)       const{ return m_poff2_bunch0[idx]; }
  double getPOff1_bunch1(int idx)       const{ return m_poff1_bunch1[idx]; }
  double getPOff2_bunch1(int idx)       const{ return m_poff2_bunch1[idx]; }
  double getPOff1_bunch2(int idx)       const{ return m_poff1_bunch2[idx]; }
  double getPOff2_bunch2(int idx)       const{ return m_poff2_bunch2[idx]; }
  double getPOff1_bunch3(int idx)       const{ return m_poff1_bunch3[idx]; }
  double getPOff2_bunch3(int idx)       const{ return m_poff2_bunch3[idx]; }
  double getFPLeft(int idx)      const{ return m_fpleft[idx];    }
  double getFPRight(int idx)     const{ return m_fpright[idx];   }
  double getFPCounter(int idx)   const{ return m_fpcounter[idx]; }

 public:
  void   setAtten(int idx,double atten)   { m_atten[idx] = atten;   }
  void   setVeff(int idx,double veff)     { m_veff[idx]  = veff;    }
  void   setP1(int idx,double p1)         { m_p1[idx]    = p1;      }
  void   setP2(int idx,double p2)         { m_p2[idx]    = p2;      }
  void   setPOff1_bunch0(int idx,double poff1_bunch0)  { m_poff1_bunch0[idx] = poff1_bunch0;   }
  void   setPOff2_bunch0(int idx,double poff2_bunch0)  { m_poff2_bunch0[idx] = poff2_bunch0;   }
  void   setPOff1_bunch1(int idx,double poff1_bunch1)  { m_poff1_bunch1[idx] = poff1_bunch1;   }
  void   setPOff2_bunch1(int idx,double poff2_bunch1)  { m_poff2_bunch1[idx] = poff2_bunch1;   }
  void   setPOff1_bunch2(int idx,double poff1_bunch2)  { m_poff1_bunch2[idx] = poff1_bunch2;   }
  void   setPOff2_bunch2(int idx,double poff2_bunch2)  { m_poff2_bunch2[idx] = poff2_bunch2;   }
  void   setPOff1_bunch3(int idx,double poff1_bunch3)  { m_poff1_bunch3[idx] = poff1_bunch3;   }
  void   setPOff2_bunch3(int idx,double poff2_bunch3)  { m_poff2_bunch3[idx] = poff2_bunch3;   }
  void   setFPLeft(int idx,double fp1)    { m_fpleft[idx]    = fp1; }
  void   setFPRight(int idx,double fp2)   { m_fpright[idx]   = fp2; }
  void   setFPCounter(int idx,double fp3) { m_fpcounter[idx] = fp3; }

 private:
  double m_atten[nBarAtten];   // effective attenuation length in Tof (cm)
  double m_veff[nBarSpeed];    // effective velocity of light in Tof (cm/ns)
  double m_p1[nBarPar];        // left time calibration constant 
  double m_p2[nBarPar];        // right time calibration constant 
  double m_poff1_bunch0[nBarParOff];  // left time calibration constant of bunch0
  double m_poff2_bunch0[nBarParOff];  // right time calibration constant of bunch0
  double m_poff1_bunch1[nBarParOff];  // left time calibration constant of bunch1
  double m_poff2_bunch1[nBarParOff];  // right time calibration constant of bunch1
  double m_poff1_bunch2[nBarParOff];  // left time calibration constant of bunch2
  double m_poff2_bunch2[nBarParOff];  // right time calibration constant of bunch2
  double m_poff1_bunch3[nBarParOff];  // left time calibration constant of bunch3
  double m_poff2_bunch3[nBarParOff];  // right time calibration constant of bunch3
  double m_fpleft[nBarSigma];  // left sigma vs z
  double m_fpright[nBarSigma]; // right sigma vs z
  double m_fpcounter[nBarSigCnt];  // weighted sigma vs z

};
#endif             
