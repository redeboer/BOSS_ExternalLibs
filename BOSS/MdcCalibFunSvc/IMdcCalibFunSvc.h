#ifndef IMDCCALIBFUNSVC_H_
#define IMDCCALIBFUNSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"

#include "TTree.h"

/* Declaration of the interface ID */
static const InterfaceID IID_IMdcCalibFunSvc("IMdcCalibFunSvc",1,0);

class IMdcCalibFunSvc: virtual public IInterface{
public:
     static const InterfaceID& interfaceID() { return IID_IMdcCalibFunSvc; }

     /* propagation velocity of the signal in the wire. unit: mm/ns */
     virtual double getVprop(int lay) const = 0;

     /* get propagation time. the unit of z is mm */
     virtual double getTprop(int lay, double z) const = 0;

     /* DriftTime is real drift time, that is 
	DriftTime = Tdc - TOF - T0 - TimeWalk */
     virtual double driftTimeToDist(double drifttime, int layid, int cellid,
				    int lr, double entrance = 0.0) const = 0;
     virtual double distToDriftTime(double dist, int layid, int cellid, int lr, 
				    double entrance = 0.0) const = 0;

     /* Sigma1 and Sigma2 are the smaller and the bigger sigma of
	the two gaussian distribution, respectively.
	Sigma = sqrt( F*Sigma1^2 + (1-F)*Sigma2^2 ) */
     virtual double getSigma(int layid, int lr, double dist,
			     double entrance = 0.0, double tanlam = 0.0,
			     double z = 0.0, double Q = 1000.0) const = 0;
     virtual double getSigmaLR(int layid, int lr, double dist,
			       double entrance = 0.0, double tanlam = 0.0,
			       double z = 0.0, double Q = 1000.0) const = 0;
     virtual double getSigma1(int layid, int lr, double dist,
			      double entrance = 0.0, double tanlam = 0.0,
			      double z = 0.0, double Q = 1000.0) const = 0;
     virtual double getSigma2(int layid, int lr, double dist,
			      double entrance = 0.0, double tanlam = 0.0,
			      double z = 0.0, double Q = 1000.0) const = 0;
     virtual double getF(int layid, int lr, double dist,
			 double entrance = 0.0, double tanlam = 0.0,
			 double z = 0.0, double Q = 1000.0) const = 0;

     virtual double getSigmaToT(int layid, int lr, double tdr,
				double entrance = 0.0, double tanlam = 0.0,
				double z = 0.0, double Q = 1000.0) const = 0;
     virtual double getSigmaToTLR(int layid, int lr, double tdr,
				  double entrance = 0.0, double tanlam = 0.0,
				  double z = 0.0, double Q = 1000.0) const = 0;

     virtual void setXtBegin() = 0;
     virtual int  getNextXtpar(int& key, double& par) = 0;
     virtual void getXtpar(int layid, int entr, int lr, double par[]) const = 0;
     virtual bool   getNewXtpar() = 0;
     virtual TTree* getNewXtparTree(int layid, int entr, int lr) const = 0;

     virtual double getT0(int layid, int cellid) const = 0;
     virtual double getT0(int wireid) const = 0;
     virtual double getDelT0(int wireid) const = 0;

     virtual double getTimeWalk(int layid, double Q) const = 0;
     virtual double getQtpar(int layid, int ord) const = 0;

     virtual double getWireEff(int layid, int cellid) const = 0;
     virtual double getWireEff(int wireid) const = 0;

     virtual void setSdBegin() = 0;
     virtual int  getNextSdpar(int& key, double& par) = 0;
     virtual void getSdpar(int layid, int entr, int lr, double par[]) const = 0;
     virtual bool   getR2tpar() = 0;
     virtual TTree* getR2tTree(int layid) const = 0;

     virtual int getXtEntrIndex(double entrance) const = 0;
     virtual int getSdEntrIndex(double entrance) const = 0;

/*      virtual std::string getCalibDataFile() = 0; */

     //add for ganga
     virtual bool initCalibConst()=0;
};

#endif /* IMDCCALIBFUNSVC_H_ */
