#ifndef BTOF_CAL_H
#define BTOF_CAL_H 

const unsigned int nBarPar    = 10;  
const unsigned int nBarParOff = 20;

class BTofCal{
 public:
   BTofCal() {}
   ~BTofCal(){}
 public:  
  double getP1(int idx)          const{ return m_p1[idx];    }
  double getP2(int idx)          const{ return m_p2[idx];    }
  //  double getPOff1(int idx)       const{ return m_poff1[idx]; }
  //  double getPOff2(int idx)       const{ return m_poff2[idx]; }

 public:  
  void   setP1(int idx,double p1)         { m_p1[idx]    = p1;      }
  void   setP2(int idx,double p2)         { m_p2[idx]    = p2;      }
  //  void   setPOff1(int idx,double poff1)   { m_poff1[idx] = poff1;   }
  //  void   setPOff2(int idx,double poff2)   { m_poff2[idx] = poff2;   }

 private:
  double m_p1[nBarPar];        // left time calibration constant 
  double m_p2[nBarPar];        // right time calibration constant 
  //  double m_poff1[nBarParOff];  // left time calibration constant 
  //  double m_poff2[nBarParOff];  // right time calibration constant 
};
#endif             
