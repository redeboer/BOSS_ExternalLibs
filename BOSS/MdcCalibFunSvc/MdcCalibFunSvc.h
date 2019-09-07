#ifndef MDCCALIBFUNSVC_H_
#define MDCCALIBFUNSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include "CalibData/CalibModel.h"
#include "CalibData/Mdc/MdcCalibData.h"
#include "CalibDataSvc/ICalibRootSvc.h"

#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"

#include <map>
#include <vector>

class MdcCalibFunSvc: public Service, virtual public IMdcCalibFunSvc, 
		      virtual public IIncidentListener{
		      public:
     MdcCalibFunSvc( const std::string& name, ISvcLocator* svcloc );
     ~MdcCalibFunSvc();

     virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
     virtual StatusCode initialize();
     virtual StatusCode finalize();

     // Incident handler
     void handle(const Incident&);

     int m_run;

     /* Access by users */

     /* propagation velocity of the signal in the wire. unit: mm/ns */
     double getVprop(int lay) const;

     /* get propagation time. the unit of z is mm */
     double getTprop(int lay, double z) const;

     /* DriftTime is real drift time, that is 
	DriftTime = Tdc - TOF - T0 - TimeWalk */
     /* the unit of distance is mm */
     double driftTimeToDist(double drifttime, int layid, int cellid, int lr, 
			    double entrance = 0.0) const ;
     double distToDriftTime(double dist, int layid, int cellid, int lr, 
			    double entrance = 0.0) const ;

     /* Sigma1 and Sigma2 are the smaller and the bigger sigma of
	the two gaussian distribution, respectively.
	Sigma = sqrt( F*Sigma1^2 + (1-F)*Sigma2^2 ) */
     /* the unit of distance is mm */
     double getSigma(int layid, int lr, double dist,
		     double entrance = 0.0, double tanlam = 0.0,
		     double z = 0.0, double Q = 1000.0) const ;
     double getSigmaLR(int layid, int lr, double dist,
		       double entrance = 0.0, double tanlam = 0.0,
		       double z = 0.0, double Q = 1000.0) const ;
     double getSigma1(int layid, int lr, double dist,
		      double entrance = 0.0, double tanlam = 0.0,
		      double z = 0.0, double Q = 1000.0) const ;
     double getSigma2(int layid, int lr, double dist,
		      double entrance = 0.0, double tanlam = 0.0,
		      double z = 0.0, double Q = 1000.0) const ;
     double getF(int layid, int lr, double dist,
		 double entrance = 0.0, double tanlam = 0.0,
		 double z = 0.0, double Q = 1000.0) const ;

     double getSigmaToT(int layid, int lr, double tdr,
			double entrance = 0.0, double tanlam = 0.0,
			double z = 0.0, double Q = 1000.0) const ;
     double getSigmaToTLR(int layid, int lr, double tdr,
			  double entrance = 0.0, double tanlam = 0.0,
			  double z = 0.0, double Q = 1000.0) const ;

     void   setXtBegin() { m_xtiter = m_xtmap.begin(); }
     int    getNextXtpar(int& key, double& par);
     void   getXtpar(int layid, int entr, int lr, double par[]) const;
     bool   getNewXtpar() ;
     TTree* getNewXtparTree(int layid, int entr, int lr) const;

     double getT0(int layid, int cellid) const ;
     double getT0(int wireid) const { return m_t0[wireid]; }
     double getDelT0(int wireid) const { return m_delt0[wireid]; }

     double getTimeWalk(int layid, double Q) const ;
     double getQtpar(int layid, int ord) const;

     double getWireEff(int layid, int cellid) const ;
     double getWireEff(int wireid) const { return m_wireEff[wireid]; }

     void setSdBegin() { m_sditer = m_sdmap.begin(); }
     int  getNextSdpar(int& key, double& par);
     void getSdpar(int layid, int entr, int lr, double par[]) const;
     bool   getR2tpar() ;
     TTree* getR2tTree(int layid) const;
     
     int getXtEntrIndex(double entrance) const;
     int getSdEntrIndex(double entrance) const;

/*      std::string getCalibDataFile() { return m_pRootSvc->getrootfile(); } */

private:
     static const int NWIRE = 6796;
     static const int NLAYER = 43;
     static const int NXTENTR = 18;
     static const int NXTPAR = 8;
     static const int NXTORD = 6;
     static const int NSDBIN = 24;

     /* initial calib const */
     bool initCalibConst();

     double t2dPoly(double drifttime, int layid, int cellid, int lr, 
		    double entrance = 0.0) const ;
     double t2dInter(double drifttime, int layid, int cellid, int lr, 
		     double entrance = 0.0) const ;

/*      bool   getNewXtpar() ; */
/*      TTree* getNewXtparTree(int layid, int entr, int lr) const; */

/*      bool   getR2tpar() ; */
/*      TTree* getR2tTree(int layid) const; */

     int  getXtKey(int layid, int entr, int lr, int ord) const;
     int getSdKey(int layid, int entr, int lr, int bin) const;
     int getXtparId(int layid, int entr, int lr, int ord) const;
     int getSdparId(int layid, int entr, int lr, int bin) const;

     void checkConst();
     int m_updateNum;
     bool m_checkConst;

     int m_layInfSig;		/* the layer with infinite sigma for reconstruction */
     int m_xtMode;              /* 0: polynomial, 1: interpretation */
     std::string m_xtfile;
     bool m_fgR2t;
     bool m_linearXT;		/* 1: v=0.03um/ns, 0: read xt from database */
     bool m_readWireEffDb;	/* 1: read from DB, 0: read from file */
     std::string m_wireEffFile;
     bool m_outputXtMode;
     bool m_fixSigma;
     double m_fixSigmaValue;

     IMdcGeomSvc*       m_pMdcGeomSvc;
     IDataProviderSvc*  m_pCalDataSvc;
     ICalibRootSvc*     m_pRootSvc;

     /* for calculation Tprop */
     double m_zst[43];

     /* calibration constants */
     std::map<int, double> m_xtmap;
     std::vector<double>   m_xtpar;

     std::vector<double>   m_t0;
     std::vector<double>   m_delt0;

     std::vector<double>   m_qtpar0;
     std::vector<double>   m_qtpar1;

     std::map<int, double> m_sdmap;
     std::vector<double>   m_sdpar;

     std::map<int, double>::iterator m_xtiter;
     std::map<int, double>::iterator m_sditer;

     double m_wireEff[6796];

     int m_nNewXt[NLAYER][NXTENTR][2];
     double m_vt[NLAYER][NXTENTR][2][200];
     double m_vd[NLAYER][NXTENTR][2][200];

     int m_nR2t[NLAYER][NXTENTR][2];
     double m_tR2t[NLAYER][NXTENTR][2][200];
     double m_sR2t[NLAYER][NXTENTR][2][200];

     static const int XTLAYER_INDEX    = 11;
     static const int XTLAYER_MASK     = 0x1F800;
     static const int XTLAYER_DECO     = 0x3F;

     static const int XTENTRA_INDEX    = 6;
     static const int XTENTRA_MASK     = 0x7C0;
     static const int XTENTRA_DECO     = 0x1F;

     static const int XTLR_INDEX       = 4;
     static const int XTLR_MASK        = 0x30;
     static const int XTLR_DECO        = 0x3;

     static const int XTORDER_INDEX    = 0;
     static const int XTORDER_MASK     = 0xF;
     static const int XTORDER_DECO     = 0xF;


     static const int SDLAYER_INDEX    = 10;
     static const int SDLAYER_MASK     = 0xFC00;
     static const int SDLAYER_DECO     = 0x3F;

     static const int SDENTRA_INDEX    = 7;
     static const int SDENTRA_MASK     = 0x380;
     static const int SDENTRA_DECO     = 0x7;

     static const int SDLR_INDEX       = 5;
     static const int SDLR_MASK        = 0x60;
     static const int SDLR_DECO        = 0x3;

     static const int SDBIN_INDEX      = 0;
     static const int SDBIN_MASK       = 0x1F;
     static const int SDBIN_DECO       = 0x1F;
};

inline double MdcCalibFunSvc::getVprop(int lay) const{
     if(lay<8) return 220.0;
     else return 240.0;
}

inline int MdcCalibFunSvc::getXtparId(int layid, int entr, int lr,
				      int ord) const{
     int parId = (layid * 432) + (entr * 24) + (lr * 8) + ord;
     return parId;
}

inline int MdcCalibFunSvc::getSdparId(int layid, int entr, int lr,
				      int bin) const{
     int parId = (layid * 288) + (entr * 48) + (lr * 24) + bin;
     return parId;
}

#endif /* MDCCALIBFUNSVC_H_ */
