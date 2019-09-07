//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You     Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_GEOMETRON_H
#define MUC_GEOMETRON_H

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Geometry/Point3D.h>
#include <CLHEP/Geometry/Plane3D.h>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Plane3D<double> HepPlane3D;
#endif

using namespace CLHEP;

/**
 * Class MucGeometron contains some geometry calculation for convenience.
 *
 * @author Zhengyun You \URL{mailto:youzy@pku.edu.cn}
 *
 */

class MucGeometron
{
 public:

  /// Constructor.
  MucGeometron();

  /// Destructor.
  ~MucGeometron();

  /// Get intersection of a line and a plane.
  bool GetIntersectionLinePlane(const HepPoint3D pLine,
				const Hep3Vector vectLine,
				const HepPlane3D plane,
				HepPoint3D& cross);

  bool GetIntersectionLinePlaneWithSigma(const HepPoint3D pLine,
      const Hep3Vector vectLine,
      const HepPoint3D pLineSigma,
      const Hep3Vector vectLineSigma,
      const HepPlane3D plane,
      HepPoint3D& cross,
      HepPoint3D& crossSigma);


  bool GetIntersectionQuadPlaneLocal(const int part,    //liangyt 2009.3.12
				const int orient,
                                const float a,              //y = a * x * x + b * x + c;
                                const float b,
                                const float c,
                                const HepPlane3D plane,
                                HepPoint3D& cross1,
                                HepPoint3D& cross2);


  bool GetIntersectionQuadPlane(const HepPoint3D pLine,    //liangyt 2007.4.9
				const float vy,
				const float y0,
				const float a,              //y = a * x * x + b * x + c;
				const float b,
				const float c,
				const HepPlane3D plane,    
				HepPoint3D& cross1,
				HepPoint3D& cross2);

  
 private:

};

#endif    /* MUC_GEOMETRON_H */
