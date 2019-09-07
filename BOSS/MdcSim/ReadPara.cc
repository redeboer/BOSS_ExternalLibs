#include "ReadPara.hh"
#include <iostream> 
#include <fstream> 
#include <string> 
using namespace std;

ReadPara::ReadPara(){

  ifstream incos1;
  incos1.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_1cos.txt");
  for(int i=0;i<5;i++) {
    incos1>>par_cos[0][i];
  }
  ifstream incos2;
  incos2.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_2cos.txt");
  for(int i=0;i<5;i++) {
    incos2>>par_cos[1][i];
  }
  ifstream incos3;
  incos3.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_3cos.txt");
  for(int i=0;i<5;i++) {
    incos3>>par_cos[2][i];
  }
  ifstream incos4;
  incos4.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_4cos.txt");
  for(int i=0;i<5;i++) {
    incos4>>par_cos[3][i];
  }
  ifstream incos5;
  incos5.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_5cos.txt");
  for(int i=0;i<5;i++) {
    incos5>>par_cos[4][i];
  }
  ifstream incos6;
  incos6.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_6cos.txt");
  for(int i=0;i<5;i++) {
    incos6>>par_cos[5][i];
  }
  ifstream incos7;
  incos7.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_7cos.txt");
  for(int i=0;i<5;i++) {
    incos7>>par_cos[6][i];
  }
  ifstream incos8;
  incos8.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_8cos.txt");
  for(int i=0;i<5;i++) {
    incos8>>par_cos[7][i];
  }
  //***************
  ifstream incoss1;
  incoss1.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_1cos_s.txt");
  for(int i=0;i<5;i++) {
    incoss1>>par_coss[0][i];
  }
  ifstream incoss2;
  incoss2.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_2cos_s.txt");
  for(int i=0;i<5;i++) {
    incoss2>>par_coss[1][i];
  }
  ifstream incoss3;
  incoss3.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_3cos_s.txt");
  for(int i=0;i<5;i++) {
    incoss3>>par_coss[2][i];
  }
  ifstream incoss4;
  incoss4.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_4cos_s.txt");
  for(int i=0;i<5;i++) {
    incoss4>>par_coss[3][i];
  }
  ifstream incoss5;
  incoss5.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_5cos_s.txt");
  for(int i=0;i<5;i++) {
    incoss5>>par_coss[4][i];
  }
  ifstream incoss6;
  incoss6.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_6cos_s.txt");
  for(int i=0;i<5;i++) {
    incoss6>>par_coss[5][i];
  }
  ifstream incoss7;
  incoss7.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_7cos_s.txt");
  for(int i=0;i<5;i++) {
    incoss7>>par_coss[6][i];
  }
  ifstream incoss8;
  incoss8.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_8cos_s.txt");
  for(int i=0;i<5;i++) {
    incoss8>>par_coss[7][i];
  }
  //*********
  ifstream indd1;
  indd1.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_1.txt");
  for(int i=0;i<6;i++) {
    indd1>>par_dd[0][i];
  }
   ifstream indd2;
  indd2.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_2.txt");
  for(int i=0;i<6;i++) {
    indd2>>par_dd[1][i];
  }
  ifstream indd3;
  indd3.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_3.txt");
  for(int i=0;i<6;i++) {
    indd3>>par_dd[2][i];
  }
  ifstream indd4;
  indd4.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_4.txt");
  for(int i=0;i<6;i++) {
    indd4>>par_dd[3][i];
  }
  ifstream indd5;
  indd5.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_5.txt");
  for(int i=0;i<6;i++) {
    indd5>>par_dd[4][i];
  }
  ifstream indd6;
  indd6.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_6.txt");
  for(int i=0;i<6;i++) {
    indd6>>par_dd[5][i];
  }
  ifstream indd7;
  indd7.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_7.txt");
  for(int i=0;i<6;i++) {
    indd7>>par_dd[6][i];
  }
  ifstream indd8;
  indd8.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_8.txt");
  for(int i=0;i<6;i++) {
    indd8>>par_dd[7][i];
  }
  //*************
  ifstream indds1;
  indds1.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_1_s.txt");
  for(int i=0;i<6;i++) {
    indds1>>par_dds[0][i];
  }
   ifstream indds2;
  indds2.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_2_s.txt");
  for(int i=0;i<6;i++) {
    indds2>>par_dds[1][i];
  }
  ifstream indds3;
  indds3.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_3_s.txt");
  for(int i=0;i<6;i++) {
    indds3>>par_dds[2][i];
  }
  ifstream indds4;
  indds4.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_4_s.txt");
  for(int i=0;i<6;i++) {
    indds4>>par_dds[3][i];
  }
  ifstream indds5;
  indds5.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_5_s.txt");
  for(int i=0;i<6;i++) {
    indds5>>par_dds[4][i];
  }
  ifstream indds6;
  indds6.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_6_s.txt");
  for(int i=0;i<6;i++) {
    indds6>>par_dds[5][i];
  }
  ifstream indds7;
  indds7.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_7_s.txt");
  for(int i=0;i<6;i++) {
    indds7>>par_dds[6][i];
  }
  ifstream indds8;
  indds8.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_8_s.txt");
  for(int i=0;i<6;i++) {
    indds8>>par_dds[7][i];
  }
  //******************
 ifstream inz1;
  inz1.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_1z.txt");
  for(int i=0;i<7;i++) {
    inz1>>par_z[0][i];
  }
   ifstream inz2;
  inz2.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_2z.txt");
  for(int i=0;i<7;i++) {
    inz2>>par_z[1][i];
  }
  ifstream inz3;
  inz3.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_3z.txt");
  for(int i=0;i<7;i++) {
    inz3>>par_z[2][i];
  }
  ifstream inz4;
  inz4.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_4z.txt");
  for(int i=0;i<7;i++) {
    inz4>>par_z[3][i];
  }
  ifstream inz5;
  inz5.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_5z.txt");
  for(int i=0;i<7;i++) {
    inz5>>par_z[4][i];
  }
  ifstream inz6;
  inz6.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_6z.txt");
  for(int i=0;i<7;i++) {
    inz6>>par_z[5][i];
  }
  ifstream inz7;
  inz7.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_7z.txt");
  for(int i=0;i<7;i++) {
    inz7>>par_z[6][i];
  }
  ifstream inz8;
  inz8.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/data/fit_8z.txt");
  for(int i=0;i<7;i++) {
    inz8>>par_z[7][i];
  }
  //*************
  ifstream inzs1;
  inzs1.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_1z_s.txt");
  for(int i=0;i<7;i++) {
    inzs1>>par_zs[0][i];
  }
   ifstream inzs2;
  inzs2.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_2z_s.txt");
  for(int i=0;i<7;i++) {
    inzs2>>par_zs[1][i];
  }
  ifstream inzs3;
  inzs3.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_3z_s.txt");
  for(int i=0;i<7;i++) {
    inzs3>>par_zs[2][i];
  }
  ifstream inzs4;
  inzs4.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_4z_s.txt");
  for(int i=0;i<7;i++) {
    inzs4>>par_zs[3][i];
  }
  ifstream inzs5;
  inzs5.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_5z_s.txt");
  for(int i=0;i<7;i++) {
    inzs5>>par_zs[4][i];
  }
  ifstream inzs6;
  inzs6.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_6z_s.txt");
  for(int i=0;i<7;i++) {
    inzs6>>par_zs[5][i];
  }
  ifstream inzs7;
  inzs7.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_7z_s.txt");
  for(int i=0;i<7;i++) {
    inzs7>>par_zs[6][i];
  }
  ifstream inzs8;
  inzs8.open("/ihepbatch/d07/liuying/6.3.5/Analysis/DedxReal/sim/fit_8z_s.txt");
  for(int i=0;i<7;i++) {
    inzs8>>par_zs[7][i];
  }
  //******************

  //******************
 //******************
}
