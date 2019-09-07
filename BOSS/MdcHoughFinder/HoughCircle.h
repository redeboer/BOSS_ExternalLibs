#ifndef CIRCLE_H
#define CIRCLE_H

#include "MdcHoughFinder/HoughHit.h"
#include "MdcHoughFinder/HoughHitList.h"
#include <vector>
typedef std::vector<const HoughHit*> hitPtrCol;
class HoughCircle{
  public:
	HoughCircle();
	HoughCircle(hitPtrCol hitCol);
	HoughCircle(double ,double ,double);
	int fitLeast();
	void setHitCol(hitPtrCol hitCol)  {_hitOnCir=hitCol;}
	double getCirX() const {return _circleX;}
	double getCirY() const {return _circleY;}
	double getCirR() const {return _circleR;}
	double getD0() const {return _d0;}
	double getPhi0() const {return _phi0;}
	double getOmega() const {return _omega;}
	double getPt() const {return _pt;}
	hitPtrCol  getHitCandi() const{return _hitCandi;}
	void printHitCandi() const;
	void printHitOnCir() const;
  private:
	double _circleR;
	double _circleX;
	double _circleY;
	double _disToCir;

	double _d0;
	double _phi0;
	double _omega;
	int   _q;
	double _pt;
	hitPtrCol  _hitCandi;
	hitPtrCol  _hitOnCir;

};
#endif
