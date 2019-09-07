#ifndef  HOUGHTRACK_H
#define  HOUGHTRACK_H

//#include "Hough/HoughMap.h"
#include "MdcHoughFinder/HoughPeak.h"
#include "MdcHoughFinder/Hough2D.h"
#include "MdcHoughFinder/Hough3D.h"
#include "MdcHoughFinder/HoughHitList.h"
#include "MdcHoughFinder/HoughRecHit.h"
#include "MdcHoughFinder/HoughZsFit.h"
#include "MdcRawEvent/MdcDigi.h"
#include <vector>
#include <utility>
class HoughTrack{
typedef std::vector<HoughRecHit> recHitCol; 
  public:
	HoughTrack();
	~HoughTrack();
	HoughTrack(const HoughTrack& other);
    HoughTrack(const  HoughPeak& centerPeak, std::vector<const HoughHit*>  trackHitList, double ,double);
//    HoughTrack(const  HoughPeak& centerPeak, std::vector<const HoughHit*>  trackHitList, int charge);
	TrkRecoTrk* getTrk() const {return p_trk;}
	TrkRecoTrk* getTrk2D() const {return p_trk2D;}
	//vector<MdcHit*>& getVecForClean2D() {return _Hough2D.getVecForClean();}
	vector<MdcHit*>& getVecForClean3D() {return _Hough3D.getVecForClean();}
    HoughPeak getCenterPeak()  const { return _centerPeak;} 
    Hough2D   get2D()    const { return _Hough2D;} 
    Hough3D   get3D()    const { return _Hough3D;} 
	recHitCol& getHoughHitList()     { return _recHitVec; }
	//std::vector<const HoughHit*> getHoughHitList() const{ return _trackHitList; }
	void setCharge(int charge) {_charge=charge;}
	int getCharge()     const {return _charge;}
	double getPt_least()   const {return _ptLeast;}    
	double getPt2D()   const {return _pt2D;}    
	double getPt3D()   const {return _pt3D;}    
	double getP()   const {return _p;}    
	double getPz()   const {return _pz;}    
	double getRho()   const {return _centerPeak.getRho();}    
	double getTheta()   const {return _centerPeak.getTheta();}    
	double getD0()   const {return _d0;}    
	double getOmega()  const {return _omega;}    
	double getPhi0()   const {return _phi0;}    
	double getTanl()   const {return _tanl;}    
	double getZ0()   const {return _z0;}    
	double getTanl_zs()   const {return _tanl_zs;}    
	double getZ0_zs()   const {return _z0_zs;}    

	int    getHitNum(int) const;   
	int    getHitNumA(int) const;   
	int    getHitNumS(int) const;   
	int    getNfit2D() const {return _Hough2D.getNfit();}   
	int    getNfit3D() const {return _Hough3D.getNfit();}   
	int    getStat2D() const {return _stat2D;}   
	int    getStat3D() const {return _stat3D;}   

	double getCirR() const {return _centerR;}   
	double getCirX() const {return _centerX;}   
	double getCirY() const {return _centerY;}   
	double getChi2_3D() const {return _chi2_aver;}   
	double getChi2_2D() const {return _chi2_aver2D;}   
	double getNfit()    const {return _nfit;}   

	void setHoughHitList(vector<HoughHit> vec_hit)  {_houghList=vec_hit;}

	void printRecHit() const;
	void print() ;
	void sortHit()     ;
	void fit(double bunchtime);
	int  calzs();
	void   fitzs();
	int    judgeTrack();
	int find_stereo_hit();
	int find_pair_hit();
	int cutNoise_inner();
	int cut_axial_inner();
	static bool m_debug;
	HoughTrack& operator=(const HoughTrack& other);
	HoughTrack& add(const HoughTrack& other);
	int fit2D(double bunchtime);
	int fit3D();
	int fit3D_inner();
	int trackCharge2D();
	int trackCharge3D();
	void outerHit();
	void cald_layer();

	double getMapRho()   const {return _maprho;}
	double getMapTheta() const {return _maptheta;}
	double getPro() 	 const {return t_pro_correct;}
	void Leastfit(vector<double>, vector<double>,double&,double&);
	void setMdcHit(const vector<MdcHit*>* mdchit)  {vec_mdcHit= mdchit;}

  private:
	void hitOnTrack();
	int    judge_half(const HoughRecHit&);
//	void hitOnTrack(recHitCol&);
	double calcuDistToTrack(const HoughRecHit& );
	double calcuDistToCir(const HoughRecHit& );
	std::pair<double,double> calcuArcTrack(const HoughRecHit& );
	int fitLeast();
	//int fit_global2D();
	int fit_global2D(vector<HoughRecHit>& recHit);
	int fit_global3D(int);
	int cutMultiCirHit();
	int cutMultiCirHit_after_zs();
	int collectAxialHit();
	void fun(int,int**,int*,int*,vector<HoughRecHit>&,int&);

    //class member
	double _dist;
	int _charge;
	double _ptLeast;
	double _pt2D;
	double _pt3D;
	double _pz;
	double _p;
	double _chi2_aver;
	double _chi2_aver2D;
	int _nfit;
	int _nfit2D;

	double _d0;
	double _omega;
	double _phi0;
	double _tanl;
	double _z0;
	double _tanl_zs;
	double _z0_zs;
    HoughPeak  _centerPeak;
	Hough2D     _Hough2D;
	Hough3D     _Hough3D;
	double      _bunchTime;
	double      _centerX;
	double      _centerY;
	double      _centerR;
	//fit stat
	int  _stat2D;
	int  _stat3D;
	std::vector<HoughRecHit> _recHitVec;
	std::vector<HoughHit> _houghList;
	TrkRecoTrk *p_trk;
	TrkRecoTrk *p_trk2D;
	//par on map
	double _maprho;
	double _maptheta;
	double t_pro_correct;
	const vector<MdcHit*>* vec_mdcHit;

};
#endif
