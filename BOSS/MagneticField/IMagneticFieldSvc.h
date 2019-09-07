// $Header: /bes/bes/BossCvs/MagneticField/MagneticField/IMagneticFieldSvc.h,v 1.4 2008/09/18 00:46:38 caogf Exp $
#ifndef GAUDIKERNEL_IMAGNETICFIELDSVC_H
#define GAUDIKERNEL_IMAGNETICFIELDSVC_H


// Include files
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"


// Forward declarations
template <class TYPE> class SvcFactory;
namespace HepGeom {
    template <class T>  class Vector3D;
    template <class T>  class Point3D;
}


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMagneticFieldSvc(80, 1 , 0); 


/** @class IMagneticFieldSvc IMagneticFieldSvc.h GaudiKernel/IMagneticFieldSvc.h

    The interface to the MagneticFieldSvc

    @author Iain Last
*/
class IMagneticFieldSvc : virtual public IService   {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMagneticFieldSvc; }

  // Get the magnetic field vector at a given point in space.
  // Input: Point3D - Point at which magnetic field vector is to be given.
  // Output: Vector3D - Magnectic field vector.
  // Return: StatusCode SUCCESS if calculation was performed.
  virtual StatusCode fieldVector( const HepGeom::Point3D<double>& xyz, HepGeom::Vector3D<double>& fvec ) const = 0;
  virtual StatusCode uniFieldVector( const HepGeom::Point3D<double>& xyz, HepGeom::Vector3D<double>& fvec ) const = 0;
  virtual double     getReferField()  = 0;
  virtual bool ifRealField() const = 0;

};

#endif  // GAUDIKERNEL_IMAGNETICFIELDSVC_H
