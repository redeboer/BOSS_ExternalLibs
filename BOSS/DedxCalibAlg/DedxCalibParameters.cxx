#include <TMath.h>
#include <cmath>
#include <vector>
#include <iostream>
#include "TGraphErrors.h"

#include "DedxCalibAlg/DedxCalibParameters.h"

extern "C" 
{
    float vavset_ (double *, double *, int*);
    float vavden_ (double *);
    float prob_ (float *, int*);
}


double mylan(double *x, double *par)
{
    double kk=(x[0]-par[1])/par[2];
    double fterm=kk+exp(-1*kk);
    double fitval=par[0]*exp(par[3]*fterm);
    return fitval;
}

double landaun(double *x, double *par)
{  

    double kk=(x[0]-par[1])/par[2];
    double fterm=kk+exp(-1*kk);
    double fitval=par[0]*exp(-0.5*fterm);
    return fitval;
} 

double Landau(double *x, double *par)
{  
    double fitval = TMath::Landau(x[0], par[0], par[1], kFALSE);
    return fitval;
}


double Vavilov(double *x, double *par)
{
    double kappa, beta2;
    int mode = 0;
    vavset_(&par[0], &par[1], &mode);
    double fitval = vavden_(&x[0]);
    return fitval;
}

double AsymGauss(double *x, double *par)
{
    double delta, a, b, c, d, fitval;
    a = TMath::Sqrt(log(4.0));
    b = (x[0]-par[1])/par[2];
    c = TMath::Power(par[3],2.0);
    delta = (1+ TMath::SinH(par[3]*a)/a)*b;
    d = TMath::Power(log(delta),2.0)/c+c;
    fitval = par[0]*TMath::Exp(-0.5*d);
    return fitval;
}

/*  -------------   calculate the expects of dE/dx   -------------*/
void dedx_pid_exp_old( int landau, int runflag, float dedx,
            int Nohit, float mom, float theta, float t0, float lsamp,
            double dedx_exp[5], double ex_sigma[5],
            double pid_prob[5], double chi_dedx[5]    )
{
    double par[5], sigma_par[4], sigma_index_nhit, sigma_index_sin;

    if(runflag==1) {
        par[0]= HV1_curvep0;
        par[1]= HV1_curvep1;
        par[2]= HV1_curvep2;
        par[3]= HV1_curvep3;
        par[4]= HV1_curvep4;
        sigma_par[0] = HV1_sigmap0;
        sigma_par[1] = HV1_sigmap1;
        sigma_par[2] = HV1_sigmap2;
        sigma_par[3] = HV1_sigmap3;
        sigma_index_nhit = HV1_index_nhit;
        sigma_index_sin  = HV1_index_sin;
    }
    else if(runflag==2) {
        par[0]= HV2_curvep0;
        par[1]= HV2_curvep1;
        par[2]= HV2_curvep2;
        par[3]= HV2_curvep3;
        par[4]= HV2_curvep4;
        sigma_par[0] = HV2_sigmap0;
        sigma_par[1] = HV2_sigmap1;
        sigma_par[2] = HV2_sigmap2;
        sigma_par[3] = HV2_sigmap3;
        sigma_index_nhit = HV2_index_nhit;
        sigma_index_sin  = HV2_index_sin;
    }

    const int    par_cand( 5 );
    const float  Charge_Mass[par_cand] = {0.00051100, 0.10566, 0.13957, 0.4937, 0.93827 };
    double       beta_G, beta, betterm, bethe_B, sig_param;

    int          Nmax_prob( 0 );
    float        max_prob( -0.01 );
    int ndf;
    float chi2;

    for( int it = 0; it < par_cand; it++ ) {
        beta_G = mom/Charge_Mass[it];
        beta = beta_G/sqrt(1+(beta_G)*(beta_G));
        betterm = par[1]-log(par[2]+pow(1/beta_G,par[4]));
        bethe_B = par[0]/pow(beta,par[3])*betterm-par[0];

        if( Nohit >0 ) {
            dedx_exp[it] = bethe_B;
            double sig_the=std::sin( (double)theta );

            if(runflag <3 && runflag>0){
                if(landau == 0) {
                    sig_param = 1.6*std::sin( (double) theta )/(lsamp*double(Nohit));
                    ex_sigma[it] = 0.05*bethe_B*sqrt( 50.0*sig_param );
                }
                else {
                    //currently use one sigmap0
                    if(beta_G < 4) {
                        sig_param=sigma_par[1]+sigma_par[2]*std::pow(beta_G,sigma_par[3]);
                    } else {
                        sig_param= sigma_par[0];
                    }
                    //double sig_the=std::sin( (double)theta );
                    sig_the=std::pow(sig_the,sigma_index_sin);
                    double sig_n;
                    sig_n=35.0/double(Nohit);
                    sig_n=std::pow(sig_n,sigma_index_nhit);
                    ex_sigma[it]=sig_param*sig_the*sig_n;
                }
            }

            double dedx_correc;
            if(runflag == 2 ) dedx_correc = SpaceChargeCorrec(theta, mom, it, dedx);
            else  dedx_correc = dedx;
            chi_dedx[it] = (dedx_correc - dedx_exp[it])/ex_sigma[it];
            chi2 = chi_dedx[it]*chi_dedx[it];
            ndf = 1;
            pid_prob[it] = prob_(&chi2,&ndf);
            //if(it ==0 ) cout<<"runflag: "<<runflag<<"    dedx : "<<dedx<<"      chi_dedx: "
            //<<chi_dedx[it] <<"      ptrk: "<<mom<<endl; 
            if( it == -999 ){            // here a debug flag
                std::cout << " mom = " << mom <<"exp"<< dedx_exp[it]
                    <<  " sigma "<<ex_sigma[it] <<"  prob   "<<pid_prob[it]
                    << std::endl;
            } 
            if( pid_prob[it] > max_prob ) {
                max_prob = pid_prob[it];
                Nmax_prob = it;
            }
        }       
        else {      
            dedx_exp[it] = 0.0;
            ex_sigma[it] = 1000.0;
            pid_prob[it] = 0.0;
            chi_dedx[it] = 999.0;
        }           
    }                   
}                   

void dedx_pid_exp(int vflag[3], float dedx, int trkalg,
            int Nohit, float mom, float theta, float t0, float lsamp,
            double dedx_exp[5], double ex_sigma[5], 
            double pid_prob[5], double chi_dedx[5],
            std::vector<double> & par, std::vector<double> & sig_par)
{
    const int    par_cand( 5 );
    const float  Charge_Mass[par_cand] = {0.00051100, 0.10566, 0.13957, 0.4937, 0.93827 };
    double       beta_G, beta, betterm, bethe_B;

    int dedxflag = vflag[0];
    int sigmaflag = vflag[1]; 
    bool ifMC = false;
    if(vflag[2] == 1)  ifMC = true;

    int          Nmax_prob(0);
    float        max_prob(-0.01);
    int ndf;
    float  chi2;

    for( int it = 0; it < par_cand; it++ ) {
        beta_G = mom/Charge_Mass[it];

        if(dedxflag == 1){
            beta = beta_G/sqrt(1+(beta_G)*(beta_G));
            betterm = par[1]-log(par[2]+pow(1/beta_G,par[4]));
            bethe_B = par[0]/pow(beta,par[3])*betterm-par[0];
        }
        else if(dedxflag == 2) {
            double A=0, B=0,C=0;
            double x = beta_G;
            if(x<4.5)
              A=1;
            else if(x<10)
              B=1;
            else
              C=1;
            double partA = par[0]*pow(sqrt(x*x+1),par[2])/pow(x,par[2])*(par[1]-par[5]*log(pow(1/x,par[3])) ) - par[4]+exp(par[6]+par[7]*x);
            double partB = par[8]*pow(x,3)+par[9]*pow(x,2)+par[10]*x+par[11];
            double partC = -par[12]*log(par[15]+pow(1/x,par[13]))+par[14];
            bethe_B = 550*(A*partA+B*partB+C*partC);
            //for fermi plateau ( the electron region) we just use 1.0
            if(beta_G> 100)  bethe_B=550*1.0;
        }

        if (ifMC) {
            double A=0, B=0,C=0;
            double x = beta_G;
            if(x<4.5)
              A=1;
            else if(x<10)
              B=1;
            else 
              C=1;
            double partA = par[0]*pow(sqrt(x*x+1),par[2])/pow(x,par[2])*(par[1]-par[5]*log(pow(1/x,par[3])) ) - par[4]+exp(par[6]+par[7]*x);
            double partB = par[8]*pow(x,3)+par[9]*pow(x,2)+par[10]*x+par[11];
            double partC = -par[12]*log(par[15]+pow(1/x,par[13]))+par[14];
            bethe_B = 550*(A*partA+B*partB+C*partC);
            //for fermi plateau ( the electron region) we just use 1.0
            if(beta_G> 100)  bethe_B=550*1.0;
        }   


        if (Nohit > 0) {
            dedx_exp[it] = bethe_B;
            double sig_the = std::sin((double)theta);
            double f_betagamma, g_sinth, h_nhit, i_t0;

            if (ifMC) {

                double x= beta_G;
                double nhit = (double)Nohit;
                double sigma_bg=1.0;
                if (x > 0.3)
                  sigma_bg = sig_par[0]*exp(sig_par[1]*x)+sig_par[2]*exp(sig_par[3]*pow(x,0.25))+sig_par[4];
                else
                  sigma_bg = sig_par[5]*exp(sig_par[6]*x)+ sig_par[7];

                double cor_nhit = 1.0;
                if (nhit < 5) nhit = 5;
                if (nhit <= 35)
                  cor_nhit =  sig_par[8]*pow(nhit,4)+sig_par[9]*pow(nhit,3)+sig_par[10]*pow(nhit,2) +
                      sig_par[11]*nhit+sig_par[12];

                double cor_sin=  1.0;
                if(sig_the>0.99) sig_the=0.99;
                cor_sin = sig_par[13]*pow(sig_the,4)+sig_par[14]*pow(sig_the,3) +
                    sig_par[15]*pow(sig_the,2)+sig_par[16]*sig_the+sig_par[17];

                //sigma vs t0
                double cor_t0 = 1.0;

                //calculate sigma
                if (trkalg == 1)
                  ex_sigma[it] = 550 * sigma_bg * cor_nhit * cor_sin * cor_t0;
                else
                  ex_sigma[it] = 550 * sigma_bg * cor_nhit * cor_sin * cor_t0 * sig_par[18];
            }
            else {
                if(sigmaflag == 1) {
                    f_betagamma=sig_par[0]*pow(beta_G,sig_par[1])+sig_par[2];
                    g_sinth=(sig_par[3]*sig_the*sig_the+sig_par[4])/(sig_par[3]*sig_par[5]*sig_par[5]+sig_par[4]);
                    h_nhit=(sig_par[6]*Nohit*Nohit+sig_par[7]*Nohit+sig_par[8]) /
                        (sig_par[6]*sig_par[9]*sig_par[9]+sig_par[7]*sig_par[9]+sig_par[8]);
                    if(sig_par[13] != 0)
                      i_t0 = (sig_par[10]*t0*t0+sig_par[11]*t0+sig_par[12]) /
                          (sig_par[10]*sig_par[13]*sig_par[13]+sig_par[11]*sig_par[13]+sig_par[12]);
                    else if(sig_par[13] == 0)
                      i_t0 =1;
                    //cout<<"f_betagamma : "<<f_betagamma<<"       g_sinth : "<<g_sinth<<"       h_nhit : "
                    //<<h_nhit<<"     i_t0 : "<<i_t0<<endl;  
                    ex_sigma[it]= f_betagamma* g_sinth * h_nhit * i_t0;
                }
                else if(sigmaflag == 2) {
                    double x = beta_G;
                    double nhit = (double)Nohit;
                    double sigma_bg=1.0;
                    if (x > 0.3)
                      sigma_bg = sig_par[0]*exp(sig_par[1]*x)+sig_par[2]*exp(sig_par[3]*pow(x,0.25))+sig_par[4];
                    else
                      sigma_bg = sig_par[5]*exp(sig_par[6]*x)+ sig_par[7];

                    double cor_nhit=1.0;
                    if(nhit<5) nhit=5;
                    if (nhit <= 35)
                      cor_nhit =  sig_par[8]*pow(nhit,4)+sig_par[9]*pow(nhit,3)+sig_par[10]*pow(nhit,2) +  sig_par[11]*nhit+sig_par[12];

                    double cor_sin=  1.0;
                    if(sig_the>0.99) sig_the = 0.99;
                    cor_sin = sig_par[13]*pow(sig_the,4)+sig_par[14]*pow(sig_the,3) +
                        sig_par[15]*pow(sig_the,2)+sig_par[16]*sig_the+sig_par[17];

                    double cor_t0 = 1;
                    if (t0 > 1200) t0 = 1200;
                    if (t0 > 800)
                      cor_t0 = sig_par[18]*pow(t0,2)+sig_par[19]*t0+sig_par[20];

                    ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin*cor_t0;
                }
                else if(sigmaflag == 3) {
                    double x= beta_G;
                    double nhit = (double)Nohit;
                    double sigma_bg=1.0;
                    if (x > 0.3)
                      sigma_bg = sig_par[0]*exp(sig_par[1]*x)+sig_par[2]*exp(sig_par[3]*pow(x,0.25))+sig_par[4];
                    else
                      sigma_bg = sig_par[5]*exp(sig_par[6]*x)+ sig_par[7];

                    double cor_nhit = 1.0;
                    if (nhit < 5) nhit = 5;
                    if (nhit <= 35)
                      cor_nhit =  sig_par[8]*pow(nhit,4)+sig_par[9]*pow(nhit,3)+sig_par[10]*pow(nhit,2) +  sig_par[11]*nhit+sig_par[12];

                    double cor_sin=  1.0;
                    if(sig_the>0.99) sig_the=0.99;
                    cor_sin = sig_par[13]*pow(sig_the,4)+sig_par[14]*pow(sig_the,3) +
                        sig_par[15]*pow(sig_the,2)+sig_par[16]*sig_the+sig_par[17];

                    ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin;
                }   
                else if(sigmaflag == 4) {
                    double x= beta_G;
                    double nhit = (double)Nohit;
                    double sigma_bg=1.0;
                    if (x > 0.3)
                      sigma_bg = sig_par[0]*exp(sig_par[1]*x)+sig_par[2]*exp(sig_par[3]*pow(x,0.25))+sig_par[4];
                    else
                      sigma_bg = sig_par[5]*exp(sig_par[6]*x)+ sig_par[7];

                    double cor_nhit = 1.0;
                    if (nhit < 5) nhit = 5;
                    if (nhit <= 35)
                      cor_nhit =  sig_par[8]*pow(nhit,4)+sig_par[9]*pow(nhit,3)+sig_par[10]*pow(nhit,2) +  sig_par[11]*nhit+sig_par[12];

                    double cor_sin=  1.0;
                    if(sig_the>0.99) sig_the=0.99;
                    cor_sin = sig_par[13]*pow(sig_the,4)+sig_par[14]*pow(sig_the,3) +
                        sig_par[15]*pow(sig_the,2)+sig_par[16]*sig_the+sig_par[17];

                    if(trkalg==1)
                      ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin;
                    else
                      ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin*sig_par[18];
                }
                else if(sigmaflag == 5) {
                    double x = beta_G;
                    double nhit = (double)Nohit;
                    double sigma_bg=1.0;
                    if (x > 0.3)
                      sigma_bg = sig_par[0]*exp(sig_par[1]*x)+sig_par[2]*exp(sig_par[3]*pow(x,0.25))+sig_par[4];
                    else
                      sigma_bg = sig_par[5]*exp(sig_par[6]*x)+ sig_par[7];

                    double cor_nhit=1.0;
                    if(nhit<5) nhit=5;
                    if (nhit <= 35)
                      cor_nhit =  sig_par[8]*pow(nhit,4)+sig_par[9]*pow(nhit,3)+sig_par[10]*pow(nhit,2) +
                          sig_par[11]*nhit+sig_par[12];
                    double cor_sin=  1.0;
                    if(sig_the>0.99) sig_the = 0.99;
                    cor_sin = sig_par[13]*pow(sig_the,4)+sig_par[14]*pow(sig_the,3) +
                        sig_par[15]*pow(sig_the,2)+sig_par[16]*sig_the+sig_par[17];

                    double cor_t0 = 1;
                    if (t0 > 1200) t0 = 1200;
                    if (t0 > 800)
                      cor_t0 = sig_par[18]*pow(t0,2)+sig_par[19]*t0+sig_par[20];

                    if(trkalg==1)
                      ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin*cor_t0;
                    else
                      ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin*cor_t0*sig_par[21];
                }   
            }       

            double dedx_correc = dedx;
            chi_dedx[it] =  (dedx_correc - dedx_exp[it])/ex_sigma[it];
            chi2 = chi_dedx[it]*chi_dedx[it]; 
            ndf=1;
            pid_prob[it] = prob_(&chi2,&ndf);
            //if(it ==0 ) cout<<"runflag: "<<runflag<<"    dedx : "<<dedx<<"      chi_dedx: "
            //<<chi_dedx[it] <<"      ptrk: "<<mom<<endl; 
            if (it == -999) {            // here a debug flag
                std::cout << " mom = " << mom <<"exp"<< dedx_exp[it]
                    <<  " sigma "<<ex_sigma[it] <<"  prob   "<<pid_prob[it]
                    << std::endl;
            } 
            if( pid_prob[it] > max_prob ){
                max_prob = pid_prob[it];
                Nmax_prob = it;
            }       
        }           
        else{       
            dedx_exp[it] = 0.0;
            ex_sigma[it] = 1000.0;
            pid_prob[it] = 0.0;
            chi_dedx[it] = 999.0;
        } //if Nohit > 0
    }
}

double SpaceChargeCorrec(double m_theta, double mom, int Particle, double dEdx )
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
