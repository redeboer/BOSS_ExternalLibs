#ifndef IDEDXCORRECSVC_H_
#define IDEDXCORRECSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "DedxCorrecSvc/Dedx_Helix.h"
/* Declaration of the interface ID */
static const InterfaceID IID_IDedxCorrecSvc("IDedxCorrecSvc",1,0);


class Identifier;

class IDedxCorrecSvc: virtual public IInterface{
public:
     static const InterfaceID& interfaceID() { return IID_IDedxCorrecSvc; }
     virtual double RungCorrec( int runNO, double ex ) const = 0;
     virtual double WireGainCorrec(int wireid, double ex) const = 0;
     virtual double DriftDistCorrec(int layid, double ddrift, double ex) const = 0;
     virtual double SaturCorrec(int layid, double costheta, double ex) const = 0;
     //virtual double CosthetaCorrec(double costheta, double ex) const = 0;
     //virtual double T0Correc(double t0, double ex) const = 0;
     //virtual double HadronCorrec( double costheta,  double dedx ) const = 0;
     virtual double EntaCorrec(int layid,double enta, double ex) const = 0;
     //virtual double DocaSinCorrec(int layid, double doca, double enta, double ex) const = 0;
     virtual double ZdepCorrec( int layer, double z,  double dedx ) const = 0;
     virtual double LayerGainCorrec( int layid, double dedx ) const = 0;
     virtual double GlobalCorrec(double dedx) const = 0;
     virtual double CellCorrec( int ser, double adc, double dd, double enta, 
				double z, double theta ) const = 0;
     virtual double LayerCorrec( int layer, double z, double costheta, 
				 double ex ) const = 0;
     virtual double TrkCorrec( double costheta, double dedx ) const = 0;
     virtual double StandardCorrec( int runFlag, int ntpFlag, int runNO, double pathl, int wid, int layid, double adc, double dd, double eangle, double z, double costheta ) const = 0;
     virtual double StandardHitCorrec(int calib_rec_Flag, int runFlag, int ntpFlag, int runNO, double pathl,  int wid, int layid, double adc, double dd, double eangle, double costheta ) const = 0;
     virtual double StandardTrackCorrec(int calib_rec_Flag, int typFlag, int ntpFlag, int runNO, double ex, double costheta, double t0 ) const = 0;

     virtual double  PathL( int ntpFlag, const Dedx_Helix& hel, int layer, int cellid, double z ) const = 0; 
     //     virtual double  PathLCosmic(const Helix& hel, int layer, int cellid, double z ,double sigmaz ) const = 0;
     virtual void set_flag( int calib_F ) = 0;
};

#endif /* IDEDXCORRECSVC_H_ */
