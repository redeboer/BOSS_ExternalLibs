#ifndef BES_EMC_HEP_POLYHEDRON_HH
#define BES_EMC_HEP_POLYHEDRON_HH

#include "HepPolyhedron.h"

class HepPolyhedronIrregBox : public HepPolyhedron
{
  public:
    HepPolyhedronIrregBox
    (
        const HepGeom::Point3D<double> & P1, const HepGeom::Point3D<double> & P2,
        const HepGeom::Point3D<double> & P3, const HepGeom::Point3D<double> & P4,
        
        const HepGeom::Point3D<double> & P5, const HepGeom::Point3D<double> & P6,
        const HepGeom::Point3D<double> & P7, const HepGeom::Point3D<double> & P8
    );
    virtual ~HepPolyhedronIrregBox(void);
};                      

class HepPolyhedronWedge : public HepPolyhedron
{
  public:
    HepPolyhedronWedge
    (
        const HepGeom::Point3D<double> & P1, const HepGeom::Point3D<double> & P2,
        const HepGeom::Point3D<double> & P3, const HepGeom::Point3D<double> & P4,
        
        const HepGeom::Point3D<double> & P5, const HepGeom::Point3D<double> & P6
        );
    virtual ~HepPolyhedronWedge(void);
};

#endif /*BES_EMC_HEP_POLYHEDRON_HH*/
