//$id$
//
//$log$

/*
 *    2003/12/24   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */


#include <iostream>
#include <math.h>
#include "MucRecEvent/MucRecQuadFit.h"
#include <CLHEP/Matrix/Matrix.h>

using namespace std;
using namespace CLHEP;

MucRecQuadFit::MucRecQuadFit()
{
    // Constructor.
}

MucRecQuadFit::~MucRecQuadFit()
{
  // Destructor.
}


/* --------------------------------------------------------------

 utiQuadFit - a least squares straight line fitting program

 DESCRIPTION:
 Performs weighted least squares fit to line (Y  =  A*X  +  B )
 using linear regression.

 INPUT ARGUMENTS:                                             
  X(N),Y(N),W(N) - Input values and weights (N=1,2,3...)
  N              - Number of pairs of data points,
  X              - Array of data points in X-axis,
  Y              - Array of data points in Y-axis,
  W              - Array of weights for data points,

 OUTPUT ARGUMENTS:            
  B              - Y intercept of best fitted straight line, 
  SIGB           - Standard deviation of B,                  
  A              - Slope of fitted straight line,            
  SIGA           - Standard deviation of A,                  
  CHISQ          - Chi-square                                
  LSWLF          - = 0  return without error                
                   = -1 got some fitting problems            
             
 AUTHOR:
 Jawluen Tang, Physics department, UT-Austin 
 J. T. Mitchell - adapted for PHENIX use. Converted to C.
                                                  
 USAGE:                                                       
 utiQuadFit(X,Y,W,N,&A,&B,&CHISQ,&SIGA,&SIGB)                   
 The arrays must start counting at element 1.

--------------------------------------------------------------- */

int
MucRecQuadFit::QuadFit(float x[],
		       float y[], 
		       float w[],
		       int n, 
		       float *a, 
		       float *b,
		       float *c,
		       int *half, //which half parabola 1: left 2 : right
		       float *chisq,
		       float *siga, 
		       float *sigb,
		       float *sigc)
  
{
  double sumx, sumx2, sumx3, sumx4,sumy, sumyx, sumyx2, det;
  float chi;
  
  /* The variable "i" is declared 8 bytes long to avoid triggering an
     apparent alignment bug in optimized code produced by gcc-2.95.3.
     The bug doesn't seem to be present in gcc-3.2. */
  long i; 

  chi = 99999999.0;
  *a = 0; *b = 0; *c = 0;
  *half = 0;

  /* N must be >= 2 for this guy to work */
  if (n < 3) 
    {
      //cout << "utiQuadFit-W: Too few points for quad fit \n" << endl;
      return -1;
    }

  /* Initialization */
  sumx  = 0.0;
  sumx2 = 0.0;
  sumx3 = 0.0;
  sumx4 = 0.0;
  sumy  = 0.0;
  sumyx = 0.0;
  sumyx2 = 0.0;


  /* Find sum , sumx ,sumy, sumxx, sumxy */
  for (i = 0; i < n; i++)
    {
      sumx = sumx + w[i] * x[i];
      sumx2 = sumx2 + w[i] * x[i] * x[i];
      sumx3 = sumx3 + w[i] * x[i] * x[i] * x[i];
      sumx4 = sumx4 + w[i] * x[i] * x[i] * x[i] * x[i];
      sumy = sumy + w[i] * y[i];
      sumyx = sumyx + w[i] * y[i] * x[i];
      sumyx2 = sumyx2 + w[i] * y[i] * x[i] * x[i];      
      //cout<<"x : y "<<x[i]<<" "<<y[i]<<endl;

    }



  /* compute the best fitted parameters A,B,C */

    HepMatrix D(3,3,1);
    HepMatrix C(3,1), ABC(3,1);
    D(1,1) = sumx4; D(1,2) = D(2,1) = sumx3; D(1,3) = D(3,1) = D(2,2) = sumx2;
    D(3,2) = D(2,3) = sumx; D(3,3) = n;
    C(1,1) = sumyx2; C(2,1) = sumyx; C(3,1) = sumy;

    int ierr;
    ABC = (D.inverse(ierr))*C;

    *a = ABC(1,1);
    *b = ABC(2,1);
    *c = ABC(3,1);
    
    //judge which half parabola these points belone to
    float center = *b/(-2*(*a));
    float mean_x;
    for(i = 0; i < n; i++){
      mean_x += x[i]/n;
    }

    if(mean_x > center) *half = 2;//right half
    else *half = 1;//left half
    
    //cout<<"in MucRecQuadFit:: which half= "<<*half<<endl;

  /* calculate chi-square */
  chi = 0.0;
  for (i=0; i<n; i++) 
    {
      chi = chi+(w[i])*((y[i])-*a*(x[i])-*b)*
	((y[i])-*a*(x[i])-*b);
    }
  
  //*siga = sum/det;
  //*sigb = sxx/det;
  
  *chisq = chi;
  return 1;
}   

