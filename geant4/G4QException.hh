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
// $Id: G4QException.hh,v 1.11 2009/02/23 09:49:24 mkossov Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//      ---------------- G4QException ----------------
//           by Mikhail Kossov, November 2003.
//  header of the class for an Exception used by the CHIPS Model
// ---------------------------------------------------------------
// Short description: Just an internal CHIPS Exception Messenger
// -------------------------------------------------------------

#ifndef G4QException_h
#define G4QException_h

#include <exception>
#include <iostream>
#include "globals.hh"

class G4QException : public std::exception
{
  public:
  G4QException(G4String message="No message");
  const G4String& GetMessage();
  virtual ~G4QException() throw();
  private:
  G4String theMessage;
};

#endif
