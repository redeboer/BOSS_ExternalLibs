// MdcLine.cc -- simple straight line fitter

#include "MdcTrkRecon/MdcLine.h"
#include <stdlib.h>
#include <math.h>
// End Implementation Dependencies -------------------------------------
#include <iostream>
using namespace std;//yzhang debug

int MdcLine::fit(int nUse) {

  double s1, sx, sy, sxx, sxy;  // The various sums. 
  double delinv, denom;
  double weight = 0.013;     // weight for hits, calculated from sigmas. 
  int ihit;
  if (nUse > nPoint) nUse = nPoint;
  s1 = sx = sy = sxx = sxy = 0.0;
  chisq = 0.0;
  for (ihit = 0; ihit < nUse; ihit++) {
    
    if (sigma[ihit] < 0.0) continue;
    weight = 1. / (sigma[ihit] * sigma[ihit]);//NEED sigma of MdcHits
    s1 += weight;    
    sx += x[ihit] * weight;
    sy += y[ihit] * weight;
    sxx += x[ihit] * (x[ihit] * weight);
    sxy += y[ihit] * (x[ihit] * weight);

    //std::cout<<__FILE__<<" "<<__LINE__<< " "<<ihit<<" sigma "<<
      //sigma[ihit] <<" weight "
      //<< weight<<" x "
      //<< x[ihit]<<" y "
      //<< y[ihit]<< std::endl;
  }
  
  /* Calculate parameters. */
  denom = s1 * sxx - sx * sx;
  delinv = (denom == 0.0) ? 1.e20 : 1. / denom;
  intercept = (sy * sxx - sx * sxy) * delinv;
  slope = (s1 * sxy - sx * sy) * delinv;
  errmat[0] = sxx * delinv;
  errmat[1] = -sx * delinv;
  errmat[2] = s1 * delinv;
  //std::cout << " After Fit:" << std::endl;//yzhang debug
  /* Calculate residuals. */
  for (ihit = 0; ihit < nUse; ihit++) {
    if (sigma[ihit] < 0.0) continue;
    resid[ihit] = ( y[ihit] - intercept - slope * x[ihit] );
    chisq += resid[ihit] * resid[ihit] / (sigma[ihit] * sigma[ihit]);
    //std::cout << "resid "<<ihit<<" "<<resid[ihit]<<" sigma "<<sigma[ihit]<<" chisq add "<<resid[ihit] * resid[ihit] / (sigma[ihit] * sigma[ihit]) << std::endl;//yzhang debug 
  }

  //std::cout<<__FILE__<<" "<<__LINE__
  //<< " intercept "<<intercept<<" slope "<<slope 
	  //<< " chisq "<<chisq<<std::endl;//yzhang debug 
  return 0;
}





