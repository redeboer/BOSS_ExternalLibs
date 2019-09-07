#ifndef BesMdcRes_H
#define BesMdcRes_H
#include <string>

class BesMdcRes{
  public:
   BesMdcRes(std::string& path);
   ~BesMdcRes(){};
 // double getpar_lay(int ii)  {return par_lay[ii];}
  double getD_dD(int ii,int jj)   {return par_lay1[ii][jj];}
  double getD_lay(int ii,int jj)  {return par_lay2[ii][jj];}
  double getD_theta(int ii,int jj){return par_lay11[ii][jj];}
  double getM_dD(int ii,int jj)  {return par_lay3[ii][jj];}
  double getM_lay(int ii,int jj) {return par_lay4[ii][jj];}
  double getM_theta(int ii,int jj) {return par_lay33[ii][jj];}
  double getD_effi(int ii ,int jj) {return par_lay5[ii][jj];}
  double getD_iEntr(int ii,int jj,int kk) {return par_lay6[ii][jj][kk];}
  double getM_iEntr(int ii,int jj,int kk) {return par_lay7[ii][jj][kk];}
  private:


  double par_lay1[43][10]; 
  double par_lay2[43][5];
  double par_lay11[43][20];
  double par_lay3[43][10];
  double par_lay4[43][5];
  double par_lay33[43][20];
  double par_lay5[43][90];
  double par_lay6[43][2][18];
  double par_lay7[43][2][18];
};
#endif
