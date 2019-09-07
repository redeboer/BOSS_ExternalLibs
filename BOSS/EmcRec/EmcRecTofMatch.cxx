//
//  Match Emc Shower and Tof Hit
//
//  He Miao 2006, 9, 15
//
#include "EmcRec/EmcRecTofMatch.h"
#include "EmcRec/EmcRecParameter.h"

EmcRecTofMatch::EmcRecTofMatch()
{}

EmcRecTofMatch::~EmcRecTofMatch()
{}

void EmcRecTofMatch::Match(RecEmcShowerMap& aShowerMap,
    RecEmcTofHitMap& tofHitMap)
{
  RecEmcShowerMap::iterator iShowerMap;
  for(iShowerMap=aShowerMap.begin();
      iShowerMap!=aShowerMap.end();
      iShowerMap++) {

    // calculate matching window
    double eShower = iShowerMap->second.e5x5();
    double matWindow = 0;
    if(eShower>0) {
      matWindow = 0.01277+0.004268/sqrt(eShower);
    }
    
    double phi = iShowerMap->second.phi();
    phi = phi < 0 ? phi+CLHEP::twopi : phi;

    int nphi1 = (int)(phi*88./CLHEP::twopi);      //tof layer1 number
    int nphi2 = (int)(phi*88./CLHEP::twopi+0.5);  //tof layer2 number
    nphi2 += 88;

    //find max energy in both layer of tof
    int nphi1max=nphi1;
    double emax=tofHitMap[nphi1].Energy();
    if(tofHitMap[nphi1-1].Energy() > emax) {
      emax = tofHitMap[nphi1-1].Energy();
      nphi1max = nphi1-1;
    }
    if(tofHitMap[nphi1+1].Energy() > emax) {
      emax = tofHitMap[nphi1+1].Energy();
      nphi1max = nphi1+1;
    }
    nphi1 = nphi1max;

    int nphi2max=nphi2;
    emax=tofHitMap[nphi2].Energy();
    if(tofHitMap[nphi2-1].Energy() > emax) {
      emax = tofHitMap[nphi2-1].Energy();
      nphi2max = nphi2-1;
    }
    if(tofHitMap[nphi2+1].Energy() > emax) {
      emax = tofHitMap[nphi2+1].Energy();
      nphi2max = nphi2+1;
    }
    nphi2 = nphi2max;
     
    double etof2x1=0;
    double etof2x3=0;
    int nphi[6] = { nphi1-1, nphi1, nphi1+1, nphi2-1, nphi2, nphi2+1 };
    
    for(int i=0;i<6;i++) {
      
      if(tofHitMap[nphi[i]].Energy()<=0) continue;
      
      HepPoint3D pos=tofHitMap[nphi[i]].Position();
      /*if(i<1) {
        //tof layer1 center
        pos = HepPoint3D(0,838.5,tofHitMap[nphi[i]].ZPosition());
      } else {
        //tof layer2 center
        pos = HepPoint3D(0,895.5,tofHitMap[nphi[i]].ZPosition());
      }*/
      
      //matching window
      if(fabs(pos.theta()-iShowerMap->second.theta())<3*matWindow) {
        etof2x3 += tofHitMap[nphi[i]].Energy();
        if( i==1 || i==4 ) {
          etof2x1 += tofHitMap[nphi[i]].Energy();
        }
      }

      tofHitMap[nphi[i]].Energy(0);
    }

    double ecorr = iShowerMap->second.e5x5() + etof2x3/GeV;
    
    iShowerMap->second.ETof2x3(etof2x3/GeV);
    iShowerMap->second.ETof2x1(etof2x1/GeV);
    iShowerMap->second.setEnergy(ECorrection(ecorr));
  }
}

RecEmcEnergy EmcRecTofMatch::ECorrection(const RecEmcEnergy eIn)
{
  EmcRecParameter& Para=EmcRecParameter::GetInstance();

  RecEmcEnergy eOut=0;
  double par[4];
  for(int i=0;i<4;i++) {
    par[i]=Para.ECorr(i);
  }
  
  eOut = eIn/(par[0]+par[1]*eIn+par[2]*eIn*eIn+par[3]*eIn*eIn*eIn);
  return eOut;
}
