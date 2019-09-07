#include "MdcHoughFinder/CFCir.h"
#include <math.h>
#include <iostream>

CFCir::CFCir(){
	}

CFCir::CFCir(double x,double y,double phi,int n,double centerX,double centerY,double centerR):_x(x),_y(y),_phi(phi),_n(n),_centerX(centerX),_centerY(centerY),_centerR(centerR)
{ 
	double normal = (y-_centerY)/(x-centerX);
	double k = -1./normal;
	double b = y - k*x;
	double x_cross = -b/(k+1/k);
	double y_cross = b/(1+k*k);
//	std::cout<<"x y centerX centerY "<<x<<" "<<y<<" "<<centerX<<" "<<centerY<<std::endl;
//	std::cout<<"k b "<<k<<" "<<b<<std::endl;
//	std::cout<<"xcross ycross "<<x_cross<<" "<<y_cross<<std::endl;

	double rho_temp=sqrt(x_cross*x_cross+y_cross*y_cross);
	double theta_temp=atan2(y_cross,x_cross);
	if(theta_temp<0)  {
	  theta_temp=theta_temp+M_PI;
	  rho_temp=-rho_temp;
	}
	if( normal ==0 && x>0)  {
	rho_temp = fabs(x);
	theta_temp = 0; 
	}
	if( normal ==0 && x<0)  {
	rho_temp = -fabs(x);
	theta_temp = M_PI; 
	}
	_theta = theta_temp;
	_rho = rho_temp;
	double slant = _y*cos(_theta)-_x*sin(_theta);
	_slant = slant; 
//	std::cout<<"THETA RHO "<<theta_temp<<" "<<rho_temp<<std::endl;
//	std::cout<<std::endl;
}
//void CFtrans(){
//
//  double Rcf = _x*_x + _y*_y - _d*_d;
//
//  _cfx = _x/Rcf;
//  _cfy = _y/Rcf;
//  _cfd = _d/Rcf;
//
//}
CFCir& CFCir::operator=( const CFCir& cir){
  _x  =cir._x;	
  _y  =cir._y;	
  _phi=cir._phi;	
  _n  =cir._n;	

  _theta  =cir._theta;
  _rho  =cir._rho;
  _centerX  =cir._centerX;
  _centerY  =cir._centerY;
  _centerR  =cir._centerR;
  _slant    =cir._slant;
}

CFCir::CFCir( const CFCir& cir) : 
  _x(cir._x),
  _y(cir._y),
  _phi(cir._phi),
  _n(cir._n),

  _theta(cir._theta),
  _rho(cir._rho),
  _centerX(cir._centerX),
  _centerY(cir._centerY),
  _centerR(cir._centerR),
  _slant (cir._slant)
{}
