#ifndef  HOUGHHIT_H
#define  HOUGHHIT_H
#include "MdcGeom/Constants.h"
#include "MdcHoughFinder/HoughGlobal.h"
#include "MdcHoughFinder/CFCir.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MdcGeom/MdcDetector.h" 
#include "CLHEP/Geometry/Point3D.h"
#include "MdcRawEvent/MdcDigi.h"
#include "McTruth/MdcMcHit.h"
#include "MdcGeom/MdcLayer.h"
#include "MdcGeom/MdcSWire.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace Event;
//using CLHEP::HepGeom;

class HoughHit{
  public:
    //construction and destruction
    HoughHit();
    HoughHit(const MdcDigi* const digi);
    HoughHit(const HoughHit& other);
    HoughHit& operator=(const HoughHit& other);

	//set truth info
    void setTruthInfo(const MdcMcHit*& mcHit);
	//set static number
    static void setMdcCalibFunSvc(const MdcCalibFunSvc* calibSvc){_calibPtr = calibSvc;}
    static void setMdcGeomSvc(MdcGeomSvc* geomSvc){_mdcGeomSvc = geomSvc;}
	static void setBunchTime(double t0) {_bunchTime=t0;}


    // CF transform
    void   conformalTrans(double x,double y,double r);  //return  x or y
    double getConformal_u(double ,double , double);
    double getConformal_v(double ,double , double);
    double getConformal_r(double ,double , double);

	//CF transform -> make circle
	void makeCir(int n,double phi_begin,double phi_last ,double r);
	CFCir getCir( int i)  const {return vec_cfcir[i];}
	static int _npart;


    //get mdc
    HoughHitType type() 	  const {return _type;}
	detectorType getDetectorType() const {return _detectorType;}
	const  MdcDigi* digi()const {return _digiPtr;}
	double getBunchTime() 	  const {return _bunchTime;}
    HepPoint3D getMidPoint()  const {return _midPoint;}
    HepPoint3D getEastPoint() const {return _eastPoint;}
    HepPoint3D getWestPoint() const {return _westPoint;}
    double getMidX()      const {return _midPoint.x();} 
    double getMidY()      const {return _midPoint.y();}
    int    getLayerId()   const {return _layer;}
    int    getWireId()    const {return _wire;}
    int    getSlayerType()const {return _slayerType;}
	const  MdcLayer*       layer()     const { return _layerPtr; }
	const  MdcSWire*       wire ()     const { return _wirePtr;  }
	double getCharge() const  {return _charge;}
	double getDriftTime()  const  {return _driftTime;}
	double getDriftDist()  const  {return _driftDist;}

	//get hough
	double getU() const {return _u;}
	double getV() const {return _v;}
	double getR() const {return CF_drift;}      //drift in CFS
	double getDeltaD() const { return _deltad; }        //l1l2-R+-r
	double getFltLen() const { return _flightLength; }  // R*dtheta


	//calculate time and drift
	double driftTime() const;
	double driftTime(double tof, double z) const;
	double calDriftDist(double, int, double, double, double) const;
	double calDriftDist(double bunchTime, int ambig) const;
	int    slayerType(int layer);

	//truth
	double getXTruth() const {return _truthPoint.x();}
	double getYTruth() const {return _truthPoint.y();}
	double getZTruth() const {return _truthPoint.z();}
	double getDriftDistTruth() const {return _truthDrift;}
	int    getIdTruth() const {return _truthId;}
	int    getLrTruth() const {return _truthlr;}
	double getUTruth() const {return _truthU;}
	double getVTruth() const {return _truthV;}
	double getRTruth() const {return _truthR;}
	HepPoint3D getPointTruth() const {return _truthPoint;}
	
	//--------------------------------------------------------------------//

	//set hough
	void   setDeltaD(double d) { _deltad=d; }             //l1l2-R+-r
	void   setFltLen(double flt) {_flightLength= flt;}    // R*dtheta

	//calcu in MC
	int    getCirList()   const {return _cirlist;}
	int    getStyle()   const {return _style;}
	void   setCirList(int cir)   {_cirlist=cir;}
	void   setStyle(int sty)   {_style=sty;}

	//print 
	void print() const;
	void printAll() const;
	void printTruth() const;

  private:
	double crudeTof() const { return  _rmid/Constants::c; }

	HoughHitType          _type;
	detectorType          _detectorType;

	static const MdcCalibFunSvc* _calibPtr;    // pointer to MdcCalibFunSvc
	static MdcGeomSvc*     _mdcGeomSvc;  // pointer to MdcGeomSvc
	static double          _bunchTime;  // pointer to MdcGeomSvc

	const  MdcDetector*    _det;  // pointer to MdcDetector
	const MdcDigi*        _digiPtr;     // pointer to digi
	const MdcSWire*       _wirePtr;     // pointer to digi
	const MdcLayer*       _layerPtr;     // pointer to digi
	Identifier            _id;          // Mdc Identifier 
	unsigned              _layer;       // layer number
	unsigned              _wire;        // wire number 
	double                _rawTime;     // raw time, in NANO seconds
	double                _driftTime;     // drift time, in NANO seconds
	double                _driftDist;     // drift dist, in NANO seconds
	double                _charge;      // raw charge, in NANO seconds
	int                   _slayerType;

//position
	HepPoint3D            _midPoint;
	HepPoint3D            _eastPoint;
	HepPoint3D            _westPoint;
	int                   _lr;
	double                _u;
	double                _v;

	//truth
	HepPoint3D            _truthPoint;
	double                _truthDrift;  // drift distance from truth
	int                   _truthId;
	int                   _truthlr;
	double 		  		  _truthU;
	double 		  		  _truthV;
	double 		  		  _truthR;      //drift in CF

	double 				  _rmid;

	// new method drift circle
	double CF_drift;
	vector<CFCir> vec_cfcir;

	//calcu by houghspace
	double _deltad;
	double 	     		  _flightLength;

	//calcu in MC
	int			 _cirlist;
	int			 _style;
	
};
#endif
