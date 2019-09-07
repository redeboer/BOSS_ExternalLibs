#ifndef  Hough2D_H 
#define Hough2D_H

#include <vector>
#include "MdcData/MdcHit.h" 
#include "MdcHoughFinder/HoughHit.h"
#include "MdcHoughFinder/HoughHitList.h"
#include "MdcHoughFinder/HoughRecHit.h"
#include "MdcHoughFinder/HoughGlobal.h"
#include "MdcData/MdcRecoHitOnTrack.h"

#include "TrkFitter/TrkContextEv.h"
#include "BField/BField.h"
#include "MdcGeom/MdcDetector.h"
#include "TrkBase/TrkFit.h"
#include "TrkBase/TrkHitList.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkFitter/TrkHelixMaker.h"
#include "TrkFitter/TrkCircleMaker.h"
#include "TrkFitter/TrkLineMaker.h"
//typedef std::vector<const HoughHit*> HitPtrCol;
typedef std::vector<HoughRecHit> recHitCol;

class Hough2D{
  public:
	Hough2D();
//	~Hough2D();
	Hough2D(const Hough2D& other);
//	Hough2D(double d0,double phi0,double omega,recHitCol recHitVec);
	//Hough2D(HitPtrCol hitCol,double bunchtime);
	Hough2D(recHitCol hitCol,double bunchtime);
	TrkRecoTrk* getTrk() const {return newTrack;}
	double getCirX() const {return _circleX;}
	double getCirY() const {return _circleY;}
	double getCirR() const {return _circleR;}
	double getD0() const   {return _d0;}
	double getPhi0() const {return _phi0;}
	double getOmega() const {return _omega;}
	int    getCharge() const {return _charge;}
//	vector<MdcHit*>& getVecForClean()  {return vec_for_clean;}

	void setCharge(int charge) {_charge=charge;}
	void setCirX(double cirx ) {_circleX=cirx;}
	void setCirY(double ciry ) {_circleY=ciry;}
	void setCirR(double cirr ) {_circleR=cirr;}

	void setD0(double d0) {_d0=d0;}
	void setPhi0(double phi0) {_phi0=phi0;}
	void setOmega(double omega) {_omega=omega;}
	void setPt(double pt) {_pt=pt;}

	void setBunchT0(double bunchT0)  {_bunchT0=bunchT0;}
	void setRecHit(const recHitCol& rechit)  {_recHitVec=rechit;}
	static void setContext(TrkContextEv* context)  {_context=context;}
	static void setCalib(const MdcCalibFunSvc *mdcCalibFunSvc)  {_mdcCalibFunSvc=mdcCalibFunSvc;}

	int fitLeast();
	virtual int fit();
	//virtual	int  digiToHots(TrkRecoTrk *newTrack);
	void print();
	void printRecHit() const;
	double getNfit() const {return _nfit;}
	double getChi2_2D() const {return _chi2_aver2D;}   
	//int  initial_det();
	double getPt() const {return _pt;}
	static int m_debug;
	static TrkContextEv*   _context;
    static const MdcCalibFunSvc*     _mdcCalibFunSvc; 

	//fit status
	static int m_qualityFactor;
	static double m_dropTrkChi2Cut;
	static double m_dropTrkDrCut;
	static double m_dropTrkDzCut;
	static double m_dropTrkNhitCut;
	static double m_dropTrkChi2NdfCut;

  private:
	double _circleR_least;
	double _circleX_least;
	double _circleY_least;
	double _pt_least;

	double _circleR;
	double _circleX;
	double _circleY;

	int    _charge;
	double _d0;
	double _phi0;
	double _omega;

	double _pt;
	double _nfit;
	double _chi2_aver2D;

	double          _bunchT0;
//	const MdcDetector*     _m_gm;
	recHitCol _recHitVec;
	TrkRecoTrk* newTrack;
	vector<MdcHit*> _MdcHitCol;

};

#endif
