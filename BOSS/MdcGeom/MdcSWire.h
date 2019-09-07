// $Id: MdcSWire.h,v 1.6 2011/12/05 00:46:23 zhangy Exp $ // -*-c++-*-
// header file for a class called "MdcSWire"
#ifndef MDCSWIRE_H
#define MDCSWIRE_H

#include "MdcGeom/MdcSagTraj.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <iostream>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif


using CLHEP::Hep3Vector;

class MdcLayer;

class MdcSWire {

  public:
    // Constructor.
    MdcSWire(HepPoint3D &eastP, HepPoint3D &westP, double sag = 0, int id =0, int cell=0);
    // Destructor
    ~MdcSWire(){};

  public: // Extractors

    // Selectors (const)
    double getSag(void) const { return _sag; }
    const HepPoint3D*  getEastPoint(void) const { return &_east; }
    const HepPoint3D*  getWestPoint(void) const { return &_west; }
    const MdcSagTraj*  getTraj(void)      const { return &_traj; }    
    double xEast(void)  const { return _east.x(); }
    double yEast(void)  const { return _east.y(); }
    double zEast(void)  const { return _east.z(); }
    double xWest(void)  const { return _west.x(); } 
    double yWest(void)  const { return _west.y(); } 
    double zWest(void)  const { return _west.z(); }    
    double xMid (void)  const { return rMid() * cos(phi()); }
    double yMid (void)  const { return rMid() * sin(phi()); }
    double rEnd (void)  const { return _rend; }
    const MdcLayer* layer(void)        const { return _lay; }
    int Id(void) const { return _id; }//wire number in Mdc
    int cell(void) const { return _cell; } //local cell number in layer
    double zLength(void) const { return getEastPoint()->z()- getWestPoint()->z(); }
    double dPhiz (void) const { return _twist; }
    double zEndDC(void) const { return 0.5*zLength(); }
    double rMid  (void) const { return rEnd() * cos( dPhiz() ); }
    double phiE  (void) const { return _phiend; }
    double phi   (void) const { return _phiend + _twist; }
    double stereo(void)  const 
    { return 2.*rEnd()*sin(dPhiz())/zLength(); }
    double stDip (void) const { return rEnd() - rMid(); }
    // in the local MDC reference
    double radiusDC(double z) const { return rEnd() - stDip() * (1.-  
	z*z/(zEndDC()*zEndDC())); } 
    //double phiDC   (double z) const { return phi() + dPhizDC(z); }//yzhang fix 2011-12-02 
    double phiDC   (double z) const { return phi() - dPhizDC(z); }
    double xWireDC (double z) const { return radiusDC(z)*cos(phiDC(z)); }
    double yWireDC (double z) const { return radiusDC(z)*sin(phiDC(z)); }
    double dPhizDC (double z) const { return atan2( z*stereo(),rMid() ); }

    // direction axis defining the local coordinate system of the sense
    // wire (eventually needed by calibration)
    Hep3Vector yAxis( double z=0. );
    const Hep3Vector& zAxis( void ) const { return _traj.rawDirection(); }

    void print(std::ostream &o) const;
    void printInfo(std::ostream &o) const;

  protected:
    void setLayerPtr(const MdcLayer* lay) { _lay =lay; }

  private:

    friend class MdcLayer;
    MdcSagTraj      _traj;  // wire trajectory
    HepPoint3D      _east;  // wire position at backward endplate (mm)
    HepPoint3D      _west;  // wire position at west endplate (mm)
    const MdcLayer* _lay;  // pointer to layer to which the wire belongs
    double          _rend;  // radius at backward endplate(mm)
    double          _phiend;  // phi angle at backward endplate
    double          _twist;  // twist angle between mid and bakcward chamber=a/2
    double          _sag;  // wire sagitta
    int             _id;  // sense wire identifier in mdc
    int             _cell;  // sense wire (cell) identifier (local)
};

#endif /* MdcSWire_CLASS */

