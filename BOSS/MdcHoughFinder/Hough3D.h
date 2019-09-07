#ifndef  Hough3D_H
#define Hough3D_H

#include "MdcHoughFinder/HoughHit.h"
//#include "Hough/HoughCircle.h"
#include "MdcHoughFinder/Hough2D.h"
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
extern vector<MdcHit*> vec_for_clean;
extern vector<TrkRecoTrk*> vectrk_for_clean;

class Hough3D {
  public:
	Hough3D();
	Hough3D(const Hough3D& other);
	Hough3D(const Hough2D& hough2D,recHitCol hitCol,double bunchtime,double tanl,double z0);
	Hough3D(const Hough2D& hough2D,recHitCol hitCol,double bunchtime,double tanl,double z0,const vector<MdcHit*>* mdchit);
	//TrkRecoTrk* getTrk() const {return p_trk;}
	TrkRecoTrk* getTrk() const {return newTrack;}
//	Hough3D(const Hough2D& hough2D,recHitCol hitCol,double bunchtime,double d0,double phi0,double omega,double tanl,double z0);
	double getCirX() const {return _circleX;}
	double getCirY() const {return _circleY;}
	double getCirR() const {return _circleR;}
	double getD0() const   {return _d0;}
	double getPhi0() const {return _phi0;}
	double getOmega() const {return _omega;}
	double getZ0() const {return _z0;}
	double getTanl() const {return _tanl;}
	int    getCharge() const {return _charge;}
	int getNfit() const {return _nfit;}
	vector<MdcHit*>& getVecForClean()  {return vec_for_clean;}

	void setD0(double d0) {_d0=d0;}
	void setPhi0(double phi0) {_phi0=phi0;}
	void setOmega(double omega) {_omega=omega;}
	void setZ0(double z0) {_z0=z0;}
	void setTanl(double tanl) {_tanl=tanl;}

	void setBunchT0(double bunchT0)  {_bunchT0=bunchT0;}
	void setRecHit(const recHitCol& rechit)  {_recHitVec=rechit;}
	void setCharge(int charge) {_charge=charge;}
	static void setContext(TrkContextEv* context)  {_context=context;}
	static void setCalib(const MdcCalibFunSvc *mdcCalibFunSvc)  {_mdcCalibFunSvc=mdcCalibFunSvc;}

	int fit();
	void outerHit();
	//int  digiToHots(TrkRecoTrk *newTrack);
	void printRecHit() const;
	double getPt() const {return _pt;}
	double getPz() const {return _pz;}
	double getP() const {return _p;}
	double getChi2() const {return _chi2_aver;}

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
	void   print();

  private:

	double _circleR;
	double _circleX;
	double _circleY;
	int    _charge;
	double _d0;
	double _phi0;
	double _omega;
	double _tanl;
	double _z0;

	double _pt;
	double _pz;
	double _p;
	int _nfit;
	double _chi2_aver;

	double          _bunchT0;
	const MdcDetector*     _m_gm;
	recHitCol _recHitVec;
	TrkRecoTrk* newTrack;
	const vector<MdcHit*>* vec_mdcHit;

};
#endif
