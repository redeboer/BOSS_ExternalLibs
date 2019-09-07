#ifndef EmcTCFinder_h
#define EmcTCFinder_h 1

#include "EmcRawEvent/EmcDigi.h"
#include "Trigger/TrigPara.h"
#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"
#include "EmcCalibConstSvc/IEmcCalibConstSvc.h"
using namespace TrigConf;

class EmcTCFinder{
public:
	EmcTCFinder();
	~EmcTCFinder();
	//void GetEmcDigi();
        void setEmcDigi(EmcDigiCol* emcDigiCol);
        void setEmcTC(std::vector<uint32_t> vTC);
        void setEmcBE(std::vector<double> vBE);

	int getTCPhiId(int partId,int ThetaNb,int PhiNb);
	int getTCThetaId(int partId,int ThetaNb,int PhiNb);
	int getBLKId(int TCTheta,int TCPhi) const;
	inline double& getBTCEnergy(int i,int j) { return BTCEnergy[i][j];}
	inline double& getEETCEnergy(int i,int j) { return EETCEnergy[i][j];}
	inline double& getWETCEnergy(int i,int j) { return WETCEnergy[i][j];}

        inline int& getBTC(int i,int j) { return BTC[i][j];}
        inline int& getEETC(int i,int j) { return EETC[i][j];}
        inline int& getWETC(int i,int j) { return WETC[i][j];}

        inline double& getBTC_ADC(int i,int j) { return BTCEnergy_adc[i][j];}
        inline double& getEETC_ADC(int i,int j) { return EETCEnergy_adc[i][j];}
        inline double& getWETC_ADC(int i,int j) { return WETCEnergy_adc[i][j];}

        inline double getBLKE(int i) { return BlkE[i]; }

        static EmcTCFinder* get_Emc(void);

private:
	int TCThetaNb;
	int TCPhiNb;
	double BTCEnergy[TrigConf::TCTHETANO_B][TrigConf::TCPHINO_B];
	double EETCEnergy[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E];
	double WETCEnergy[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E];

	int WETC[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E];
	int EETC[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E];
	int BTC[TrigConf::TCTHETANO_B][TrigConf::TCPHINO_B];

        double BTCEnergy_adc[TrigConf::TCTHETANO_B][TrigConf::TCPHINO_B];
        double EETCEnergy_adc[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E];
        double WETCEnergy_adc[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E];

        double BlkE[16];

        static EmcTCFinder* emc_Pointer;
        // Reference to RealizationSvc
       RealizationSvc* m_RealizationSvc;
       IEmcCalibConstSvc *emcCalibConstSvc;
};
#endif
