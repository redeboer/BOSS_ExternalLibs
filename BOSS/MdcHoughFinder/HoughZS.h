#ifndef  HoughZS_H
#define HoughZS_H
#include "MdcHoughFinder/HoughHit.h"
#include "MdcHoughFinder/HoughCircle.h"
#include "MdcHoughFinder/Hough2D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
typedef HepGeom::Vector3D<double> HepVector3D;
class HoughZS
{
  public: 
	HoughZS();
	HoughZS(double bunchTime);
	HoughZS( double bunchTime, Hough2D* circle , HoughRecHit *rechit);
	void setAmb(int i);
	int cald();
	void setRecHit();
	void print();
	Hough2D* getCircle()     const {return _circle;}
	HoughRecHit* getRecHit() const {return _rechit;}
	double getlLeft()  const{return _lleft;}
	double getlRight() const{return _lright;}
	double getzLeft()  const{return _zleft;}
	double getzRight() const{return _zright;}

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
