//-----------------------------------------------------------fmt version 0.00--
// $Id: Range.h,v 1.2 2005/09/09 07:47:07 zangsl Exp $
//-----------------------------------------------------------------------------
// Header file for Multi-TDC data
//-----------------------------------------------------------------------------
// Filename : MultiTDC.h
// Section  : CDC TSF
// Owner    : Yoshi Iwasaki
// Email    : yiwaskai@kekvax.kek.jp
//-----------------------------------------------------------------------------

#ifndef Range_FLAG_
#define Range_FLAG_

//
//...To define band width...
//
/// to specify 1-dim region or range by two floats
class Range {

  public:
    /// Constructor
    Range();

    /// Copy constructor
    Range(const Range &);

    /// Constructor
    Range(float low, float high);

  public:// Selectors
    /// returns lower limit.
    virtual float low(void) const;

    /// returns higher limit.
    virtual float high(void) const;

    /// returns center.
    virtual float center(void) const;

    /// returns width.
    virtual float width(void) const;

  public:// Modifiers
    /// sets lower limit.
    virtual float low(float lowIn);

    /// sets higher limit.
    virtual float high(float highIn);

    /// sets range.
    virtual Range & set(float low, float high);

    /// sets range by center and width.
    virtual Range & setByCenter(float center, float width);

  public:// Operators
    /// Copy operator
    Range & operator = (const Range &);

    /// returns true if range is the same.
    bool operator == (const Range &) const;

    /// returns true if range is different.
    bool operator != (const Range &) const;

    /// returns true if two are overlaped each other.
    bool operator & (const Range &) const;

    /// returns true if given value is within a range.
    bool within(const float value) const;

    /// returns true if given value is within a range.
    bool within2(const float value) const;

    /// returns true if given Range is within(included in) a range.
    bool within(const Range &) const;

  public:// Common interfaces
    /// displays debug information.
    virtual int dump(void) const;

  private:
    /// Lower limit
    float _low;
    /// Higher limit
    float _high;
};

//-----------------------------------------------------------------------------

#ifdef TANA_NO_INLINE
#define inline
#else
#undef inline
#define Range_INLINE_DEFINE_HERE
#endif

#ifdef Range_INLINE_DEFINE_HERE

inline
float
Range::low(void) const {
    return _low;
}

inline
float
Range::low(float i) {
    if (i > _high) i = _high;
    return _low = i;
}

inline
float
Range::high(void) const {
    return _high;
}

inline
float
Range::high(float i) {
    if (i < _low) i = _low;
    return _high = i;
}

inline
float
Range::center(void) const {
    return (_low + _high) / 2.;
}

inline
float
Range::width(void) const {
    return (_high - _low);
}

inline
Range &
Range::set(float iLow, float iHigh) {
    if (iHigh > iLow) {
	_low = iLow;
	_high = iHigh;
    }
    else {
	_low = iHigh;
	_high = iLow;
    }
    return * this;
}

inline
Range &
Range::setByCenter(float center, float width) {
    _low = center - width;
    _high = center + width;
    return * this;
}

inline
Range &
Range::operator = (const Range & ib) {
    _low = ib.low();
    _high = ib.high();
    return * this;
}

inline
bool
Range::within(const float f) const{
    if (_low == -999. && _high == -999.) {
	return false;
    }
    if (_low == -999.) {
	if (f <= _high) return true;
	return false;
    }
    if (_high == -999.) {
	if (f >= _low) return true;
    }
    if (f >= _low && f <= _high) return true;
    return false;
}

inline
bool
Range::within2(const float f) const{
    if (_low == -999. && _high == -999.) {
	return true;
    }
    if (_low == -999.) {
	if (f <= _high) return true;
	return false;
    }
    if (_high == -999.) {
	if (f >= _low) return true;
    }
    if (f >= _low && f <= _high) return true;
    return false;
}

inline
bool
Range::operator != (const Range & a) const {
    if ((* this) == a) return false;
    return true;
}

inline
bool
Range::operator & (const Range & a) const {
    if (within(a.low())) return true;
    if (within(a.high())) return true;
    if (a.within(low())) return true;
    if (a.within(high())) return true;
    return false;
}

inline
bool
Range::within(const Range & a) const {
    if (within(a.low()) && within(a.high())) return true;
    return false;
}

#endif

#undef inline

#endif /* Range_FLAG_ */

