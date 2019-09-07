//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcSagTraj.cxx,v 1.3 2009/12/17 00:38:40 zhangy Exp $
//
// Description:
//	Class MdcSagTraj
//      
//      
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	R. Stroili		originator
//	
//
// Copyright Information:
//	Copyright (C) 1998	INFN & Padova University
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "MdcGeom/MdcSagTraj.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <assert.h>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "MdcGeom/Trajectory.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------
MdcSagTraj::MdcSagTraj( const double sag, const HepPoint3D& start, 
			const HepPoint3D& stop )
  : TrkGeomTraj(0.0,start.distance(stop)), _sag(sag), _start(start), _stop(stop)
{
  //_direction = stop - start;
  _direction = ((CLHEP::Hep3Vector) stop) - ((CLHEP::Hep3Vector) start);
  _length = _direction.mag();
  assert( _length != 0 );

  _direction.setMag(1.0);
  _a = _sag*4./(_length*_length);
  _b = -_a*_length;

}

MdcSagTraj::MdcSagTraj( const MdcSagTraj& other )
  : TrkGeomTraj(0.0,other._start.distance(other._stop)), 
    _sag(other._sag), _a(other._a), _b(other._b), _length(other._length),
    _start(other._start), _stop(other._stop), 
    _direction(other._direction)
{}

MdcSagTraj::~MdcSagTraj(){;}
//-------------
// Methods   --
//-------------
MdcSagTraj*
MdcSagTraj::clone() const 
{
  return new MdcSagTraj(*this);
}
    
//-------------
// Operators --
//-------------
//-----------------------------------------------------------------------------
MdcSagTraj& 
MdcSagTraj::operator = (const MdcSagTraj& other) 
{
  if(&other != this){
    for(int iend=0;iend<2;iend++)
      flightrange[iend] = other.flightrange[iend];
    _start = other._start;
    _stop = other._stop;
    _sag = other._sag;
    _a = other._a;
    _b = other._b;
    _length = other._length;
    _direction = other._direction;
  }
  return *this;
}

Hep3Vector 
MdcSagTraj::deviation( double flightlen ) const 
{
  // only correction in y
//   Hep3Vector deviation(0., (_a*flightlen+_b)*flightlen, 0.);
//   Hep3Vector deviaH(0.,cosh((flightlen-_length/2.)/_a_H)+_b_H,0.);
//   return deviation;
  return Hep3Vector(0., (_a*flightlen+_b)*flightlen, 0.);
}

HepPoint3D
MdcSagTraj::position(double flightlen) const 
{
  static HepPoint3D tmppos;
  tmppos = _start;
//CHANGE   tmppos += _direction*flightlen;
  tmppos += (HepPoint3D) _direction*flightlen;//yzhang TEMP 
  tmppos.setY(tmppos.y()+(_a*flightlen+_b)*flightlen);
  return tmppos;
}

Hep3Vector 
MdcSagTraj::direction( double flightlen ) const {
  if ( flightlen <= 0. ) return _direction;
//    Hep3Vector dir =  _direction*flightlen + delDirect(flightlen);
  static Hep3Vector tmpdir;
  tmpdir =  _direction*flightlen;
//   register double newy = tmpdir.y() + 2.*_a*flightlen+_b;
//   tmpdir.setY(newy);
  tmpdir.setY(tmpdir.y() + 2.*_a*flightlen+_b);
//   tmpdir += delDirect(flightlen);
  tmpdir.setMag(1.0);
  return tmpdir;
}
  
Hep3Vector 
MdcSagTraj::delDirect( double /*flightlen*/ ) const 
{
  return Hep3Vector(0., 2.*_a, 0.);
}

void  
MdcSagTraj::getInfo(double flightlen, HepPoint3D& pos, Hep3Vector& dir) const 
{
// std::cout<<"Dyz*"<<"MdcSagTraj01 "<<std::endl;//yzhang DEBUG
  // Written using +=, etc to avoid temporaries
  pos = _start;
//CHANGE   pos += _direction*flightlen;
  pos += (HepPoint3D) _direction*flightlen;
  
  dir = _direction;
  if ( flightlen > 0. ) {
    pos.setY(pos.y() + deltaY(flightlen));
    dir.setY(dir.y() + 2.*_a*flightlen+_b);
    dir.setMag(1.0);
  }
}

void  
MdcSagTraj::getInfo( double flightlen, HepPoint3D& pos, Hep3Vector& dir, 
		     Hep3Vector& delDir ) const 
{
//	 std::cout<<"Dyz*"<<"MdcSagTraj011 "<<std::endl;//yzhang DEBUG
	 
  pos = _start;
//CHANGE   pos += _direction*flightlen ;
  pos += (HepPoint3D) _direction*flightlen ;
  
  pos.setY( pos.y() + (_a*flightlen+_b)*flightlen) ;

  dir = _direction;
  dir.setY( dir.y() + 2*_a*flightlen+_b) ;
  // Note: `dir' is on purpose not normalized (WDH, Jan 2003)

  delDir.setX(0.);
  delDir.setY(2.*_a);
  delDir.setZ(0.);
}

double
MdcSagTraj::curvature( double /*f*/ ) const
{
  return _sag;
}

double 
MdcSagTraj::distTo1stError(double flightlen, double tol, int pathDir) const 
{
  double dtmp = pathDir*2.*_a*flightlen+_b;

  return dtmp==0. ? 9999.e4 : fabs(tol/dtmp);
}
  
double 
MdcSagTraj::distTo2ndError(double /*s*/, double /*tol*/, int /*pathDir*/) const
{ 
  return 999.e4 ;
  //return _a==0. ? 999.e4 : tol/(2.*_a); 
}

// Support Visitor pattern (see TrkGeomTraj.h)
void 
MdcSagTraj::accept(TrkGeomTrajVisitor& /*visitor*/) const 
{
  std::cout<<"ErrMsg(error)"<<"accept visitor NOT implemented yet"<<std::endl;
}

