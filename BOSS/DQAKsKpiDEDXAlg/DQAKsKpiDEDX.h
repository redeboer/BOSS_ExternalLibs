#ifndef Physics_ControlSample_DQAKsKpiDEDX_H
#define Physics_ControlSample_DQAKsKpiDEDX_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class ITHistSvc;

/**********************************************************************/
/** <project name> - 
 ** <description> Head file for event selection program 
 **
 **  \file   
 **
 **  \author Ji Xiaobin 
 **
 **  \version $Id: DQAKsKpiDEDX.h,v 1.1.1.1 2009/09/02 08:46:37 zuojx Exp $
 **
 **
*************************************************************************/

class DQAKsKpiDEDX : public Algorithm {

public:

    DQAKsKpiDEDX(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();  

private:

    double m_vr0cut;
    double m_vz0cut;
    double m_vr1cut;
    double m_vz1cut;  
    double m_cthcut;

    double m_energyThreshold;
    double m_gammaAngCut;

    int m_checkDedx;
    int m_checkTof;

    ITHistSvc* m_thsvc;
    
    NTuple::Tuple*  m_tuple;     // kskpi 4C
    NTuple::Item<long>  m_runNo;
    NTuple::Item<long>  m_event;
//    NTuple::Item<long>  m_nchrg;
//    NTuple::Item<long>  m_nneu;
    NTuple::Item<long>  m_ngch;
    
    NTuple::Array<long>   m_pidcode;  	
    NTuple::Array<double> m_pidprob;
    NTuple::Array<double> m_pidchiDedx;
    NTuple::Array<double> m_pidchiTof1;
    NTuple::Array<double> m_pidchiTof2; 

    NTuple::Array<long>   m_charge;
    NTuple::Array<double> m_vx0;
    NTuple::Array<double> m_vy0;
    NTuple::Array<double> m_vz0;
    NTuple::Array<double> m_vr0;

    NTuple::Array<double> m_vx;
    NTuple::Array<double> m_vy;
    NTuple::Array<double> m_vz;
    NTuple::Array<double> m_vr;

    NTuple::Array<double> m_px;
    NTuple::Array<double> m_py;
    NTuple::Array<double> m_pz;
    NTuple::Array<double> m_p;
    NTuple::Array<double> m_cost;

    NTuple::Array<double> m_probPH;
    NTuple::Array<double> m_normPH;
    NTuple::Array<double> m_chie;
    NTuple::Array<double> m_chimu;
    NTuple::Array<double> m_chipi;
    NTuple::Array<double> m_chik;
    NTuple::Array<double> m_chip;
    NTuple::Array<double> m_ghit;
    NTuple::Array<double> m_thit;

    NTuple::Array<double> m_e_emc;

    NTuple::Array<double> m_qual_etof;
    NTuple::Array<double> m_tof_etof;
    NTuple::Array<double> m_te_etof;
    NTuple::Array<double> m_tmu_etof;
    NTuple::Array<double> m_tpi_etof;
    NTuple::Array<double> m_tk_etof;
    NTuple::Array<double> m_tp_etof;

    NTuple::Array<double> m_qual_btof1;
    NTuple::Array<double> m_tof_btof1;
    NTuple::Array<double> m_te_btof1;
    NTuple::Array<double> m_tmu_btof1;
    NTuple::Array<double> m_tpi_btof1;
    NTuple::Array<double> m_tk_btof1;
    NTuple::Array<double> m_tp_btof1;

    NTuple::Array<double> m_qual_btof2;
    NTuple::Array<double> m_tof_btof2;
    NTuple::Array<double> m_te_btof2;
    NTuple::Array<double> m_tmu_btof2;
    NTuple::Array<double> m_tpi_btof2;
    NTuple::Array<double> m_tk_btof2;
    NTuple::Array<double> m_tp_btof2;

    NTuple::Item<long>   m_npip;
    NTuple::Item<long>   m_npim;
    NTuple::Item<long>   m_nkp;
    NTuple::Item<long>   m_nkm; 
    NTuple::Item<long>   m_np;
    NTuple::Item<long>   m_npb;

    NTuple::Item<double> m_vfits_chi; 
    NTuple::Item<double> m_vfits_vx; 
    NTuple::Item<double> m_vfits_vy; 
    NTuple::Item<double> m_vfits_vz; 
    NTuple::Item<double> m_vfits_vr; 

    NTuple::Item<double> m_vfitp_chi;
    NTuple::Item<double> m_vfitp_vx; 
    NTuple::Item<double> m_vfitp_vy; 
    NTuple::Item<double> m_vfitp_vz; 
    NTuple::Item<double> m_vfitp_vr; 

    NTuple::Item<double> m_vfit2_chi;
    NTuple::Item<double> m_vfit2_mks;
    NTuple::Item<double> m_vfit2_ct;
    NTuple::Item<double> m_vfit2_dl;
    NTuple::Item<double> m_vfit2_dle;

    NTuple::Item<double> m_chi2_fs4c;
    NTuple::Item<double> m_mks_fs4c;
    NTuple::Item<double> m_mkspi_fs4c;
    NTuple::Item<double> m_mksk_fs4c;
    NTuple::Item<double> m_mkpi_fs4c;

    NTuple::Item<double> m_4c_chi2;
    NTuple::Item<double> m_4c_mks;
    NTuple::Item<double> m_4c_mkspi;
    NTuple::Item<double> m_4c_mksk;
    NTuple::Item<double> m_4c_mkpi;
    NTuple::Item<double> m_4c_ks_px;
    NTuple::Item<double> m_4c_ks_py;
    NTuple::Item<double> m_4c_ks_pz;
    NTuple::Item<double> m_4c_ks_p;
    NTuple::Item<double> m_4c_ks_cos;

};


#endif
