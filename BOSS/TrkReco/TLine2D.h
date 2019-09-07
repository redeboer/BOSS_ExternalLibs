//-----------------------------------------------------------------------------
// $Id: TLine2D.h,v 1.5 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TLine2D.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a line in 2D.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TLINE2D_FLAG_
#define TLINE2D_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
class TPoint2D;
template <class T> class CAList;

/// A class to represent a line in 2D.
class TLine2D {

  public:
    /// Constructors
    TLine2D();
    TLine2D(double slope, double yOffset);
    TLine2D(const AList<TPoint2D> &);

    /// Destructor
    virtual ~TLine2D();

  public:// Selectors
    double slope(void) const;
    double yOffset(void) const;
    double xOffset(void) const;

  public:// Modifiers
    double slope(double);
    double yOffset(double);

  public:// Fitting
    int fit(void);
    double det(void) const;

  public:// Geometry
    double distance(const TPoint2D &) const;

  public:// Point members
    void append(const TPoint2D &);
    void remove(const TPoint2D &);
    const CAList<TPoint2D> & list(void) const;

  private:
    double _slope;
    double _yOffset;
    double _det;
    mutable CAList<TPoint2D> * _list;
};

//-----------------------------------------------------------------------------

#ifdef TLINE2D_NO_INLINE
#define inline
#else
#undef inline
#define TLINE2D_INLINE_DEFINE_HERE
#endif
#ifdef TLINE2D_INLINE_DEFINE_HERE

inline
double
TLine2D::slope(void) const {
    return _slope;
}

inline
double
TLine2D::yOffset(void) const {
    return _yOffset;
}

inline
double
TLine2D::xOffset(void) const {
    return - _yOffset / _slope;
}

inline
double
TLine2D::slope(double a) {
    return _slope = a;
}

inline
double
TLine2D::yOffset(double a) {
    return _yOffset = a;
}

#endif
#undef inline

#endif /* TLINE2D_FLAG_ */
