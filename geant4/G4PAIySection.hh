//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: G4PAIySection.hh,v 1.1 2007/10/01 17:45:14 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 
// G4PAIySection.hh -- header file
//
//
// Preparation of ionizing collision cross section according to Photo Absorption 
// Ionization (PAI) model for simulation of ionization energy losses in very thin
// absorbers. Author: Vladimir.Grichine@cern.ch
//
// History:
//
// 01.10.07, V.Ivanchenko create using V.Grichine G4PAIxSection class

#ifndef G4PAIYSECTION_HH
#define G4PAIYSECTION_HH

#include "G4ios.hh"
#include "globals.hh"
#include "Randomize.hh"

#include "G4SandiaTable.hh"

class G4PAIySection
{
public:

  G4PAIySection();
	  
  ~G4PAIySection();

  void Initialize(const G4Material* material, 
		  G4double maxEnergyTransfer, 
		  G4double betaGammaSq);
	            
  void InitPAI() ;

  void NormShift( G4double betaGammaSq ) ;
  
  void SplainPAI( G4double betaGammaSq ) ;
	  	  
  // Physical methods
  G4double RutherfordIntegral( G4int intervalNumber,
			       G4double limitLow,
			       G4double limitHigh     ) ;

  G4double ImPartDielectricConst( G4int intervalNumber,
				  G4double energy        ) ;

  G4double RePartDielectricConst(G4double energy) ;

  G4double DifPAIySection( G4int intervalNumber,
			   G4double betaGammaSq    ) ;

  G4double PAIdNdxCerenkov( G4int intervalNumber,
			    G4double betaGammaSq    ) ;

  G4double PAIdNdxPlasmon( G4int intervalNumber,
			   G4double betaGammaSq    ) ;

  void     IntegralPAIySection() ;
  void     IntegralCerenkov() ;
  void     IntegralPlasmon() ;

  G4double SumOverInterval(G4int intervalNumber) ;
  G4double SumOverIntervaldEdx(G4int intervalNumber) ;
  G4double SumOverInterCerenkov(G4int intervalNumber) ;
  G4double SumOverInterPlasmon(G4int intervalNumber) ;

  G4double SumOverBorder( G4int intervalNumber,
			  G4double energy          ) ;
  G4double SumOverBorderdEdx( G4int intervalNumber,
			      G4double energy          ) ;
  G4double SumOverBordCerenkov( G4int intervalNumber,
				G4double energy          ) ;
  G4double SumOverBordPlasmon( G4int intervalNumber,
			       G4double energy          ) ;

  G4double GetStepEnergyLoss( G4double step ) ;
  G4double GetStepCerenkovLoss( G4double step ) ;
  G4double GetStepPlasmonLoss( G4double step ) ;
	 
  // Inline access functions

  G4int GetNumberOfGammas() const { return fNumberOfGammas ; }
	  
  G4int GetSplineSize() const { return fSplineNumber ; }
	  
  G4int GetIntervalNumber() const { return fIntervalNumber ; }

  G4double GetEnergyInterval(G4int i){ return fEnergyInterval[i] ; } 

  G4double GetDifPAIySection(G4int i){ return fDifPAIySection[i] ; } 
  G4double GetPAIdNdxCrenkov(G4int i){ return fdNdxCerenkov[i] ; } 
  G4double GetPAIdNdxPlasmon(G4int i){ return fdNdxPlasmon[i] ; } 
	  
  G4double GetMeanEnergyLoss() const {return fIntegralPAIySection[0] ; }
  G4double GetMeanCerenkovLoss() const {return fIntegralCerenkov[0] ; }
  G4double GetMeanPlasmonLoss() const {return fIntegralPlasmon[0] ; }

  G4double GetNormalizationCof() const { return fNormalizationCof ; }
          
  inline G4double GetPAItable(G4int i,G4int j) const ;

  inline G4double    GetLorentzFactor(G4int i) const ;
	  	  
  inline G4double GetSplineEnergy(G4int i) const ;
	  
  inline G4double GetIntegralPAIySection(G4int i) const ;
  inline G4double GetIntegralPAIdEdx(G4int i) const ;
  inline G4double GetIntegralCerenkov(G4int i) const ;
  inline G4double GetIntegralPlasmon(G4int i) const ;

private :

// Local class constants
 
  static const G4double fDelta ; // energy shift from interval border = 0.001
  static const G4double fError ; // error in lin-log approximation = 0.005

  static       G4int fNumberOfGammas ;         // = 111 ;
  static const G4double fLorentzFactor[112] ;  //  static gamma array

  static
  const G4int fRefGammaNumber  ; // The number of gamma for creation of spline (15)

  G4int    fIntervalNumber  ;    //  The number of energy intervals
  G4double fNormalizationCof ;   // Normalization cof for PhotoAbsorptionXsection

  G4double fDensity ;            // Current density
  G4double fElectronDensity ;    // Current electron (number) density
  G4int    fSplineNumber ;       // Current size of spline

  G4SandiaTable*  fSandia;

  G4double fEnergyInterval[500] ;
  G4double fA1[500] ; 
  G4double fA2[500] ;
  G4double fA3[500] ; 
  G4double fA4[500] ;

  static
  const G4int   fMaxSplineSize  ;          // Max size of output splain arrays = 500

  G4double          fSplineEnergy[500] ;   // energy points of splain
  G4double fRePartDielectricConst[500] ;   // Real part of dielectric const
  G4double fImPartDielectricConst[500] ;   // Imaginary part of dielectric const
  G4double          fIntegralTerm[500] ;   // Integral term in PAI cross section
  G4double        fDifPAIySection[500] ;   // Differential PAI cross section
  G4double          fdNdxCerenkov[500] ;   // dNdx of Cerenkov collisions
  G4double          fdNdxPlasmon[500] ;    // dNdx of Plasmon collisions

  G4double   fIntegralPAIySection[500] ;   // Integral PAI cross section  ?
  G4double   fIntegralPAIdEdx[500] ;       // Integral PAI dEdx  ?
  G4double   fIntegralCerenkov[500] ;      // Integral Cerenkov N>omega  ?
  G4double   fIntegralPlasmon[500] ;       // Integral Plasmon N>omega  ?

  G4double   fPAItable[500][112] ;         // Output array

} ;    

////////////////  Inline methods //////////////////////////////////
//


inline G4double G4PAIySection::GetPAItable(G4int i, G4int j) const
{
   return fPAItable[i][j] ;
}

inline G4double G4PAIySection::GetLorentzFactor(G4int j) const
{
   return fLorentzFactor[j] ;
}

inline G4double G4PAIySection::GetSplineEnergy(G4int i) const 
{
   if(i < 1 || i > fSplineNumber)
   {
      G4Exception("Invalid argument in G4PAIySection::GetSplineEnergy");
   }
   return fSplineEnergy[i] ;
}
	  
inline G4double G4PAIySection::GetIntegralPAIySection(G4int i) const 
{
   if(i < 1 || i > fSplineNumber)
   {
    G4Exception("Invalid argument in G4PAIySection::GetIntegralPAIySection");
   }
   return fIntegralPAIySection[i] ;
}

inline G4double G4PAIySection::GetIntegralPAIdEdx(G4int i) const 
{
   if(i < 1 || i > fSplineNumber)
   {
    G4Exception("Invalid argument in G4PAIySection::GetIntegralPAIySection");
   }
   return fIntegralPAIdEdx[i] ;
}

inline G4double G4PAIySection::GetIntegralCerenkov(G4int i) const 
{
   if(i < 1 || i > fSplineNumber)
   {
    G4Exception("Invalid argument in G4PAIySection::GetIntegralCerenkov");
   }
   return fIntegralCerenkov[i] ;
}

inline G4double G4PAIySection::GetIntegralPlasmon(G4int i) const 
{
   if(i < 1 || i > fSplineNumber)
   {
    G4Exception("Invalid argument in G4PAIySection::GetIntegralPlasmon");
   }
   return fIntegralPlasmon[i] ;
}

#endif   

// -----------------   end of G4PAIySection header file    -------------------
