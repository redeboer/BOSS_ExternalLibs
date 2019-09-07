#include "MdcGeom/MdcLayer.h"
#include "MdcGeom/MdcDetector.h"
#include <iomanip>
#include <iostream>
using namespace std;

    // Constructor.

MdcLayer::MdcLayer(int lay, int nwir, MdcSWire** wires, const MdcDetector &theDet)
  : _mdcDet(theDet), _exist(1),  _layer(lay), _nwires(nwir),
    _wires(wires)
{
  if (_nwires == 0) _exist = 0;
  // set pointer to layer for wires
  for (int wire=0; wire<_nwires; wire++) {
    getWire(wire)->setLayerPtr(this);
//    _cells.push_back(buildCell(wire));
  }

  // get nominal values from wire # 0
  _rend = wires[0]->rEnd();
  _rmid = wires[0]->rMid();
  _stdip = _rend - _rmid;
  // in the local (DC) system frame
  _zend = wires[0]->zEndDC();
  _stereo = wires[0]->stereo();
  // half twist angle
  _delphi = wires[0]->dPhiz();
  // stereo view
  if((_layer>=0 && _layer<=7)||(_layer>=20&&_layer<=35)){
    _view = (int)(_stereo/(fabs(_stereo)));
  } else {
    _view = 0;
  }
  _phiOffset = wires[0]->phiE();
}



void 
MdcLayer::print( std::ostream& o ) const
{
  o<< setw(3)<<layNum()<<
    setw(3)<<view()<<
    setw(4)<<nWires()<<
    setw(3)<<superLayer()<<
    setw(3)<<subLayer()<<
    setw(10)<<stDip()<<
    setw(10)<<zEnd()<<
    setw(10)<<rMid()<<
    setw(10)<<rEnd()<<
    setw(10)<<rIn()<<
    setw(10)<<rOut()<<
    setw(10)<<stereo()<<
    setw(10)<<dPhiz()<<
    setw(7)<<zLength()<<
    setw(10)<<sag()<<
    setw(10)<<dPhi()<<
    setw(10)<<phiOffset()<<
    setw(10)<<phiEPOffset()<<
    setw(10)<<dPhizDC(0.)<<
    setw(10)<<radiusDC(0.)<<
    setw(10)<<phiWireDC(0,0.)<<
    setw(10)<<phiWire(0)<<
    setw(10)<<xWire(0)<<
    setw(10)<<yWire(0)<<
    setw(10)<<cellWidth()<<
    setw(10)<<cellWidth(0.)<<
    std::endl;
}


double 
MdcLayer::phiWireDC(int cell, double z) const 
{
  if ( cell >= 0 && cell < nWires() ) {
    return getWire(cell)->phiDC(z);
  } else {
    std::cout << "phiWireDC: wrong cell # " << cell << "\n"
		  << "  number of cells in this layer is " << nWires() 
		  << std::endl;
    return 0.;
  }
}

double 
MdcLayer::phiWire(int cell) const 
{
  // in the middle of the chamber
  if ( cell >= 0 && cell < nWires() ) {
    return getWire(cell)->phi();
  } else {
    std::cout  << "phiWire: wrong cell # " << cell << "\n"
		  << "  number of cells in this layer is " << nWires() 
		  << std::endl;
    return 0.;
  }
}

double 
MdcLayer::xWire(int cell) const 
{
  // in the middle of the chamber
  if ( cell >= 0 && cell < nWires() ) {
    return getWire(cell)->xMid();
  } else {
    std::cout << "xWire: wrong cell # " << cell << "\n"
		  << "  number of cells in this layer is " << nWires() 
		  << std::endl;
    return 0.;
  }
}

double 
MdcLayer::yWire(int cell) const 
{
  // in the middle of the chamber 
  if ( cell >= 0 && cell < nWires() ) {
    return getWire(cell)->yMid(); 
  } else {
    std::cout << "YWire: wrong cell # " << cell << "\n"
		  << "  number of cells in this layer is " << nWires() 
		  << std::endl;
    return 0.;
  }
}




const Trajectory*
MdcLayer::makeHitTrajInGlobalCoords(int wire, double /*z*/) const
{
  if ( wire >= 0 && wire < nWires() ) {
    return getWire(wire)->getTraj();
  } else {
    std::cout << "Error In makeHitTrajInGlobalCoords: wrong cell # "<< wire << "\n"
                 <<"  number of cells in this layer is " << nWires() << std::endl;
    return getWire(0)->getTraj();
  }
}
