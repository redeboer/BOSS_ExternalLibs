#include "TrkExtAlg/ExtBesMagneticField.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"

#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "globals.hh"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif


using namespace CLHEP;

ExtBesMagneticField::ExtBesMagneticField()
{
	GetFieldManager()->SetDetectorField(this);
	GetFieldManager()->CreateChordFinder(this);
	//	GetFieldManager()->SetDeltaOneStep(0.5*mm);
	//	GetFieldManager()->SetDeltaIntersection(0.5*mm);
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	StatusCode sc = svcLocator->service("MagneticFieldSvc",m_pIMF);
	if(sc!=StatusCode::SUCCESS) {
		G4cout<< "Unable to open Magnetic field service"<<G4endl;
  }
}


ExtBesMagneticField::~ExtBesMagneticField(){}


void ExtBesMagneticField::GetFieldValue(const double Point[3],double *Bfield) const
{
        double x=Point[0];
	double y=Point[1];
	double z=Point[2];

	HepPoint3D r(x,y,z);
	HepVector3D b;

	if(m_pIMF->ifRealField() == true) 
	  m_pIMF->fieldVector(r,b);
	else
	  m_pIMF->uniFieldVector(r,b);

	Bfield[0]=b.x();
	Bfield[1]=b.y();
	Bfield[2]=b.z();
}

#include "G4TransportationManager.hh"

G4FieldManager* ExtBesMagneticField::GetFieldManager()
{
	return G4TransportationManager::GetTransportationManager()->GetFieldManager();
}

