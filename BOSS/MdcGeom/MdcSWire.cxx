#include "MdcGeom/MdcSWire.h"
#include "MdcGeom/BesAngle.h"
#include "MdcGeom/MdcLayer.h"
#include <iomanip>
#include <iostream>
using namespace std;


MdcSWire::MdcSWire(HepPoint3D &eastP, HepPoint3D &westP, double sag, int id, int cell)
: _traj(sag,eastP,westP), _east(eastP), _west(westP), _sag(sag), _id(id), _cell(cell)
{
  _rend = sqrt(xEast()*xEast() + yEast()*yEast());
  _phiend    = _east.phi();
  BesAngle westPphi(westP.phi());
  BesAngle eastPphi(eastP.phi());
  _twist = ( westPphi - eastPphi)* 0.5;
}

Hep3Vector
MdcSWire::yAxis( double z )
{
  Hep3Vector vec( xWireDC(z), yWireDC(z), z);
  return vec.unit();
}

void MdcSWire::print(std::ostream &o) const 
{
  o<< setw(6)<<Id()<<
    setw(10)<<xEast()<<
    setw(10)<<yEast()<<
    setw(10)<<zEast()<<
    setw(10)<<xWest()<<
    setw(10)<<yWest()<<
    setw(10)<<zWest()<<
    setw(10)<<xMid()<<
    setw(10)<<yMid()<<
    setw(10)<<rEnd()<<
    setw(10)<<getSag()<<
    setw(10)<<cell()<<
    setw(10)<<zLength()<<
    setw(10)<<dPhiz()<<
    setw(10)<<zEndDC()<<
    setw(10)<<rMid()<<
    setw(10)<<phiE()<<
    setw(10)<<phi()<<
    setw(10)<<stereo()<<
    setw(10)<<stDip()<<
    setw(10)<<radiusDC(0.)<<
    setw(10)<<phiDC(0.)<<
    setw(10)<<xWireDC(0.)<<
    setw(10)<<yWireDC(0.)<<
    setw(10)<<dPhizDC(0.)<<
    setw(10)<<getTraj()->sag()<<//
    setw(10)<<getTraj()->position(0.)<<
    setw(10)<<getTraj()->direction(0.)<<
    setw(10)<<getTraj()->curvature()<<//
    setw(10)<<getTraj()->rawDirection()<<
    std::endl;
}

void MdcSWire::printInfo(std::ostream &o) const 
{
  o << "east " << getEastPoint()->x() << " " << getEastPoint()->y() << " "
    << getEastPoint()->z() <<"\n"
    << "west " << getWestPoint()->x() <<" " << getWestPoint()->y()
    << " " << getWestPoint()->z() << "\n"
    << "sagitta: " << getSag()
    << " z length: " << zLength()
    << " twist: " << dPhiz()
    << " stereo: " << stereo() <<"\n"
    << "x-y mid: " << xMid() << " - " << yMid() << std::endl;

}

std::ostream&  operator << (std::ostream& o, const MdcSWire& w) 
{ 
  w.print(o);
  return o;
}

