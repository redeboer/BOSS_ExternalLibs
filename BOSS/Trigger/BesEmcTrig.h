#ifndef BesEmcTrig_h
#define BesEmcTrig_h 1

#include <vector>

class EmcESum;
class EmcCCount;
class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;

class BesEmcTrig{
public:
	BesEmcTrig();
	~BesEmcTrig();
	void startEmcTrig();

//	void clear();

private:
	EmcESum* m_EmcESum;
	EmcCCount* m_EmcCCount;

	bool BClus_Phi,EClus_Phi,Clus_Z,BClusBB,EClusBB,BL_BBLK;
	bool BL_Z, Diff_E, Diff_B, BL_EBLK;
        bool Etot_L, Etot_M, BEtot_H, EEtot_H;
        bool NClus1, NClus2, NBClus1, NEClus1;

        BesGlobalTrigSvc* m_pIBGT;
        IBesGlobalTrigSvc* m_tmpSvc;
        
};
#endif
