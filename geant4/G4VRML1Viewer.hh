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
// $Id: G4VRML1Viewer.hh,v 1.8 2006/06/29 21:25:33 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// G4VRML1Viewer.hh
// Satoshi Tanaka & Yasuhide Sawada

#ifndef WIN32

#ifdef  G4VIS_BUILD_VRML_DRIVER

#ifndef G4VRML1_VIEWER_HH
#define G4VRML1_VIEWER_HH

#include "G4VViewer.hh"
#include "globals.hh"

class G4VRML1SceneHandler;

class G4VRML1Viewer: public G4VViewer {
public:
	G4VRML1Viewer(G4VRML1SceneHandler& scene, const G4String& name = "");
	virtual ~G4VRML1Viewer();
	void ClearView();
	void DrawView();
	void ShowView();
	void FinishView();
private:
	void SetView();

private:
	G4VRML1SceneHandler& fSceneHandler; // Reference to Graphics Scene for this view.
};

#endif //G4VRML1_VIEW_HH
#endif //G4VIS_BUILD_VRML_DRIVER
#endif //WIN32
