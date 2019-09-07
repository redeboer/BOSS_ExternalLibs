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
// $Id: G4NURBS.hh,v 1.10 2006/06/29 19:05:28 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
// 
// Olivier Crumeyrolle  12 September 1996

// G4NURBS.hh
// prototype for class G4NURBS - see documentation in graphics_reps/doc.
// OC 280896

// Class Description:
// Base class for shapes with NURBS drawing style.
// See documentation in graphics_reps/doc for details.

#ifndef __C_G4NURBS__ 
#define __C_G4NURBS__ 1

#include "globals.hh"
#include "G4Visible.hh"

// The internal floating point type is G4Float, defined line 162

#include "G4ios.hh"
#include "G4Point3D.hh"
#include "G4Vector3D.hh"

class G4NURBS : public G4Visible
{ 
 public:
  // NO public constructor. A G4NURBS must be builded with a child class.
  // Pure virtual function Whoami so one can't instanciate G4NURBS at all.

  // Whoami return a string describing the NURBS (e.g "Box")
  // * this string must not contain any \n *
  // this string is *not* yours (const char)
  virtual const char* Whoami() const = 0;

  // the copy constructor is private.

  // destructor. 
  virtual ~G4NURBS();

  // direction selector defined as a type because the user will use it
  // and we want the user to be well-manered.
  // However internally this typed enum is not as easy to use as it
  // could be (we can't ++). "t_" means it's a kind of local type.
  enum t_direction
  {
    U     = 0,
    V     = 1,
    DMask = 1,  // NofD : Number of Directions 
    NofD  = 2   // DMask : direction mask for fast range control,
  };            // e.g. : m[a_dir & DMask]

  // external representation for t_direction (just U -> 'U' V -> 'V') 
  static char Tochar(t_direction in_dir);

  // mother index type (I'd like to be able to use unsigned G4int
  // but it's impossible)
  typedef unsigned int t_index;

  // type for knot index, derivate from t_index
  typedef t_index t_indKnot;

  // type for ctrlpt coord and ctrlpt index
  typedef unsigned int t_indCoord;
  typedef unsigned int t_indCtrlPt;	// mono index
  typedef t_index      t_inddCtrlPt;    // bi dim index, derivate from t_index
		
  // why only t_inddCtrlPt and t_indKnot (and t_order further)
  // "derive" of t_index ? Because only these ones need
  // to be compatible (order + nbrctrlpts = nbrknots in a given direction)
  // Ok, typedefs are not true type derivation,
  // but this is the "spirit" of declarations with t_index.
  // To do true derivation we need true classes 
  // but classes for int are wastefull with today's compilers.

  // Note that these index types are defined
  // without knowledge of the indexed items types and that's perfect.

  // interface data type for the rationnal control points
  enum { X, Y, Z, W, NofC };	// NofC : number of coordinates

  // not typed as t_indCoord so loops are easy
  // to write, but the user is less restricted
  typedef G4double t_doubleCtrlPt [NofC]; // with doubles 
  typedef G4float  t_floatCtrlPt [NofC];  // with floats

  // access functions for others (e.g. GraphicsModel)
  G4int GetUorder() const;
  G4int GetVorder() const;
  G4int GetUnbrKnots() const;
  G4int GetVnbrKnots() const;
  G4int GetUnbrCtrlPts() const;	
  G4int GetVnbrCtrlPts() const;
  G4int GettotalnbrCtrlPts() const;	

  G4double GetUmin() const;
  G4double GetUmax() const;
  G4double GetVmin() const;
  G4double GetVmax() const;
  void CalcPoint(G4double u, G4double v,
		 G4Point3D &p, G4Vector3D &utan, G4Vector3D &vtan) const;

  // alternate access functions with G4NURBS::t_direction
  // e.g. mynurb.Getorder(G4NURBS::U)
  // these functions never fail because in_dir is masked
  G4int Getorder(t_direction in_dir) const;
  G4int GetnbrKnots(t_direction in_dir) const;
  G4int GetnbrCtrlPts(t_direction in_dir) const;	

  // crude access to knots vector and control points.
  // float and double versions.
  // * one should rather use the iterators below *
		
  // get a *copy* of the value; this copy is the user's
  // one, so the user is intended to manage it (including delete).
  // in_dir is masked, in_index checked and rounded.
  // errors on G4cerr
  G4float         GetfloatKnot(t_direction in_dir, t_indKnot in_index) const;
  G4double        GetdoubleKnot(t_direction in_dir, t_indKnot in_index) const;
  t_floatCtrlPt*  GetfloatCtrlPt(t_indCtrlPt in_onedimindex) const;
  t_floatCtrlPt*  GetfloatCtrlPt(t_inddCtrlPt in_Uindex,
                                 t_inddCtrlPt in_Vindex) const;
  t_doubleCtrlPt* GetdoubleCtrlPt(t_indCtrlPt in_onedimindex) const;
  t_doubleCtrlPt* GetdoubleCtrlPt(t_inddCtrlPt in_Uindex,
                                  t_inddCtrlPt in_Vindex) const;

  // complete copy functions
  // the user don't control the allocation and the copy process
  // but he/she own the result and will have to delete it
  // when he/she does not need it any more.
  G4float*  GetfloatAllKnots(t_direction in_dir) const;
  G4double* GetdoubleAllKnots(t_direction in_dir) const;
  G4float*  GetfloatAllCtrlPts() const;
  G4double* GetdoubleAllCtrlPts() const;

  // the iterators need that, the user does not
  
 protected:
  // internal type for reel numbers
  // ( Float is defined in templates.hh and is
  // under the control of HIGH_PRECISION )
  typedef Float G4Float;

 public:

  // internal type for order, derivate from t_index
  typedef t_index t_order;

  // internal type for knot
  typedef G4Float t_Knot;

 protected:

  // internal types for the control points
  typedef G4Float t_Coord;
  typedef t_Coord t_CtrlPt [NofC];

  // (nb: templates.hh included in globals.hh)
  // type for ref counting
  //typedef unsigned int t_refcount; 

 public:
  // iterators for an .... iterative access to knots and control points

  // errors are reported on G4cerr
  // they are friends, they use the protected members.
  // one can have as many iterators as he/she wants working in the same time.

  // declarations of iterators
  class KnotsIterator;
  class CtrlPtsCoordsIterator;
  class CtrlPtsIterator;

  // friendness declarations for iterators
  friend class KnotsIterator;
  friend class CtrlPtsCoordsIterator;
  friend class CtrlPtsIterator;

  // Example for the KnotsIterator
  //   G4float * my_array, * my_float_p;
  //   my_float_p = my_array = new float [my_nurb.GetnbrKnots(G4NURBS::U)]; 
  //   G4NURBS::KnotsIterator  my_iterator(my_nurb, G4NURBS::U);
  //   while (my_iterator.pick(my_float_p++));
  // that's all! my_array contain all the U knots.

  class KnotsIterator
  {
  public:
    KnotsIterator(const G4NURBS & in_rNurb, t_direction in_dir,
                                            t_indKnot in_startIndex = 0);
    G4bool pick(G4double * inout_pDbl);
    G4bool pick(G4float * inout_pFlt);
    //~KnotsIterator();
 
  protected:
    const t_direction    kmdir;
    const t_Knot * const kmpMax;
    const t_Knot *       mp;
  };

  // the CtrlPtsCoordsIterator. Works like the knots' one :
  //   G4float * my_array, * my_float_p;
  //   my_float_p = my_array =
  //      new float [my_nurb.GettotalnbrCtrlPts()*G4NURBS::NofC*sizeof(float)]; 
  //   G4NURBS::CtrlPtsCoordsIterator my_iterator(my_nurb);
  //   while (my_iterator.pick(my_float_p++));
  // after the while statement; my_float_p point just after the array
  // Remember ctrlpts are given U index increasing first

  class CtrlPtsCoordsIterator
  {
  public:
    CtrlPtsCoordsIterator(const G4NURBS & in_rNurb,
                          t_indCtrlPt in_startCtrlPtIndex = 0); 
    G4bool pick(G4double * inout_pDbl);
    G4bool pick(G4float * inout_pFlt);
    //~CtrlPtsCoordsIterator();

  protected:
    const t_Coord * const kmpMax;
    const t_Coord * mp;
  };

  // this iterator work CtrlPt by CtrlPt
  // see the << overload for an example
  class	CtrlPtsIterator
  {
  public:
    CtrlPtsIterator(const G4NURBS & in_rNurb, t_indCtrlPt in_startIndex = 0);
    G4bool pick(t_doubleCtrlPt * inout_pDblCtrlPt);
    G4bool pick(t_floatCtrlPt * inout_pFltCtrlPt);
    //~CtrlPtsIterator();

  protected:
    const t_CtrlPt * const  kmpMax;
    const t_CtrlPt *        mp;
  };

  // Q: a directional Iterator to extract one col/row of CtrlPts ?
	
 protected:
		
  // little structure containing data for each direction
  struct t_Dir
  {
    t_order      order;
    t_inddCtrlPt nbrCtrlPts;
    t_indKnot    nbrKnots;
    t_Knot *     pKnots;
    //t_refcount	nbralias; 
  };

  // check flag for the constructor
  typedef enum { NOcheck, check } t_CheckFlag;
		
  // first constructor (see G4NURBScylinder.cc for an example)
  // compulsory arguments :
  //   order of the surface in U and V direction
  //   number of control points in U and V direction
  //   control points array (usualy empty here, *but* allocated)
  // optional arguments :
  //   U and V knots vector (can be automaticaly generated)
  //   check flag	(default is to check!)
  //
  G4NURBS (t_order in_Uorder, t_order in_Vorder,
	   t_inddCtrlPt in_UnbrCtrlPts, t_inddCtrlPt in_VnbrCtrlPts,
	   t_CtrlPt * in_pCtrlPts,
	   t_Knot * in_pUKnots = 0, t_Knot * in_pVKnots = 0,
	   t_CheckFlag in_CheckFlag = check );

  // NB: the minimal NURBS is order 1, 2 knots, => 1 control points
  // one can actually define some curves with G4NURBS, set U as you want
  // set the V dir as order 1, 1 ctrlpt, 2 knots { 0 1 }
  // OpenGL work with this kind of data

  // second constructor (easier to use) (see G4NURBStube.cc for an example)
  // compulsory arguments :
  //   order of the surface in U and V direction
  //   number of control points in U and V direction
  // optional arguments :
  //   U and V knots vector generation flag (automaticaly or not)
  //   check flag	(default is to check!)
  // Allocations are Done for the user
  // but he/she still have to fill some arrays
  // For the moment I don't see yet how to ensure
  // that the user correctly fill the arrays
  // (in particular how avoid out of range access)
  // without class types for arrays.

 public:

  // knots vector generation flag
  enum t_KnotVectorGenFlag
  { 
    UserDefined, // The user will fill the array (in the child constructor
                 // for instance).

    Regular,     // First and last knot repeated order time
                 // other knots regularly spaced, unrepeated.
                 // Typically used for "linear" knots vector

    RegularRep	 // First and last knot repeated order time
                 // other knots regularly spaced but repeated one time.
                 // Typically used for "circular" knots vector and alikes.
  }; //t_KnotVectorGenFlag

 protected:

  // external representation for t_KnotVectorGenFlag
  // as a << overload.
  // (used in errors report)
  friend std::ostream & operator << (std::ostream & inout_OutStream,
				t_KnotVectorGenFlag in_KVGFlag);

  G4NURBS (t_order in_Uorder, t_order in_Vorder,
	   t_inddCtrlPt in_UnbrCtrlPts, t_inddCtrlPt in_VnbrCtrlPts,
	   t_KnotVectorGenFlag in_UKVGFlag = Regular,
	   t_KnotVectorGenFlag in_VKVGFlag = Regular,
	   t_CheckFlag in_CheckFlag = check );

  // nurbs data
  t_Dir         m[NofD];        // t_Dir : order nbrCtrlPts nbrKnots pKnots
  t_indCtrlPt   mtotnbrCtrlPts; // Total number of control points
  t_CtrlPt *    mpCtrlPts;      // U increasing first, V after
  //t_refcount  mnbralias;      // ref count for mpCtrlPts
		
  // 2dim index to 1 dim conversion
  t_indCtrlPt  To1d(t_inddCtrlPt in_Uindex, t_inddCtrlPt in_Vindex) const;

  // internal functions for converting the internal
  // data points to the interface type required
  // one can do some better things with class conversion
  // but for the moment control point data types are not class. 
  // static functions.
  // if changed to member functions, one must add the const
  // status and rewrite calls with an instance in 
  // some of the get functions.
		
  // return a float copy  
  static t_floatCtrlPt* TofloatCtrlPt(const t_CtrlPt &);

  // return a double copy
  static t_doubleCtrlPt* TodoubleCtrlPt(const t_CtrlPt &);


  // Building functions

  // KnotsVector builder
  // static function that work on a t_Dir and its
  // knot vector. So we can define
  // some knots vector outside a nurbs
  // object. (This avoid the existence
  // of some incompletly defined nurbs object,
  // used just as knots vector container)
  // Return true if succesfull.
  // ALWAYS allocate the knots array.
  // (return false and do nothing if it already exists (ie != 0))
  // Always fail if order + nbrCtrlPt != nbrKnots
  static G4bool MakeKnotVector(t_Dir & inout_dirdat,
                               t_KnotVectorGenFlag in_KVGFlag);
  static G4bool MakeKnotVector(t_Dir * p_inoutdirdat,
                               t_KnotVectorGenFlag in_KVGFlag);

  static void CP(G4NURBS::t_CtrlPt & rcp, t_Coord x, t_Coord y,
                                          t_Coord z, t_Coord w);
  static void CP(G4NURBS::t_CtrlPt & rcp, t_Coord x, t_Coord y,
                                          t_Coord z, t_Coord w, G4Float factor);

 private:	
  // check function used internally by constructors.
  // no returned value because all errors reported are fatals.
  // (assume order + nbrCtrlPts == nbrKnots
  //  cf constructors to understand why)
  void Conscheck() const;

  // copy constructor.
  // Not really necessary for geant. A warning is issued when used.
  G4NURBS(const G4NURBS &);

};

// external representation for t_KnotVectorGenFlag
std::ostream & operator << (std::ostream & inout_OutStream,
                              G4NURBS::t_KnotVectorGenFlag in_KVGFlag);


// << overload to dump a nurbs
// writted with public access functions
// do not depends on protected part

std::ostream & operator << (std::ostream & inout_outStream,
                              const G4NURBS & in_kNurb);

/***********************************************************************
 *                                                                     *
 * Inline code for public access functions.                            *
 * depends on the protected part                                       *
 *                                                                     *
 ***********************************************************************/

inline G4int G4NURBS::GetUorder() const          { return m[U].order; }
inline G4int G4NURBS::GetVorder() const          { return m[V].order; }
inline G4int G4NURBS::GetUnbrKnots() const       { return m[U].nbrKnots; }
inline G4int G4NURBS::GetVnbrKnots() const       { return m[V].nbrKnots; }
inline G4int G4NURBS::GetUnbrCtrlPts() const     { return m[U].nbrCtrlPts; }
inline G4int G4NURBS::GetVnbrCtrlPts() const     { return m[V].nbrCtrlPts; }
inline G4int G4NURBS::GettotalnbrCtrlPts() const { return mtotnbrCtrlPts; }

inline G4double G4NURBS::GetUmin() const {
  return (G4double) m[U].pKnots[GetUorder()-1];
}

inline G4double G4NURBS::GetUmax() const {
  return (G4double) m[U].pKnots[GetUnbrCtrlPts()];
}

inline G4double G4NURBS::GetVmin() const { 
  return (G4double) m[V].pKnots[GetVorder()-1];
}

inline G4double G4NURBS::GetVmax() const {
  return (G4double) m[V].pKnots[GetVnbrCtrlPts()];
}

inline G4int G4NURBS::Getorder(G4NURBS::t_direction in_dir) const {
  return m[in_dir & DMask].order;
}

inline G4int G4NURBS::GetnbrKnots(G4NURBS::t_direction in_dir) const {
  return m[in_dir & DMask].nbrKnots;
}

inline G4int G4NURBS::GetnbrCtrlPts(G4NURBS::t_direction in_dir) const {
  return m[in_dir & DMask].nbrCtrlPts;
} 

inline char G4NURBS::Tochar(G4NURBS::t_direction in_dir) {
  return (in_dir?'V':'U');
}

/***********************************************************************
 *                                                                     *
 * inline code for protected functions                                 *
 *                                                                     *
 ***********************************************************************/

// convert two dim. index to one dim.
//( Ctrl Pts are stored U increasing first )
// no check.
inline G4NURBS::t_indCtrlPt
G4NURBS::To1d(t_inddCtrlPt in_Uindex, t_inddCtrlPt in_Vindex) const
{
  return in_Uindex + in_Vindex*m[U].nbrCtrlPts;
}

// return a float copy
inline G4NURBS::t_floatCtrlPt*
G4NURBS::TofloatCtrlPt(const t_CtrlPt & in_krcp)
{
  G4NURBS::t_floatCtrlPt * pcopy = new G4NURBS::t_floatCtrlPt [1];
  for (G4int indCoord = X; indCoord < NofC; indCoord++)
    (*pcopy)[indCoord] = (G4float)in_krcp[indCoord];
  return pcopy;
}
 		
// return a double copy
inline G4NURBS::t_doubleCtrlPt* 
G4NURBS::TodoubleCtrlPt(const t_CtrlPt & in_krcp)
{
  G4NURBS::t_doubleCtrlPt *  pcopy = new G4NURBS::t_doubleCtrlPt [1];
  for (G4int indCoord = X; indCoord < NofC; indCoord++)
    (*pcopy)[indCoord] = (G4double)in_krcp[indCoord];
  return pcopy;
}

// MakeKnotVector alias
inline G4bool G4NURBS::MakeKnotVector(G4NURBS::t_Dir * p_inoutdirdat,
                             G4NURBS::t_KnotVectorGenFlag in_KVGFlag)
{
  return MakeKnotVector(*p_inoutdirdat, in_KVGFlag);
}

/***********************************************************************
 *                                                                     *
 * inlines functions to simplify control points definition             *
 * see GG4NURBSbox.cc for instance                                     *
 *                                                                     *
 ***********************************************************************/

inline void G4NURBS::CP(G4NURBS::t_CtrlPt & rcp,
			t_Coord x, t_Coord y, t_Coord z, t_Coord w)
{
  rcp[G4NURBS::X]=x;
  rcp[G4NURBS::Y]=y;
  rcp[G4NURBS::Z]=z;
  rcp[G4NURBS::W]=w;	
}

// with a common factor
inline void G4NURBS::CP(G4NURBS::t_CtrlPt & rcp, t_Coord x,
                        t_Coord y, t_Coord z, t_Coord w, G4Float factor)
{
  rcp[G4NURBS::X]=factor*x;
  rcp[G4NURBS::Y]=factor*y;
  rcp[G4NURBS::Z]=factor*z;
  rcp[G4NURBS::W]=factor*w;	
}

#endif /* end of __C_G4NURBS__ */
