#ifdef CIRCEL_H
#define CIRCEL_H
#endif

#include "MdcHoughFinder/HoughHit.h"
#include "MdcHoughFinder/HoughTrack.h"

class Circle2D{
  public:
	Circle2D();
	fitChisq();
  private:
	double _circleR;
	double _circleX;
	double _circleY;
	double _disToCir;

	double _d0;
	double _phi0;
	double _omega;
	double _z0;
	double _tanl;
	double _q;
	std::vector<const HoughHit*>   _hitOnCir;

}
