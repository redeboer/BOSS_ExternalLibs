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
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:  G4VhShellCrossSection
//
// Author:      S. Dussoni and A. Mantero (Alfonso.Mantero@ge.infn.it)
// 
// History:
// -----------
// 23 Oct 2001 A. Mantero   1st implementation
// 24 Oct 2001 MGP          Cleaned up
// 29 Oct 2001 VI           Add delta energy
//
// -------------------------------------------------------------------

// Class Description: 
//
// Abstract class for models of shell cross sections in proton ionisation

// -------------------------------------------------------------------
//

#ifndef G4VHSHELLCROSSSECTION_HH
#define G4VHSHELLCROSSSECTION_HH 1

#include "globals.hh"
#include <vector>

class G4VhShellCrossSection 
{

public:

  G4VhShellCrossSection();

  virtual ~G4VhShellCrossSection();

  G4int SelectRandomShell(G4int Z, 
                          G4double incidentEnergy,
			  G4double mass, 
			  G4double deltaEnergy) const;

 virtual std::vector<G4double> GetCrossSection(G4int Z,
						G4double incidentEnergy,
						G4double mass,
						G4double deltaEnergy,
					        G4bool testFlag = false) const =0;

  //protected:

  virtual std::vector<G4double> Probabilities(G4int Z,
					      G4double incidentEnergy,
					      G4double mass,
					      G4double deltaEnergy) const = 0;


  virtual void SetTotalCS(G4double);


private:

  // Hide copy constructor and assignment operator 
  G4VhShellCrossSection(const  G4VhShellCrossSection&);
  G4VhShellCrossSection & operator=(const  G4VhShellCrossSection &right);

};

#endif

