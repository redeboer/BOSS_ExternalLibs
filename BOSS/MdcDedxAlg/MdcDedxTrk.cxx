//    BesIII MDC dE/dx Reconstruction Module
//    Class: MdcDedxTrk
//    Created by Dayong WANG 2003/11

#include "MdcDedxAlg/MdcDedxTrk.h"
#include "MdcDedxAlg/MdcDedxCorrection.h"

#include "TGraphErrors.h"
#include "TGraph.h"
#include <stdlib.h>
#include <search.h>
#include <TMath.h>

MdcDedxTrk::MdcDedxTrk()
{
    m_trk = 0;
    m_trk_kal = 0;
    m_stat = -1;
    m_trk_id = 0;
    m_quality = -99;
    m_charge = 0;
    m_P = 0;
    m_theta = 0;
    m_phi = 0;
    m_pl_rp = 0;
    m_nsample = 0;

    m_dEdx = 0;
    m_truncate = 1;
    for( unsigned a = 0; a < 5 ; a++ )
    {
        dedx_exp[a]=0.0;
        ex_sigma[a]=0.0;
        pid_prob[a]=0.0;
        chi_ex[a]=999.0;
    }

#ifdef DEBUG   
    std::cout<<"MdcDedxTrk(1) constructed!"<<std::endl;
#endif   
}

MdcDedxTrk::MdcDedxTrk(  RecMdcTrack& trk )
{
    m_trk = 0;
    m_trk_kal = 0;
    m_stat = -1;
    m_trk_id = 0;
    m_quality = -99;
    m_charge = 0;
    m_P = 0; 
    m_theta = 0;
    m_phi = 0; 
    m_pl_rp = 0;
    m_nsample = 0;
    m_dEdx = 0; 
    m_truncate = 0.7;

    set_ExTrk( trk );
#ifdef DEBUG
    std::cout<<"MdcDedxTrk(2) constructed!"<<std::endl;
#endif   
}

MdcDedxTrk::MdcDedxTrk(  RecMdcKalTrack& trk_kal , int pid) 
{ 
    m_trk = 0;
    m_trk_kal = 0;
    m_stat = -1;
    m_trk_id = 0;
    m_quality = -99;
    m_charge = 0;
    m_P = 0; 
    m_theta = 0;
    m_phi = 0; 
    m_pl_rp = 0;
    m_nsample = 0;
    m_dEdx = 0; 
    m_truncate = 0.7;

    set_ExTrk_Kal( trk_kal, pid ); 
#ifdef DEBUG 
    std::cout<<"MdcDedxTrk(2) kal constructed!"<<std::endl;
#endif 
} 

MdcDedxTrk::~MdcDedxTrk()
{
#ifdef DEBUG   
    std::cout<<"MdcDedxTrk destructed!!!"<<std::endl;
#endif   
}

void MdcDedxTrk::set_ExTrk(  RecMdcTrack& trk )
{
    for( unsigned a = 0; a < 5 ; a++ )
    {
        dedx_exp[a]=0.0;
        ex_sigma[a]=0.0;
        pid_prob[a]=0.0;
        chi_ex[a]=0.0;
    }
    m_stat = 1;
    m_trk = &trk;
    m_trk_id = trk.trackId();
    m_quality = trk.stat();

    m_charge = ( trk.helix(2) > 0 )? 1 : -1;
    float m_Pt = 1.0/fabs( trk.helix(2) );
    m_P = m_Pt*sqrt(1 + trk.helix(4)*trk.helix(4));
    m_theta = M_PI_2 - atan( trk.helix(4) );
    m_phi = ( trk.helix(1) < 3*M_PI_2 )? trk.helix(1)+M_PI_2 : trk.helix(1)-3*M_PI_2;
    m_nsample = trk.getNhits();
    m_pl_rp = 1.5;
    //cout<<"set_ExTrk: "<<trk.helix(0)<<"  "<<trk.helix(1)<<"  "<<trk.helix(2)<<"  "<<trk.helix(3)<<"  "<<trk.helix(4)<<endl;

#ifdef DEBUG     
    std::cout<<"MdcDedxTrk::set_ExTrk(&trk)!!!"<<std::endl;
#endif
}

void MdcDedxTrk::set_ExTrk_Kal(  RecMdcKalTrack& trk_kal, int pid)
{
    DstMdcKalTrack* dstTrk = &trk_kal;
    for( unsigned a = 0; a < 5 ; a++ )
    {
        dedx_exp[a]=0.0;
        ex_sigma[a]=0.0;
        pid_prob[a]=0.0;
        chi_ex[a]=0.0;
    }      
    if(pid<0 || pid>4) pid = 2;
    m_stat = 1;
    m_trk_kal = &trk_kal;
    m_trk_id = trk_kal.trackId();
    m_quality = dstTrk->getStat(pid);

    HepVector kal_helix = dstTrk->getFHelix(pid);
    m_charge = ( kal_helix[2] > 0 )? 1 : -1;
    float m_Pt = 1.0/fabs( kal_helix[2] );
    m_P = m_Pt*sqrt(1 + kal_helix[4]*kal_helix[4]);
    m_theta = M_PI_2 - atan( kal_helix[4] );
    m_phi = ( kal_helix[1] < 3*M_PI_2 )? kal_helix[1]+M_PI_2 : kal_helix[1]-3*M_PI_2;
    m_nsample = trk_kal.getNhits(pid);
    m_pl_rp = 1.5;
    //cout<<"set_ExTrk_Kal: "<<kal_helix[0]<<"  "<<kal_helix[1]<<"  "<<kal_helix[2]<<"  "<<kal_helix[3]<<"  "<<kal_helix[4]<<endl;
#ifdef DEBUG     
    std::cout<<"MdcDedxTrk::set_ExTrk_Kal(&trk_kal)!!!"<<std::endl;
#endif
}


double MdcDedxTrk::cal_dedx( float truncate )
{
    m_truncate = truncate;
    sort(m_phlist.begin(),m_phlist.end());
    int nsampl = (int)( m_phlist.size()*truncate );
    double qSum = 0;
    unsigned int i = 0;
    for(vector<double>::iterator ql= m_phlist.begin();ql!=m_phlist.end();ql++)
    {
        i++;
        if(i<= nsampl) qSum += (*ql);
    }

    float trunc=qSum/nsampl;
    return trunc;
    std::cout<<"MdcDedxTrk::cal_dedx()!!!"<<std::endl;
}

double MdcDedxTrk::cal_dedx_bitrunc(float truncate, int alg, int & usedhit )
{
    m_truncate = truncate;
    sort(m_phlist.begin(),m_phlist.end());
    int nsampl = (int)( m_phlist.size()*truncate );
    int smpl = (int)(m_phlist.size()*(truncate+0.05));
    int min_cut = (int)( m_phlist.size()*0.05 + 0.5 );
    double qSum = 0;
    unsigned i = 0;
    for(vector<double>::iterator ql= m_phlist.begin();ql!=m_phlist.end();ql++)
    {
        i++;
        if(i<= smpl && i>=min_cut ) qSum += (*ql);
    }
    double trunc=-99;
    usedhit = smpl-min_cut+1;
    if(usedhit>0)  trunc=qSum/usedhit;

    return trunc;
    std::cout<<"MdcDedxTrk::cal_dedx()!!!"<<std::endl;
}

double MdcDedxTrk::cal_dedx_median( float truncate )
{
    sort(m_phlist.begin(),m_phlist.end());

    int nsample = m_phlist.size();
    double median;   
    if( fmod(double(nsample),2.0) ) {   
        median = m_phlist[(nsample-1)/2];
    } else {
        median= 0.5*( m_phlist[nsample/2] + m_phlist[nsample/2-1] );
    }
    return median;
}

double MdcDedxTrk::cal_dedx_geometric( float truncate )
{
    sort(m_phlist.begin(),m_phlist.end());

    int nsampl = m_phlist.size();
    double qSum = 1.0;
    for(vector<double>::iterator ql= m_phlist.begin();ql!=m_phlist.end();ql++){
        qSum *= (*ql);
    }

    double trunc = pow(qSum,1/double(nsampl));
    return trunc;
    std::cout<<"MdcDedxTrk::cal_dedx()!!!"<<std::endl;
}

double MdcDedxTrk::cal_dedx_geometric_trunc( float truncate )
{
    m_truncate = truncate;
    sort(m_phlist.begin(),m_phlist.end());
    int nsampl = (int)( m_phlist.size()*truncate );
    double qSum = 1.0;
    unsigned i = 0;
    for(vector<double>::iterator ql= m_phlist.begin();ql!=m_phlist.end();ql++){
        i++;
        if(  i<= nsampl )
          qSum *= (*ql);
    }

    double trunc = pow(qSum,1/double(nsampl));
    return trunc;

    std::cout<<"MdcDedxTrk::cal_dedx()!!!"<<std::endl;
}

double MdcDedxTrk::cal_dedx_harmonic( float truncate )
{
    sort(m_phlist.begin(),m_phlist.end());

    int nsampl = m_phlist.size();
    double qSum = 0;
    for(vector<double>::iterator ql= m_phlist.begin();ql!=m_phlist.end();ql++){
        qSum += 1/(*ql);
    }

    float trunc=nsampl/qSum;
    return trunc;
    std::cout<<"MdcDedxTrk::cal_dedx()!!!"<<std::endl;
}

double MdcDedxTrk::cal_dedx_harmonic_trunc( float truncate )
{
    m_truncate = truncate;
    sort(m_phlist.begin(),m_phlist.end());
    int nsampl = (int)( m_phlist.size()*truncate );
    double qSum = 0;
    unsigned i = 0;
    for(vector<double>::iterator ql= m_phlist.begin();ql!=m_phlist.end();ql++){
        i++;
        if(  i<= nsampl )
          qSum += 1/(*ql);
    }

    float trunc= nsampl/qSum;
    return trunc;
    std::cout<<"MdcDedxTrk::cal_dedx()!!!"<<std::endl;
}

double MdcDedxTrk::cal_dedx_transform( int alg )
{
    sort(m_phlist.begin(),m_phlist.end());

    int nsampl = m_phlist.size();
    double qSum = 0;
    for(vector<double>::iterator ql= m_phlist.begin();ql!=m_phlist.end();ql++){
        qSum += 1/sqrt(*ql);
    }

    float trunc=1/qSum;

    return trunc;
    std::cout<<"MdcDedxTrk::cal_dedx()!!!"<<std::endl;
}


double MdcDedxTrk::cal_dedx_log( float truncate ,int alg)
{
    double qSum = 0;
    unsigned i = 0;
    for(vector<double>::iterator ql= m_phlist.begin();ql!=m_phlist.end();ql++){
        i++;
        qSum += log(*ql);
    }

    float trunc=qSum/m_phlist.size();
    return trunc;
    std::cout<<"MdcDedxTrk::cal_dedx_log()!!!"<<std::endl;
}



void MdcDedxTrk::set_dEdx( int landau, float dEdx, int trkalg, int runflag, int vflag[3] , double t0, vector<double>& DedxCurve_Parameter, vector<double>& DedxSigma_Parameter, MdcDedxCorrection* ex_calib)
{
#ifdef DEBUG
    cout<<"in MdcDedxTrk::set_dEdx()  landau: "<<landau<<"  dedx:  "<<dEdx<<" nsample(): "<<nsample()<<"  ph size: "<<m_phlist.size()<<"  m_P: "<<m_P<<"  theta: "<<m_theta<<"  pl-rp: "<<m_pl_rp<<endl;
#endif   

    m_dEdx = dEdx;
    int dedxhit_use = (int)(m_phlist.size()*m_truncate);  

    //some old data with od methods
    if(runflag ==1 || runflag ==2 )
      ex_calib->dedx_pid_exp_old( landau, runflag, (float)dEdx, (int)dedxhit_use,
                  (float)m_P, (float)m_theta, (float)t0,(float)m_pl_rp,
                  dedx_exp, ex_sigma, pid_prob, chi_ex);
    //for 2009 psip data and after
    else
      ex_calib->dedx_pid_exp( vflag, (float)dEdx, trkalg,(int)dedxhit_use,
                  (float)m_P, (float)m_theta, (float)t0,(float)m_pl_rp,
                  dedx_exp, ex_sigma, pid_prob, chi_ex, DedxCurve_Parameter, DedxSigma_Parameter); 

#ifdef DEBUG
    std::cout<<"MdcDedxTrk::set_dEdx()!!!"<<std::endl;
#endif   
}

//---------------------------------------------------------------------------
double MdcDedxTrk::SpaceChargeCorrec(double m_theta, double mom, int Particle, double dEdx )
{
    const int    par_cand( 5 );
    const float  Charge_Mass[par_cand] = {0.00051100, 0.10566, 0.13957, 0.4937, 0.93827 }; 
    double       beta_G;
    double e_Par[5] = {143.349, 1.7315, 0.192616, 2.90437, 1.08248};
    double Beta_Gamma[22] ={0.373026, 0.479605, 0.586184, 0.692763, 0.799342, 782.779, 1565.56,
        2348.34,  17.2727,  18.1245,  1.43297,  2.14946,  12.1803, 13.6132, 6.62515,  10.4109,  
        14.1967,  17.9825,  21.7683,  26.0274, 30.7596, 35.4919 }; 
    double K_par[22] ={4.64411e-05, 5.86544e-05, 8.05289e-05, 8.46981e-05, 8.92014e-05, 4.74517e-05, 
        4.51684e-05, 5.32732e-05, 6.12803e-05, 6.14592e-05, 8.08608e-05, 6.73184e-05, 5.46448e-05, 
        6.1377e-05,  6.57385e-05, 7.03053e-05, 6.61171e-05, 6.86824e-05, 6.246e-05,   7.25988e-05, 
        7.11034e-05, 6.24924e-05 };
    double D_par[22] ={0.0871504, 0.0956379, 0.117193,  0.118647,  0.127203, 0.0566449, 0.0529198,
        0.0642525, 0.0764562, 0.081341, 0.0952263, 0.0987536, 0.0639901, 0.0845994,0.0777062, 
        0.0823206, 0.0783874, 0.079537, 0.0815792, 0.0849875, 0.0824751,0.0776296 };
    double DSqr_par[22] = {0.00759519,  0.0091466,  0.0137341,  0.0140772,  0.0161807, 0.00320864,
        0.00280051, 0.00412839, 0.00584555, 0.00661636, 0.00906805, 0.00975227, 0.00409473, 
        0.00715706, 0.00603826, 0.00677668, 0.00614458, 0.00632613, 0.00665516, 0.00722288, 
        0.00680214, 0.00602635}; 

    beta_G = mom/Charge_Mass[Particle];
    if(beta_G <0.3) beta_G =0.3;
    double bet=beta_G/TMath::Sqrt(beta_G*beta_G+1);
    double fterm=TMath::Log(e_Par[2]+1/pow(beta_G,e_Par[4]));
    double fitval=e_Par[0]/pow(bet,e_Par[3])*(e_Par[1]-pow(bet,e_Par[3])-fterm);
    TGraphErrors *gr1 = new TGraphErrors(22,Beta_Gamma, K_par,0,0);
    TGraphErrors *gr2 = new TGraphErrors(22,Beta_Gamma, DSqr_par,0,0);

    double par[3];
    par[0] = fitval;
    par[1] = gr1->Eval(m_theta);
    par[2] = gr2->Eval(m_theta);
    Double_t y = fabs(cos(m_theta));
    double electron_par[3] ={334.032, 6.20658e-05, 0.00525673};
    double arg= TMath::Sqrt(y*y+ par[2]);
    //double cal_factor =par[0]*TMath::Exp(-(par[1]* par[0])/arg);
    double cal_factor =TMath::Exp(-(par[1]* par[0])/arg);
    double arg_electron = TMath::Sqrt(y*y + electron_par[2]);
    //double electron_factor = electron_par[0]*TMath::Exp(-(electron_par[1]* electron_par[0])/arg);
    double electron_factor = TMath::Exp(-(electron_par[1]* electron_par[0])/arg_electron);
    //cout<<"cal_factor = "<<cal_factor<<"         electron_factor = "<<electron_factor<<endl;
    double dedx_cal = dEdx/(cal_factor/electron_factor);
    //double dedx_cal = dEdx/cal_factor;
    //cout<<"m_theta= "<<m_theta<<"      y ="<<y<<"      beta_G  = "<<beta_G <<"     dEdx = "<<dEdx<<"     cal dedx = "<<dedx_cal<<endl;
    delete gr1;
    delete gr2;
    return dedx_cal;
}
