#ifndef BTOFCOMMON_CAL_H
#define BTOFCOMMON_CAL_H 

const unsigned int nBarSigCor = 8;
const unsigned int nBarOffset = 2;
  
class BTofCommonCal{
 public:
   BTofCommonCal() {}
   ~BTofCommonCal(){}
 public:
  double getSigmaCorr(int idx)   const{ return m_sigmac[idx]; }
  double getOffset(int idx)      const{ return m_offset[idx]; }

 public:
  void   setSigmaCorr(int idx, double sigmac)   { m_sigmac[idx] = sigmac; }
  void   setOffset(int idx, double offset)      { m_offset[idx] = offset; }

 private:
  double m_sigmac[nBarSigCor];
  double m_offset[nBarOffset];
};
#endif             
