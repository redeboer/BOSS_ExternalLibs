#include "BesMdcRes.hh"
#include <iostream> 
#include <fstream> 
#include <string> 
#include <cstdlib>
using namespace std;

BesMdcRes::BesMdcRes(string& path){
	int layerId, bin,layId ;
	double sigma;
	double cons1,sigma1,cons2,sigma2,Sig,sigmaE;
	double effi,m_nhit_Raw,m_nhit_Exp;


	/*
	//read lay efficiency parameter
	ifstream  infpar5;
	string logeff_raw= path+"/data/logeffi_0_42.dat";
	infpar5.open(logeff_raw.c_str());
	if( !infpar5.good()){
	std::cout<<"Error,mdc parameter file4 Data-effi-lay not exist "<<std::endl;
	exit (0);
	}else{
	std::cout<<" open mdc parameter file4 Data-effi-lay "<<std::endl;
	for(int lay =0; lay<43;lay++){
	infpar5 >> lay;
	for(int dD = 0; dD<91;dD++ ){
	infpar5 >> dD >> effi >> m_nhit_Raw >> m_nhit_Exp ;
	par_lay5[lay][dD] = effi;
	//cout<<"lay : "<<lay<<" dD  "<<dD <<" =" <<effi<<endl;
	}
	}// end of lay loop
	}

*/
	//read resolution Vs enter angle and doca 
	ifstream infpar6;
	string logres_iEntr = path+"/data/logrVetr.dat";
	infpar6.open(logres_iEntr.c_str());
	if(!infpar6.good()){
		std::cout<<" Error , mdc parameters file6 Data_res_iEnter not exist "<<std::endl;
		exit (0);
	}else{
		std::cout<<" open mdc parameter file6 Data_res_iEnter  "<<std::endl;
		for(int lay =0;lay<43;lay++){
			infpar6 >> lay;
			for(int iEntr =0;iEntr<2;iEntr++){
				infpar6 >> iEntr;
				for(int dD =0;dD<18;dD++){
					infpar6 >> dD >> sigmaE;
					par_lay6[lay][iEntr][dD] = sigmaE;
					// cout<<" lay : "<<lay<<" iEntr : "<<iEntr<<" dD : "<<dD<<" = "<<sigmaE<<endl;
				}
			}
		}// end of lay loop
	}

	//read resolution Vs enter angle and doca
	ifstream infpar7;
	string logres_M_iEntr = path + "/mc/logrVetr.dat";
	infpar7.open(logres_M_iEntr.c_str());
	if(!infpar7.good()){
		std::cout<<"Error,mdc parameters file7 Mc_res_iEntr not exist"<<std::endl;
		exit (0);
	}else{
		std::cout<<"open mdc parameters file7 Mc_res_iEntr "<<std::endl;
		for(int lay =0;lay<43;lay++){
			infpar7 >> lay;
			for(int iEntr =0;iEntr<2;iEntr++){
				infpar7 >> iEntr;
				for(int dD =0;dD<18;dD++){
					infpar7 >> dD >> sigmaE;
					par_lay7[lay][iEntr][dD] = sigmaE;
					// std::cout<<"par_lay7["<<lay<<"]["<<iEntr<<"]["<<dD<<"] : "<<sigmaE<<std::endl;
				}
			}
		}//end of lay loop
	} 

} 
