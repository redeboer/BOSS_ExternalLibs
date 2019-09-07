#include "TVCrossPart.h"

#include <iostream>
double TVCrossPart::BornCrossSection(const double& theta){
  //save vacuum polarization status 
  bool tempVP = fZeroVP;
  // set zero vacuum polarization to calculate Born cross-section
  fZeroVP = true;
  Init();

  // integration in symmetric angle
  int NInt = 1<<14;
  double x, gsum = 0., h=-(cos(M_PI-theta)-cos(theta))/NInt;
  // simpson integration
  x = cos(M_PI-theta); fEvent->MakeEvent(x); gsum += BornShift(1,1,x);
  for(int i=1; i<NInt; i++){
    x += h; fEvent->MakeEvent(x); gsum += 2*(1+i%2)*BornShift(1,1,x);
  }
  x += h; fEvent->MakeEvent(x); gsum += BornShift(1,1,x);

  //restore vacuum polarization status
  fZeroVP = tempVP;
  Init();

  return gsum*h/3;
}
