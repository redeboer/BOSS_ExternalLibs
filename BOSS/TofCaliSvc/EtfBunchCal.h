#ifndef ETF_BUNCHCAL_H
#define ETF_BUNCHCAL_H 

const unsigned int nEtfBunch  = 4;
  
class EtfBunchCal{
 public:
  EtfBunchCal() {}
  ~EtfBunchCal(){}

 public:
  double getBunchP(int idx)            const{ return m_pbunch[idx];   }

 public:
  void   setBunchP(int idx,double pbunch)   { m_pbunch[idx] = pbunch; }

 private:
  double m_pbunch[nEtfBunch];

};
#endif             
