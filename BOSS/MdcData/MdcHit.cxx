//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcHit.cxx,v 1.37 2012/08/13 00:01:16 zhangy Exp $
//
// Description:
//	Class Implementation |MdcHit|
//
// Environment:
//	Software developed for the BESIII Detector at the BEPCII
//
// Author List:
//      S. Schaffner - Original
//	A. Snyder - Mods
//      S. Sen                  Feb 17 1998     New digi()->MCPtr() references
//
// Copyright Information:
//	Copyright (C) 1996	SLAC
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#include "MdcData/MdcHit.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkRecoTrk.h"
#include "RawEvent/RawDataUtil.h"
#include "Identifier/MdcID.h"

bool MdcHit::m_cosmicFit= false;
bool MdcHit::m_countPropTime = true;		

MdcHit::MdcHit( const MdcDigi*& aDigi, const MdcDetector*& det):
  TrkFundHit(), _digiPtr(aDigi), _geomPtr(det)
{ 
  _digiPtr  = aDigi;
  _geomPtr  = det;
  assert( _digiPtr != NULL);
  assert( _geomPtr != NULL);
  _id       = aDigi->identify();
  _layerPtr = det->Layer(_id);
  _wirePtr  = det->Wire(_id);
  assert( _layerPtr != NULL);
  assert( _wirePtr  != NULL);
  m_mdcCalibFunSvc = NULL;
  _layer    = MdcID::layer(_id); 
  _wire     = MdcID::wire (_id); 
  _iTdc     = _digiPtr->getTimeChannel();
  _iAdc     = _digiPtr->getChargeChannel();
  _rawTime  = RawDataUtil::MdcTime(_iTdc);
  _charge   = RawDataUtil::MdcCharge(_iAdc);
  _rmid     = _wirePtr->rMid();
  _zlen     = _layerPtr->zLength();   
  _phi      = _wirePtr->phi();
  _cosphi   = cos(_phi);
  _sinphi   = sin(_phi);
  _status   = 0;
}

MdcHit::MdcHit(const MdcHit& other) :
  TrkFundHit() , _digiPtr(other._digiPtr), _geomPtr(other._geomPtr),
  _layerPtr(other._layerPtr), _wirePtr(other._wirePtr), _id(other._id),
  _layer(other._layer), _wire(other._wire),
  _iTdc(other._iTdc), _iAdc(other._iAdc),
  _rawTime(other._rawTime), _charge(other._charge),
  _rmid(other._rmid), _zlen(other._zlen), 
  _phi(other._phi), _cosphi(other._cosphi), _sinphi(other._sinphi),
  _status(other._status), _T0Walk(other._T0Walk) 
{
  m_mdcCalibFunSvc = other.m_mdcCalibFunSvc;
}

MdcHit&
MdcHit::operator = (const MdcHit& other) {
  if(&other != this){
    _digiPtr     = other._digiPtr;
    _geomPtr     = other._geomPtr;
    _layerPtr    = other._layerPtr;
    _wirePtr	 = other._wirePtr;
    m_mdcCalibFunSvc = other.m_mdcCalibFunSvc;
    _id          = other._id;
    _layer       = other._layer;
    _wire        = other._wire;
    _iTdc        = other._iTdc;
    _iAdc        = other._iAdc;
    _rawTime     = other._rawTime;
    _charge      = other._charge;
    _digiPtr     = other._digiPtr;
    _rmid        = other._rmid;
    _zlen        = other._zlen;
    _phi         = other._phi;
    _cosphi      = other._cosphi;
    _sinphi      = other._sinphi;
    _status      = other._status;
    _T0Walk      = other._T0Walk;
  }
  return *this;
}


//Destructor
MdcHit::~MdcHit() {
  // This is ugly and inefficient.  This, along with the rest of 
  //  the hitList mess, should be cleaned up by handling the 
  //  association in an external map

  // Not written as a loop because removeHit() modifies TrkFundHit::_hitList
  short count = 0;
  while (nUsedHits() > count) {
    bool removed = _hitList[count]->parentTrack()->hits()->removeHit(this);
    if (!removed) count++;
  }
}

bool
MdcHit::operator==( const MdcHit& rhs ) const {
  return (this == &rhs);
}

void 
MdcHit::print( ostream& o ) const {
  o << "(" << _layer << ","<< _wire <<","<<digi()->getTrackIndex()<<")";
}

void
MdcHit::printAll( ostream& o ) const {
  o << "Hit:(" <<  _layer << "," << _wire 
    << ") x:" << x() << " y:" << y()
    << " rt:" << rawTime()
    << " dt:" <<driftTime(0.0,0)
    << " dd:" <<driftDist(0.0,1)
    << endl;
}

void 
MdcHit::setCalibSvc(const MdcCalibFunSvc* calibSvc) { 
  m_mdcCalibFunSvc = calibSvc;  
  _T0Walk = m_mdcCalibFunSvc->getT0(_layer,_wire) +  m_mdcCalibFunSvc->getTimeWalk(_layer, _iAdc);
}

double
MdcHit::driftTime(double tof, double z) const {
  // t prop
  double tprop = 0.;
  if (m_countPropTime){ tprop = m_mdcCalibFunSvc->getTprop(_layer,z*10.); }

  //tof in s, driftTime in ns, _T0Walk in ns
  double driftT = _rawTime - _T0Walk -1.e9*tof - tprop;

  //if(driftT >10000)
  //std::cout<< "driftTime ("<<_layer<<","<<_wire<<") dt "<<driftT<<" rawTime "<<_rawTime<<" z "<<z<<" tprop "<<tprop <<" t0walk "<<_T0Walk<<" tof "<<1.e9*tof<< std::endl;//yzhang debug
  return driftT; 
}  

double 
MdcHit::driftDist(double tof, int ambig, double entranceAngle,  
    double /*dipAngle*/, double z) const
{ 
  double driftD;
  //drift time ns, layer id begin with 0, entrance angle rads,
  //lr ambig: wire ambig 1,-1,0 -> Calib 0,1,2
  int lrCalib=2;
  if (ambig==1) lrCalib = 0;
  else if (ambig==-1) lrCalib = 1;

  // tof in s, driftDist in cm, dirftTime in ns
  if (fabs(z)>150. || fabs(driftTime(tof,z))>1500.){
    return 9999.;
  }
  driftD = 0.1 * m_mdcCalibFunSvc->driftTimeToDist(driftTime(tof,z),_layer,_wire,lrCalib,entranceAngle);//to cm
  //std::cout<<"driftDist "<<"("<<_layer <<","<<_wire <<") dd "<<driftD<<" dt "<<driftTime(tof,z) <<" lr "<<lrCalib <<" eAng "<<entranceAngle <<" tof "<<tof*1.e9<<" z "<<z <<" t0walk "<<_T0Walk<<" rawT "<<_rawTime <<" tprop "<< _rawTime - driftTime(tof,z)- _T0Walk-1.e9*tof<<std::endl;

  if (abs(driftD)<Constants::epsilon) driftD = 0.00001;
  return driftD;
}

double 
MdcHit::driftDist(double bunchTime, int ambig) const  {
  return driftDist(bunchTime+crudeTof(), ambig, 0., 0., 0. );
}


double 
MdcHit::sigma(double driftdist, int ambig, double entranceAngle, 
    double dipAngle, double z) const {
  double sig = 9999.;

#ifdef MDCPATREC_RESLAYER
  if (_layer == m_resLayer){
    //give a huge sigma to skip this layer when fit track
    return 9999.;
  }
#endif
  //if(fabs(z)>150. || driftdist==9999. ) {
  //  return 9999.;
  //} 
  if ( m_mdcCalibFunSvc ) {
    //layid begin with 0, entrance angle in rads,
    //distance: cm ->Calib mm //z: cm -> Calib mm
    //Q: MonteCalo eV -> Calib fC FIXME
    //lr ambig: wire ambig 1,-1,0 -> Calib 0,1,2 
    int lrCalib=2;
    if (ambig==1) lrCalib = 0;
    else if (ambig==-1) lrCalib = 1;

    //std::cout<<"layer "<<_layer<< " lrCalib "<< lrCalib<< " driftdist "<< driftdist << " eAngle "<<entranceAngle
    //<<" tan(dipAngle) "<<tan(dipAngle)<< " z "<<z <<" iAdc "<<_iAdc<<std::endl;
    sig = 0.1 * m_mdcCalibFunSvc->getSigma(_layer,lrCalib,driftdist*10.,entranceAngle,
	tan(dipAngle),z*10.,_iAdc); //Calib special resolution mm -> cm

    if(fabs(sig)<Constants::epsilon){
      sig = 999.;
    }
    //if(sig<=0){
    //std::cout<<__FILE__<<"   "<<__LINE__ <<" sigma "<<sig
    //<<" layer "<<_layer <<" lrCalib "<<lrCalib <<" driftdist "<<driftdist*10
    //<<" eAngle "<<entranceAngle<<" dipAngle "<<(dipAngle) <<" tanl "<<tan(dipAngle)
    //<<" z "<<z <<" iAdc "<<_iAdc <<std::endl;
    //}
  }
  //if (_layer<4) sig /=5.;//yzhang TEST 2011-05-27 
  return sig;
}

double 
MdcHit::sigma(double driftdist, int ambig) const {
  return sigma(driftdist, ambig, 0., 0., 0.);// cm
}

const Trajectory*
MdcHit::hitTraj() const {
  return layer()->makeHitTrajInGlobalCoords(wirenumber(),0.0);
}

extern ostream& 
operator<<( ostream &o, const MdcHit& hit ) {
  hit.print(o);
  return o;
} 

