#ifndef ETOF_GEO_H
#define ETOF_GEO_H 
  
class ETofGeo{
 public:
   ETofGeo() {}
   ~ETofGeo(){}
 public:  
   double getInrad()                  const { return m_inrad; }
   double getOutrad()                 const { return m_outrad; }
   double getThick()                  const { return m_thick; }
   double getPhiMax()                 const { return m_phiMax; }
   double getPhiMin()                 const { return m_phiMin; }
   double getThetaMax()               const { return m_thetaMax; }
   double getThetaMin()               const { return m_thetaMin; }
   void   setInrad(double inrad)      { m_inrad=inrad; }
   void   setOutrad(double outrad)    { m_outrad=outrad; }
   void   setThick(double thick)      { m_thick=thick; }
   void   setPhiMax(double phiMax)    { m_phiMax=phiMax; }
   void   setPhiMin(double phiMax)    { m_phiMin=phiMax; }
   void   setThetaMax(double thetaMax){ m_thetaMax=thetaMax; }
   void   setThetaMin(double thetaMax){ m_thetaMin=thetaMax; }
 private:
   double m_inrad;
   double m_outrad;
   double m_thick;
   double m_phiMax;
   double m_phiMin;
   double m_thetaMax;
   double m_thetaMin;
};
#endif             
