//    BesIII MDC dE/dx Reconstruction Module
//    Class: MdcDedxCorrection
//    Created by Dayong WANG 2003/11

//#include "CLHEP/config/CLHEP.h"
#include "MdcDedxAlg/MdcDedxCorrection.h"
#include "MdcDedxAlg/MdcDedxParam.h"
#include "MdcDedxAlg/MdcDedxTrk.h"

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

extern "C" {float prob_ (float *, int*);}

MdcDedxCorrection::MdcDedxCorrection()
{
#ifdef DEBUG
    std::cout<<"MdcDedxCorrection constructed!!!"<<std::endl;
#endif
}

MdcDedxCorrection::~MdcDedxCorrection()
{
#ifdef DEBUG
    std::cout<<"MdcDedxCorrection destructed!!!"<<std::endl;
#endif
}


/*  -------------   calculate the expects of dE/dx   -------------*/
void MdcDedxCorrection::dedx_pid_exp_old( int landau, int runflag, float dedx, 
            int Nohit, float mom, float theta, float t0, float lsamp, 
            double dedx_exp[5], double ex_sigma[5], 
            double pid_prob[5], double chi_dedx[5]    ) const
{
    double par[5], sigma_par[4], sigma_index_nhit, sigma_index_sin; 

    if(runflag==1) {
        par[0]= MdcDedxParam::HV1_curvep0;
        par[1]= MdcDedxParam::HV1_curvep1;
        par[2]= MdcDedxParam::HV1_curvep2;
        par[3]= MdcDedxParam::HV1_curvep3;
        par[4]= MdcDedxParam::HV1_curvep4;
        sigma_par[0] = MdcDedxParam::HV1_sigmap0;
        sigma_par[1] = MdcDedxParam::HV1_sigmap1;
        sigma_par[2] = MdcDedxParam::HV1_sigmap2;
        sigma_par[3] = MdcDedxParam::HV1_sigmap3;
        sigma_index_nhit = MdcDedxParam::HV1_index_nhit;
        sigma_index_sin  = MdcDedxParam::HV1_index_sin;
    }
    else if(runflag==2) {
        par[0]= MdcDedxParam::HV2_curvep0;
        par[1]= MdcDedxParam::HV2_curvep1;
        par[2]= MdcDedxParam::HV2_curvep2;
        par[3]= MdcDedxParam::HV2_curvep3;
        par[4]= MdcDedxParam::HV2_curvep4;
        sigma_par[0] = MdcDedxParam::HV2_sigmap0;
        sigma_par[1] = MdcDedxParam::HV2_sigmap1;
        sigma_par[2] = MdcDedxParam::HV2_sigmap2;
        sigma_par[3] = MdcDedxParam::HV2_sigmap3;
        sigma_index_nhit = MdcDedxParam::HV2_index_nhit;
        sigma_index_sin  = MdcDedxParam::HV2_index_sin;
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

            MdcDedxTrk dedxtrk;   
            double dedx_correc;        
            if(runflag == 2 ) dedx_correc = dedxtrk.SpaceChargeCorrec(theta, mom, it, dedx);
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
    //   std::cout<<"MdcDedxCorrection::dedx_pid_exp(blum)!!!"<<std::endl; 
}

void MdcDedxCorrection::dedx_pid_exp(int vflag[3], float dedx, int trkalg,
            int Nohit, float mom, float theta, float t0, float lsamp,
            double dedx_exp[5], double ex_sigma[5], 
            double pid_prob[5], double chi_dedx[5],
            std::vector<double> & par, std::vector<double> & sig_par) const 
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

	      if(sigmaflag >= 6) {
		
		double nhit = (double)Nohit;
		double sigma_bg=1.0;
		double ndedx=bethe_B/550;
		sigma_bg= sig_par[0]+sig_par[1]*ndedx+sig_par[2]*ndedx*ndedx;
		
		double cor_nhit = 1.0;
		if (nhit < 5) nhit = 5;
		if (nhit <= 35)
		  cor_nhit =  sig_par[3]*pow(nhit,4)+sig_par[4]*pow(nhit,3)+sig_par[5]*pow(nhit,2) +
		    sig_par[6]*nhit+sig_par[7];
		
		double cor_sin=  1.0;
		if(sig_the>0.99) sig_the=0.99;
		cor_sin = sig_par[8]*pow(sig_the,4)+sig_par[9]*pow(sig_the,3) +
		  sig_par[10]*pow(sig_the,2)+sig_par[11]*sig_the+sig_par[12];
		
		//sigma vs t0
		double cor_t0 = 1.0;
		
		//calculate sigma
		if (trkalg == 1)
		  ex_sigma[it] = 550 * sigma_bg * cor_nhit * cor_sin * cor_t0;
		else
		  ex_sigma[it] = 550 * sigma_bg * cor_nhit * cor_sin * cor_t0 * sig_par[13];
		
	      }
	      
	      else{
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
            }//end of MC
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
                else if(sigmaflag == 6) {
                    double x= bethe_B/550;
                    double nhit = (double)Nohit;
                    double sigma_bg=1.0;
		    
		    sigma_bg= sig_par[0]+sig_par[1]*x+sig_par[2]*x*x;

                    double cor_nhit = 1.0;
                    if (nhit < 5) nhit = 5;
                    if (nhit <= 35)
                      cor_nhit =  sig_par[3]*pow(nhit,4)+sig_par[4]*pow(nhit,3)+sig_par[5]*pow(nhit,2) +  sig_par[6]*nhit+sig_par[7];

                    double cor_sin=  1.0;
                    if(sig_the>0.99) sig_the=0.99;
                    cor_sin = sig_par[8]*pow(sig_the,4)+sig_par[9]*pow(sig_the,3) + 
                        sig_par[10]*pow(sig_the,2)+sig_par[11]*sig_the+sig_par[12];

                    if(trkalg==1) 
                      ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin;
                    else 
                      ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin*sig_par[13];
                }
                else if(sigmaflag == 7) {
		  //		  for(int i=0; i<17; i++) cout << i << "  " << sig_par[i] << endl; 
		    double x= bethe_B/550;
                    double nhit = (double)Nohit;
                    double sigma_bg=1.0;
		    
		    sigma_bg= sig_par[0]+sig_par[1]*x+sig_par[2]*x*x;


                    double cor_nhit=1.0;
                    if(nhit<5) nhit=5;
                    if (nhit <= 35)
                      cor_nhit =  sig_par[3]*pow(nhit,4)+sig_par[4]*pow(nhit,3)+sig_par[5]*pow(nhit,2) +
                          sig_par[6]*nhit+sig_par[7];
                    double cor_sin=  1.0;
                    if(sig_the>0.99) sig_the = 0.99;
                    cor_sin = sig_par[8]*pow(sig_the,4)+sig_par[9]*pow(sig_the,3) +
                        sig_par[10]*pow(sig_the,2)+sig_par[11]*sig_the+sig_par[12];

                    double cor_t0 = 1;
                    if (t0 > 1200) t0 = 1200;
                    if (t0 < 800) t0 = 800;
		    cor_t0 = sig_par[13]*pow(t0,2)+sig_par[14]*t0+sig_par[15];

                    if(trkalg==1)
                      ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin*cor_t0;
                    else
                      ex_sigma[it]=550*sigma_bg*cor_nhit*cor_sin*cor_t0*sig_par[16];
		    // cout << "x " << x << "  nhit  " << nhit <<  "  sigma_bg " << sigma_bg << "  cor_nhit " << cor_nhit << " cor_sin " << cor_sin << " cor_t0 " << cor_t0 << " trkalg " << trkalg << endl;
		    // cout << "it " << it <<  "  mom " << mom  << "  dedx " << dedx << "  sinth " << sig_the << "  t0 " << t0 << endl;
                }
            }

            MdcDedxTrk dedxtrk;   
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
    //   std::cout<<"MdcDedxCorrection::dedx_pid_exp(blum)!!!"<<std::endl; 
}

/*void                                                                           
  MdcDedxCorrection::getCalib(void) const {
  Db2GeoDeDx obj1;
  vector<float> VecA(43);
  DbGeoDeDx obj2;
  int i,j,m;


  std::cout<<"success"<<std::endl;
  obj1.Get_DbLayerGain(VecA);
  std::cout<<"success!!"<<std::endl;

  for(j=0;j<43;j++)
  {
  std::cout<<"VecA[" << j << "] is: "<<VecA[j]<<std::endl;
  }

  vector<float> VecB(6860);
  std::cout<<"success"<<std::endl;
  obj1.Get_DbWireGain(VecB);
  for (i=0;i<6860;i++)
  {
  cout<<"VecB[" << i << "] is: "<<VecB[i]<<endl;
  }
  std::cout<<"success!!"<<std::endl;


  vector<float> VecC(129);
  std::cout<<"success"<<std::endl;
  obj1.Get_DbDeDxAttach(VecC);
  for(i=0;i<43;i++)
  for(j=0;j<3;j++)
  {
  m=i*3+j;
  cout<<"VecC[" << m << "] is "<<VecC[m]<<endl;
  }
  cout<<"success!!"<<endl;

  vector<float> VecD(172);
  std::cout<<"success"<<std::endl;
  obj1.Get_DbDeDxSatur(VecD);
  for(i=0;i<43;i++)
  for(j=0;j<4;j++)
  {
  m=i*4+j;
  cout<<"VecD[" << m << "] is "<<VecD[m]<<endl;
  }
  cout<<"success!!"<<endl;

  }*/
