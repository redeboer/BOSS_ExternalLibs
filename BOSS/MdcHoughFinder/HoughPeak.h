#ifndef  HOUGHPEAK_H
#define  HOUGHPEAK_H

#include <vector>
#include <string>
#include "MdcHoughFinder/HoughHit.h"
#include "MdcHoughFinder/HoughHitList.h"

using namespace std;

class HoughPeak{
  public:
	HoughPeak();
	HoughPeak(const HoughPeak& other);
	~HoughPeak();
	//HoughPeak(int itheta, int irho , double theta,double rho, vector< const HoughHit* >**  mapHitList,bool is);
	HoughPeak(int itheta, int irho , double theta,double rho, vector< const HoughHit* >**  mapHitList,bool is,int peakNum);
	HoughPeak(int height,int itheta, int irho , double theta,double rho, bool is,int peakNum,int charge);
	// collect hits
	vector<const HoughHit*> getHoughHitList() const {return _houghPeakHitList;}
	int peakHeight() const {return _height;}
	int getPeakNum()   const {return _peakNum;}
	int getThetaBin()   const {return _thetaBin;}
	int getRhoBin()     const {return _rhoBin;}
	double getTheta()   const {return _theta;}
	double getRho()     const {return _rho;}
	double getRho_reci()     const {return 1./_rho;}
	bool getisCandiTrack()     const {return _isCandiTrack;}
	void setisCandiTrack(bool is) { _isCandiTrack=is;}
	int    getHitNum(int)   const ;
	int    getHitNumA(int)   const ;
	int    getHitNumS(int)   const ;
	void printAllHit() const;
	void print();
	HoughPeak& operator=(const HoughPeak& other);
	int collectHits(const HoughHitList& );

	static double m_dSigma_cut;
	static double m_dSigma_cut2;
	static string m_file;

  private:
	//class member
	double _theta;
	double _rho;
	int _thetaBin;
	int _rhoBin;
	vector<const HoughHit*> _houghPeakHitList;
	bool _isCandiTrack;
	int _peakNum;
	int _height;
	int _charge;
};

#endif
