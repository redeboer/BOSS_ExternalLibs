#ifndef BTOF_GEO_H
#define BTOF_GEO_H 
  
class BTofGeo{
 public:
   BTofGeo() {}
   ~BTofGeo(){}
 public:  
   double getInrad()                const { return m_inrad; }
   double getThick()                const { return m_thick; }
   double getPhiMax()               const { return m_phiMax; }
   double getPhiMin()               const { return m_phiMin; }
   double getLength()               const { return m_length; }
   void   setInrad(double inrad)    { m_inrad=inrad; }
   void   setThick(double thick)    { m_thick=thick; }
   void   setPhiMax(double phiMax)  { m_phiMax=phiMax; }
   void   setPhiMin(double phiMax)  { m_phiMin=phiMax; }
   void   setLength(double length)  { m_length=length; } 
 private:
   double m_inrad;
   double m_thick;
   double m_phiMin;
   double m_phiMax;
   double m_length;
};
#endif             
