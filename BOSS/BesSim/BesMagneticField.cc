//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: BesMagneticField.cc,v 1.9 2015/03/17 05:50:57 sunss Exp $
// GEANT4 tag $Name: BesSim-00-01-24 $
//
//   User Field setup class implementation.
//

#include "BesMagneticField.hh"
#include "BesMagneticFieldMessenger.hh"

#include "G4MagneticField.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4PropagatorInField.hh"

#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"
#include "Randomize.hh"
#include "CLHEP/Random/RanecuEngine.h"
#include "CLHEP/Random/RandGauss.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
//#include "MagneticField/MagneticFieldSvc.h"

#include "ReadBoostRoot.hh"
#include <fstream>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

//////////////////////////////////////////////////////////////////////////
//
//  Magnetic field

BesMagneticField::BesMagneticField()
  : fChordFinder(0), fStepper(0),m_pIMF(0)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MagneticFieldSvc",m_pIMF);
  if(sc!=StatusCode::SUCCESS) {
    G4cout<< "Unable to open Magnetic field service"<<G4endl;
  }
  InitialiseAll();
}

BesMagneticField::~BesMagneticField()
{
  if(fFieldMessenger) delete fFieldMessenger;
  if(fChordFinder)   delete fChordFinder;
  if(fEquation)      delete fEquation;
  if(fStepper)       delete fStepper;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BesMagneticField::GetFieldValue( const double Point[3], double *Bfield) const
{
	double x=Point[0];
	double y=Point[1];
	double z=Point[2];

	HepPoint3D r(x,y,z);
	HepVector3D b;

	if(ReadBoostRoot::GetField()==2)
	  m_pIMF->fieldVector(r,b);
	else
	  m_pIMF->uniFieldVector(r,b);

	Bfield[0]=b.x();
	Bfield[1]=b.y();
	Bfield[2]=b.z();
        
//caogf debug	
//	ofstream haha("field_out.dat",ios_base::app);
//	haha<<x/mm<<" "<<y/mm<<" "<<z/mm<<" "<<Bfield[0]/tesla<<" "<<Bfield[1]/tesla<<" "<<Bfield[2]/tesla<<G4endl;
//	haha.close();
} 

////////////////////////////////////////////////////////////// 
//Initialise

void BesMagneticField::InitialiseAll()
{
    
  fFieldMessenger=new BesMagneticFieldMessenger(this); 
  fEquation = new G4Mag_UsualEqRhs(this); 
 
  fMinStep     = 0.01*mm ; // minimal step of 1 mm is default

  fStepperType =4;      // ClassicalRK4 is default stepper
  fFieldManager = G4TransportationManager::GetTransportationManager()
                                         ->GetFieldManager();
  G4cout<<"before CreateStepperAndChordFinder"<<G4endl;
  CreateStepperAndChordFinder();
}

////////////////////////////////////////////////////////////////////////////////
// Update field
//

void BesMagneticField::CreateStepperAndChordFinder()
{
  SetStepper();
  G4cout<<"The minimal step is equal to "<<fMinStep/mm<<" mm"<<G4endl ;

  fFieldManager->SetDetectorField(this );

  if(fChordFinder) delete fChordFinder;

   fChordFinder = new G4ChordFinder(this , fMinStep,fStepper);
 
   fChordFinder->SetDeltaChord(0.25*mm);
   fFieldManager->SetChordFinder( fChordFinder );
   fFieldManager->SetDeltaOneStep(1.0e-2*mm);
   fFieldManager->SetDeltaIntersection(1.0e-3*mm);
   fFieldManager->SetMinimumEpsilonStep(5.0e-5);
   fFieldManager->SetMaximumEpsilonStep(1.0e-3);

   G4PropagatorInField* fieldPropagator
	         = G4TransportationManager::GetTransportationManager()
		      ->GetPropagatorInField();
   G4cout<<"LargestAcceptableStep is "<<fieldPropagator->GetLargestAcceptableStep()/m<<G4endl; 
//read some values
    G4cout<<"field has created"<<G4endl;
    G4cout<<"fDelta_One_Step_Value is "<<fFieldManager->GetDeltaOneStep()<<G4endl;
    G4cout<<"fDelta_Intersection_Val is "<<fFieldManager->GetDeltaIntersection()<<G4endl;
    G4cout<<"fEpsilonMin is "<<fFieldManager->GetMinimumEpsilonStep()<<G4endl;
    G4cout<<"fEpsilonMax is "<< fFieldManager->GetMaximumEpsilonStep()<<G4endl;
    return;
}

/////////////////////////////////////////////////////////////////////////////
//
// Set stepper according to the stepper type
//

void BesMagneticField::SetStepper()
{
  if(fStepper) delete fStepper;

  switch ( fStepperType ) 
  {
    case 0:  
      fStepper = new G4ExplicitEuler( fEquation ); 
      G4cout<<"G4ExplicitEuler is called"<<G4endl;     
      break;
    case 1:  
      fStepper = new G4ImplicitEuler( fEquation );      
      G4cout<<"G4ImplicitEuler is called"<<G4endl;     
      break;
    case 2:  
      fStepper = new G4SimpleRunge( fEquation );        
      G4cout<<"G4SimpleRunge is called"<<G4endl;     
      break;
    case 3:  
      fStepper = new G4SimpleHeum( fEquation );         
      G4cout<<"G4SimpleHeum is called"<<G4endl;     
      break;
    case 4:  
      fStepper = new G4ClassicalRK4( fEquation );       
      G4cout<<"G4ClassicalRK4 (default) is called"<<G4endl;     
      break;
    case 5:  
      fStepper = new G4HelixExplicitEuler( fEquation ); 
      G4cout<<"G4HelixExplicitEuler is called"<<G4endl;     
      break;
    case 6:  
      fStepper = new G4HelixImplicitEuler( fEquation ); 
      G4cout<<"G4HelixImplicitEuler is called"<<G4endl;     
      break;
    case 7:  
      fStepper = new G4HelixSimpleRunge( fEquation );   
      G4cout<<"G4HelixSimpleRunge is called"<<G4endl;     
      break;
    case 8:  
      fStepper = new G4CashKarpRKF45( fEquation );      
      G4cout<<"G4CashKarpRKF45 is called"<<G4endl;     
      break;
    case 9:  
      fStepper = new G4RKG3_Stepper( fEquation );       
      G4cout<<"G4RKG3_Stepper is called"<<G4endl;     
      break;
    default: fStepper = 0;
  }
  return; 
}

/////////////////////////////////////////////////////////////////////////////
void BesMagneticField::SetDeltaOneStep(double newvalue)
{
	fFieldManager = G4TransportationManager::GetTransportationManager()
		                                         ->GetFieldManager();
	fFieldManager->SetDeltaOneStep(newvalue);
}
void BesMagneticField::SetDeltaIntersection(double newvalue)
{
	fFieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
	fFieldManager->SetDeltaIntersection(newvalue);
}
void BesMagneticField::SetMinimumEpsilonStep(double newvalue)
{
	fFieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
	fFieldManager->SetMinimumEpsilonStep(newvalue);
}
void BesMagneticField::SetMaximumEpsilonStep(double newvalue)
{
	fFieldManager =G4TransportationManager::GetTransportationManager()->GetFieldManager();
	fFieldManager->SetMaximumEpsilonStep(newvalue);
}
	
