//
//  Tof digit to hit
//
//  He Miao 2006, 9,26 
//
#include "EmcRec/EmcRecTofDigitCalib.h"
#include "Identifier/TofID.h"
#include "RawEvent/RawDataUtil.h"

EmcRecTofDigitCalib::EmcRecTofDigitCalib()
{}

EmcRecTofDigitCalib::~EmcRecTofDigitCalib()
{}

void EmcRecTofDigitCalib::Convert(TofDigiCol* tofDigiCol,
    RecEmcTofHitMap& tofHitMap)
{
  TofDigiCol::iterator iter2;
  for(iter2=tofDigiCol->begin();
      iter2!=tofDigiCol->end();
      iter2++,iter2++) {
    
    Identifier id=(*iter2)->identify();
    bool   is_barrel    = TofID::is_barrel(id);
    int    layer        = TofID::layer(id);
    int    im           = TofID::phi_module(id);
    
    if(!is_barrel) return;
    
    double adc0 = RawDataUtil::TofCharge((*iter2)->getChargeChannel())*2;
    double tdc0 = RawDataUtil::TofTime((*iter2)->getTimeChannel());
    double adc1 = RawDataUtil::TofCharge((*(iter2+1))->getChargeChannel())*2;
    double tdc1 = RawDataUtil::TofTime((*(iter2+1))->getTimeChannel());
    
    double rand0 = (double)(rand()%100)/1000.-0.05;   //-0.05 --> 0.05
    double rand1 = (double)(rand()%100)/1000.-0.05;   //-0.05 --> 0.05

    //smear 5%
    adc0 *= (1+rand0);
    adc1 *= (1+rand1);

    double zpos, energy, e0, e1;
    zpos = 0.08331*(tdc1-tdc0);
    if(fabs(zpos)>1.15) continue;

    //ADC->energy
    if((adc0>46.&&adc0<920.) && (adc1<=46.||adc1>=920.)) {
      energy = adc0/exp(2.783+0.3133*zpos);
    } else if((adc1>46.&&adc1<920.) && (adc0<=46.||adc0>=920.)) {
      energy = adc1/exp(2.784-0.3079*zpos);
    } else if((adc0>46.&&adc0<920.) && (adc1>46.&&adc1<920.)) {
      e0 = adc0/exp(2.783+0.3133*zpos);
      e1 = adc1/exp(2.784-0.3079*zpos);
      energy = (e0+e1)/2;
    } else if(adc0>=920.&&adc1>=920.) {
      e0 = 920./exp(2.783+0.3133*zpos);
      e1 = 920./exp(2.784-0.3079*zpos);
      energy = e0>e1?e0:e1;
    } else {
      continue;
    }

    double phi,r;
    if(layer==0) {
      phi=(im+0.5)*CLHEP::twopi/88.;
      r=838.5;
    } else {
      phi=im*CLHEP::twopi/88.;
      r=895.5;
    }

    zpos*=1000; //zpos: m->mm
    HepPoint3D pos(r*cos(phi), r*sin(phi), zpos);
     
    tofHitMap[im+layer*88] = RecEmcTofHit(id, energy, pos);
  }
}
