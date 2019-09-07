//-----------------------------------------------------------------------------
// $Id: TMDCLayer.h,v 1.4 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMDCLayer.h
// Section  : Tracking MDC
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a wire layer.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMDCLayer_FLAG_
#define TMDCLayer_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"

class TMDCWire;
//struct geocdc_layer;
class MdcGeoLayer;

/// A class to represent a wire layer.
class TMDCLayer : public AList<TMDCWire> {

  public:
    /// Constructor.
    TMDCLayer(const MdcGeoLayer *);

    /// Destructor
    virtual ~TMDCLayer();

  public:// Selectors
    /// returns id.
    unsigned id(void) const;

    /// returns super layer id.
    unsigned superLayerId(void) const;

    /// returns local layer id in a super layer.
    unsigned localLayerId(void) const;

    /// returns id of axial or stereo id.
    unsigned axialStereoLayerId(void) const;

    /// returns \# of wires.
    unsigned nWires(void) const;

    /// returns a pointer to a wire. 'id' can be negative or 'id' can be greater than 'nWires()'.
    const TMDCWire * const wire(int id) const;

    /// returns true if this is an axial layer.
    bool axial(void) const;

    /// returns true if this is a stereo layer.
    bool stereo(void) const;

    /// returns offset.
    float offset(void) const;

    /// returns cell size.
    float cellSize(void) const;

    /// returns a pointer to GEOMDC_WIR.
    const MdcGeoLayer * geocdc(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Modifiers

  private:
    const MdcGeoLayer * _geo;

    unsigned _id;
//_type=0 axial; _type=1 stereo;
    unsigned _type;
    unsigned _superLayerId;
    unsigned _localLayerId;
    unsigned _axialStereoLayerId;

    float _offset;
    float _cellSize;
    unsigned _nWires;
};

//-----------------------------------------------------------------------------

#ifdef TMDCLayer_NO_INLINE
#define inline
#else
#undef inline
#define TMDCLayer_INLINE_DEFINE_HERE
#endif

#ifdef TMDCLayer_INLINE_DEFINE_HERE

inline
unsigned
TMDCLayer::id(void) const {
    return _id;
}

inline
unsigned
TMDCLayer::superLayerId(void) const {
    return _superLayerId;
}

inline
unsigned
TMDCLayer::localLayerId(void) const {
    return _localLayerId;
}

inline
const MdcGeoLayer *
TMDCLayer::geocdc(void) const {
    return _geo;
}

inline
float
TMDCLayer::offset(void) const {
    return _offset;
}

inline
unsigned
TMDCLayer::nWires(void) const {
    return _nWires;
}

inline
bool
TMDCLayer::axial(void) const {
// cout<<"zsl,axial:"<<_superLayerId<<endl;	
//    if (_superLayerId % 2) return false;
    if (_type==1) return false;   
    return true;
}

inline
bool
TMDCLayer::stereo(void) const {
//    cout<<"zsl,stereo:"<<_superLayerId<<endl;  
//    if (_superLayerId % 2) return true;
    if (_type==1) return true;     
    return false;
}

inline
unsigned
TMDCLayer::axialStereoLayerId(void) const {
    return _axialStereoLayerId;
}

inline
float
TMDCLayer::cellSize(void) const {
    return _cellSize;
}

#endif

#undef inline

#endif /* TMDCLayer_FLAG_ */

