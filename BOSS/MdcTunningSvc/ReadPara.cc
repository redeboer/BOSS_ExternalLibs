#include "ReadPara.hh"
#include <iostream> 
#include <fstream> 
#include <string> 
using namespace std;

ReadPara::ReadPara(string& path){

	std::cout<<"step1"<<std::endl;
	ifstream incos1;
	string s1=path+"/data/fit_1cos.txt";
	incos1.open(s1.c_str());
	for(int i=0;i<5;i++) {
		incos1>>par_cos[0][i];
	}


	ifstream incos2;
	string s2=path+"/data/fit_2cos.txt";
	incos2.open(s2.c_str());
	for(int i=0;i<5;i++) {
		incos2>>par_cos[1][i];
	}
	ifstream incos3;
	string s3=path+"/data/fit_3cos.txt";
	incos3.open(s3.c_str());
	for(int i=0;i<5;i++) {
		incos3>>par_cos[2][i];
	}
	ifstream incos4;
	string s4=path+"/data/fit_4cos.txt";
	incos4.open(s4.c_str());
	for(int i=0;i<5;i++) {
		incos4>>par_cos[3][i];
	}
	ifstream incos5;
	string s5=path+"/data/fit_5cos.txt";
	incos5.open(s5.c_str());
	for(int i=0;i<5;i++) {
		incos5>>par_cos[4][i];
	}
	ifstream incos6;
	string s6=path+"/data/fit_6cos.txt";
	incos6.open(s6.c_str());
	for(int i=0;i<5;i++) {
		incos6>>par_cos[5][i];
	}
	ifstream incos7;
	string s7=path+"/data/fit_7cos.txt";
	incos7.open(s7.c_str());
	for(int i=0;i<5;i++) {
		incos7>>par_cos[6][i];
	}
	ifstream incos8;
	string s8=path+"/data/fit_8cos.txt";
	incos8.open(s8.c_str());
	for(int i=0;i<5;i++) {
		incos8>>par_cos[7][i];
	}
	//***************
	ifstream incoss1;
	string ins1=path+"/sim/fit_1cos_s.txt";
	incoss1.open(ins1.c_str());
	for(int i=0;i<5;i++) {
		incoss1>>par_coss[0][i];
	}
	ifstream incoss2;
	string ins2=path+"/sim/fit_2cos_s.txt";
	incoss2.open(ins2.c_str());
	for(int i=0;i<5;i++) {
		incoss2>>par_coss[1][i];
	}
	ifstream incoss3;
	string ins3=path+"/sim/fit_3cos_s.txt";
	incoss3.open(ins3.c_str());
	for(int i=0;i<5;i++) {
		incoss3>>par_coss[2][i];
	}
	ifstream incoss4;
	string ins4=path+"/sim/fit_4cos_s.txt";
	incoss4.open(ins4.c_str());
	for(int i=0;i<5;i++) {
		incoss4>>par_coss[3][i];
	}
	ifstream incoss5;
	string ins5=path+"/sim/fit_5cos_s.txt";
	incoss5.open(ins5.c_str());
	for(int i=0;i<5;i++) {
		incoss5>>par_coss[4][i];
	}
	ifstream incoss6;
	string ins6=path+"/sim/fit_6cos_s.txt";
	incoss6.open(ins6.c_str());
	for(int i=0;i<5;i++) {
		incoss6>>par_coss[5][i];
	}
	ifstream incoss7;
	string ins7=path+"/sim/fit_7cos_s.txt";
	incoss7.open(ins7.c_str());
	for(int i=0;i<5;i++) {
		incoss7>>par_coss[6][i];
	}
	ifstream incoss8;
	string ins8=path+"/sim/fit_8cos_s.txt";
	incoss8.open(ins8.c_str());
	for(int i=0;i<5;i++) {
		incoss8>>par_coss[7][i];
	}
	//*********
	ifstream indd1;
	string inds1=path+"/data/fit_1.txt";
	indd1.open(inds1.c_str());
	for(int i=0;i<6;i++) {
		indd1>>par_dd[0][i];
	}
	ifstream indd2;
	string inds2=path+"/data/fit_2.txt";
	indd2.open(inds2.c_str());
	for(int i=0;i<6;i++) {
		indd2>>par_dd[1][i];
	}
	ifstream indd3;
	string inds3=path+"data/fit_3.txt";
	indd3.open(inds3.c_str());
	for(int i=0;i<6;i++) {
		indd3>>par_dd[2][i];
	}
	ifstream indd4;
	string inds4=path+"data/fit_4.txt";
	indd4.open(inds4.c_str());
	for(int i=0;i<6;i++) {
		indd4>>par_dd[3][i];
	}
	ifstream indd5;
	string inds5=path+"data/fit_5.txt";
	indd5.open(inds5.c_str());
	for(int i=0;i<6;i++) {
		indd5>>par_dd[4][i];
	}
	ifstream indd6;
	string inds6=path+"data/fit_6.txt";
	indd6.open(inds6.c_str());
	for(int i=0;i<6;i++) {
		indd6>>par_dd[5][i];
	}
	ifstream indd7;
	string inds7=path+"data/fit_7.txt";
	indd7.open(inds7.c_str());
	for(int i=0;i<6;i++) {
		indd7>>par_dd[6][i];
	}
	ifstream indd8;
	string inds8= path +"data/fit_8.txt";
	indd8.open(inds8.c_str());
	for(int i=0;i<6;i++) {
		indd8>>par_dd[7][i];
	}
	//*************
	ifstream indds1;
	string indd_s1=path+"/sim/fit_1_s.txt";
	indds1.open(indd_s1.c_str());
	for(int i=0;i<6;i++) {
		indds1>>par_dds[0][i];
	}
	ifstream indds2;
	string indd_s2=path+"/sim/fit_2_s.txt";
	indds2.open(indd_s2.c_str());
	for(int i=0;i<6;i++) {
		indds2>>par_dds[1][i];
	}
	ifstream indds3;
	string indd_s3=path+"/sim/fit_3_s.txt";
	indds3.open(indd_s3.c_str());
	for(int i=0;i<6;i++) {
		indds3>>par_dds[2][i];
	}
	ifstream indds4;
	string indd_s4=path+"/sim/fit_4_s.txt";
	indds4.open(indd_s4.c_str());
	for(int i=0;i<6;i++) {
		indds4>>par_dds[3][i];
	}
	ifstream indds5;
	string indd_s5=path+"/sim/fit_5_s.txt";
	indds5.open(indd_s5.c_str());
	for(int i=0;i<6;i++) {
		indds5>>par_dds[4][i];
	}
	ifstream indds6;
	string indd_s6=path+"/sim/fit_6_s.txt";
	indds6.open(indd_s6.c_str());
	for(int i=0;i<6;i++) {
		indds6>>par_dds[5][i];
	}
	ifstream indds7;
	string indd_s7=path+"/sim/fit_7_s.txt";
	indds7.open(indd_s7.c_str());
	for(int i=0;i<6;i++) {
		indds7>>par_dds[6][i];
	}
	ifstream indds8;
	string indd_s8=path+"/sim/fit_8_s.txt";
	indds8.open(indd_s8.c_str());
	for(int i=0;i<6;i++) {
		indds8>>par_dds[7][i];
	}
	//******************
	ifstream inz1;
	string in_z1=path+"/data/fit_1z.txt";
	inz1.open(in_z1.c_str());
	for(int i=0;i<7;i++) {
		inz1>>par_z[0][i];
	}
	ifstream inz2;
	string in_z2= path+"/data/fit_2z.txt";
	inz2.open(in_z2.c_str());
	for(int i=0;i<7;i++) {
		inz2>>par_z[1][i];
	}
	ifstream inz3;
	string in_z3= path+"/data/fit_3z.txt";
	inz3.open(in_z3.c_str());
	for(int i=0;i<7;i++) {
		inz3>>par_z[2][i];
	}
	ifstream inz4;
	string in_z4= path+"/data/fit_4z.txt";
	inz4.open(in_z4.c_str());
	for(int i=0;i<7;i++) {
		inz4>>par_z[3][i];
	}
	ifstream inz5;
	string in_z5=path+"/data/fit_5z.txt";
	inz5.open(in_z5.c_str());
	for(int i=0;i<7;i++) {
		inz5>>par_z[4][i];
	}
	ifstream inz6;
	string in_z6=path+"/data/fit_6z.txt";
	inz6.open(in_z6.c_str());
	for(int i=0;i<7;i++) {
		inz6>>par_z[5][i];
	}
	ifstream inz7;
	string in_z7=path+"/data/fit_7z.txt";
	inz7.open(in_z7.c_str());
	for(int i=0;i<7;i++) {
		inz7>>par_z[6][i];
	}
	ifstream inz8;
	string in_z8=path+"/data/fit_8z.txt";
	inz8.open(in_z8.c_str());
	for(int i=0;i<7;i++) {
		inz8>>par_z[7][i];
	}
	//*************
	ifstream inzs1;
	string in_zs1=path+"/sim/fit_1z_s.txt";
	inzs1.open(in_zs1.c_str());
	for(int i=0;i<7;i++) {
		inzs1>>par_zs[0][i];
	}
	ifstream inzs2;
	string in_zs2=path+"/sim/fit_2z_s.txt";
	inzs2.open(in_zs2.c_str());
	for(int i=0;i<7;i++) {
		inzs2>>par_zs[1][i];
	}
	ifstream inzs3;
	string in_zs3=path+"/sim/fit_3z_s.txt";
	inzs3.open(in_zs3.c_str());
	for(int i=0;i<7;i++) {
		inzs3>>par_zs[2][i];
	}
	ifstream inzs4;
	string in_zs4=path+"/sim/fit_4z_s.txt";
	inzs4.open(in_zs4.c_str());
	for(int i=0;i<7;i++) {
		inzs4>>par_zs[3][i];
	}
	ifstream inzs5;
	string in_zs5=path+"/sim/fit_5z_s.txt";
	inzs5.open(in_zs5.c_str());
	for(int i=0;i<7;i++) {
		inzs5>>par_zs[4][i];
	}
	ifstream inzs6;
	string in_zs6=path+"/sim/fit_6z_s.txt";
	inzs6.open(in_zs6.c_str());
	for(int i=0;i<7;i++) {
		inzs6>>par_zs[5][i];
	}
	ifstream inzs7;
	string in_zs7=path+"/sim/fit_7z_s.txt";
	inzs7.open(in_zs7.c_str());
	for(int i=0;i<7;i++) {
		inzs7>>par_zs[6][i];
	}
	ifstream inzs8;
	string in_zs8=path+"/sim/fit_8z_s.txt";
	inzs8.open(in_zs8.c_str());
	for(int i=0;i<7;i++) {
		inzs8>>par_zs[7][i];
	}
	//******************

	//******************
	//******************
}
