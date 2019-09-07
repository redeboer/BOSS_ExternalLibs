// $Id: MdcHitOnTrack.cxx,v 1.22 2012/08/13 00:01:16 zhangy Exp $
//
#include "MdcData/MdcHitOnTrack.h"
#include "MdcData/MdcHit.h"
#include "MdcGeom/Constants.h"
#include "MdcGeom/BesAngle.h"
// MdcGeom needed to verify if hit is inside of chamber...
// and to find the trajectory describing the hit, i.e. wire
#include "MdcGeom/MdcLayer.h"
#include "MdcGeom/MdcSWire.h"
#include "TrkBase/TrkPoca.h"
#include "TrkBase/TrkDifTraj.h"
#include "TrkBase/TrkRep.h"
#include "TrkBase/TrkSimpTraj.h"
#include "TrkBase/TrkRecoTrk.h"
#include "MdcGeom/EntranceAngle.h"
using std::cout;
using std::endl;
/*
#include "GaudiKernel/NTuple.h"
extern NTuple::Tuple*  		        g_tupleHit;
extern NTuple::Item<int>                g_hitLayer;
extern NTuple::Item<int>                g_hitWire;
extern NTuple::Item<double>             g_hitAmbig;
extern NTuple::Item<double>             g_hitEAngle;
extern NTuple::Item<double>             g_hitZ;
extern NTuple::Item<double>             g_hitAmbigMc;
extern NTuple::Item<double>             g_hitEAngleMc;
extern NTuple::Item<double>             g_hitZMc;
extern NTuple::Item<double>             g_hitDrift;
extern NTuple::Item<double>             g_hitDriftMc;
extern NTuple::Item<int>                g_hitTkIdMc;
extern NTuple::Item<double>             g_hitPhiPoca;
extern NTuple::Item<double>             g_hitPhiHit;
extern NTuple::Item<double>             g_hitPhiHit0;
extern NTuple::Item<double>             g_hitPhiHitDel;
extern int haveDigiTk[43][288];
extern int haveDigiMcLr[43][288];
extern double haveDigiEAngle[43][288];
extern double haveDigiZ[43][288];
extern double haveDigiDrift[43][288];
*/

//-------------
// Constructors
//-------------
MdcHitOnTrack::MdcHitOnTrack(const TrkFundHit& fundHit,
    const MdcHit& baseHit,
    int ambig,  double t0)
: TrkHitOnTrk(&fundHit,10.e-4),
  _ambig(ambig),
  _hitTraj(baseHit.hitTraj()),  
  _fitTime(baseHit.rawTime()-t0*1e-9),
  _dHit(&baseHit)
{
  // need to flag somehow that that we haven't computed things yet...
  //   now, we know that _drift[0] is for ambig <0, and _drift[1] is ambig >0
  //   and _drift is signed according to ambig. Thus _drift[0] should be -tive
  //   and _drift[1] should be +tive. To indicate this are not yet initialized,
  //   put 'impossible' values here...
  _drift[0] = +9999;
  _drift[1] = -9999;
  setHitResid(-21212121.0);
  setHitRms( 0.02 );
  setHitLen(0.5 * baseHit.layer()->zLength());
  setFltLen(0.);
  _startLen = hitTraj()->lowRange() - 5.;
  _endLen   = hitTraj()->hiRange()  + 5.;
}

//MdcHitOnTrack::MdcHitOnTrack(const TrkFundHit *fundHit, int ambig, double fittime,
//                             int layer, int wire)
//  : TrkHitOnTrk(fundHit,10.e-4),
//    _ambig(ambig), _fitTime(fittime)
//{
//  cout << "using old MdcHitOnTrack constructor..." << endl;

//  IService* ser;
//  StatusCode sc = Gaudi::svcLocator()->getService("MdcGeomSvc",ser);
//  if (!sc.isSuccess())
//    cout <<"MdcHitOnTrack::Could not open Geometry Service"<<endl;
//  MdcGeomSvc *m_gmSvc = dynamic_cast<MdcGeomSvc*> (ser);
//  if(!m_gmSvc) cout <<"MdcHitOnTrack::Could not open Geometry Service"<<endl;



//  _drift[0] = +9999;
//  _drift[1] = -9999;
//  _hitTraj = gblEnv->getMdc()->getMdcLayer(layer)->makeHitTrajInGlobalCoords(wire);
//    _hitTraj = m_gmSvc->Layer(layer)->makeHitTrajInGlobalCoords(wire);
//  setHitResid(-21212121.0);
//  setHitRms( 0.01 );
//  setHitLen(2.);
//  setFltLen(0.);
//  _startLen = hitTraj()->lowRange() - 5.;
//  _endLen   = hitTraj()->hiRange()  + 5.;
//}

MdcHitOnTrack::MdcHitOnTrack(const MdcHitOnTrack &hot,TrkRep *newRep,
    const TrkDifTraj* trkTraj, const MdcHit *hb)
: TrkHitOnTrk(hot,newRep,trkTraj)
{
  _ambig = hot._ambig;
  _hitTraj  = hot._hitTraj;
  _fitTime  = hot._fitTime;
  _drift[0] = hot._drift[0];
  _drift[1] = hot._drift[1];
  _startLen = hot._startLen;
  _endLen   = hot._endLen;
  _dHit = (hb==0?hot._dHit:hb);
}

MdcHitOnTrack::~MdcHitOnTrack()
{ ; }

  void
MdcHitOnTrack::setT0(double t0)
{ 
  _fitTime= _dHit->rawTime()-t0*1e-9; 
}

double
MdcHitOnTrack::dcaToWire() const
{
  double dca = -9999.;
  if ( getParentRep() == 0 ) {
    //     cout << "no parent rep" << endl;
    return dca;
  }
  // WARNING: cannot use the internal _poca, as it lags one iteration
  //          behind the fit... therfore use _EXTERNAL_ residual
  if (isActive())  {  // FIXME: currently can only use 'resid()' if isActive..
    dca = resid()+drift();
  } else {
    TrkPoca poca(getParentRep()->traj(), fltLen(), *hitTraj(), hitLen(),
	_tolerance);
    if (poca.status().success()) dca = poca.doca();
  }
  return dca;
}

  bool
MdcHitOnTrack::updateAmbiguity(double dca)
{
  if (dca < 0 && ambig() >= 0) {
    setAmbig(-1); return isActive();
  } else if (dca > 0 && ambig() <= 0) {
    setAmbig(1); return isActive();
  } else {
    return false;
  }
}

const MdcHitOnTrack*
MdcHitOnTrack::mdcHitOnTrack() const
{
  return this;
}

double
MdcHitOnTrack::entranceAngleHit() const
{
  static Hep3Vector dir;
  static HepPoint3D pos;
  if (getParentRep() == 0) return 0.;
  getParentRep()->traj().getInfo(fltLen(), pos, dir);

  return BesAngle(dir.phi() - pos.phi());
}

double
MdcHitOnTrack::entranceAngle() const
{
  static Hep3Vector dir;
  static HepPoint3D pos;
  if (getParentRep() == 0) return 0.;
  getParentRep()->traj().getInfo(fltLen(), pos, dir);

  return entranceAngle(pos, dir);
}

double
MdcHitOnTrack::entranceAngle(const HepPoint3D pos, const Hep3Vector dir) const
{
  double angle = EntranceAngle(dir.phi() - _dHit->phi(pos.z()));
  //std::cout<< "eAngle("<<layernumber()<<","<<wire()<<") dir.phi()  "<<dir.phi()*180./3.14<<" hit phi "<<_dHit->phi(pos.z())*180./3.14<<" eAngle "<<angle*180./3.14<<" degree "<<angle<<std::endl;

  //std::cout<< __FILE__ << "   " << __LINE__ << "   ("<<layernumber()<<","<<wire()<<") "<<
  //" phiPoca "<<dir.phi()*180/3.14<< " phiWire "<<_dHit->phi(pos.z())*180/3.14<<" z "<<pos.z()<<
  //" dPhiz "<<_dHit->wire()->dPhizDC(pos.z())*180/3.14<< " eAngle "<<angle*180/3.14<< " angle "<<(dir.phi() - _dHit->phi(pos.z()))*180./3.14<<std::endl;
  /*
     if(g_tupleHit && fabs(angle)>0.0001){
     int layer = layernumber();
     int wireId = wire();
     g_hitLayer = layer;
     g_hitWire = wireId;

     int lrCalib=2;
     if (ambig()==1) lrCalib = 0;
     else if (ambig()==-1) lrCalib = 1;
     g_hitAmbig = lrCalib;
     g_hitAmbigMc = haveDigiMcLr[layer][wireId];
     g_hitEAngle = angle*180./3.14;
     g_hitEAngleMc = haveDigiEAngle[layer][wireId]*180./3.14;
     g_hitZ = pos.z();
     g_hitZMc = haveDigiZ[layer][wireId];
     g_hitDrift = _drift[ambig()];
     g_hitDriftMc = haveDigiDrift[layer][wireId];
     g_hitTkIdMc = haveDigiTk[layer][wireId];
     g_hitPhiPoca = dir.phi()*180./3.41;
     g_hitPhiHit = _dHit->phi(pos.z())*180./3.41;
     g_hitPhiHit0 = _dHit->phi()*180./3.41;
     g_hitPhiHitDel = _dHit->wire()->dPhiz()*180./3.41;
     g_tupleHit->write();
     }
     */
  return angle;
}

unsigned
MdcHitOnTrack::layerNumber() const
{
  return layernumber();
}

double
MdcHitOnTrack::dipAngle() const
{
  return getParentRep()==0?0:Constants::pi/2-getParentRep()->traj().direction(fltLen()).theta();
}

  TrkErrCode
MdcHitOnTrack::updateMeasurement(const TrkDifTraj* traj, bool maintainAmb)
{
  TrkErrCode status=updatePoca(traj,maintainAmb);
  if (status.failure()) {
#ifdef MDCPATREC_DEBUG
    std::cout<<" ErrMsg(warning) " << "MdcHitOnTrack::updateMeasurement failed " << status << std::endl;
#endif
    return status;
  }
  assert (_poca!=0);
  double dca=_poca->doca();
  bool forceIteration = (maintainAmb&&ambig()!=0)?false:updateAmbiguity(dca);
  //std::cout<< __FILE__ << "   " << __LINE__ << " maintainAmb "<< maintainAmb
  //<< " maintain&& ambig "<<(maintainAmb&&ambig()!=0)
  //<< " forceIteration  "<<forceIteration<<std::endl;
  assert(ambig()!=0);
  // Check for hits beyond end plates.  !!Turn off hit if it is == temp. hack
  if (isBeyondEndflange()) setUsability(false);
  if (forceIteration || !driftCurrent() ) {
    updateCorrections(); // force recomputation of drift for current ambig(), setting of hitRms
    forceIteration=true;
  }
  setHitResid(dca-drift());
  return !forceIteration?status:
    TrkErrCode(TrkErrCode::succeed, 11, "Ambiguity flipped");
}

  void
MdcHitOnTrack::updateCorrections()
{
  const TrkRep* tkRep = getParentRep();
  assert(tkRep != 0);
  static HepPoint3D pos; static Hep3Vector dir;
  _trkTraj->getInfo(fltLen(), pos, dir);

  // for bes3 cosmic test
  int wireAmb = wireAmbig();
  double tof = tkRep->arrivalTime(fltLen());
  // at this point, since dcaToWire is computed, _ambig must be either -1 or +1
  assert( ambig() == -1 || ambig() == 1 );
  double eAngle = entranceAngle(pos, dir);//2011-11-22
  double dAngle = Constants::pi/2 - dir.theta();
  double z = pos.z();
  // note the special case for INCOMING tracks:
  //    wire left of track implies hit on the right side of wire
  //int wireAmb= fabs(eAngle)<Constants::pi/2?_ambig:-_ambig; //yzhang delete 2012-07-17 

  // provide the underlying hit with the *external* information
  // needed to compute the drift distance, i.e. those numbers that
  // the hit cannot figure out by itself...
  double dist = _dHit->driftDist(tof, wireAmb, eAngle, dAngle, z); 
  //<<" dir "<<dir<<" pos "<<pos
  //assert(dist>0);//yzhang 2011-12-05 delete
  _fitTime = _dHit->driftTime(tof,z);
  //std::cout<<" MdcHitOnTrack  ("<<layernumber()<<","<<wire()<<") "<<mdcHit()->isCosmicFit()<<" fltLen "<<fltLen() <<" eAngle "<<eAngle<<" ambig "<<ambig()<<" wireAmb "<<wireAmb<<" tof "<<tof<<" dd "<<dist<<" dt "<<_fitTime<<std::endl;
  _drift[ambig()<0?0:1] = ambig() * dist;
  assert( driftCurrent() );

  double newSig = _dHit->sigma(dist, wireAmb, eAngle, dAngle, z);

  //assert(newSig>0);//yzhang 2011-12-05 delete
  setHitRms(newSig);
}

double
MdcHitOnTrack::driftVelocity() const
{
  const TrkRep* tkRep = getParentRep();
  assert(tkRep != 0);
  double tof =  tkRep->arrivalTime(fltLen());
  static HepPoint3D pos; static Hep3Vector dir;
  _trkTraj->getInfo(fltLen(), pos, dir);
  double eAngle = entranceAngle(pos, dir);//2011-11-22
  double dAngle = Constants::pi/2 - dir.theta();
  double z = pos.z();
  // note the special case for INCOMING tracks:
  //    wire left of track implies hit on the right side of wire
  int wireAmb= fabs(eAngle)<Constants::pi/2?_ambig:-_ambig;

  static const double epsilon = 0.5e-9; // tof are in s;
  double dist1 =  _dHit->driftDist(tof+epsilon, wireAmb, eAngle, dAngle, z);
  double dist2 =  _dHit->driftDist(tof-epsilon, wireAmb, eAngle, dAngle, z);

  return (dist2-dist1)/(2*epsilon); // velocity in cm/s
}

bool
MdcHitOnTrack::timeResid(double &t, double &tErr) const
{
  double v = driftVelocity();
  if (v <= 0) return false;
  t = (fabs(drift())-fabs(dcaToWire()))/v;
  tErr= hitRms()/v;
  return true;
}

bool
MdcHitOnTrack::timeAbsolute(double &t, double &tErr) const
{
  double tresid(-1.0);
  if(timeResid(tresid,tErr)){
    // add back the track time
    t = tresid + getParentRep()->parentTrack()->trackT0();
    return true;
  } else
    return false;
}

TrkEnums::TrkViewInfo
MdcHitOnTrack::whatView() const
{
  return _dHit->whatView();
}

int
MdcHitOnTrack::layernumber() const
{
  return _dHit->layernumber();
}

const MdcLayer*
MdcHitOnTrack::layer() const
{
  return _dHit->layer();
}

int
MdcHitOnTrack::wire() const
{
  return _dHit->wirenumber();
}

int
MdcHitOnTrack::whichView() const
{
  return _dHit->whichView();
}

double
MdcHitOnTrack::rawTime() const
{
  return _dHit->rawTime();
}

double
MdcHitOnTrack::charge() const
{
  return _dHit->charge();
}

const Trajectory*
MdcHitOnTrack::hitTraj() const
{
  return _hitTraj;
}

const MdcHit*
MdcHitOnTrack::mdcHit() const
{
  return 0;
}


// Replace underlying hit pointer (base class doesn't own it)

  void 
MdcHitOnTrack::changeBase(MdcHit* newBase)
{
  _dHit = newBase;
}

/*
   MdcHitOnTrack::isBeyondEndflange() const { 
   std::cout<<__FILE__<<" "<<__LINE__
   <<" hitLen "<<hitLen()
   <<" startLen "<<_startLen
   <<" endLen "<<_endLen
   <<  std::endl;
   return (hitLen() < _startLen || hitLen() > _endLen); 
   }
   */

int MdcHitOnTrack::wireAmbig() const {
  // hit wrt the wire location

  //return fabs(entranceAngle())<Constants::pi/2?ambig():-ambig();
  const TrkRep* tkRep = getParentRep();
  static Hep3Vector dir;
  static HepPoint3D pos;
  if (getParentRep() == 0) return 0.;
  getParentRep()->traj().getInfo(fltLen(), pos, dir);

  double wireAmb = ambig();
  if (mdcHit()->isCosmicFit()){
    HepPoint3D poca = tkRep->position(0.);
    if ( pos.y() > poca.y()){
      wireAmb = -1.*_ambig;//yzhang 2012-07-17 
      //std::cout<<"MdcHitOnTrack CosmicFit  up ambig *-1"<<std::endl;
    }
  } 

  return wireAmb;
}
