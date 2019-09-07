#ifndef BES_EMC_POLYHEDRON_HH
#define BES_EMC_POLYHEDRON_HH

#include "G4Polyhedron.hh"
#include "G4Point3D.hh"

class G4PolyhedronIrregBox : public G4Polyhedron
{
  public:
    G4PolyhedronIrregBox
    (
        const G4Point3D & P1, const G4Point3D & P2,
        const G4Point3D & P3, const G4Point3D & P4,
        
        const G4Point3D & P5, const G4Point3D & P6,
        const G4Point3D & P7, const G4Point3D & P8
    );
    virtual ~G4PolyhedronIrregBox (void);
};

class G4PolyhedronWedge : public G4Polyhedron
{
  public:
    G4PolyhedronWedge
    (
        const G4Point3D & P1, const G4Point3D & P2,
        const G4Point3D & P3, const G4Point3D & P4,
        //   P3 above P5,  P4 above P6
        const G4Point3D & P5, const G4Point3D & P6
    );
    virtual ~G4PolyhedronWedge (void);
};

#endif /* BES_EMC_POLYHEDRON_HH */
