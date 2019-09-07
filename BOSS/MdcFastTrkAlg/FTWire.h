// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTWire
// 
// Description:  wire class for MdcFastTrkAlg

#ifndef FTWire_FLAG_
#define FTWire_FLAG_

//...Defs...
#define FTWireHit 1
#define FTWireHitInvalid 2
#define FTWireHitAppended 4
#define FTWireHitAppendedOrInvalid 6
#define FTWireNeighbor0 8
#define FTWireNeighbor1 16
#define FTWireNeighbor2 32
#define FTWireNeighbor3 64
#define FTWireNeighbor4 128
#define FTWireNeighbor5 256
#define FTWireHitSegment 16384
#define FTWireHitLinked 131072
#define FTWireHitRight 32768
#define FTWireHitLeft 65536

#define FTWireFittingInvalid 0x10000000
#define FTWireInvalid 0x20000000
#define FTWireStateMask 0xf0000000

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif

#include "MdcFastTrkAlg/MdcFastTrkAlg.h"
#include "MdcFastTrkAlg/FTLayer.h"
#include "MdcFastTrkAlg/FTSuperLayer.h"
#include "MdcFastTrkAlg/FTList.h"
#include "TrackUtil/Lpav.h"
#include "CLHEP/Matrix/Vector.h"
using namespace CLHEP;

class FTWire{
public:
  /// constructors
  FTWire(const float x, const float y, const float dx, const float dy,
	 const FTLayer &, const int localID, FTWire * const vt);

  FTWire();
  /// destructor
  ~FTWire();

public: //Selectors

  /// initNeighbor
  void initNeighbor(void);

  /// returns position x
  const float x(void) const;

  /// returns position y
  const float y(void) const;

  /// returns phi
  float phi(void) const;

  /// returns local ID
  const int localId(void) const;

  /// returns layer
  const FTLayer & layer(void) const;

  /// returns pointer of neighbor array
  FTWire ** neighborPtr(void);

  /// returns drift distance
  float distance(void) const;

  /// returns z_distance from the center of wire by drift distance
  float distance_z(void) const;

  /// returns t0 read at begin run
  float t0(void) const;

  /// returns pedestal read at begin run
  float pedestal(void) const;

  /// returns state
  unsigned state(void) const;

  /// returns state bit
  unsigned stateAND(const unsigned mask) const;

  /// returns z for track la
  int z(const Lpav & la, double &  z) const;

  /// rerurns TDC time(after t0 subtraction)
  float time(void) const;
   
  // wangdy add:return ADC value 
  float getAdc(void) const;

  //max add:return residual
  float getChi2(void) const;

public: // Modifires
  /// clear
  void clear(void);

  /// set drift distance
  float distance(const float distance);

  /// set t0 at begin-run
  float t0(const float t0);

  /// set pedestal at begin-run
  float pedestal(const float ped);

  /// set state
  unsigned state(const unsigned state);

  /// set state bit
  void stateOR(const unsigned mask);

  /// invert state bit
  void stateXOR(const unsigned mask);

  /// reset state bit
  void stateORXOR(const unsigned mask);
  
  /// check neighbors of phi-side and raise invalid flag if both hits
  void chk_left_and_right(void);

  /// reset time
  float time(float t);

  /// set wireId
  void wireId(int wireID); 

  /// get wireId
  int getWireId(void);

  /// wangdy add:set Adc value 
  void setAdc(float adc);

  /// set residual fit chi2
  void setChi2(float chi2);

private: //private member functions
  /// returns left wire;
  const FTWire * left(void) const;

  /// returns left wire;
  const FTWire * right(void) const;

  /// compute inner Neighbor
  void getInnerNeighbor(FTWire * const vtWire);

  /// compute outer Neighbor
  void getOuterNeighbor(FTWire * const vtWire);

  /// compute mid Neighbor
  void getMidNeighbor(void);

private: //private data members
  const float _x;
  const float _y;
  const float _dx;
  const float _dy;
  const FTLayer & _layer;
  FTWire *const & _wire;
  const int _localId;
  const float _phi;
  int _wireId;
  float _distance;
  float _t0;
  float _time;
  float _pedestal;
  unsigned int _state;
  FTWire * _neighbor[6];
  float m_posx;
  float m_posy;
  float m_posz;
  float m_adc; //wangdy adds
  float  m_chi2; //max adds
};

//----------------------------------------------
#ifdef FTWire_NO_INLINE
#define inline
#else
#undef inline
#define FTWire_INLINE_DEFINE_HERE
#endif

#ifdef FTWire_INLINE_DEFINE_HERE

inline
FTWire::FTWire(const float x, const float y,
	       const float dx, const float dy, const FTLayer & layer,
               const int localID, FTWire * const vt)
  : _x(x),			// x position in z = 0;
    _y(y),			// y position in z = 0;
    _dx(dx),
    _dy(dy),
    _layer(layer),
    _wire(vt),
    _localId(localID),
    _phi((const float)2*M_PI*_localId/(float)_layer.NWire()+_layer.offset()),
    //_wireId(wireID),
    //_hep(NULL),
    _distance(0),
    _t0(0),
    _time(0),
    m_adc(0),
    _pedestal(0),
    _state(FTWireHitInvalid)
{
    getMidNeighbor();
}

inline
FTWire::FTWire()
  : _x(0),
    _y(0),
    _dx(0),
    _dy(0),
    _layer(*(FTLayer *)NULL),
    _wire(NULL),
    _localId(0),
    //_wireId(0),
    _phi(0),
    _distance(0),
    _t0(0),
    _time(0),
    m_adc(0),
    _pedestal(0),
    _state(FTWireHitInvalid)
{
  _neighbor[0] = NULL;
  _neighbor[1] = NULL;
  _neighbor[2] = NULL;
  _neighbor[3] = NULL;
  _neighbor[4] = NULL;
  _neighbor[5] = NULL;
}

inline
FTWire::~FTWire()
{
}

inline
void
FTWire::initNeighbor()
{
  getInnerNeighbor(_wire);
  getOuterNeighbor(_wire);
  /*cout << "_layer.layerId()::" << _layer.layerId() << endl;
  cout << "The WireId ::" << _localId << endl;
  cout << "Its neighbor 0 ::" << _neighbor[0]->localId() << endl;
  cout << "Its neighbor 1 ::" << _neighbor[1]->localId() << endl;
  cout << "Its neighbor 2 ::" << _neighbor[2]->localId() << endl;
  cout << "Its neighbor 3 ::" << _neighbor[3]->localId() << endl;
  cout << "Its neighbor 4 ::" << _neighbor[4]->localId() << endl;
  cout << "Its neighbor 5 ::" << _neighbor[5]->localId() << endl;*/
}
inline
void
FTWire::getInnerNeighbor(FTWire * const vtWire)
{
  if (!_layer.localLayerId()) {
        _neighbor[0] = _neighbor[1] = vtWire;
        return;
  }
  float thisPhi = phi();
  const FTLayer * pLayer = &_layer;
  int n = (*(pLayer-1)).NWire(); 
  int low=0, high=n-1, mid = 0;
  int innerPosition = 1; 
  FTWire * lastInnerPointer = this - localId() - 1;
  while (low<=high){
    mid=(low+high)/2;
    if ((*(lastInnerPointer-mid)).phi() <= thisPhi)
      high=mid-1;
    else 
      low=mid+1;
  }
  
  innerPosition = mid; 
  if((*(lastInnerPointer-innerPosition)).phi() <= thisPhi) {
      _neighbor[0] = lastInnerPointer-innerPosition;
      if(innerPosition == 0) _neighbor[1] = lastInnerPointer-n+1;
      else _neighbor[1] = lastInnerPointer-innerPosition+1;
  } else {
      if((innerPosition+1) == n) _neighbor[0] = lastInnerPointer;
      else _neighbor[0] = lastInnerPointer-innerPosition-1;
      _neighbor[1] = lastInnerPointer-innerPosition;
  }

}

inline
void
FTWire::getOuterNeighbor(FTWire * const vtWire)
{
  if (_layer.localLayerId()==_layer.superLayer().layerMaxId()) {
        _neighbor[4] = _neighbor[5] = vtWire;
        return;
  }
  float thisPhi = phi();
  const FTLayer * pLayer = &_layer;  
  int n = (*(pLayer+1)).NWire();
  int low=0, high=n-1, mid = 0;
  int position;
  FTWire * firstPointer = this + _layer.NWire() - localId();
  while (low<=high){
    mid=(low+high)/2;
    if ((*(firstPointer+mid)).phi() > thisPhi)
      high=mid-1;
    else
      low=mid+1;
  }
 
  position = mid; 
  if((*(firstPointer+position)).phi() <= thisPhi) {
      _neighbor[4] = firstPointer+position;
      if((position+1) == n) _neighbor[5] = firstPointer;
      else _neighbor[5] = firstPointer+position+1;
  } else {
      if((position-1) == -1) _neighbor[4] = firstPointer + n - 1;
      else _neighbor[4] = firstPointer+position-1;
      _neighbor[5] = firstPointer+position;
  }
}

inline
void
FTWire::getMidNeighbor(void)
{
    _neighbor[2] = (FTWire *) left();
    _neighbor[3] = (FTWire *) right();
}

inline
void
FTWire::clear(void)
{
  _distance = 0.;
  _time = 0.;
  _t0 = 0.;
  m_adc = 0.0; 
  _pedestal = 0.;
  _state = FTWireHitInvalid;
}

inline
const FTLayer &
FTWire::layer(void) const
{
  return _layer;
}

inline
const float
FTWire::x(void) const
{
  return _x;
}

inline
const float
FTWire::y(void) const
{
  return _y;
}

inline
const int
FTWire::localId(void) const
{
  return _localId;
}

inline
FTWire **
FTWire::neighborPtr(void)
{
  return _neighbor;
}

inline
float
FTWire::phi(void) const
{
  return _phi;
}

inline
float
FTWire::distance(void) const
{
  return _distance;
}

inline
float
FTWire::distance_z(void) const
{
  return _distance * std::fabs(_layer.tanSlant());
}

inline
float
FTWire::distance(const float distance)
{
  return _distance = distance;
}

inline
float
FTWire::t0(void) const
{
  return _t0;
}

inline
float
FTWire::t0(const float t0)
{
  return _t0 = t0;
}

inline
float
FTWire::pedestal(void) const
{
  return _pedestal;
}

inline
float
FTWire::pedestal(const float ped)
{
  return _pedestal = ped;
}

inline
float
FTWire::time(float t)
{
  return _time = t;
}

inline
float
FTWire::time(void) const
{
  return _time;
}

#ifdef FZISAN_DEBUG
inline
Gen_hepevt *
FTWire::hep(Gen_hepevt * src)
{
  return _hep = src;
}

inline
Gen_hepevt *
FTWire::hep(void) const
{
  return _hep;
}
#endif

inline
unsigned
FTWire::state(void) const
{
  return _state;
}

inline
unsigned
FTWire::state(const unsigned state)
{
  return _state = (_state&FTWireStateMask)|state;
}

inline
unsigned
FTWire::stateAND(const unsigned mask) const
{
  return _state&mask;
}

inline
void
FTWire::stateOR(const unsigned mask)
{
  _state|=mask;
}

inline
void
FTWire::stateXOR(const unsigned mask)
{
  _state^=mask;
}

inline
void
FTWire::stateORXOR(const unsigned mask)
{
  _state=(_state|mask)^mask;
}

inline
int
FTWire::z(const Lpav & la, double &  z) const{
  HepVector center = la.center();
  double rho = la.radius();
  double dx2 = center(1) - _x;
  double dy2 = center(2) - _y;
  double par1 = _dx*_dx + _dy*_dy;
  double par2 = (_dx*dx2 + _dy*dy2)/par1;
  double par3 = _dx*dy2 - _dy*dx2;
  double par4 = rho*rho*par1 -  par3*par3;
  if (par4<0.) return 0;
  par4 = std::sqrt(par4)/par1;
  double delta = par2 + par4;
  //double delta = 0.5;
  if (delta>=0. && delta<1.){
    //z = _layer.zb()+delta*(_layer.zf()-_layer.zb());
    z = _layer.zf()+delta*(_layer.zb()-_layer.zf());
    return 1;
  }else{
    delta = par2 - par4;
    if (delta>=0. && delta<1.){
      //z = _layer.zb()+delta*(_layer.zf()-_layer.zb());
      z = _layer.zf()+delta*(_layer.zb()-_layer.zf());
      return 1;
    }
  }
  return 0;
}

inline
void
FTWire::chk_left_and_right(void){
  if (((**(_neighbor+2))._state&FTWireHit) &&
      ((**(_neighbor+3))._state&FTWireHit)){
    _state |= FTWireHitInvalid;
    (**(_neighbor+2))._state |= FTWireHitInvalid;
    (**(_neighbor+3))._state |= FTWireHitInvalid;
  }
}

inline
const FTWire *
FTWire::left(void) const{
  const FTWire * tmp = this;
  if (!_localId) tmp += _layer.NWire();
  return --tmp;
}

inline
const FTWire *
FTWire::right(void) const{
  const FTWire * tmp = this;
  if (_localId == (_layer.NWire()-1)) {
    tmp -= _layer.NWire();
  }
  return ++tmp;
}

// set wireId
inline void FTWire::wireId(int wireID){
  _wireId = wireID;
}

// get wireId
inline int FTWire::getWireId(void){
  return _wireId;
}

// ADC-related extractor and modifier
// by wangdy
inline float FTWire::getAdc(void) const{
  return m_adc;
}

inline void FTWire::setAdc(float adc) {
   m_adc = adc;
}

//chi2 by fit
inline float FTWire::getChi2(void) const{
  return m_chi2;
}

inline void FTWire::setChi2(float chi2) {
   m_chi2 = chi2;
}

#endif

#undef inline

#endif /* FTWire_FLAG_ */
