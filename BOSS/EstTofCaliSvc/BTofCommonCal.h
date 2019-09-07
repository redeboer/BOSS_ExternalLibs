#ifndef BTOFCOMMON_CAL_H
#define BTOFCOMMON_CAL_H 
  
const unsigned int nBarOffset = 2;

class BTofCommonCal{
 public:
   BTofCommonCal() {}
   ~BTofCommonCal(){}
 public:
  double getOffset(int im) const{ return m_offset[im]; }

 public:
  void   setOffset(int idx, double offset) { m_offset[idx] = offset; }

 private:
  double m_offset[2];
};
#endif             
