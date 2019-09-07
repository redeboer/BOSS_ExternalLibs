//    BesIII MDC dE/dx Reconstruction Module
//    Class: MdcDedxTrk
//    Created by Dayong WANG 2003/11

#ifndef _INCLUDE_ExTrk_H
#define _INCLUDE_ExTrk_H

#ifndef _ExTrk_DEFINED_
#define _ExTrk_DEFINED_
#endif

#include <cmath>
#include <vector>

#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcDedx.h"
//#include "MdcDedxAlg/MdcDedxCorrection.h"

class MdcDedxCorrection;
class MdcDedxTrk
{

    public:
        MdcDedxTrk( RecMdcTrack& trk );
        MdcDedxTrk( RecMdcKalTrack& trk_kal, int pid);
        MdcDedxTrk();
        ~MdcDedxTrk();

        void set_ExTrk(  RecMdcTrack& trk );
        void set_ExTrk_Kal(  RecMdcKalTrack& trk_kal, int pid);

        double cal_dedx(  float );
        double cal_dedx_bitrunc(float, int, int& );   
        double cal_dedx_median(  float );
        double cal_dedx_geometric(  float );
        double cal_dedx_geometric_trunc(  float );
        double cal_dedx_harmonic(  float );
        double cal_dedx_harmonic_trunc(  float );
        double cal_dedx_transform(  int );
        double cal_dedx_log(  float , int);   
        double SpaceChargeCorrec( double, double, int, double );

        void set_dEdx( int l, float dEdx_meas, int trkalg,int runflag, int vflag[3], double t0, vector<double> & DedxCurve_Parameter, vector<double> & DedxSigma_Parameter, MdcDedxCorrection*);
        void set_phlist( const vector<double>& phlist) {m_phlist = phlist;}
        void set_phlist_hit( const vector<double>& phlist) {m_phlist_hit = phlist;}
        void setVecDedxHits(const DedxHitRefVec& vecdedxhit) { m_vecdedxhit= vecdedxhit;} 

        vector<double> get_phlist(void) {return m_phlist;}
        vector<double> get_phlist_hit(void) {return m_phlist_hit;}
        DedxHitRefVec  getVecDedxHits() const {return m_vecdedxhit;}

        RecMdcTrack*  trk_ptr() const {return m_trk;}   
        RecMdcKalTrack*  trk_ptr_kal() const {return m_trk_kal;}


        int stat()const {return m_stat;}
        int trk_id()const {return m_trk_id;}
        //int trk_id_kal()const {return m_trk_id_kal;}                          
        int nsample()const {return m_nsample;}
        int quality()const {return m_quality;}
        double dedx()const {return m_dEdx;}
        int charge()const {return m_charge;}
        double P()const {return m_P;}
        double theta()const {return m_theta;}
        double phi()const {return m_phi;}
        double Pt()const {return m_P*std::sin(m_theta);}
        double expect(int pid)const;
        double exp_sigma(int pid)const;
        double prob(int pid)const;
        double chi_dedx(int pid) const;
        double* pexpect() {return dedx_exp;}
        double* pexp_sigma() {return ex_sigma;}
        double* pprob() {return pid_prob;}   
        double* pchi_dedx() {return chi_ex;}  

    private:
        double dedx_exp[5];
        double ex_sigma[5];
        double pid_prob[5];
        double chi_ex[5];
        double m_dEdx;
        float  m_truncate;

        RecMdcTrack* m_trk;
        RecMdcKalTrack* m_trk_kal;
        int m_stat;
        int m_trk_id;
        //int m_trk_id_kal;
        int m_quality;
        int m_charge;
        float m_P; 
        double m_theta;
        double m_phi;
        int m_nsample;   //all the hits on track;
        double m_pl_rp;       

        vector<double> m_phlist;
        vector<double> m_phlist_hit;
        DedxHitRefVec m_vecdedxhit;
};

//--------------------------------------------------------------------
inline double MdcDedxTrk::expect( int pid) const 
{
    if( pid >= 0 && pid < 5 ) return dedx_exp[pid];
    else return -1;
}

inline double MdcDedxTrk::exp_sigma( int pid) const 
{
    if( pid >= 0 && pid < 5 ) return ex_sigma[pid];
    else return -1;
}

inline double MdcDedxTrk::prob( int pid) const 
{
    if( pid >= 0 && pid < 5 ) return pid_prob[pid];
    else return -1;
}

inline double MdcDedxTrk::chi_dedx( int pid) const 
{
    if( pid >= 0 && pid < 5 ) return chi_ex[pid];
    else return -1;
}
#endif
