#ifndef  HoughStereo_H
#define HoughStereo_H
#include "MdcHoughFinder/HoughHit.h"
//#include "Hough/HoughCircle.h"
#include "MdcHoughFinder/Hough2D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
typedef HepGeom::Vector3D<double> HepVector3D;
class HoughStereo
{
  public: 
	//HoughStereo();
	//HoughStereo(double bunchTime);
	HoughStereo( double bunchTime, Hough2D* circle , HoughRecHit* rechit);
	void setAmb(int i);
	int cald();
	void calcu(double x1 ,double y1 ,double xc,double yc,double rc,double x_east,double y_east,double z_east,double x_west,double y_west,double z_west,double& theta,double& l,double& z);
	void setRecHit();
	void print();
	Hough2D* getCircle()     const {return _circle;}
	HoughRecHit* getRecHit() const {return _rechit;}
	double getlLeft()  const{return _lleft;}
	double getlRight() const{return _lright;}
	double getzLeft()  const{return _zleft;}
	double getzRight() const{return _zright;}
	static int m_debug;

  private:

	Hough2D* _circle;
	HoughRecHit* _rechit;
	double _bunchTime;
	double _lleft;
	double _zleft;
	double _lright;
	double _zright;

	int _ambig;
	int _charge;
};
#endif
