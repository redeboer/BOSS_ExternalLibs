//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: An Fenfen
//Created: Nov, 2015

//---------------------------------------------------------------------------//
// $Id: BesTofDigitizerEcV4.hh

#ifndef BesTofDigitizerEcV4_h
#define BesTofDigitizerEcV4_h 1

#include "BesTofDigitizerV.hh"
#include "BesTofDigi.hh"
#include "BesTofGeoParameter.hh"
#include "globals.hh"
#include "TFile.h"
#include "TTree.h"
#include "ScintSingle.hh"
#include <vector>

using namespace std;

typedef std::map<int, double> AVALANCH;

class BesTofDigitizerEcV4 : public BesTofDigitizerV
{
    public:
        BesTofDigitizerEcV4();
        ~BesTofDigitizerEcV4();

        void initial();
        virtual void Digitize(ScintSingle*, BesTofDigitsCollection*);
        int calStrip(double locZ);
        bool underStrip(double locX, double locZ);
        double calTdcRes_charge(double charge_fC);
        double charge2Time(double charge_fC);
        double calAdcRes_charge(double charge_fC);
        double calTdcRes_charge1(double charge_fC);
        double charge2Time1(double charge_fC);
        double calAdcRes_charge1(double charge_fC);

        struct Param
        {
            Param();
            void print();
            void setPar(int nstep, double E_weight, double E);
            double getAlpha(double E);
            double getEta(double E);
            double getV(double E);

            //parameters fixed
            BesTofGeoParameter* tofPara;
            int nstrip;
            int nmodule;
            double strip_x[12];
            double strip_z;
            double strip_gap;
            int deadChannel[72][12];
            double ngap;
            double gapWidth;
            int nstep;
            double stepWidth;
            double E_weight;
            double eCharge;
            double E;
            double alpha;
            double eta;
            double v_drift;
        };

        struct HitStruct
        {
            HitStruct();
            void initial();
            void print();
            void calTPropagate();
            double calAvaLength();

            Param m_param;
            int trkIndex;
            int pdgCode;
            int ions;
            int strip;
            int underStrip;
            int gap;
            double glbTime;
            double locx;
            double locy;
            double locz;
            double x;
            double y;
            double z;
            double px;
            double py;
            double pz;
            double v_propagate;
            double tPropagate_sphi;
            double tPropagate_xphi;


            AVALANCH ava_num;
            AVALANCH ava_pos;
        };

        struct StripStruct
        {
            StripStruct();
            void initial();
            void print();
            void avalanche();
            void calFirstHit();
            long int calNextN(int num);
            long int multiply(double rdm);
            double calSigma();
            void setPar(double alpha_n, double eta_n, double drift_n, double threshold, bool saturationFlag=true);

            Param m_param;
            vector<HitStruct> hitStructCol;

            //properties to get
            int strip;
            int trkIndex;
            double tStart;
            double tPropagate_sphi;
            double tPropagate_xphi;
            double tThreshold;
            double charge;

            //parameters to tune
            double E;
            double alpha;
            double eta;
            long int threshold;
            double v_drift;
            bool saturationFlag;
        };


    private:
        Param m_param;
        int partId;
        int module;

        double tdc_sphi;
        double tdc_xphi;
        double tdcRes_const;
        double tdcRes;
        double adc;
        double adcRes_const;
        double adcRes;

        double time_leading_sphi;
        double time_leading_xphi;
        double time_trailing_sphi;
        double time_trailing_xphi;

        bool m_rootFlag;
        string m_fileName;
        double m_V;
        double m_threshold;
        TFile* m_file;
        TTree* m_tree;

        int m_nstep;
        double m_E_weight;
        bool m_saturationFlag;
        int m_calTdcRes_charge_flag;
        int m_charge2Time_flag;
        int m_calAdcRes_charge_flag;

        double m_event;
        double m_partId;
        double m_module;
        double m_time_leading_sphi;
        double m_time_leading_xphi;
        double m_time_trailing_sphi;
        double m_time_trailing_xphi;
        double m_tdcRes;
        double m_tdcRes_charge;
        double m_adc;
        double m_adcRes;
        double m_adcRes_charge;
        double m_strip;
        double m_trkIndex;
        double m_tStart;
        double m_tPropagate_sphi;
        double m_tPropagate_xphi;
        double m_tThreshold;
        double m_charge;
        int m_nhit;
        double m_ions_hit[10000];
        double m_trkIndex_hit[10000];
        double m_pdgCode_hit[10000];
        double m_gap_hit[10000];
        double m_underStrip_hit[10000];
        double m_locx_hit[10000];
        double m_locy_hit[10000];
        double m_locz_hit[10000];
        double m_x_hit[10000];
        double m_y_hit[10000];
        double m_z_hit[10000];
        double m_px_hit[10000];
        double m_py_hit[10000];
        double m_pz_hit[10000];
};


#endif

