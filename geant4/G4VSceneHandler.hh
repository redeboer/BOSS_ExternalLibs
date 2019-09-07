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
// $Id: G4VSceneHandler.hh,v 1.42 2009/11/04 12:55:33 allison Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 
// John Allison  19th July 1996.
//
// Class description
//
// Abstract interface class for graphics scene handlers.
// Inherits from G4VGraphicsScene, in the intercoms category, which is
// a minimal abstract interface for the GEANT4 kernel.

#ifndef G4VSCENEHANDLER_HH
#define G4VSCENEHANDLER_HH

#include "globals.hh"

#include "G4VGraphicsScene.hh"
#include "G4ViewerList.hh"
#include "G4ViewParameters.hh"
#include "G4THitsMap.hh"

class G4Scene;
class G4VViewer;
class G4Colour;
class G4Visible;
class G4ModelingParameters;
class G4VModel;
class G4VGraphicsSystem;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4Event;
class G4AttHolder;

class G4VSceneHandler: public G4VGraphicsScene {

public: // With description

  friend std::ostream& operator << (std::ostream& os, const G4VSceneHandler& s);

  enum MarkerSizeType {world, screen};

  G4VSceneHandler (G4VGraphicsSystem& system,
		   G4int id,
		   const G4String& name = "");

  virtual ~G4VSceneHandler ();

  ///////////////////////////////////////////////////////////////////
  // Methods for adding raw GEANT4 objects to the scene handler.  They
  // must always be called in the triplet PreAddSolid, AddSolid and
  // PostAddSolid.  The transformation and visualization attributes
  // must be set by the call to PreAddSolid.  If your graphics system
  // is sophisticated enough to handle a particular solid shape as a
  // primitive, in your derived class write a function to override one
  // or more of the following.  See the implementation of
  // G4VSceneHandler::AddSolid (const G4Box& box) for more
  // suggestions.  If not, please implement the base class invocation.

  virtual void PreAddSolid (const G4Transform3D& objectTransformation,
			   const G4VisAttributes& visAttribs);
  // objectTransformation is the transformation in the world
  // coordinate system of the object about to be added, and visAttribs
  // is its visualization attributes.
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::PreAddSolid
  //  (const G4Transform3D& objectTransformation,
  //   const G4VisAttributes& visAttribs) {
  //   G4VSceneHandler::PreAddSolid (objectTransformation, visAttribs);
  //   ...
  // }

  virtual void PostAddSolid ();
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::PostAddSolid () {
  //   ...
  //   G4VSceneHandler::PostAddSolid ();
  // }

  virtual void AddSolid (const G4Box&);
  virtual void AddSolid (const G4Cons&);
  virtual void AddSolid (const G4Tubs&);
  virtual void AddSolid (const G4Trd&);
  virtual void AddSolid (const G4Trap&);
  virtual void AddSolid (const G4Sphere&);
  virtual void AddSolid (const G4Para&);
  virtual void AddSolid (const G4Torus&);
  virtual void AddSolid (const G4Polycone&);
  virtual void AddSolid (const G4Polyhedra&);
  virtual void AddSolid (const G4VSolid&);  // For solids not above.

  ///////////////////////////////////////////////////////////////////
  // Methods for adding "compound" GEANT4 objects to the scene
  // handler.  These methods may either (a) invoke "user code" that
  // uses the "user interface", G4VVisManager (see, for example,
  // G4VSceneHandler, which for trajectories uses
  // G4VTrajectory::DrawTrajectory, via G4TrajectoriesModel in the
  // Modeling Category) or (b) invoke AddPrimitives below (between
  // calls to Begin/EndPrimitives) or (c) use graphics-system-specific
  // code or (d) any combination of the above.

  virtual void AddCompound (const G4VTrajectory&);
  virtual void AddCompound (const G4VHit&);
  virtual void AddCompound (const G4THitsMap<G4double>&);

  //////////////////////////////////////////////////////////////
  // Functions for adding primitives.

  virtual void BeginModeling ();
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::BeginModeling () {
  //   G4VSceneHandler::BeginModeling ();
  //   ...
  // }

  virtual void EndModeling ();
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::EndModeling () {
  //   ...
  //   G4VSceneHandler::EndModeling ();
  // }

  virtual void BeginPrimitives
  (const G4Transform3D& objectTransformation);
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::BeginPrimitives
  // (const G4Transform3D& objectTransformation) {
  //   G4VSceneHandler::BeginPrimitives (objectTransformation);
  //   ...
  // }

  virtual void EndPrimitives ();
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::EndPrimitives () {
  //   ...
  //   G4VSceneHandler::EndPrimitives ();
  // }

  virtual void BeginPrimitives2D
  (const G4Transform3D& objectTransformation);
  // The x,y coordinates of the primitives passed to AddPrimitive are
  // intrepreted as screen coordinates, -1 < x,y < 1.  The
  // z-coordinate is ignored.
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::BeginPrimitives2D
  // (const G4Transform3D& objectTransformation) {
  //   G4VSceneHandler::BeginPrimitives2D ();
  //   ...
  // }

  virtual void EndPrimitives2D ();
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::EndPrimitives2D () {
  //   ...
  //   G4VSceneHandler::EndPrimitives2D ();
  // }

  virtual void AddPrimitive (const G4Polyline&)   = 0;
  virtual void AddPrimitive (const G4Scale&);
  // Default implementation in this class but can be over-ridden.
  virtual void AddPrimitive (const G4Text&)       = 0;
  virtual void AddPrimitive (const G4Circle&)     = 0;      
  virtual void AddPrimitive (const G4Square&)     = 0;      
  virtual void AddPrimitive (const G4Polymarker&);
  // Default implementation in this class but can be over-ridden.
  virtual void AddPrimitive (const G4Polyhedron&) = 0;  
  virtual void AddPrimitive (const G4NURBS&)      = 0;       

  //////////////////////////////////////////////////////////////
  // Access functions.
  const G4String&     GetName           () const;
  G4int               GetSceneHandlerId () const;
  G4int               GetViewCount      () const;
  G4VGraphicsSystem*  GetGraphicsSystem () const;
  G4Scene*            GetScene          () const;
  const G4ViewerList& GetViewerList     () const;
  G4VModel*           GetModel          () const;
  G4VViewer*          GetCurrentViewer  () const;
  G4bool              GetMarkForClearingTransientStore () const;
  G4bool              IsReadyForTransients () const;
  G4bool              GetTransientsDrawnThisEvent () const;
  G4bool              GetTransientsDrawnThisRun   () const;
  void          SetName          (const G4String&);
  void          SetCurrentViewer (G4VViewer*);
  void          SetScene         (G4Scene*);
  G4ViewerList& SetViewerList    ();  // Non-const so you can change.
  void          SetModel         (G4VModel*);
  void          SetMarkForClearingTransientStore (G4bool);
  // Sets flag which will cause transient store to be cleared at the
  // next call to BeginPrimitives().  Maintained by vis manager.
  void          SetTransientsDrawnThisEvent      (G4bool);
  void          SetTransientsDrawnThisRun        (G4bool);
  // Maintained by vis manager.

  //////////////////////////////////////////////////////////////
  // Public utility functions.

  const G4Colour& GetColour (const G4Visible&);
  const G4Colour& GetColor  (const G4Visible&);
  // Returns colour of G4Visible object, or default global colour.

  const G4Colour& GetTextColour (const G4Text&);
  const G4Colour& GetTextColor  (const G4Text&);
  // Returns colour of G4Text object, or default text colour.

  G4double GetLineWidth(const G4VisAttributes*);
  // Returns line width of G4VisAttributes multiplied by GlobalLineWidthScale.

  G4ViewParameters::DrawingStyle GetDrawingStyle (const G4VisAttributes*);
  // Returns drawing style from current view parameters, unless the user
  // has forced through the vis attributes, thereby over-riding the
  // current view parameter.

  G4bool GetAuxEdgeVisible (const G4VisAttributes*);
  // Returns auxiliary edge visibility from current view parameters,
  // unless the user has forced through the vis attributes, thereby
  // over-riding the current view parameter.

  G4int GetNoOfSides(const G4VisAttributes*);
  // Returns no. of sides (lines segments per circle) from current
  // view parameters, unless the user has forced through the vis
  // attributes, thereby over-riding the current view parameter.

  G4double GetMarkerSize (const G4VMarker&, MarkerSizeType&);
  // Returns applicable marker size (diameter) and type (in second
  // argument).  Uses global default marker if marker sizes are not
  // set.  Multiplies by GlobalMarkerScale.

  G4double GetMarkerDiameter (const G4VMarker&, MarkerSizeType&);
  // Alias for GetMarkerSize.

  G4double GetMarkerRadius (const G4VMarker&, MarkerSizeType&);
  // GetMarkerSize / 2.

  G4ModelingParameters* CreateModelingParameters ();
  // Only the scene handler and view know what the Modeling Parameters should
  // be.  For historical reasons, the GEANT4 Visualization Environment
  // maintains its own Scene Data and View Parameters, which must be
  // converted, when needed, to Modeling Parameters.

  void DrawEvent(const G4Event*);
  // Checks scene's end-of-event model list and draws trajectories,
  // hits, etc.

  void DrawEndOfRunModels();
  // Draws end-of-run models.

  //////////////////////////////////////////////////////////////
  // Administration functions.

  G4int IncrementViewCount ();

  virtual void ClearStore ();
  // Clears graphics database (display lists) if any.  This base class
  // implements some common functionality so...
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::ClearStore () {
  //   G4VSceneHandler::ClearStore ();
  //   ...
  // }

  virtual void ClearTransientStore ();
  // Clears transient part of graphics database (display lists) if any.
  // This base class implements some common functionality so...
  // IMPORTANT: invoke this from your polymorphic versions, e.g.:
  // void MyXXXSceneHandler::ClearTransientStore () {
  //   G4VSceneHandler::ClearTransientStore ();
  //   ...
  // }

  void AddViewerToList      (G4VViewer* pView);  // Add view to view List.
  void RemoveViewerFromList (G4VViewer* pView);  // Remove view from view List.

protected:

  //////////////////////////////////////////////////////////////
  // Default routine used by default AddSolid ().

  virtual void RequestPrimitives (const G4VSolid& solid);

  //////////////////////////////////////////////////////////////
  // Other internal routines...

  virtual const G4Polyhedron* CreateSectionPolyhedron ();
  virtual const G4Polyhedron* CreateCutawayPolyhedron ();
  // Generic clipping using the BooleanProcessor in graphics_reps is
  // implemented in this class.  Subclasses that implement their own
  // clipping should provide an override that returns zero.
  void LoadAtts(const G4Visible&, G4AttHolder*);
  // Load G4AttValues and G4AttDefs associated with the G4Visible
  // object onto the G4AttHolder object.  It checks fpModel, and also
  // loads the G4AttValues and G4AttDefs from G4PhysicalVolumeModel,
  // G4VTrajectory, G4VTrajectoryPoint, or G4VHits, as appropriate.
  // The G4AttHolder object is an object of a class that publicly
  // inherits G4AttHolder - see, e.g., SoG4Polyhedron in the Open
  // Inventor driver.  G4AttHolder deletes G4AttValues in its
  // destructor to ensure proper clean-up of G4AttValues.

  //////////////////////////////////////////////////////////////
  // Data members

  G4VGraphicsSystem& fSystem;          // Graphics system.
  const G4int        fSceneHandlerId;  // Id of this instance.
  G4String           fName;
  G4int              fViewCount;       // To determine view ids.
  G4ViewerList       fViewerList;      // Viewers.
  G4VViewer*         fpViewer;         // Current viewer.
  G4Scene*           fpScene;          // Scene for this scene handler.
  G4bool             fMarkForClearingTransientStore;
  G4bool             fReadyForTransients;  // I.e., not processing the
			                   // run-duration part of scene.
  G4bool             fTransientsDrawnThisEvent;  // Maintained by vis
  G4bool             fTransientsDrawnThisRun;    // manager.
  G4bool             fProcessingSolid; // True if within Pre/PostAddSolid.
  G4bool             fSecondPassRequested;
  G4bool             fSecondPass;    // ...in process.
  G4VModel*          fpModel;        // Current model.
  const G4Transform3D* fpObjectTransformation; // Current accumulated
					       // object transformation.
  G4int              fNestingDepth; // For Begin/EndPrimitives.
  const G4VisAttributes* fpVisAttribs;  // Working vis attributes.
  const G4Transform3D fIdentityTransformation;

private:

  G4VSceneHandler (const G4VSceneHandler&);
  G4VSceneHandler& operator = (const G4VSceneHandler&);

  //////////////////////////////////////////////////////////////
  // Friend function accessed only by views of this scene.

  friend void G4VViewer::ProcessView ();

  //////////////////////////////////////////////////////////////
  // Private functions, etc..

  void   ProcessScene     (G4VViewer& view);
  // Accessed by G4VViewer::ProcessView ().

};

#include "G4VSceneHandler.icc"

#endif
