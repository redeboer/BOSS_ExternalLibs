//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcHit.h,v 1.20 2010/05/24 11:41:12 zhangy Exp $
//
// Description:
//	Class Header  for |MdcHit| 
//      Fundamental Hit class for use by drift chamber fits and pattern
//      recogition code
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      S. Schaffner - Original
//	A. Snyder - Modifications to use |MdcGeom| and to construct
//                  from |MdcDigi|s
//
// Copyright Information:
//	Copyright (C) 1996	SLAC
// 
// History:
//	Migration for BESIII MDC
//
// 20080303 Zhang Yao -- Delete return of DetElemId
//------------------------------------------------------------------------

#ifndef MDCHIT_H
#define MDCHIT_H
#include "MdcGeom/Constants.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcGeom/MdcLayer.h"
#include "MdcGeom/MdcSWire.h"
#include "TrkBase/TrkFundHit.h"
#include "TrkBase/TrkEnums.h"
#include "Identifier/Identifier.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"

class Trajectory;

extern const CLID& CLID_MdcHit;


class MdcHit : public TrkFundHit {
public:

  MdcHit(const MdcDigi*& digi, const MdcDetector*& det);
  MdcHit(const MdcHit&);

  virtual ~MdcHit();

  MdcHit& operator=(const MdcHit&);
  bool operator==(const MdcHit&) const;

  const MdcDigi*      	digi () 	const { return _digiPtr;  }
  const MdcLayer*     	layer() 	const { return _layerPtr; }
  const MdcSWire*     	wire () 	const { return _wirePtr;  }
  const MdcDetector*  	geom () 	const { return _geomPtr;  }
  const MdcCalibFunSvc* calibSvc() 	const { return m_mdcCalibFunSvc; }
  Identifier   		mdcId()         const { return _id; }
  unsigned   		layernumber()   const { return _layer; }
  unsigned   		wirenumber()    const { return _wire; }
  unsigned 		tdcIndex()      const { return _iTdc; }
  unsigned 		adcIndex()      const { return _iAdc; }
  double 		charge()        const { return _charge; } 
  double 		rawTime()       const { return _rawTime; }
  double 		driftTime(double tof, double z) const;
  double 		driftDist(double, int, double, double, double) const;
  double 		driftDist(double bunchTime, int ambig) const;  
  double 		sigma(double, int, double, double, double) const;
  double 		sigma(double driftdist, int ambig = 0) const;
  int 			whichView() const{ return _layerPtr->view(); } 
  const  Trajectory* 	hitTraj() const;
  TrkEnums::TrkViewInfo whatView() const { return whichView()==0?TrkEnums::xyView:TrkEnums::bothView; }
  double 		phi() const { return _phi;}	      // phi at chamber center
  double 		x()   const { return _rmid*_cosphi; } // x at chamber center
  double 		y()   const { return _rmid*_sinphi; } // y at chamber center
  const double 		phi(double z) const{return wire()->phiDC(z);}  // phi at z 
  double 		x  (double z) const{return wire()->xWireDC(z);}// x at global z
  double 		y  (double z) const{return wire()->yWireDC(z);}// y at global z
  double 		rMid()        const { return _rmid; } // R at chamber center
  double 		zlen()        const { return _zlen; } // chamber extent in z
  unsigned 		status()      const { return _status;}// status

  void 			setCalibSvc(const MdcCalibFunSvc* calibSvc);
  void 			setCountPropTime(const bool count) { m_countPropTime = count;}
  void 			setCosmicFit(const bool cosmicfit) { m_cosmicFit = cosmicfit; }
  bool 			isCosmicFit() const { return m_cosmicFit; }

  void 			print(std::ostream &o) const;
  void 			printAll(std::ostream &o) const;
                                                                                
private:

  // Calculate crude (no track info) correction of time for flight 
  // delay (assumes tracks from origin); wire propagation assumes z=0;
  // return time in seconds(_rmid is in cm)
  double crudeTof() const { return  _rmid/Constants::c; }

  //hide the copy ctor and assignment op, at least until somebody needs them
  MdcHit();

  friend class MdcMakeHits;

  const MdcDigi* 	_digiPtr;  	// pointer to digi
  const MdcDetector* 	_geomPtr;  	// pointer to geometry
  const MdcLayer* 	_layerPtr; 	// pointer to layer  
  const MdcSWire* 	_wirePtr;  	// pointer to wire
  const MdcCalibFunSvc* m_mdcCalibFunSvc;//pointer to MdcCalibFunSvc
  Identifier 		_id;            // Mdc Identifier 
  unsigned 		_layer; 	// layer number
  unsigned 		_wire;		// wire number 
  unsigned 		_iTdc;		// time channel
  unsigned 		_iAdc;		// charge channel
  double 		_rawTime;	// raw time, in NANO seconds
  double 		_charge;     	// charge 
  double   		_rmid;		// radius of wire at mid chamber
  double   		_zlen;		// wire length
  double   		_phi;		// phi of wire
  double   		_cosphi;	// cos(phi)
  double   		_sinphi;	// sin(phi)
  unsigned 		_status;        // status flag;
  double   		_T0Walk;        // time of T0(from calib) and time walk
  static bool 		m_cosmicFit;	// flag for csmc
  static bool 		m_countPropTime;// flag if count propergation time		
};
extern std::ostream& operator<<(std::ostream &o,const MdcHit& aHit);

typedef ObjectVector<MdcHit> MdcHitCol;

#endif
