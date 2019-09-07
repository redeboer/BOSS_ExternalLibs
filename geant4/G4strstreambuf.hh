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
// $Id: G4strstreambuf.hh,v 1.16 2007/11/13 17:35:06 gcosmo Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
// ====================================================================
//
//   G4strstreambuf
//
// ====================================================================
#ifndef G4_STR_STREAM_BUF_HH
#define G4_STR_STREAM_BUF_HH

#include "globals.hh"
#include "G4coutDestination.hh"
#include <streambuf>

class G4strstreambuf;

#if defined G4IOS_EXPORT
extern G4DLLEXPORT G4strstreambuf G4coutbuf;
extern G4DLLEXPORT G4strstreambuf G4cerrbuf;
#else
extern G4DLLIMPORT G4strstreambuf G4coutbuf;
extern G4DLLIMPORT G4strstreambuf G4cerrbuf;
#endif

class G4strstreambuf : public std::basic_streambuf<char>
{
  public:

    G4strstreambuf();
    ~G4strstreambuf();
    
    virtual G4int overflow(G4int c=EOF);
    virtual G4int sync();

#ifdef WIN32
    virtual G4int underflow();
#endif

    void SetDestination(G4coutDestination* dest);
    inline G4int ReceiveString ();
  
  private:

    char* buffer;
    G4int count, size;
    G4coutDestination* destination;

    // hidden...
    G4strstreambuf(const G4strstreambuf&);
    G4strstreambuf& operator=(const G4strstreambuf&);
};

#include "G4strstreambuf.icc"

#endif

