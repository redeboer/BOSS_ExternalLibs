#ifndef BesGlobalTrigSvc_h
#define BesGlobalTrigSvc_h 1

#include "Trigger/IBesGlobalTrigSvc.h"
#include "GaudiKernel/Service.h"
#include <vector>
#include <map>
#include <complex>
#include <fstream>

#include "GaudiKernel/IIncidentListener.h"
#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"
#include "Trigger/TrigPara.h"

using namespace std;

class IDataProviderSvc;
class BesMdcTrig;
class BesEmcTrig;
class BesTofTrig;
class BesMucTrig;
class BesTMTrig;

class BesGlobalTrigSvc: public Service,
                         virtual public IIncidentListener,
                         virtual public IBesGlobalTrigSvc {
public:
	BesGlobalTrigSvc(const std::string& name, ISvcLocator* svc);
	virtual ~BesGlobalTrigSvc();
public:
        virtual StatusCode queryInterface( const InterfaceID& riid, 
				     void** ppvInterface);  
	/// Initialise the service (Inherited Service overrides) 
        virtual StatusCode initialize();
        /// Finalise the service.
        virtual StatusCode finalize();
        // Incident handler
        void handle(const Incident&);

        void startMdcTrig();
        void startTofTrig();
        void startEmcTrig();
        void startTMTrig();

        StatusCode GlobalTrig();

        inline void setEvtTime(double i) { m_evttime = i; };
        inline double getEvtTime() { return m_evttime; }

        inline void setGoodEvent(bool i) { m_goodEvent = i; };
        inline bool getGoodEvent() { return m_goodEvent; };
        //set trigger conditions
        StatusCode setTrigCondition();

        //values controled in jobOptions
        inline double getL1TC_GATE() { return L1TC_GATE; }
        inline double getL1TC_THRESH() { return L1TC_THRESH; }
        inline int getL1ETOT_L_Peak() { return L1ETOT_L_Peak; }
        inline int getL1ETOT_L_Sigma() { return L1ETOT_L_Sigma; }
        inline int getL1ETOT_M_Peak() { return L1ETOT_M_Peak; }
        inline int getL1ETOT_M_Sigma() { return L1ETOT_M_Sigma; }
        inline int getL1ETOT_BR_Peak() { return L1ETOT_BR_Peak; }
        inline int getL1ETOT_BR_Sigma() { return L1ETOT_BR_Sigma; }
        inline int getL1ETOT_EC_Peak() { return L1ETOT_EC_Peak; }
        inline int getL1ETOT_EC_Sigma() { return L1ETOT_EC_Sigma; }
        inline int getL1EBL_Z_Peak() { return L1EBL_Z_Peak; }
        inline int getL1EBL_Z_Sigma() { return L1EBL_Z_Sigma; }
        int getL1ETOT_L();
        int getL1ETOT_M();
        int getL1ETOT_BR(); 
        int getL1ETOT_EC();
        int getL1EBL_BR(); 
        int getL1EBL_EC(); 
        int getL1EDIFF_BR(); 
        int getL1EDIFF_EC(); 
        int getL1BLK_GATE(); 
        int getL1EBL_Z(); 

        inline int    getMAXMDCTRACK() { return MAXMDCTRACK; }
        inline double getEnergy_Ratio() { return Energy_Ratio; }

        inline double getBarrelGate() { return emcBarrelGate; }
        inline double getEndGate() { return emcEndGate; }
        inline int getTofLayerControl() { return m_tofLayer; }

	//interface of the trigger channels in each event
	const int getTrigChan(int i) { return m_trigChannel[i]; }
        const int* getTrigChan() const { return m_trigChannel;  }

        inline void setTrigChan(int i, int j) { m_trigChannel[i] = j; }

	//interface to get trigger conditions in each events
	const int getTrigCond(int i)     { return m_trigCondition[i]; }
        const int* getTrigCond() const   { return m_trigCondition;    }

        inline void setTrigCond(int i, bool j) { 
          trigCond[i] = j; 
          if(j) m_trigCondition[i] = 1;
          else m_trigCondition[i] = 0;
        }

	//set L1 signal
        inline void setIfpass(bool i) { ifpass = i; }
	//get L1 signal
        inline bool getIfpass() { return ifpass; }

        //set run mode
        inline void setRunMode(int mode) { m_runMode = mode; }
        //get run mode
        inline int getRunMode() { return m_runMode; }

        //set MDC information
	//set MDC trigger conditions
	inline void setSTrk_BB(bool i) { mdc_STrk_BB = i; }
	inline void setNStrkN(bool i)  { mdc_NStrkN = i; }
	inline void setNStrk2(bool i)  { mdc_NStrk2 = i; }
	inline void setNStrk1(bool i)  { mdc_NStrk1 = i; }
        inline void setLTrk_BB(bool i) { mdc_LTrk_BB = i; }
	inline void setNLtrkN(bool i)  { mdc_NLtrkN = i; }
	inline void setNLtrk2(bool i)  { mdc_NLtrk2 = i; }
	inline void setNLtrk1(bool i)  { mdc_NLtrk1 = i; }
	inline void setNItrk1(bool i)  { mdc_NItrk1 = i; }
	inline void setNItrk2(bool i)  { mdc_NItrk2 = i; }

	//set other information of MDC
        inline void setMdcStrkId(std::vector<int> i) { mdcStrkId = i; }
        inline void setMdcLtrkId(std::vector<int> i) { mdcLtrkId = i; }

        //get MDC information
	//get MDC trigger conditions
	inline bool getSTrk_BB() { return mdc_STrk_BB; }
	inline bool getNStrkN() { return mdc_NStrkN; }
	inline bool getNStrk2() { return mdc_NStrk2; }
	inline bool getNStrk1() { return mdc_NStrk1; }
	inline bool getLTrk_BB() { return mdc_LTrk_BB; }
	inline bool getNLtrkN()  { return mdc_NLtrkN; }
	inline bool getNLtrk2() { return mdc_NLtrk2; }
	inline bool getNLtrk1() { return mdc_NLtrk1; }
	inline bool getNItrk1() { return mdc_NItrk1; }
	inline bool getNItrk2() { return mdc_NItrk2; }
	//get other information in MDC
        inline std::vector<int> getMdcStrkId() { return mdcStrkId; }
        inline std::vector<int> getMdcLtrkId() { return mdcLtrkId; }

        //set TOF information
	//set TOF trigger conditions
	inline void setETofBB(bool i) { tof_ETofBB = i; }
	inline void setBTofBB(bool i) { tof_BTofBB = i; }
	inline void setNETof2(bool i) { tof_NETof2 = i; }
	inline void setNETof1(bool i) { tof_NETof1 = i; }
	inline void setNBTof2(bool i) { tof_NBTof2 = i; }
	inline void setNBTof1(bool i) { tof_NBTof1 = i; }
	inline void setNTof1(bool i) { tof_NTof1 = i; }

	//set other information of TOF
        inline void setTofHitPos(map<int,vector<int>,greater<int> >  i) { tofHitPos = i; }
        inline void setBTofHitMap(vector<int>  i) { btofHitMap = i; }
        inline void setETofHitMap(vector<int>  i) { etofHitMap = i; }

        //get TOF information
	//get TOF trigger conditions
        inline bool getNBTof1() { return tof_NBTof1; }
        inline bool getNBTof2() { return tof_NBTof2; }
        inline bool getNETof1() { return tof_NETof1; }
        inline bool getNETof2() { return tof_NETof2; }
        inline bool getNTof1() { return tof_NTof1; }
        inline bool getBTofBB() { return tof_BTofBB; }
        inline bool getETofBB() { return tof_ETofBB; }

	//get other information in TOF
        inline map<int,vector<int>,greater<int> > getTofHitPos() { return tofHitPos; } 
        inline vector<int> getBTofHitMap() { return btofHitMap; } 
        inline vector<int> getETofHitMap() { return etofHitMap; } 

        //set EMC information
	//set trigger conditions
        inline void setNClus1(bool i) { emc_NClus1 = i; }
        inline void setNClus2(bool i) { emc_NClus2 = i; }
        inline void setNBClus1(bool i) { emc_NBClus1 = i; }
        inline void setNEClus1(bool i) { emc_NEClus1 = i; }
	inline void setBClusBB(bool i) { emc_BClusBB = i; }
	inline void setEClusBB(bool i) { emc_EClusBB = i; }
	inline void setClus_Z(bool i)  { emc_Clus_Z = i; }
	inline void setBClus_PHI(bool i) { emc_BClus_PHI = i; }
	inline void setEClus_PHI(bool i) { emc_EClus_PHI = i; }
	inline void setBEtot_H(bool i) { emc_BEtot_H = i; }
	inline void setEEtot_H(bool i) { emc_EEtot_H = i; }
	inline void setEtot_L(bool i) { emc_Etot_L = i; }
	inline void setEtot_M(bool i) { emc_Etot_M = i; }
	inline void setBL_Z(bool i) { emc_BL_Z = i; }
	inline void setDiff_B(bool i) { emc_Diff_B = i; }
	inline void setDiff_E(bool i) { emc_Diff_E = i; }
	inline void setBL_BBLK(bool i) { emc_BL_BBLK = i; }
	inline void setBL_EBLK(bool i) { emc_BL_EBLK = i; }
        
	//set other information of EMC
        inline void setEmcNbCluster(int i) { emcNbCluster = i; }
        inline void setEmcNweCluster(int i) { emcNweCluster = i; }
        inline void setEmcNeeCluster(int i) { emcNeeCluster = i; }
        inline void setEmcTotE(double i) { emcTotE = i; }
        inline void setEmcLTotE(double i) { emcLTotE = i; }
        inline void setEmcRTotE(double i) { emcRTotE = i; }
        inline void setEmcBTotE(double i) { emcBTotE = i; }
        inline void setEmcETotE(double i) { emcETotE = i; }  
        inline void setEmcLBTotE(double i) { emcLBTotE = i; }
        inline void setEmcRBTotE(double i) { emcRBTotE = i; }
        inline void setEmcWETotE(double i) { emcWETotE = i; }
        inline void setEmcEETotE(double i) { emcEETotE = i; }
        inline void setBTCEnergy(int theta, int phi, double energy) {  emcBTCEnergy[theta][phi] = energy; }
        inline void setWETCEnergy(int theta, int phi, double energy) {  emcWETCEnergy[theta][phi] = energy; }
        inline void setEETCEnergy(int theta, int phi, double energy) {  emcEETCEnergy[theta][phi] = energy; }
        inline void setEmcBlockE(std::vector<double> i) { emcBlockE = i; }       
        inline void setEmcClusId(map<int,vector<complex<int> >, greater<int> > i) { emcClusId = i; }
        inline void setEmcClusE(std::vector<double> i) { emcClusE = i; }
 
        //get EMC information
	//get EMC trigger conditions
        inline bool getNClus1() { return emc_NClus1; }
	inline bool getNClus2() { return emc_NClus2; } 
	inline bool getNBClus1() { return emc_NBClus1; } 
	inline bool getNEClus1() { return emc_NEClus1; } 
	inline bool getBClusBB() { return emc_BClusBB; }
	inline bool getEClusBB() { return emc_EClusBB; }
	inline bool getClus_Z()  { return emc_Clus_Z; }
	inline bool getBClus_PHI() { return emc_BClus_PHI; }
	inline bool getEClus_PHI() { return emc_EClus_PHI; }
	inline bool getBEtot_H() { return emc_BEtot_H; }
	inline bool getEEtot_H() { return emc_EEtot_H; }
	inline bool getEtot_L() { return emc_Etot_L; }
	inline bool getEtot_M() { return emc_Etot_M; }
	inline bool getBL_Z() { return emc_BL_Z; }
	inline bool getDiff_B() { return emc_Diff_B; }
	inline bool getDiff_E() { return emc_Diff_E; }
	inline bool getBL_BBLK() { return emc_BL_BBLK; }
	inline bool getBL_EBLK() { return emc_BL_EBLK; }

	//get other information of EMC
        inline std::vector<double>& getEmcBlockE() { return emcBlockE; };
        inline int getEmcNbCluster() { return emcNbCluster; }
        inline int getEmcNweCluster() { return emcNweCluster; }
        inline int getEmcNeeCluster() { return emcNeeCluster; }
        inline double getEmcTotE() { return emcTotE; }
        inline double getEmcLTotE() { return emcLTotE; }
        inline double getEmcRTotE() { return emcRTotE; }
        inline double getEmcBTotE() { return emcBTotE; }
        inline double getEmcETotE() { return emcETotE; }
        inline double getEmcLBTotE() { return emcLBTotE; }
        inline double getEmcRBTotE() { return emcRBTotE; }
        inline double getEmcWETotE() { return emcWETotE; }
        inline double getEmcEETotE() { return emcEETotE; }
        inline double getBTCEnergy(int theta, int phi) { return emcBTCEnergy[theta][phi]; }
        inline double getWETCEnergy(int theta, int phi) { return emcWETCEnergy[theta][phi]; }
        inline double getEETCEnergy(int theta, int phi) { return emcEETCEnergy[theta][phi]; }
        inline map<int,vector<complex<int> >,greater<int> > getEmcClusId() { return emcClusId; }
        inline std::vector<double> getEmcClusE() { return emcClusE; }

        //set Muc information 
        //set Muc trigger conditions
        inline void setMucBB3478(bool i)     { muc_BB3478 = i; }
        inline void setMucBB37(bool i)       { muc_BB37   = i; }
        
        //set other Muc infor.
        inline void setMucIndex(std::vector<int> i) { muc_index = i; }
        inline void setMucNtrackBR(int i) { muc_ntrackBR = i; }
        inline void setMucNtrackEC(int i) { muc_ntrackEC = i; }
        inline void setMucN3ltrackTotal(int i) { muc_n3ltracktotal = i; }

        //get Muc information
        //get Muc trigger conditions
        inline bool getMucBB3478()    { return muc_BB3478; }
        inline bool getMucBB37()      { return muc_BB37;   }
        
        //get other Muc infor.
        inline std::vector<int> getMuclayerSeg() { return muc_vlayerSeg; }
        inline std::vector<int> getMuchitLayer() { return muc_vhitLayer; }
        inline std::vector<int> getMuchitSeg() { return  muc_vhitSeg; }
        inline std::vector<int> getMucIndex() { return muc_index; }
        inline int getMucNhitEE() { return muc_nhitEE; }
        inline int getMucNhitBR() { return muc_nhitBR; }
        inline int getMucNhitWE() { return muc_nhitWE; }
        inline int getMucNhitTotal() { return muc_nhitTotal; }
        inline int getMucNlayerBR() { return muc_nlayerBR; }
        inline int getMucNlayerWE() { return muc_nlayerWE; }
        inline int getMucNlayerEE() { return muc_nlayerEE; }
        inline int getMucNtrackBR() { return muc_ntrackBR; }
        inline int getMucNtrackEC() { return muc_ntrackEC; }
        inline int getMucN3ltrackTotal() { return muc_n3ltracktotal; }

        //set track match trigger conditions
        inline void setTMNATrk1(bool i) { tm_NATrk1 = i; }
        inline void setTMNATrk2(bool i) { tm_NATrk2 = i; }
        inline void setTMATrkBB(bool i) { tm_ATrk_BB = i; }
        inline void setTMNBTrk1(bool i) { tm_NBTrk1 = i; }
        inline void setTMNBTrk2(bool i) { tm_NBTrk2 = i; }
        inline void setTMBTrkBB(bool i) { tm_BTrk_BB = i; }
        inline void setTMNCTrk1(bool i) { tm_NCTrk1 = i; }
        inline void setTMNCTrk2(bool i) { tm_NCTrk2 = i; }
        inline void setTMCTrkBB(bool i) { tm_CTrk_BB = i; } 

        //get track match trigger conditions
        inline bool getTMNATrk1() { return tm_NATrk1; }
        inline bool getTMNATrk2() { return tm_NATrk2; }
        inline bool getTMATrkBB() { return tm_ATrk_BB; }
        inline bool getTMNBTrk1() { return tm_NBTrk1; }
        inline bool getTMNBTrk2() { return tm_NBTrk2; }
        inline bool getTMBTrkBB() { return tm_BTrk_BB; }
        inline bool getTMNCTrk1() { return tm_NCTrk1; }
        inline bool getTMNCTrk2() { return tm_NCTrk2; }
        inline bool getTMCTrkBB() { return tm_CTrk_BB; }

        //get endcap track match mode
        inline int getEndCapMode() { return tm_endcap; }

private:
	BesMdcTrig* m_MdcTrig;
	BesTofTrig* m_TofTrig;
	BesEmcTrig* m_EmcTrig;
	BesMucTrig* m_MucTrig;
        BesTMTrig*  m_TMTrig;

        std::vector<int> trigOut;

        std::vector<std::string> trigChannel;
        std::vector<std::string> trigCondName;
        std::vector<std::string> trigTable;
        std::vector<int> channelNo;
  
        double m_evttime;

        bool m_goodEvent;

        //MDC
	//MDC trigger conditions
        bool mdc_STrk_BB,mdc_NStrkN,mdc_NStrk2,mdc_NStrk1;
	bool mdc_LTrk_BB,mdc_NLtrkN,mdc_NLtrk2,mdc_NLtrk1;
	bool mdc_NItrk1,mdc_NItrk2;

	//Other variables in MDC
        std::vector<int> mdcStrkId;
        std::vector<int> mdcLtrkId;

        //TOF
	//TOF trigger conditions
        bool tof_NBTof1,tof_NBTof2,tof_NETof1,tof_NETof2,tof_NTof1;
        bool tof_BTofBB,tof_ETofBB;
	//Other variables in TOF
        map<int,vector<int>,greater<int> > tofHitPos;   
        std::vector<int> btofHitMap;
        std::vector<int> etofHitMap;     

        //EMC
	//EMC trigger conditions
        bool emc_NClus1, emc_NClus2, emc_NBClus1, emc_NEClus1, emc_BClusBB,emc_EClusBB,emc_Clus_Z,emc_BClus_PHI;
	bool emc_EClus_PHI,emc_BEtot_H,emc_EEtot_H,emc_Etot_L,emc_Etot_M,emc_BL_Z;
	bool emc_Diff_B,emc_Diff_E,emc_BL_BBLK,emc_BL_EBLK;
        
	//Other variables in EMC
        double emcBarrelGate, emcEndGate;
        double emcTotE,emcLTotE,emcRTotE,emcBTotE,emcETotE,emcLBTotE,emcRBTotE,emcWETotE,emcEETotE;
        int emcNbCluster,emcNweCluster,emcNeeCluster;
        std::vector<double> emcBlockE;
        std::vector<double> emcClusE;
        map<int,vector<complex<int> >, greater<int> > emcClusId; 
        double emcBTCEnergy[TrigConf::TCTHETANO_B][TrigConf::TCPHINO_B];
        double emcWETCEnergy[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E];
        double emcEETCEnergy[TrigConf::TCTHETANO_E][TrigConf::TCPHINO_E];

        //muc
        //MUC trigger conditions
        bool muc_BB3478, muc_BB37;

        //Other variables in MUC
        int muc_nhitEE, muc_nhitBR, muc_nhitWE, muc_nhitTotal;
        int muc_nlayerBR, muc_nlayerWE, muc_nlayerEE;
        int muc_ntrackBR, muc_ntrackEC;
        int  muc_n3ltracktotal;
        std::vector<int> muc_vlayerSeg;
        std::vector<int> muc_vhitLayer;
        std::vector<int>  muc_vhitSeg;
        std::vector<int> muc_index;

        //Track match
        //TM trigger conditions
        bool tm_NATrk1, tm_NATrk2, tm_ATrk_BB, tm_NBTrk1, tm_NBTrk2, tm_BTrk_BB, tm_NCTrk1, tm_NCTrk2, tm_CTrk_BB;
        
	//trigger lines which can trigger events
        std::vector<std::string> vTrigLine;

	//trigger condition status in each events
	std::vector<int> vTrigCond;

        //trigger condition Id, from 0--47
        bool trigCond[48];

        //trigger data which need to be registered to TDS
        int m_trigCondition[48];
        int m_trigChannel[16];

	//L1 signal
        bool ifpass;
 
        //run mode
        int m_runMode;
        int m_tofLayer;
        std::string trigTableFile;

        //property
        double  L1TC_GATE; 
        double  L1TC_THRESH; 
        int  L1ETOT_L; 
        int  L1ETOT_L_Peak; 
        int  L1ETOT_L_Sigma; 
        int  L1ETOT_M; 
        int  L1ETOT_M_Peak; 
        int  L1ETOT_M_Sigma; 
        int  L1ETOT_BR; 
        int  L1ETOT_BR_Peak; 
        int  L1ETOT_BR_Sigma; 
        int  L1ETOT_EC; 
        int  L1ETOT_EC_Peak; 
        int  L1ETOT_EC_Sigma; 
        int  L1EBL_BR; 
        int  L1EBL_EC; 
        int  L1EDIFF_BR; 
        int  L1EDIFF_EC; 
        int  L1BLK_GATE; 
        int  L1EBL_Z; 
        int  L1EBL_Z_Peak; 
        int  L1EBL_Z_Sigma; 
        int     MAXMDCTRACK; 
        double Energy_Ratio;
        //endcap track match 0: MDC&TOF; 1: MDC&TOF&EMC
        int tm_endcap;

        // Reference to RealizationSvc
       RealizationSvc* m_RealizationSvc;
  
       static const std::string COND_NAME[48];  //The name of each trigger condition
};
#endif
