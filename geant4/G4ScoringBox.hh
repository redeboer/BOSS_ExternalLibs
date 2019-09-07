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
// $Id: G4ScoringBox.hh,v 1.18 2007/11/06 09:41:34 akimura Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//

#ifndef G4ScoringBox_h
#define G4ScoringBox_h 1

#include "globals.hh"
#include "G4VScoringMesh.hh"
#include "G4RotationMatrix.hh"
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4VPrimitiveScorer;
class G4VScoreColorMap;

#include <vector>

class G4ScoringBox : public G4VScoringMesh
{
public:
  G4ScoringBox(G4String wName);
  ~G4ScoringBox();

public:
  virtual void Construct(G4VPhysicalVolume* fWorldPhys);
  virtual void List() const;
  virtual void Draw(std::map<G4int, G4double*> * map, G4VScoreColorMap* colorMap, G4int axflg=111);
  virtual void DrawColumn(std::map<G4int, G4double*> * map, G4VScoreColorMap* colorMap, 
                          G4int idxProj, G4int idxColumn); 

  // set a direction to segment this mesh
  void SetSegmentDirection(G4int dir) {fSegmentDirection = dir;}

private:
  // construct this mesh
  void SetupGeometry(G4VPhysicalVolume * fWorldPhys);
  // get replicated position from 3D index (x,y,z)
  G4ThreeVector GetReplicaPosition(G4int x, G4int y, G4int z);
  // get 3D index (x,y,z) from sequential index
  void GetXYZ(G4int index, G4int q[3]) const;
  // get sequential index from 3D index (x,y,z)
  G4int GetIndex(G4int x, G4int y, G4int z) const;

private:
  G4int fSegmentDirection; // =1: x, =2: y, =3: z
  G4LogicalVolume * fMeshElementLogical;
  
};




#endif

