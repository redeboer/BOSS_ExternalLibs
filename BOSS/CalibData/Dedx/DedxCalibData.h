// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/Dedx/DedxCalibData.h,v 1.8 2010/10/08 01:12:53 huangb Exp $
#ifndef CalibData_DedxCalibData_h
#define CalibData_DedxCalibData_h

/* @class CalibData_DedxCalibData_h

  calibration data class  
  calibration infrastructure

  @author huangb
*/

#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include <vector>
using namespace std;
namespace CalibData {
  class DedxCalibData:  public CalibBase1{
 
     public:
    DedxCalibData(){};

    virtual ~DedxCalibData() {
                                 }

    // Re-implemented from DataObject
    inline virtual const CLID& clID() const { return classID(); } 
    
    inline static  const CLID& classID() { return CLID_Calib_DedxCal; }

    virtual StatusCode   update(CalibBase1& other, MsgStream* log);
 
    
    //set the Calibration data by matrix element
    void setddg(const double ddg,int i,int j);
  
    void setggs(const double ggs,int i,int j);
    
    void setwireg(const double wireg,int i);
  
    void setgain(const double gain);
     
    void setzdep(const double zdep,int i,int j);

    void setresol(const double resol);
    
    void setlayerg(const double layerg,int i);
 
    void setenta(const double enta,int i,int j);

    void setrunNO(const int run_NO);  
    void setrung(const double rung,int i,int j);    

    //get the Calibration Data
     
    double  getddg(int i,int j) const{return m_ddg[i][j];}

    double  getggs(int i,int j) const{return m_ggs[i][j];}
    
    double  getwireg(int i) const{return m_wireg[i];}

    double  getgain() const {return m_gain;}
   
    double  getzdep(int i,int j) const{return m_zdep[i][j];}

    double  getresol() const {return m_resol;}
       
    double  getlayerg(int i) const{return m_layerg[i];}

    double getenta(int i,int j) const{return m_enta[i][j];}
    
    int getrunNO() const {return m_runNO; }
    double getrung(int i,int j) const{
    if(j>=m_runNO) return 0;
   if(i==0) return *(m_rung1.begin()+j);
   if(i==1) return *(m_rung2.begin()+j);
   if(i==2) return *(m_rung3.begin()+j);
   if(i==3) return *(m_rung4.begin()+j);};
    // add on June 4th,2009
    double get_id_doca(int i) const{return m_id_doca[i];}
    double get_iner_chi(int i) const{return m_iner_chi[i];}
    double get_iner_gain(int i) const{return m_iner_gain[i];}
    double get_iner_hits(int i) const{return m_iner_hits[i];}
    double get_ip_eangle(int i) const{return  m_ip_eangle[i];}
    double get_out_chi(int i) const{return m_out_chi[i];}
    double get_out_gain(int i) const{return m_out_gain[i];}
    double get_out_hits(int i)   const{return m_out_hits[i];}
    double get_costheta(int i)  const{return m_costheta[i];}
    double get_t0(int i)  const{return m_t0[i];}
    double get_dedx(int i) const{return m_dedx[i];}
    double get_hadron(int i) const{return m_hadron[i];}
    int get_hadronNo() const{return m_hadronNo;}   
    int get_enanglesize() const{return m_enanglesize;}
    double get_enangle(int i) const{return m_enangle[i];}

   void set_id_doca(const double aa,int i) { m_id_doca[i]=aa;}
   void set_iner_chi(const double aa,int i) { m_iner_chi[i]=aa;}
   void set_iner_gain(const double aa,int i) { m_iner_gain[i]=aa;}
   void set_iner_hits(const double aa,int i) { m_iner_hits[i]=aa;}
   void set_ip_eangle(const double aa,int i) { m_ip_eangle[i]=aa;}
   void set_out_chi(const double aa,int i) { m_out_chi[i]=aa;}
   void set_out_gain(const double aa,int i) { m_out_gain[i]=aa;}
   void set_out_hits(const double aa,int i) { m_out_hits[i]=aa;}
   void set_costheta(const double aa,int i) { m_costheta[i]=aa;}
   void set_t0(const double aa,int i) { m_t0[i]=aa;}
   void set_dedx(const double aa,int i)  {m_dedx[i]=aa;}
   void set_hadron(const double aa,int i)  {m_hadron[i]=aa;}
   void set_hadronNo(int no){m_hadronNo=no;}
   void set_enanglesize(int no){m_enanglesize=no;}
   void set_enangle(const double aa,int i) {m_enangle[i]=aa;}

  private:
   int m_runNO;   

   double m_ddg[4][43];
   double m_ggs[4][43];
   double m_wireg[6796];
   double m_gain;
   double m_zdep[4][43];
   double m_resol;
   double m_layerg[43];
   double m_enta[4][43]; 
  // double m_rung[4][1000];
   vector<double> m_rung1;
   vector<double> m_rung2;
   vector<double> m_rung3;
   vector<double> m_rung4;
   //vector <Double_t> rung   
 
   // add on June 4th,2009
   double m_id_doca[1600];
   double m_iner_chi[1600];
   double m_iner_gain[1600];
   double m_iner_hits[1600];
   double m_ip_eangle[1600];
   double m_out_chi[1600];
   double m_out_gain[1600];
   double m_out_hits[1600];
   double m_costheta[80];
   double m_t0[35];
   double m_dedx[35];
   double m_hadron[20];
   double m_enangle[100];
   int m_hadronNo;
   int m_enanglesize;
  };
}


#endif
