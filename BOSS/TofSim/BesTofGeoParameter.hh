//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$ID: BesTofGeoParameter.hh

#ifndef BesTofGeoParameter_h
#define BesTofGeoParameter_h 1

#include "globals.hh"
#include <map>
#include <vector>

typedef std::vector<double> VEC;
typedef std::map<std::string, double> CONTAINER;
typedef std::map<std::string, VEC> CONTAINER2;

class BesTofGeoParameter
{
    protected:
        BesTofGeoParameter();
        ~BesTofGeoParameter();

    public:
        static BesTofGeoParameter* GetInstance(); 
        double Get(std::string key);
        VEC GetVec(std::string key);
        void Get_deadChannel(int deadChannel[72][12]);

        G4int GetnScinBr() {return nScinBr;}
        G4double GetBr1L() {return br1L;}
        G4double GetBr1TrapW1() {return br1TrapW1;}
        G4double GetBr1TrapW2() {return br1TrapW2;}
        G4double GetBr1TrapH() {return br1TrapH;}
        G4double GetBr1R1() {return br1R1;}
        G4double GetAlThickness() {return AlThickness;}
        G4double GetPVFThickness() {return PVFThickness;}

        G4double GetBr2L() {return br2L;}
        G4double GetBr2TrapW1() {return br2TrapW1;}
        G4double GetBr2TrapW2() {return br2TrapW2;}
        G4double GetBr2TrapH() {return br2TrapH;}
        G4double GetBr2R1() {return br2R1;}

        G4double GetBucketDBr() {return bucketDBr;}
        G4double GetBucketLBr() {return bucketLBr;}

        G4int GetnScinEc() {return nScinEc;}
        G4double GetEcL() {return ecL;}
        G4double GetEcTrapW1() {return ecTrapW1;}
        G4double GetEcTrapW2() {return ecTrapW2;}
        G4double GetEcTrapH() {return ecTrapH;}
        G4double GetEcTrapH1() {return ecTrapH1;}
        G4double GetzPosEastEc() {return zPosEastEc;}
        G4double GetzPosWestEc() {return zPosWestEc;}
        G4double GetEcR1() {return ecR1;}
        G4double GetEcR2() {return ecR2;}


        G4double GetBucketDEc() {return bucketDEc;}
        G4double GetBucketLEc() {return bucketLEc;}
        G4double GetBucketPosR() {return bucketPosR;}

        G4double GetTau1() {return m_tau1;}
        G4double GetTau2() {return m_tau2;}
        G4double GetTau3() {return m_tau3;}
        G4double GetTauRatio() {return m_tauRatio;}
        G4double GetRefIndex() {return m_refIndex;}
        G4double GetPhNConst() {return m_phNConst;}
        G4double GetCpe2pmt() {return m_Cpe2pmt;}
        G4double GetRAngle() {return m_rAngle;}
        G4double GetQE() {return m_QE;}
        G4double GetCE() {return m_CE;}
        G4double GetPeCorFac() {return m_peCorFac;}
        G4double GetAtten(int i) {return m_atten[i];}

        G4double GetTTSmean() {return m_ttsMean;}
        G4double GetTTSsigma() {return m_ttsSigma;}
        G4double GetPMTgain() {return m_PMTgain;}
        G4double GetCe() {return m_Ce;}
        G4double GetRiseTime() {return m_riseTime;}
        G4double GetLLthresh() {return m_LLthresh;}
        G4double GetHLthresh() {return m_HLthresh;} 
        G4double GetPreGain() {return m_preGain;}
        G4double GetNoiseSigma() {return m_noiseSigma;}

        G4double GetTau1Ec() {return m_tau1Ec;}
        G4double GetTau2Ec() {return m_tau2Ec;}
        G4double GetTau3Ec() {return m_tau3Ec;}
        G4double GetTauRatioEc() {return m_tauRatioEc;}
        G4double GetRefIndexEc() {return m_refIndexEc;}
        G4double GetPhNConstEc() {return m_phNConstEc;}
        G4double GetCpe2pmtEc() {return m_Cpe2pmtEc;}
        G4double GetRAngleEc() {return m_rAngleEc;}
        G4double GetQEEc() {return m_QEEc;}
        G4double GetCEEc() {return m_CEEc;}
        G4double GetPeCorFacEc() {return m_peCorFacEc;}
        G4double GetAttenEc() {return m_attenEc;}

        G4double GetTTSmeanEc() {return m_ttsMeanEc;}
        G4double GetTTSsigmaEc() {return m_ttsSigmaEc;}
        G4double GetPMTgainEc() {return m_PMTgainEc;}
        G4double GetCeEc() {return m_CeEc;}
        G4double GetRiseTimeEc() {return m_riseTimeEc;}
        G4double GetLLthreshEc() {return m_LLthreshEc;}
        G4double GetHLthreshEc() {return m_HLthreshEc;} 
        G4double GetPreGainEc() {return m_preGainEc;}
        G4double GetNoiseSigmaEc() {return m_noiseSigmaEc;}
        //++++++++++ 
        //tian added, for barrel parameters
        G4double GetBrEPMTgain(int scinNb)  { return m_BrEPMTgain[scinNb]; }  
        G4double GetBrERiseTime(int scinNb) { return m_BrERiseTime[scinNb];} 
        G4double GetBrWPMTgain(int scinNb)  { return m_BrWPMTgain[scinNb]; } 
        G4double GetBrWRiseTime(int scinNb) { return m_BrWRiseTime[scinNb];} 



    private:
        //tianhl, for reading barrel parameters
        void ReadData();
        void ReadBrData();
        void ReadMrpcData();


        static BesTofGeoParameter* m_instance;
        G4String m_dataPath;
        CONTAINER container; 
        CONTAINER UNIT;
        CONTAINER2 container2;
        int m_deadChannel[72][12];

        G4int nScinBr;
        G4double br1L;
        G4double br1TrapW1;
        G4double br1TrapW2;
        G4double br1TrapH;
        G4double br1R1;
        G4double AlThickness;
        G4double PVFThickness;

        G4double br2L;
        G4double br2TrapW1;
        G4double br2TrapW2;
        G4double br2TrapH;
        G4double br2R1;


        G4double bucketDBr;
        G4double bucketLBr;

        G4int nScinEc;
        G4double ecL;
        G4double ecTrapW1;
        G4double ecTrapW2;
        G4double ecTrapH;
        G4double ecTrapH1;
        G4double zPosEastEc;
        G4double zPosWestEc;
        G4double ecR1;
        G4double ecR2;

        G4double bucketDEc;
        G4double bucketLEc;
        G4double bucketPosR;

        G4double m_tau1;
        G4double m_tau2;
        G4double m_tau3;
        G4double m_tauRatio;
        G4double m_refIndex;
        G4double m_phNConst;
        G4double m_QE;
        G4double m_CE;
        G4double m_rAngle;
        G4double m_Cpe2pmt;
        G4double m_peCorFac;

        G4double m_ttsMean;
        G4double m_ttsSigma;
        G4double m_PMTgain;
        G4double m_Ce;
        G4double m_riseTime;
        G4double m_LLthresh;
        G4double m_HLthresh;
        G4double m_preGain;
        G4double m_noiseSigma;

        G4double m_tau1Ec;
        G4double m_tau2Ec;
        G4double m_tau3Ec;
        G4double m_tauRatioEc;
        G4double m_refIndexEc;
        G4double m_phNConstEc;
        G4double m_Cpe2pmtEc;
        G4double m_rAngleEc;
        G4double m_QEEc;
        G4double m_CEEc;
        G4double m_peCorFacEc;
        G4double m_attenEc;

        G4double m_ttsMeanEc;
        G4double m_ttsSigmaEc;
        G4double m_PMTgainEc;
        G4double m_CeEc;
        G4double m_riseTimeEc;
        G4double m_LLthreshEc;
        G4double m_HLthreshEc;
        G4double m_preGainEc;
        G4double m_noiseSigmaEc;

        G4double m_atten[176];
        G4double m_BrEPMTgain[176] ; 
        G4double m_BrERiseTime[176]; 
        G4double m_BrWPMTgain[176] ; 
        G4double m_BrWRiseTime[176]; 
};

#endif

