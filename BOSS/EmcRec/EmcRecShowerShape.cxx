
#include "EmcRec/EmcRecShowerShape.h"
#include "EmcRecEventModel/RecEmcEventModel.h"

#include "EmcRec/EmcRecParameter.h"  //

#include <vector>
#include <complex>

void EmcRecShowerShape::CalculateMoment(RecEmcShower& aShower) const
{

  SecondMoment(aShower);
  LatMoment(aShower);
  A20Moment(aShower);
  A42Moment(aShower);
}

void EmcRecShowerShape::SecondMoment(RecEmcShower& aShower) const
{
  double etot=0;
  double sum=0;
  HepPoint3D center(aShower.position());
  RecEmcFractionMap fracMap=aShower.getFractionMap5x5();
  
  RecEmcFractionMap::const_iterator it;
  for(it=fracMap.begin();
      it!=fracMap.end();
      it++){
    HepPoint3D pos(it->second.getFrontCenter());   //digi front center

    //////////////////////////
      EmcRecParameter& Para=EmcRecParameter::GetInstance(); 
    if(Para.DataMode()==1) {
   
      RecEmcID id = it->second.getCellId();
      const unsigned int module = EmcID::barrel_ec(id);
      const unsigned int thetaModule = EmcID::theta_module(id);
      const unsigned int phiModule = EmcID::phi_module(id);
      double lengthemc;//
      lengthemc = abs(pos.z()/cos(pos.theta()));//
      double posDataCorPar;
      if(module==1) posDataCorPar=Para.BarrPosDataCor(thetaModule,phiModule);
      if(module==0) posDataCorPar=Para.EastPosDataCor(thetaModule,phiModule);
      if(module==2) posDataCorPar=Para.WestPosDataCor(thetaModule,phiModule);	
      // cout<<module<<"\t"<<thetaModule<<"\t"<<phiModule<< "  "<< posDataCorPar<<endl;
      pos.setTheta(pos.theta()-posDataCorPar/lengthemc);
      // cout<<"poscor"<<pos<< "  "<<endl;
    }

    ///////////////////////////////////

    etot+=it->second.getEnergy()*it->second.getFraction();
    sum+=it->second.getEnergy()*it->second.getFraction()*pos.distance2(center);
  }

  if(etot>0) {
    sum/=etot;
  }

  aShower.setSecondMoment(sum);
}

void EmcRecShowerShape::LatMoment(RecEmcShower& aShower) const
{
  RecEmcFractionMap fracMap=aShower.getFractionMap5x5();
   if(fracMap.size()<2) {
    aShower.setLatMoment(0);
    return;
  }
 
  vector<RecEmcFraction> aFractionVec;
  RecEmcFractionMap::const_iterator it;
  for(it=fracMap.begin();
      it!=fracMap.end();
      it++){
    aFractionVec.push_back(it->second);
  }

  //find the largest 2 energy
  partial_sort(aFractionVec.begin(),
      aFractionVec.begin()+2,
      aFractionVec.end(),
      greater<RecEmcFraction>());

  //caculate LAT
  vector<RecEmcFraction>::iterator iVec;
  double numerator=0;
  double denominator=0;
  int n=0;
  for(iVec=aFractionVec.begin();
      iVec!=aFractionVec.end();
      iVec++) {
    n++;
    HepPoint3D pos((*iVec).getFrontCenter());  //digi front center

    //////////////////////////
      EmcRecParameter& Para=EmcRecParameter::GetInstance(); 
    if(Para.DataMode()==1) {
   
      RecEmcID id = (*iVec).getCellId();
      const unsigned int module = EmcID::barrel_ec(id);
      const unsigned int thetaModule = EmcID::theta_module(id);
      const unsigned int phiModule = EmcID::phi_module(id);
      double lengthemc;//
      lengthemc = abs(pos.z()/cos(pos.theta()));//
      double posDataCorPar;
      if(module==1) posDataCorPar=Para.BarrPosDataCor(thetaModule,phiModule);
      if(module==0) posDataCorPar=Para.EastPosDataCor(thetaModule,phiModule);
      if(module==2) posDataCorPar=Para.WestPosDataCor(thetaModule,phiModule);	
      // cout<<module<<"\t"<<thetaModule<<"\t"<<phiModule<< "  "<< posDataCorPar<<endl;
      pos.setTheta(pos.theta()-posDataCorPar/lengthemc);
      // cout<<"poscor"<<pos<< "  "<<endl;
    }

    ///////////////////////////////////

    double r=pos.mag()*sin(aShower.position().angle(pos));

    double energy=(*iVec).getEnergy()*(*iVec).getFraction();
    if(n<3) {
      denominator+=5.2*5.2*energy;
    } else {
      numerator+=r*r*energy;
      denominator+=r*r*energy;
    }
  }
  double lat=-99;
  if(denominator>0) lat=numerator/denominator;

  aShower.setLatMoment(lat);

}

void EmcRecShowerShape::A20Moment(RecEmcShower& aShower) const
{
  double a20=0;
  const double R0=15.6;
  Hep3Vector r0(aShower.position());    //shower center
  RecEmcFractionMap fracMap=aShower.getFractionMap5x5();
  RecEmcFractionMap::const_iterator it;
  for(it=fracMap.begin();
      it!=fracMap.end();
      it++){
    double energy=it->second.getEnergy()*it->second.getFraction();
    HepPoint3D pos(it->second.getFrontCenter());   //digi front center

    Hep3Vector r=pos-r0;
    r=r-r.dot(r0)*r0/(r0.mag()*r0.mag());

    a20+=(energy/aShower.e5x5())*(2*pow(r.mag()/R0,2.)-1);
  }

  aShower.setA20Moment(fabs(a20));
}

void EmcRecShowerShape::A42Moment(RecEmcShower& aShower) const
{
  complex<double> a42(0.,0.);
  const double R0=15.6;
  Hep3Vector r0(aShower.position());    //shower center
  RecEmcFractionMap fracMap=aShower.getFractionMap5x5();
  RecEmcFractionMap::const_iterator it;
  for(it=fracMap.begin();
      it!=fracMap.end();
      it++){
    double energy=it->second.getEnergy()*it->second.getFraction();
    HepPoint3D pos(it->second.getFrontCenter());   //digi front center

    Hep3Vector r=pos-r0;
    r=r-r.dot(r0)*r0/(r0.mag()*r0.mag());

    complex<double> a(0.,2.*r.phi());

    a42+=(energy/aShower.e5x5())*(4.*pow(r.mag()/R0,4.)-3.*pow(r.mag()/R0,2.))*exp(a);
  }

  aShower.setA42Moment(abs(a42));
}
