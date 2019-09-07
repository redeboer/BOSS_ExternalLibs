#ifndef EmcESum_h
#define EmcESum_h 1
#include "Trigger/EmcTCFinder.h"

class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;

 
class EmcESum{
public:
	EmcESum();
	~EmcESum();
	void getESum();
	inline double getTotE() { return _TotE; }
	inline double getLTotE() { return _LTotE; }
	inline double getRTotE() { return _RTotE; }
	inline double getBTotE() { return _BTotE; }
	inline double getETotE() { return _ETotE; }
	inline double getLBTotE() { return _LBTotE; }
	inline double getRBTotE() { return _RBTotE; }
	inline double getWETotE() { return _WETotE; }
	inline double getEETotE() { return _EETotE; }
	inline double getBBLKE(int i) { return BLK[i]; }
	inline double getWEBLKE(int i) { return WEBLK[i]; }
	inline double getEEBLKE(int i) { return EEBLK[i]; }
private:
	EmcTCFinder* m_EmcTCFinder;
	double _TotE;
	double _LTotE;
	double _RTotE;
	double _BTotE;
	double _ETotE;
	double _LBTotE;
	double _RBTotE;
	double _WETotE;
	double _EETotE;
	double BLK[12];
        double WEBLK[2];
        double EEBLK[2];
        
        BesGlobalTrigSvc* m_pIBGT;
        IBesGlobalTrigSvc* m_tmpSvc;
};
#endif
