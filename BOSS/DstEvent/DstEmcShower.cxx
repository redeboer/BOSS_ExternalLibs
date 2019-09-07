#include "DstEvent/DstEmcShower.h"

double DstEmcShower::dx() const
{
  double xx=99.;
  if(m_errorMatrix[0][0]>0) xx=sqrt(m_errorMatrix[0][0]);
  else if(m_errorMatrix[0][0]==0) xx=0.;
  return xx;
}

double DstEmcShower::dy() const
{
  double yy=99.;
  if(m_errorMatrix[1][1]>0) yy=sqrt(m_errorMatrix[1][1]);
  else if(m_errorMatrix[1][1]==0) yy=0.;
  return yy;
}

double DstEmcShower::dz() const
{
  double zz=99.;
  if(m_errorMatrix[2][2]>0) zz=sqrt(m_errorMatrix[2][2]);
  else if(m_errorMatrix[2][2]==0) zz=0.;
  return zz;
}

ostream& operator<<(ostream & os, const DstEmcShower& aShower)
{
  os<<"------------------DstEmcShower:"<<endl;
  os<<"Track id: "<<aShower.trackId()<<", number of hits: "<<aShower.numHits()
    <<", status: "<<aShower.status()<<", cell id: "<<aShower.cellId()
    <<", module: "<<aShower.module()<<", time: "<<aShower.time()<<endl;
  os<<"Energy: "<<aShower.energy()<<", de: "<<aShower.dE()
    <<", eseed: "<<aShower.eSeed()
    <<", e3x3: "<<aShower.e3x3()<<", e5x5: "<<aShower.e5x5()<<endl;
  os<<"Position: "<<aShower.position()
    <<", theta: "<<aShower.theta()<<", phi: "<<aShower.phi()
    <<"\ndx: "<<aShower.dx()<<", dy: "<<aShower.dy()<<", dz: "<<aShower.dz()
    <<", dtheta: "<<aShower.dtheta()<<", dphi: "<<aShower.dphi()<<endl;
  os<<"Second moment: "<<aShower.secondMoment()
    <<", lat moment: "<<aShower.latMoment()
    <<", a20 moment: "<<aShower.a20Moment()
    <<", a42 moment: "<<aShower.a42Moment()<<endl;
  os<<"Error matrix: \n"<<aShower.errorMatrix()<<endl;

  return os;
}
