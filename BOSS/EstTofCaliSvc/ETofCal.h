#ifndef ETOF_CAL_H
#define ETOF_CAL_H 
  
const unsigned int nEndPar    = 8;

class ETofCal{
 public:
   ETofCal() {}
   ~ETofCal(){}
 public:  
   double getP(int i)const{ return m_p[i]; }
   void   setP(int i,double p){ m_p[i] = p;}

 private:
  double m_p[nEndPar];                         // time calibration constant 
};
#endif             
