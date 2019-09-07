#ifndef ReadPara_h
#define ReadPara_h

class ReadPara{
  public:
  ReadPara();
  double getpar_dd(int i,int j){return par_dd[i][j];}
  double getpar_dds(int i,int j){return par_dds[i][j];}
  double getpar_cos(int i,int j){return par_cos[i][j];}
  double getpar_coss(int i,int j){return par_coss[i][j];} 
  double getpar_z(int i,int j){return par_z[i][j];}
  double getpar_zs(int i,int j){return par_zs[i][j];}
  double getpar_entra(int i,int j){return par_entra[i][j];}
  double getpar_entras(int i,int j){return par_entras[i][j];} 
  private:
  double par_dd[8][6], par_dds[8][6],par_cos[8][5],par_coss[8][5],par_z[8][7],par_zs[8][7],par_entra[8][7],par_entras[8][7];
};
#endif
