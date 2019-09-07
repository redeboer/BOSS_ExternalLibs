#ifndef BTOFINNER_CAL_H
#define BTOFINNER_CAL_H 
  
class BTofInnerCal{
 public:
   BTofInnerCal() {}
   ~BTofInnerCal(){}
 public:
  double getInnP1(int im) const{ return m_innp1[im]; }
  double getInnP2(int im) const{ return m_innp2[im]; }

 public:  
  void   setInnP1(int idx,double innp1) { m_innp1[idx] = innp1;}
  void   setInnP2(int idx,double innp2) { m_innp2[idx] = innp2;}

 private:
  double m_innp1[10];
  double m_innp2[10];
};
#endif             
