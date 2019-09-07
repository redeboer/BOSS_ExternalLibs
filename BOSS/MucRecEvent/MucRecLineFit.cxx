//$id$
//
//$log$

/*
 *    2003/12/24   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

using namespace std;

#include <iostream>
#include <math.h>
#include "MucGeomSvc/MucGeomSvc.h"
#include "MucRecEvent/MucRecLineFit.h"

MucRecLineFit::MucRecLineFit()
{
    // Constructor.
}

MucRecLineFit::~MucRecLineFit()
{
  // Destructor.
}


/* --------------------------------------------------------------

 utiLineFit - a least squares straight line fitting program

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
 utiLineFit(X,Y,W,N,&A,&B,&CHISQ,&SIGA,&SIGB)                   
 The arrays must start counting at element 1.

--------------------------------------------------------------- */

// add (part,seg,orient) to choose better coordinator.

  int
MucRecLineFit::LineFit(float x[],
    float y[], 
    float w[],
    int part,
    int seg,
    int orient,
    int n,
    float *a,
    float *b,
    float *chisq,
    float *siga, 
    float *sigb)
{
  if(part>2||part<0)  {
    cout<<"BAD part id from MucRecLineFit"<<endl;
    return -1;
  }

  int status = -1;
  float c,d,sigc,sigd;
  
  if(part != 1 ){
    for(int i = 0; i < n; i++){
      float gap0z = (MucGeoGeneral::Instance()->GetGap(part, seg, 0)->GetCenter()).z();
      //cout<<"in MucRecLineFit change x from "<<x[i]<<" to ";
      x[i] -= gap0z;
      //cout<<x[i]<<endl;
    }
    status = LineFit(x,y,w,n,a,b,chisq,siga,sigb);
    //cout<<"in MucRecLineFit sigb = "<<*sigb<<endl;
  }
  else{
    if(orient == 0){  //need change x and y because x has error now.

      for(int i = 0; i < n; i++){
        float gap0r = (MucGeoGeneral::Instance()->GetGap(part,0 , 0)->GetCenter()).x();
        y[i] -= gap0r;
      }
      status = LineFit(y,x,w,n,&c,&d,chisq,&sigc,&sigd);
      //need to recalculate a,b,siga,sigb now
      // y=ax+b;  x=cy+d
      // a = 1/c; b = -d/c
      // siga^2 = a^2/c^2 * sigc^2
      // sigb^2 = b^2/c^2 * sigc^2 + b^2/d^2 * sigd^2;

      *a = 1/c;
      *b = -1 * d / c;
      *siga = 1/c/c * sigc;
      //*sigb = sqrt(fabs(1/c/c*sigd*sigd + d*d/c/c/c/c*sigc*sigc));
      *sigb = sigd;  ////////////
    }
    else{  // barrel... seg0,4 need not change; seg2,6 change x<->y; other seg need more complicate operation!
      if(seg == 0||seg == 4){
        float gap0x = (MucGeoGeneral::Instance()->GetGap(part,seg , 0)->GetCenter()).x();
        for(int i = 0; i < n; i++){
          x[i] -= gap0x;
        }
        status = LineFit(x,y,w,n,a,b,chisq,siga,sigb);
      }
      else if(seg == 2||seg == 6){
        float gap0y = (MucGeoGeneral::Instance()->GetGap(part,seg , 0)->GetCenter()).y();
        for(int i = 0; i < n; i++){
          y[i] -= gap0y;
        }   
        status = LineFit(y,x,w,n,&c,&d,chisq,&sigc,&sigd);
        //need to recalculate a,b,siga,sigb now
        *a = 1/c; 
        *b = -1 * d / c;
        *siga = 1/c/c * sigc;
        //*sigb = sqrt(fabs(1/c/c*sigd*sigd + d*d/c/c/c/c*sigc*sigc));
        *sigb = sigd;
      }
      else{
        for(int i = 0; i < n; i++){
          float temp = (y[i] + x[i])/sqrt(2.0);
          y[i] = (y[i] - x[i])/sqrt(2.0);
          x[i] = temp;
        }

        if(seg == 1 || seg == 5){
          for(int i = 0; i < n; i++){
            float gap0x = (MucGeoGeneral::Instance()->GetGap(part,seg-1 , 0)->GetCenter()).x();
            x[i] -= gap0x;
          }
          status = LineFit(x,y,w,n,a,b,chisq,siga,sigb);
        }
       else if(seg == 3 || seg == 7){
         for(int i = 0; i < n; i++){
           float gap0y = (MucGeoGeneral::Instance()->GetGap(part,seg-1 , 0)->GetCenter()).y();
           y[i] -= gap0y;
         }
         status = LineFit(y,x,w,n,a,b,chisq,siga,sigb);

       }

      }


    }
    
  }

  return status;

}



int
MucRecLineFit::LineFit(float x[],
		       float y[], 
		       float w[],
		       int n, 
		       float *a, 
		       float *b,
		       float *chisq,
		       float *siga, 
		       float *sigb)
  
{
  double sum, sx, sy, sxx, sxy, syy, det;
  float chi;
  
  /* The variable "i" is declared 8 bytes long to avoid triggering an
     apparent alignment bug in optimized code produced by gcc-2.95.3.
     The bug doesn't seem to be present in gcc-3.2. */
  long i; 

  chi = 99999999.0;

  /* N must be >= 2 for this guy to work */
  if (n < 2) 
    {
      cout << "utiLineFit-W: Too few points for line fit \n" << endl;
      return -1;
    }

  /* Initialization */
  sum = 0.0;
  sx  = 0.0;
  sy  = 0.0;
  sxx = 0.0;
  sxy = 0.0;
  syy = 0.0;

  /* Find sum , sumx ,sumy, sumxx, sumxy */
  for (i = 0; i < n; i++)
    {
      //cout<<"x: "<<x[i]<<"  y: "<<y[i]<<"  w: "<<w[i]<<endl;
      sum = sum + w[i];
      sx  = sx + w[i] * x[i];
      sy  = sy + w[i] * y[i];
      sxx = sxx + w[i] * x[i] * x[i];
      sxy = sxy + w[i] * x[i] * y[i];
      syy = syy + w[i] * y[i] * y[i];
    }

  det = sum*sxx-sx*sx;
  if (fabs(det) < 1.0e-20) 
    {
      *a = 1.0e20;
      *b = x[0];
      *chisq = 0.0;
      *siga  = 0.0;
      *sigb  = 0.0;

      return 0;
    }

  /* compute the best fitted parameters A,B */
  *a = (sum*sxy-sx*sy)/det;
  *b = (sy*sxx-sxy*sx)/det;
    
  //cout<<"a: "<<*a<<"  b: "<<*b<<endl;
  /* calculate chi-square */
  chi = 0.0;
  for (i=0; i<n; i++) 
    {
      chi = chi+(w[i])*((y[i])-*a*(x[i])-*b)*
	((y[i])-*a*(x[i])-*b);
    }
  
  *siga = sqrt(fabs(sum/det));
  *sigb = sqrt(fabs(sxx/det));
  
  
  *chisq = chi;
  return 0;
}   

