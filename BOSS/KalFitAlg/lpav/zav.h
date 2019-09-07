#ifndef _ZAV_H
#define _ZAV_H
//#include "mscl.h"
#include <iostream>

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
//#include "zhit.h"

using namespace CLHEP;

class zav {
public:
  zav();
  zav(int) {} // dummy for one of the chain constructor
  void set(const zav*);
  double chisq() const { return _chisq; }
  void add(double , double, double);
  //void add(const zhit&);
  double calculate();
  double a() const { return _a; }
  double b() const { return _b; }
  double z(double s) const { return _a * s + _b; }
  double d(double s, double z) const { return z - _a * s - _b; }
  int nc() const { return _nc; }
  void clear(void);
  inline friend std::ostream&operator<<(std::ostream&, const zav&);
  //HepSymMatrix cov() const;
  //friend class chain;
private:
  double _a;
  double _b;
  double _w;
  double _sav;
  double _ssav;
  double _zav;
  double _szav;
  double _zzav;
  double _chisq;
  double _sig_inv;
  double _c11;
  double _c21;
  double _c22;
  int _nc;
};

inline zav::zav() {
  _a = _b = _w = _sav = _ssav = _zav = _szav = _zzav = 0;
  _chisq = -1;
  _c22 = _c21 = _c11 = _sig_inv = 0;
  _nc = 0;
}

inline void zav::clear(void){
  _w = _sav = _ssav = _zav = _szav = _zzav = 0;
  _chisq = -1;
  _c22 = _c21 = _c11 = _sig_inv = 0;
  _nc = 0;
}
  
inline void zav::set(const zav *c) {
  if(c) {
    _w = c->_w;
    _sav = c->_sav;
    _ssav = c->_ssav;
    _zav = c->_zav;
    _szav = c->_szav;
    _zzav = c->_zzav;
    _sig_inv = c->_sig_inv;
    _c11 = c->_c11;
    _c21 = c->_c21;
    _c22 = c->_c22;
    _nc = c->_nc;
  } else {
    _w = _sav = _ssav = _zav = _szav = _zzav =
      _sig_inv = _c11 = _c21 = _c22 = 0;
    _nc = 0;
  }
  _a = _b = 0;
  _chisq = -1;
}

inline void zav::add( double s, double z, double w) {
  _w += w;
  double sw = s * w;
  _sav += sw;
  _ssav += sw * s;
  double zw = z * w;
  _zav += zw;
  _szav += zw * s;
  _zzav += zw * z;
  _chisq = -1;
  _nc++;
}
/*
inline void zav::add(const zhit & h) {
  double errsq = h._z.error();
  if (errsq==0) return;
  errsq *= errsq;
  double w = 1/errsq;
  _w += w;
  double s = (double)h._s;
  double sw = s * w;
  _sav += sw;
  _ssav += sw * s;
  double z = (double)h._z;
  double zw = z * w;
  _zav += zw;
  _szav += zw * s;
  _zzav += zw * z;
  _chisq = -1;
  _nc++;
}
*/
inline double zav::calculate() {
  double sig = _ssav * _w - _sav * _sav;
  if (sig!=0) {
    _sig_inv = 1/sig;
    _a = ( _szav * _w - _sav * _zav ) * _sig_inv;
    _b = ( _ssav * _zav - _sav * _szav ) * _sig_inv;
    _chisq = _zzav - 2 * _a * _szav - 2 * _b * _zav + _a * _a * _ssav
      + _b * _b * _w + 2 * _a * _b * _sav;
    _c11 = _w * _sig_inv;
    _c21 = - _sav * _sig_inv;
    _c22 = _ssav * _sig_inv;
  } else {
    _sig_inv = 0;
    _c11 = _c21 = _c22 = 0;
    _chisq = -1;
  }
  if(_nc==2) {
    _chisq = 0;
  }
  return _chisq;
}

inline std::ostream&operator<<(std::ostream&o,const zav&z) {
  o << " zav::w=" << z._w << " sav=" << z._sav << " zav=" << z._zav
    << " nc=" << z._nc << " chisq=" << z._chisq << " a=" << z._a
    << " b=" << z._b << " c11=" << z._c11 << " c21=" << z._c21
    << " c22=" << z._c22 << " sig_inv=" << z._sig_inv << std::endl;
  return o;
}

/*inline HepSymMatrix zav::cov() const
{
#ifndef __GNUG__
  HepSymMatrix vret(2);
#endif
//   vret(1,1) = _ssav;
//   vret(2,1) = _sav;
//   vret(2,2) = _w;
//   int iret = invert(vret);
//   if (iret) cerr << "zav::cov(): cannot invert the matrix" << endl;
  vret(1,1) = _ssav;
  vret(2,1) = _sav;
  vret(2,2) = _w;
  return vret;
}*/
#endif /* ZAV */

