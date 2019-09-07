#ifndef DEDXCORRECSVC_H_
#define DEDXCORRECSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"
#include "DedxCorrecSvc/IDedxCorrecSvc.h"
#include "DedxCorrecSvc/Dedx_Helix.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentListener.h"


class Identifier;

class DedxCorrecSvc: public Service, virtual public IDedxCorrecSvc,virtual public IIncidentListener{
public:
     DedxCorrecSvc( const std::string& name, ISvcLocator* svcloc );
     ~DedxCorrecSvc();

     virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
     virtual StatusCode initialize();
     virtual StatusCode finalize();

     // Incident handler
     void handle(const Incident&);

     double RungCorrec( int runNO, double ex ) const;
     double WireGainCorrec( int wireid,double ex) const;
     double DriftDistCorrec(int layid, double ddrift, double ex) const;
     double SaturCorrec(int layid,double costheta, double ex) const;
     double CosthetaCorrec(double costheta, double ex) const;
     double T0Correc(double t0, double ex) const;
     double HadronCorrec( double costheta,  double dedx ) const;
     double ZdepCorrec(int layid,double zhit, double ex) const;
     double EntaCorrec(int layid,double enta, double ex) const;
     double LayerGainCorrec( int layid,double ex) const;
     double DocaSinCorrec(int layid, double doca, double enta, double ex) const;
     double DipAngCorrec(int layid, double costheta, double ex) const;
     double GlobalCorrec(double dedx) const;
     double CellCorrec( int ser, double adc, double dd, double enta,
			  double z, double costheta ) const;
     double LayerCorrec( int layer, double z, double costheta, double ex ) const;
     double TrkCorrec( double costheta, double dedx ) const;
     double StandardCorrec( int runFlag, int ntpFlag, int runNO, double pathl, int wid, int layid, double adc, double dd, double eangle, double z, double costheta ) const;
     double StandardHitCorrec( int calib_rec_Flag, int runFlag, int ntpFlag, int runNO, double pathl,  int wid, int layid, double adc, double dd, double eangle, double costheta ) const;
     double StandardTrackCorrec(int calib_rec_Flag, int typFlag, int ntpFlag, int runNO, double ex, double costheta, double t0 ) const;
     double PathL( int ntpFlag, const Dedx_Helix& hel, int layer, int cellid, double z ) const; 
     //     double  PathLCosmic(const Helix& hel, int layer, int cellid, double z ,double sigmaz ) const;
     double D2I(const double& cosTheta, const double& D) const;
     double I2D(const double& cosTheta, const double& I) const;
     void set_flag( int calib_F );
     // correction according to large fabs(cos(theta))
     double f_larcos(double x, int nbin);

 private:
     bool m_initConstFlg;

     void init_param();
     void init_param_svc();

     // simple polynomials
      double sq(double& x) const {return (x*x);}
      double cub(double& x) const {return (x*x*x);}
      double sq4(double& x) const {return (x*x*x*x);}
      double sq5(double& x) const {return sq(x)*cub(x);}
      double sq6(double& x) const {return cub(x)*cub(x);}
      double sq7(double& x) const {return sq4(x)*cub(x);}
      double sq8(double& x) const {return sq4(x)*sq4(x);}
      double sq9(double& x) const {return sq4(x)*sq5(x);}
      double sq10(double& x) const {return sq5(x)*sq5(x);}
      double sq11(double& x) const {return sq5(x)*sq6(x);} 
     // Chebyshev's polynomials
      double T0(){return 1;}
      double T1(double& x) const {return x;}
      double T2(double& x) const {return (2*sq(x)-1);}
      double T3(double& x) const {return (4*cub(x)-3*x);}
      double T4(double& x) const {return (8*sq4(x)-8*sq(x)+1);}
      double T5(double& x) const {return (16*sq5(x)-20*cub(x)+5*x);}
      double T6(double& x) const {return (32*sq6(x)-48*sq4(x)+18*sq(x)-1);}
      double T7(double& x) const {return (64*sq7(x)-112*sq5(x)+56*cub(x)-7*x);}
      double T8(double& x) const {return (128*sq8(x)-256*sq6(x)+160*sq4(x)-32*sq(x)+1);}
      double T9(double& x) const {return (256*sq9(x)-576*sq7(x)+432*sq5(x)-120*cub(x)+9*x);}
      double T10(double& x) const {return (512*sq10(x)-1280*sq8(x)+1120*sq6(x)-400*sq4(x)+50*sq(x)-1);}
      double T11(double& x) const {return (1024*sq11(x)-2816*sq9(x)+2816*sq7(x)-1232*sq5(x)+220*cub(x)-11*x);}
      double T12(double& x) const {return (2*x*T11(x)-T10(x));}
      double T13(double& x) const {return (2*x*T12(x)-T11(x));}
      double T14(double& x) const {return (2*x*T13(x)-T12(x));}
      double T15(double& x) const {return (2*x*T14(x)-T13(x));}
      double T16(double& x) const {return (2*x*T15(x)-T14(x));}
      double T17(double& x) const {return (2*x*T16(x)-T15(x));}
      double T18(double& x) const {return (2*x*T17(x)-T16(x));}
      double T19(double& x) const {return (2*x*T18(x)-T17(x));}
      double T20(double& x) const {return (2*x*T19(x)-T18(x));}      

 private:
     IntegerProperty  m_run;
     double m_valid[6796];
     double m_wire_g[6796];
     double m_rung[4][10000];
     double m_ddg[4][43];
     double m_docaeangle[40][40];
     vector<double> m_venangle;
     double m_ggs[4][43];   
     double m_enta[4][43];   
     double m_zdep[4][43];
     double m_layer_g[43];
     double m_dedx_gain;
     double m_dedx_resol;
     int N_run;     
     double curve_par[5];
     double sigma_par[6];

     vector<double> cos_k;
     vector<double> cos_b;

     vector<double> t0_k;
     vector<double> t0_b;
     
  
     double Iner_Stepdoca;
     double Out_Stepdoca;
     
     double  m_alpha;
     double  m_gamma;
     double  m_delta;
     double  m_power;
     double  m_ratio;

     //int m_valid_flag; 
     /// correction : for individual cell  ( calib_F & 2 )? ON:OFF
     
     
     /// calibration flag : run by run correction ( calib_F & 1 )? ON : OFF 
     int m_rung_flag;
     
     /// calibration flag : wire by wire correction ( calib_F & 2 )? ON : OFF 
     int m_wireg_flag;   
     
     /// calibration flag : electron dip angle correction ( calib_F & 4 )? ON : OFF
     int m_ggs_flag;
     
     /// calibration flag : drift distance ( calib_F & 8 ) ? ON : OFF
     int m_ddg_flag;

     /// calibration flag : t0 correction ( calib_F & 16 ) ? ON : OFF
     int m_t0_flag;

     /// calibration flag : hadron saturation correction ( calib_F & 32 ) ? ON : OFF
     int m_sat_flag;

     

     /// calibration flag : entrance angle depsndence  ( calib_F & 16 )? ON : OFF
     int m_enta_flag;
     /// calibration flag : z depsndence  ( calib_F & 32 )? ON : OFF
     int m_zdep_flag;
     /// correction : mdc layer gain   ( calib_F & 64 )? ON:OFF     
     int m_layerg_flag;
     /// correction : doca and sinenta correction  ( calib_F & 128 )? ON:OFF
     int m_dcasin_flag;
     /// correction : Qsaturation correction ( calib_F & 256 )? ON:OFF
     int m_dip_flag;
     /// correction : mdc gain (run by run)  ( calib_F & 512 )? ON:OFF
     int m_mdcg_flag;
     /// control flags
     int m_init;
     int m_par_flag;
     /// mdc geometry
     IMdcGeomSvc* geosvc;
     IMagneticFieldSvc* m_pIMF; 
     bool m_debug;
     int m_debug_i, m_debug_j;
};

#endif /* DEDXCORRECSVC_H_ */
