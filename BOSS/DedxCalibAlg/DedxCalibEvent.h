#ifndef DEDXCALIBEVENT_H
#define DEDXCALIBEVENT_H

#include "GaudiKernel/NTuple.h"

#include "DedxCalibAlg/DedxCalib.h"

class DedxCalibEvent: public DedxCalib
{
    public:
        DedxCalibEvent(const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalibEvent() {};

        void initializing();
        void BookHists() {}
        void genNtuple();
        void FillHists() {}
        void AnalyseHists() {}
        void WriteHists() {}

    private:
        NTuple::Tuple* m_nt1;
        NTuple::Item<float>  m_ptrk;
        NTuple::Item<float>  m_ptrk_t;
        NTuple::Item<float>  m_costheta;
        NTuple::Item<float>  m_sintheta;
        NTuple::Item<float>  m_charge;
        NTuple::Item<float>  m_runNO;
        NTuple::Item<float>  m_runFlag; 
        NTuple::Item<float>  m_evtNO;
        NTuple::Item<float>  m_t0;
        NTuple::Item<float>  m_trackId;
        NTuple::Item<float>  m_poca_x;
        NTuple::Item<float>  m_poca_y;
        NTuple::Item<float>  m_poca_z;
        NTuple::Item<float>  m_recalg;
        NTuple::Item<float>  m_nhit;
        NTuple::Item<float>  m_nhits;
        NTuple::Item<float>  m_usedhit;
        NTuple::Item<long>  m_nphlisthit;
        NTuple::Array<double> m_dEdx_hit;
        NTuple::Array<double> m_pathlength_hit; 
        NTuple::Array<double> m_wid_hit;
        NTuple::Array<double> m_layid_hit;
        NTuple::Array<double> m_dd_in_hit;
        NTuple::Array<double> m_eangle_hit;
        NTuple::Array<double> m_zhit_hit;
        NTuple::Item<float>  m_dEdx_meas_hit;
        NTuple::Item<float>  m_dEdx_meas;
        NTuple::Item<float>  m_dEdx_meas_esat;
        NTuple::Item<float>  m_dEdx_meas_norun;
        NTuple::Array<float>  m_probpid;
        NTuple::Array<float>  m_expectid;
        NTuple::Array<float>  m_sigmaid;
        NTuple::Item<float>  m_parttype;
        NTuple::Item<float>  m_chidedxe;
        NTuple::Item<float>  m_chidedxmu;
        NTuple::Item<float>  m_chidedxpi;
        NTuple::Item<float>  m_chidedxk;
        NTuple::Item<float>  m_chidedxp;

        NTuple::Tuple* m_nt2;
        NTuple::Item<float>  m_charge1;
        NTuple::Item<double>  m_phraw;
        NTuple::Item<double>  m_exraw;
        NTuple::Item<float>  m_wire;
        NTuple::Item<float>  m_localwid;
        NTuple::Item<float>  m_runNO1;
        NTuple::Item<float>  m_runFlag1;
        NTuple::Item<float>  m_doca_in;
        NTuple::Item<float>  m_doca_ex;
        NTuple::Item<float>  m_driftdist;
        NTuple::Item<float>  m_eangle;
        NTuple::Item<float>  m_zhit;
        NTuple::Item<float>  m_costheta1;
        NTuple::Item<float>  m_pathL;
        NTuple::Item<float>  m_layer;
        NTuple::Item<float>  m_ptrk1;
        NTuple::Item<float>  m_ptrk_hit;
        NTuple::Item<float>  m_t01;
        NTuple::Item<float>  m_tdc_raw;
        NTuple::Item<float>  m_driftT;
        NTuple::Item<float>  m_trackId1;

};
#endif
