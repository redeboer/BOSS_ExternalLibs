#ifndef TZEROOFFSET_CAL_H
#define TZEROOFFSET_CAL_H 
 
class EsTimeOffsetCal{
 public:
   EsTimeOffsetCal() {}
   ~EsTimeOffsetCal(){}
 public:
  double getEsTimeOffset(int idx)    const{ return m_EsTimeOffset[idx];}

 public:
  void   setEsTimeOffset(int idx,double offset){ m_EsTimeOffset[idx] = offset;}

 private:
  double m_EsTimeOffset[2];//the T0 offset run by run;

};
#endif      
