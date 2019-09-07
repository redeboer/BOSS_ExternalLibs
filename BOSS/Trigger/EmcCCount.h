#ifndef EmcCCount_h
#define EmcCCount_h 1

#include "Trigger/EmcTCFinder.h"
#include "Trigger/TrigPara.h"

class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;

using namespace TrigConf;

class EmcCCount{
public:
	EmcCCount();
	~EmcCCount();
	void getClusterId();
	bool findCluster(int partId,int TCThetaNb,int TCPhiNb);
	inline int getBClusterPhi(int i) { return BClusterPhi[i]; }
	inline int getWEClusterPhi(int i) { return WEClusterPhi[i]; }
	inline int getEEClusterPhi(int i) { return EEClusterPhi[i]; }
	inline int getBClusterId(int i,int j) { return BClusterId[i][j]; }
	inline int getEEClusterId(int i,int j) { return EEClusterId[i][j]; }
	inline int getWEClusterId(int i,int j) { return WEClusterId[i][j]; }
	inline bool getEClus_Z() { return EClus_Z; }
	inline bool getWClus_Z() { return WClus_Z; }
private:
	EmcTCFinder* m_EmcTCFinder;
	
	int BClusterPhi[TrigConf::TCPHINO_B];
	int WEClusterPhi[TrigConf::TCPHINO_E/2];
	int EEClusterPhi[TrigConf::TCPHINO_E/2];
	
        int BClusterId[TrigConf::TCTHETANO_B][TrigConf::TCPHINO_B];
        int EEClusterId[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E/2];
        int WEClusterId[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E/2];

	bool EClus_Z;
	bool WClus_Z;

        BesGlobalTrigSvc* m_pIBGT;
        IBesGlobalTrigSvc* m_tmpSvc;
};
#endif
