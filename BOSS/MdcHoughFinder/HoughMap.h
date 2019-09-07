#ifndef  HOUGHMAP_H
#define  HOUGHMAP_H

#include <vector>
#include "TH2D.h"
#include "TF1.h" 
#include "TGraph.h" 
#include "MdcHoughFinder/HoughHit.h"
#include "MdcHoughFinder/HoughHitList.h"
#include "MdcHoughFinder/HoughPeak.h"
#include "MdcHoughFinder/HoughTrack.h"
#include <memory>

using namespace std;

//class HoughPeak;

class HoughMap{
  public:
    HoughMap(int charge, HoughHitList &houghHitList,int mapHit,int ntheta,int nrho,double rhoMin,double rhoMaxi,int peakWidth,int peakHigh,double hitpro);
    HoughMap();
    HoughMap(const HoughMap& other);
	void doMap();
    ~HoughMap();

    TH2D*  getMdcHoughFinder()  const {return _houghSpace;}
//    TH2D*  getHoughR()      const {return _houghR;}
	const HoughHitList& getHitList() const {return _hitList;}
	//
    int   		getPeakNumber() 			const  {return _houghPeakList.size();}
    int   		getTrackNumber() 			const  {return _houghTrackList.size();}
    const  		HoughPeak&  getPeak(int i)  const {return _houghPeakList[i];}
    const       HoughTrack& getTrack(int i) const {return _houghTrackList[i];}
    //HoughPeak&  getPeak(int i) 				{return _houghPeakList[i];}

    const vector<HoughPeak>  getPeakList()  const {return _houghPeakList;}
	const vector<HoughTrack> getTrackList() const {return _houghTrackList;}
//	vector<const HoughHit*>  getMapHitList(int i,int j) const {return _mapHitList[i][j];}
	int getPeakWidth()     const {return _peakWidth;}
	int getPeakHigh()      const {return _peakHigh;}
	int getNTheta()        const {return _nTheta;}
	int getNRho()          const {return _nRho;}
//	double** getNL()       const {return _houghNL;}
//	double** getRL()       const {return _houghRL;}
//	double** getS2()       const {return _houghS2;}
//	double** getS()        const {return _houghS;}
	void printPeak();
	void printTrack();
    void clearMap();

	//unused
	void select_slant();
	vector<double> get_maxlayer_slant()   const {return maxlayer_slant;} 
	double 	       get_maxlayer() 				  const {return m_maxlayer;} 
	vector<double> get_nomaxlayer_slant() const {return nomaxlayer_slant;} 
	vector<int>    get_nomaxlayerid() 		  const {return nomaxlayerid;} 

	static int m_debug;
	static int m_useHalfCir;
	static int m_N1;
	static int m_N2;

	//unused
	double Rho;
	double Theta;
	double Height;
	//
    int MAX;

	double exTheta(int,double ,double ,int );
	double exRho(int,double ,double ,int ); 
	int exThetaBin(double );
	int exRhoBin(double ); 
	void printMapHit();

  private:
	void   buildMap();
	void   findPeaks(vector< vector<int> > vec_hist,double ,double ,double ,double );
	//void   loopPeak(double ,double ,int ,int, double hist[1000][1000]);
	void   loopPeak(double ,double ,int ,int);
	void   sortPeaks();
	void   hitFinding();
	void   trackFinder();
	void   compareTrack_and_Peak(HoughTrack& track,HoughPeak& peak);  
	// distribution of noise and peak
	void   mapDev(vector< vector<int> > vec_hist,double& aver ,double& sigma);
	void   mapDev(TH2D* h,double& aver ,double& sigma);

	//unused
	int    mergeNeighbor(int** hough_trans_CS_peak,double ,double ,double ,double ); 
	void   combineNeighbor(int ipeak); 
	void   combine_two_cells(vector< const HoughHit* >&cellHitList,int ax,int py); 
	void   candiTrack();

	//in study
	void   gravity();
	void   cald_layer();
	void   Leastfit(vector<double>, vector<double>,double&,double&);

	//class member
	int    		 _mapHit;   //0 all ; 1 axial
	int    		 _nTheta;   //number of theta bin
	int    		 _nRho;     //number of rho bin
	double       _thetaMin; //0
	double       _thetaMax; //M_PI
	double       _rhoMin;   // -0.1
	double       _rhoMax;   // 0.1
	int 		_peakWidth;  //no use ?
	int 		_peakHigh;   //no use ?
	double 		_hitpro;    // proportion cut in other track
	int 		_charge;   //map charge  
	HoughHitList 				_hitList;//hit list in this space
	vector<HoughPeak>   		_houghPeakList;//peak list of HoughRec space
	vector<HoughTrack>   		_houghTrackList;//peak list of HoughRec space
	TH2D*        				_houghSpace;//accumulate array of HoughRec space
	//double** 						_houghRL;
	//double** 						_houghNL;
	//TH2D* 						_houghR;
//	double** 						_houghS;
//	double** 						_houghS2;
//	vector< const HoughHit* >** _mapHitList;//hits of each bin in the accumulate array

	// in map study
	vector<double> maxlayer_slant;
	double m_maxlayer;
	vector<double> nomaxlayer_slant;
	vector<int> nomaxlayerid;



};
#endif

