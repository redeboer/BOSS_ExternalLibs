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
// $Id: BesMagneticField.hh,v 1.5 2015/03/17 05:50:57 sunss Exp $
// GEANT4 tag $Name: BesSim-00-01-24 $
//
//
//    A class for control of the Magnetic Field of the detector.
//  The field is assumed to be uniform.
// 
//  $ Id:  $

// Should this be a:
//    i) messenger
//   ii) user class that creates the field       ? 
//  iii) simply a derived class of Uniform field ?  <== I have chosen this now.
//   iv) a field manager that creates/updates field    (Prefered?)
//


#ifndef BesMagneticField_H
#define BesMagneticField_H

#include "G4MagneticField.hh"
#include "globals.hh"
#include "MagneticField/IMagneticFieldSvc.h"

using namespace std;

class G4FieldManager;
class G4ChordFinder;
class G4Mag_UsualEqRhs;
class G4MagIntegratorStepper;
class BesMagneticFieldMessenger;

class BesMagneticField:public G4MagneticField
{
public:
    
   BesMagneticField() ;               //  A zero field

 ~BesMagneticField() ;  
  void  GetFieldValue( const  double Point[3],
	                             double *Bfield          ) const;
  
  
  void SetStepperType( G4int i) { fStepperType = i ; }

  void SetStepper();

  void SetMinStep(G4double s) { fMinStep = s ; }

  void InitialiseAll();    //  Set parameters and call method below
  void CreateStepperAndChordFinder();
  
  void SetDeltaOneStep(double newvalue);
  void SetDeltaIntersection(double newvalue);
  void SetMinimumEpsilonStep(double newvalue);
  void SetMaximumEpsilonStep(double newvalue);

  
protected:

  G4FieldManager*         fFieldManager ;
  G4ChordFinder*          fChordFinder ;
  G4Mag_UsualEqRhs*       fEquation ; 
   

  G4MagIntegratorStepper* fStepper ;
  G4int                   fStepperType ;

  G4double                fMinStep ;
 
  BesMagneticFieldMessenger*      fFieldMessenger;
  IMagneticFieldSvc*               m_pIMF;
};

#endif
