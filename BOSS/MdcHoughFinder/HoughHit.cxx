#include "MdcHoughFinder/HoughHit.h"
#include "MdcHoughFinder/HoughGlobal.h"
#include "RawEvent/RawDataUtil.h"
#include "Identifier/MdcID.h" 
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include <assert.h>

const MdcCalibFunSvc* HoughHit::_calibPtr=NULL;
MdcGeomSvc*     HoughHit::_mdcGeomSvc=NULL;
double HoughHit::_bunchTime;
int  HoughHit::_npart=360;
//MdcDetector*      HoughHit::_det=NULL;

HoughHit::HoughHit(){
  _det=Global::m_gm;
  _digiPtr=NULL;
  _layerPtr=NULL;
  _wirePtr=NULL;
}
HoughHit::HoughHit(const MdcDigi* const aDigi){
  _det= Global::m_gm;
  _digiPtr=aDigi;
  _id       = aDigi->identify();
  _layer    = MdcID::layer(_id); 
  _wire     = MdcID::wire(_id); 
  _layerPtr   = _det->Layer(_id); 
  _wirePtr    = _det->Wire(_id); 
  _rawTime  = RawDataUtil::MdcTime(_digiPtr->getTimeChannel());
  _charge   = _digiPtr->getChargeChannel();
  _driftTime = driftTime();
  _driftDist = calDriftDist(_bunchTime,0);
  _slayerType=slayerType(_layer);
  _lr = 0;
  _rmid     = _wirePtr->rMid();

  assert(_mdcGeomSvc!=NULL);
  const MdcGeoWire* wire = _mdcGeomSvc->Wire(_layer,_wire);
  assert(wire!=NULL);
  HepPoint3D eastP = wire->Backward()/10.;
  HepPoint3D westP = wire->Forward()/10.;
  _eastPoint = eastP;
  _westPoint = westP;
  _midPoint = (eastP+westP)/2.;

  _type = MIDPOINT;
  conformalTrans( _midPoint.x(), _midPoint.y(),_driftDist);
}

HoughHit::HoughHit(const HoughHit& other) :
  _digiPtr(other._digiPtr), 
  _det(other._det),
  _layerPtr(other._layerPtr), _wirePtr(other._wirePtr), _id(other._id),
  _layer(other._layer), _wire(other._wire),
  _rawTime(other._rawTime), _charge(other._charge),
  _eastPoint(other._eastPoint),_westPoint(other._westPoint),_midPoint(other._midPoint),
  _u(other._u),_v(other._v),
  _type(other._type),_detectorType(other._detectorType),
  _rmid(other._rmid), _slayerType(other._slayerType),
  _cirlist(other._cirlist),_style(other._style),
  _driftTime(other._driftTime), _driftDist(other._driftDist),
  CF_drift(other.CF_drift),
  vec_cfcir(other.vec_cfcir),
  //truth
  _truthU(other._truthU),
  _truthV(other._truthV),
  _truthR(other._truthR),
  _truthlr(other._truthlr),
  _truthPoint(other._truthPoint),
  _truthDrift(other._truthDrift),
  _deltad(other._deltad),
  _flightLength(other._flightLength)
  { }

HoughHit& HoughHit::operator=(const HoughHit& other){ 
  _digiPtr=(other._digiPtr); 
  _det=(other._det);
  _layerPtr=(other._layerPtr); _wirePtr=(other._wirePtr); _id=(other._id);
  _layer=(other._layer); _wire=(other._wire);
  _rawTime=(other._rawTime); _charge=(other._charge);
  _eastPoint=(other._eastPoint);_westPoint=(other._westPoint);_midPoint=(other._midPoint);
  _u=(other._u);_v=(other._v);
  _type=(other._type);_detectorType=(other._detectorType);
  _rmid=(other._rmid); _slayerType=(other._slayerType);
  _cirlist=(other._cirlist);_style=(other._style);
  _driftTime=(other._driftTime); _driftDist=(other._driftDist);
  CF_drift = (other.CF_drift);
  vec_cfcir = (other.vec_cfcir);
  //truth
  _truthU= (other._truthU);
  _truthV= (other._truthV);
  _truthR= (other._truthR);
  _truthlr= (other._truthlr);
  _truthDrift= (other._truthDrift);
  _truthPoint= (other._truthPoint);
  _deltad= (other._deltad);
  _flightLength= (other._flightLength);
}

void HoughHit::setTruthInfo(const MdcMcHit*& aMcHit){
  if(!aMcHit) return;
  _truthDrift = aMcHit->getDriftDistance()/10.;//mm to cm
  _truthPoint.setX(aMcHit->getPositionX()/10.);//mm to cm
  _truthPoint.setY(aMcHit->getPositionY()/10.);
  _truthPoint.setZ(aMcHit->getPositionZ()/10.);

  int mcLR = aMcHit->getPositionFlag();
//  if (mcLR == 0) mcLR = -1;//FIXME
//  for truth reserve from digi
  if (mcLR == 1) mcLR = -1;//
  if (mcLR == 0) mcLR = 1;//
  _truthId = aMcHit->getTrackIndex();
  _truthlr = mcLR;

//fix
  _truthU = getConformal_u( _truthPoint.x(), _truthPoint.y(), _truthDrift );
  _truthV = getConformal_v( _truthPoint.y(), _truthPoint.x(), _truthDrift );
  _truthR = getConformal_r( _truthPoint.x(), _truthPoint.y(), _truthDrift );

  //std::cout<<__FILE__<<"   "<<_layer<<","<<_wire<<" "<<_truthId<<" truth  "<<_truthPoint<<" truthU "<<_truthU<<std::endl;

  _type = DIGIWITHTRUTH;
}

void HoughHit::conformalTrans(double x,double y,double r){
	_u = 2*x/(x*x+y*y-r*r);
	_v = 2*y/(x*x+y*y-r*r);
	CF_drift = 2*r/(x*x+y*y-r*r);
}

double HoughHit::getConformal_u(double x,double y,double r) {
  return 2*x/(x*x+y*y-r*r);
}
double HoughHit::getConformal_v(double x,double y,double r) {
  return 2*x/(x*x+y*y-r*r);
}
double HoughHit::getConformal_r(double x,double y,double r) {
  return 2*r/(x*x+y*y-r*r);
}

double HoughHit::driftTime() const {
  double tprop = _calibPtr->getTprop(_layer,0); 
  double T0Walk = _calibPtr->getT0(_layer,_wire) +  _calibPtr->getTimeWalk(_layer, _charge);
  //tof in ns, driftTime in ns, _T0Walk in ns
  double driftT = _rawTime - T0Walk - 1.e9*_bunchTime- tprop;
  return driftT;
}
double HoughHit::driftTime(double tof, double z) const {
  double tprop = _calibPtr->getTprop(_layer,z*10.); 
  double T0Walk = _calibPtr->getT0(_layer,_wire) +  _calibPtr->getTimeWalk(_layer, _charge);
  //tof in ns, driftTime in ns, _T0Walk in ns
  double driftT = _rawTime - T0Walk - 1.e9*tof - tprop;

  return driftT;
}

double HoughHit::calDriftDist(double bunchTime, int ambig) const  {

  //  double crudeTof = 0; //FIXME
  return calDriftDist(bunchTime+crudeTof(), ambig, 0., 0., 0. );
}

double HoughHit::calDriftDist(double tof, int ambig, double entranceAngle, double /*dipAngle*/, double z) const {

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
  driftD = 0.1 * _calibPtr->driftTimeToDist(driftTime(tof,z),_layer,_wire,lrCalib,entranceAngle);//to cm
  //std::cout<<"driftDist "<<"("<<_layer <<","<<_wire <<") dd "<<driftD<<" dt "<<driftTime(tof,z) <<" lr "<<lrCalib <<" eAng "<<entranceAngle <<" tof "<<tof*1.e9<<" z "<<z <<" t0walk "<<_T0Walk<<" rawT "<<_rawTime <<" tprop "<< _rawTime - driftTime(tof,z)- _T0Walk-1.e9*tof<<std::endl;

  if (abs(driftD)<0.00001) driftD = 0.00001;
  return driftD;
}

void HoughHit::print() const{
  std::cout<<"("<<_layer<<","<<_wire<<") "<<std::endl;
}

void HoughHit::printAll() const{
  std::cout<<"("<<_layer<<","<<_wire<<") id "<<this->digi()->getTrackIndex()<<" xyz "<<_midPoint<<endl;
}
void HoughHit::printTruth() const{
  std::cout<<"("<<_layer<<","<<_wire<<") truth "<<_truthId<<" xyz "<<_truthPoint<<" uv "<<_truthU<<","<<_truthV<<")  "<<" cir list "<<_cirlist<<" ambig "<<_truthlr<<std::endl;
}

int HoughHit::slayerType(int layer){
  //get nominal shift cell of this layer
  double nomShift= _mdcGeomSvc->Layer(layer)->nomShift();

  if(nomShift>0) return 1;
  else if(nomShift<0) return -1;
  else return 0;

  return -999;
}

//calcu drift cir
void HoughHit::makeCir(int n,double phi_begin,double phi_last,double r){
  vec_cfcir.clear();
  vector<CFCir>().swap(vec_cfcir);
  for(int i =0; i<n; i++){
	double phi_slice = (phi_last-phi_begin)/n;
	double phi = phi_begin + (1/2.+i)*phi_slice;
	double x = _u + r*cos(phi);
	double y = _v + r*sin(phi);
	vec_cfcir.push_back( CFCir(x,y,phi,n,_u,_v,r) );
  }
}

