// $Id: MdcLayer.h,v 1.2 2009/12/17 00:38:40 zhangy Exp $ // -*-c++-*-
// header file for a class called "MdcLayer"
#ifndef MDCLAYER_H
#define MDCLAYER_H

#include <CLHEP/Geometry/Point3D.h>
#include <iostream>
#include <vector>
#include "MdcGeom/MdcSWire.h"
#include "MdcGeom/Constants.h"

class MdcDetector;
class Trajectory;

class MdcLayer {

  public:
    // Constructor.
     MdcLayer(int lay, int nwir, MdcSWire** wires, const MdcDetector &theDet);

    // Destructor
    ~MdcLayer(){};

    void print( std::ostream& o ) const;
                              
  public: // Extractors
    bool exist(void)        const { return _exist; }
    int view(void)          const { return _view; }
    int layNum(void)        const { return _layer; }
    int nWires(void)        const { return _nwires; }
    int superLayer(void)  const { return 1+((layNum()-1)/4);}
    int subLayer(void)    const { return layNum()-4*(superLayer()-1);}
    double cellHeight(void) const { return _cellHeight; }
    double stDip(void)      const { return _stdip; }
    double zEnd(void)       const { return _zend; }
    double rMid(void)       const { return _rmid; }
    double rEnd(void)       const { return _rend; }
    double rIn(void)        const { return rMid() - 0.5 * cellHeight(); }//yzhang del
    double rOut(void)       const { return rMid() + 0.5 * cellHeight(); }//yzhang del
//    double rIn(void)        const { return rMid() - RCSiz1(); }//yzhang change FIXME
//    double rOut(void)       const { return rMid() + RCSiz2(); }//yzhang change FIXME
    double stereo(void)     const { return _stereo; }
    double dPhiz(void)      const { return _delphi; }
    double zLength(void)    const { return getWire(0)->zLength(); }
    double sag(void)        const { return getWire(0)->getSag(); }
    //  this function returns the phi offset in the middle of the chamber
    double phiOffset(void)    const { return phiWire(0); }
    //  this function returns the cell offset w.r.t rear end-plate
    double phiEPOffset(void)  const { return _phiOffset; }
    double dPhizDC(double z)  const { return phiWireDC(0, z); }
    double radiusDC(double z) const { return getWire(0)->radiusDC(z); }
    MdcSWire *getWire(int wire) const { return _wires[wire]; }
//    int view(void) const { return _view; }
//    TrkEnums::TrkViewInfo whatView() const {
//      return _view==0?TrkEnums::xyView:TrkEnums::bothView;
//    }
    double phiWireDC(int cell, double z) const;
    double phiWire(int cell) const;
    double xWire(int cell) const;
    double yWire(int cell) const;
    double dPhi(void) const { return Constants::twoPi/nWires(); }
    // return the width of the cell at mid chamber corrected by the
    // stereo angle
    double cellWidth(void) const { return dPhi()*rMid()*cos(stereo()); }
    // return the width of the cell at z along the chamber
    double cellWidth(double z) const { return dPhi()*radiusDC(z)*cos(stereo()); }
    
//    const MdcSuperLayer SLayer(void) const {return *_sLay;}
      
      

    const Trajectory* makeHitTrajInGlobalCoords(int wire, double z=0) const;

  public: // Modifiers

    void View(int v ) { _view = v; }
    void setCellHeight(double height) { _cellHeight = height; }
    
  private:

    const MdcDetector& _mdcDet;   // Reference to layer's owner (for whichCells)

    bool _exist;                  // this layer exist ? 
    int _layer;                   // layer number
    int _nwires;                  // number of wires
    MdcSWire **_wires;            // array of pointers to hits on wires    
//    const MdcSuperLayer* _sLay;   // pointer to SuperLayer
                                      
    double _stdip;                // change in radius from mid to end - nominal
                                  // value
    double _rend;                 // radius on rear end-plate
    double _rmid;                 // radius in the center of the chamber
    double _zend;                 // z position of end-plate (in the local reference system)
    double _stereo;               // stereo angle
    double _delphi;               // twist angle between mid and rear chamber
    double _phiOffset;            // offset of first cell
    double _cellHeight;           // cell height
    int  _view;                   // +1, 0, -1: U, axial, V

//  private:
//    friend class MdcDetector;
//    void updateInfo(std::vector<MdcSWire *>::const_iterator it);

};

#endif /* MdcLayer_CLASS */

