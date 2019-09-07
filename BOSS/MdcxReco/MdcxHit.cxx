//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxHit.cxx,v 1.20 2012/07/20 05:48:16 zhangy Exp $
//
// Description:
//	Class Implementation for |MdcxHit|: drift chamber hit that can compute
//      derivatives and plot itself.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A. Snyder
//
// Copyright Information:
//	Copyright (C) 1995	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#include "MdcxReco/MdcxHits.h"
#include "CLHEP/Alist/AIterator.h"
#include "MdcData/MdcHit.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "EventModel/Event.h"
#include "Identifier/MdcID.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcxReco/MdcxHel.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcGeom/Constants.h"
#include "MdcGeom/EntranceAngle.h"
#include <iomanip>

using std::endl;
using std::ostream;
#ifdef MDCXRECO_RESLAYER 
extern int g_resLayer;
#endif
const MdcCalibFunSvc* MdcxHit::m_mdcCalibFunSvc = 0;
bool MdcxHit::m_countPropTime = true;
const MdcDetector* MdcxHit::m_gm= 0;

MdcxHit::MdcxHit(const MdcDigi *pdcdatum, float c0, float cresol)
    :_mdcHit(0), _mdcDigi(pdcdatum), _c0(c0), _cresol(cresol)
{
  process();
}

MdcxHit::MdcxHit(const MdcHit *pdchhit, float c0, float cresol)
    :_mdcHit(pdchhit),_mdcDigi(pdchhit->digi()), _c0(c0), _cresol(cresol)
{
  process();
}

void MdcxHit::setMdcCalibFunSvc(const MdcCalibFunSvc* calibSvc) {
  m_mdcCalibFunSvc = calibSvc;
}

void MdcxHit::setCountPropTime(bool countPropTime) {
  m_countPropTime = countPropTime;
}

void MdcxHit::setMdcDetector(const MdcDetector* gm) {
  m_gm = gm;
}

void MdcxHit::process() {
  assert(m_gm);
  assert(m_mdcCalibFunSvc);
  Identifier _id = (*_mdcDigi).identify();
  _layernumber=MdcID::layer(_id);
  _wirenumber=MdcID::wire(_id);
  _superlayer=(_layernumber)/4;
  _iTdc = _mdcDigi->getTimeChannel();
  _iAdc = _mdcDigi->getChargeChannel();
  _t = RawDataUtil::MdcTime(_iTdc);
  _q = RawDataUtil::MdcCharge(_iAdc);
  _T0Walk = m_mdcCalibFunSvc->getT0(_layernumber,_wirenumber)
    + m_mdcCalibFunSvc->getTimeWalk(_layernumber,_iAdc);
  // pointer to layer
  const MdcLayer* layerPtr= m_gm->Layer(_layernumber);
  _L = layerPtr->zLength(); 
  _x = layerPtr->xWire(_wirenumber);  
  _y = layerPtr->yWire(_wirenumber); 
  _r = sqrt(_x*_x + _y*_y);
  _s = layerPtr->stereo(); 
  _p = layerPtr->phiOffset() + _wirenumber*layerPtr->dPhi();
  //double deltaz = m_gm->zOffSet(); //yzhang to BESIII
  //double deltaz = 0.0;
  double tst = _s;
  double pw = atan2(_y, _x);
  _pw = pw; 
  _wx = -tst*sin(pw);
  _wy =  tst*cos(pw); 
  _wz = (tst*tst < 1.0) ? sqrt(1.0-tst*tst) : 0.0;
  _sp = sin(_p);
  _cp = cos(_p);
  _consterr = 0;  //zoujh FIXME
  _e = _cresol;
  _d = d();
  // note _v is a total cludge
  _v = (_t-_c0 > 0.0) ? _d/(_t-_c0) : 0.0013;//yzhang 2009-11-03 
  _xneg = _x + _d*_sp;
  _yneg = _y - _d*_cp;
  _xpos = _x - _d*_sp;
  _ypos = _y + _d*_cp;
  usedonhel = 0;
  _type = Constants::viewOfsLayer[_superlayer];
}

float MdcxHit::tcor(float z, float tof, float tzero)const {
  //drift time 
  double tprop = 0.;
  if (m_countPropTime){ tprop = m_mdcCalibFunSvc->getTprop(_layernumber,z*10); }
  double dt = (_t - _T0Walk -_c0 - tprop - tof - tzero);
/*
  std::cout<<"("<<_layernumber<<","<<_wirenumber<<") dt "<<dt
    <<" rt "<<_t
    <<" t0walk "<< _T0Walk
    <<" z "<<z
    <<" _c0 "<<_c0
    <<" tzero "<<tzero
    <<" tof "<<tof
    <<" tprop "<<tprop
    <<std::endl;
   */ 
  return dt;
}

float MdcxHit::d(float z, float tof, float tzero, int wamb, float entranceAngle)const {

  //std::cout<<__FILE__<<" "<<__LINE__<<" call entrance "<< entranceAngle<< std::endl;
  //tof = hel.Doca_Tof();//yzhang delete for csmc temp FIXME
  float t_corr = tcor(z,tof,tzero);
  double eAngle = EntranceAngle(entranceAngle);

  //yzhang add for 64-bit
  if (fabs(z)>150. || fabs(t_corr)>1500. || fabs(eAngle)>999){
    return 9999.;
  }
  //zhangy

  int lrCalib = 2;
  if (wamb==1)lrCalib = 0;
  else if (wamb==-1) lrCalib = 1;

  double driftD = 0.1 * m_mdcCalibFunSvc->driftTimeToDist(t_corr,_layernumber,_wirenumber,lrCalib,eAngle);//to cm
  //std::cout<<"MdcxHit ("<<_layernumber<<","<<_wirenumber<<" dd "<<driftD<<" dt "<<t_corr<<" lr "<<lrCalib<<" eAngle "<<eAngle<<std::endl;


  if (fabs(driftD)<=0.0001) driftD = 0.001;
  return driftD;
} 

float MdcxHit::d(MdcxHel &hel) const {
  float doca=hel.Doca(*this); // changes hel's internal state...
  return d(hel.Doca_Zh(),hel.Doca_Tof(),hel.T0(),
      hel.Doca_Wamb(),hel.Doca_Eang());
}//endof d

float MdcxHit::e(float dd) const{
  //if (0!=_consterr) return _cresol;//yzhang delete 2009-11-03 
  float errTemp = fabs(getSigma(dd));
  //  protect against resolution = 0; set min resol to 1 nm
  float errMin = 1.0e-7;
  return errTemp<errMin?errMin:errTemp;
}

float MdcxHit::pull(MdcxHel &hel) const {
  // compute pulls for |hel|
  float doca=hel.Doca(*this); if(hel.Mode() == 0)doca=fabs(doca);
  return (d(hel.Doca_Zh(),hel.Doca_Tof(),hel.T0(),
	hel.Doca_Wamb(),hel.Doca_Eang())-doca)/e(doca);
  //return residual(hel)/e();
}

float MdcxHit::residual(MdcxHel &hel) const {
  // compute residuals for |hel|
  float doca = hel.Doca(_wx, _wy, _wz, _x, _y);
  if(hel.Mode() == 0) doca = fabs(doca);
  doca += v()*hel.T0();
  return d(hel.Doca_Zh(),hel.Doca_Tof(),hel.T0(),
      hel.Doca_Wamb(),hel.Doca_Eang())-doca;
  //return d(hel) - doca;
}

std::vector<float> MdcxHit::derivatives(MdcxHel &hel) const {
  // compute derivatives for |hel|
  std::vector<float> deriv = hel.derivatives(*this);
  float dtemp = d(hel.Doca_Zh(), hel.Doca_Tof(), hel.T0(),
      hel.Doca_Wamb(), hel.Doca_Eang());
  //float dtemp = d(hel);
  deriv[0] = dtemp - deriv[0];
  //  deriv[0] -= v()*hel.T0();
  float ewire = e(dtemp);
  for (uint32_t i = 0; i < deriv.size(); i++) deriv[i] /= ewire;
  return deriv;
}//endof derivatives

void MdcxHit::print(ostream &o, int i) const {
  o << "(" << Layer() << "," << WireNo() << ",id "<< getDigi()->getTrackIndex()<<") " ;
}

void MdcxHit::printAll(ostream &o, int i) const {
  o << " hit" << i << " (" << Layer() << "," << WireNo() << ")  ";
  if (getMdcHit()) o << " phi "<< getMdcHit()->phi();
  o << "dd " << d() << " dde "
    << e() << " rt " << t() << endl;
}

double MdcxHit::getSigma(float driftDist, int ambig, double entranceAngle,
    double dipAngle, double z) const {
  int lrCalib = 2;
  if (ambig != 0) lrCalib = (ambig == 1) ? 0 : 1;
  double eAngle = EntranceAngle(entranceAngle); 
  //std::cout<<_layernumber<<" "<<lrCalib<<" dd "<<driftDist<<" "<<eAngle<<" "<<dipAngle<<" "<<z<<"   "<<_iAdc<<std::endl;
  double sig = 0.1 * m_mdcCalibFunSvc->getSigma(_layernumber, lrCalib,
      driftDist*10., eAngle, tan(dipAngle), z*10., _iAdc);
  //double sig = 0.1 * m_mdcCalibFunSvc->getSigma(_layernumber, _wirenumber, lrCalib,
  //driftDist*10., eAngle, tan(dipAngle), z*10., _iAdc);
#ifdef MDCXRECO_RESLAYER
  if (Layer() == g_resLayer) {
    //give a huge sigma to skip this layer when fit track
    return 9999.;
  }
#endif 
  return sig;
}
