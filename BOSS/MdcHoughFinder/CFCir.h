#ifndef CFCIR_H 
#define CFCIR_H 
class CFCir{
  public:
	CFCir();
	CFCir(double x,double y,double phi,int n,double ,double ,double);
	CFCir(const CFCir& );
	CFCir& operator=(const CFCir& );
//	CFtrans();
	double getx()  const { return _x;}
	double gety()  const { return _y;}
	double getphi()  const { return _phi;}
	int    getn()    const { return _n;}
	double getTheta()  const { return _theta;}
	double getRho()    const { return _rho;}
	double getSlant()  const { return _slant;}
	double    getCenterX()    const { return _centerX;}
	double    getCenterY()    const { return _centerY;}
	double    getCenterR()    const { return _centerR;}
  private:
	//real space
	double _x;
	double _y;
	double _phi;
	int    _n;
	double _theta;
	double _rho;
	double _centerX;
	double _centerY;
	double _centerR;
	double _slant;     // slant of sin curve on houghspace

};
#endif
