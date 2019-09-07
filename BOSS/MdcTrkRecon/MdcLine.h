// MdcLine.hh
//  simple class to do a line fit -- all public
//   y = ax + b;

// Interface Dependencies ----------------------------------------------

#ifndef MDCLINE_H
#define MDCLINE_H

//  End Interface Dependencies -----------------------------------------

// Class definition//
class MdcLine {
public:
  double *x;
  double *y;
  double *sigma;
  double *resid;
  int nPoint;
  double slope;
  double intercept;
  double chisq;
  double errmat[3];

  MdcLine(int n) { 
    nPoint = n;
    x = new double[n];
    y = new double[n];
    sigma = new double[n];
    resid = new double[n];
  };
  ~MdcLine() { 
    delete [] x;
    delete [] y;
    delete [] sigma;
    delete [] resid;
  };
  
  // The fit:
  int fit(int nUse);  // fit using first nUse pts
};
#endif







