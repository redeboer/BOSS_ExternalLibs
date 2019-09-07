//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkGeomTrajVisitor.h,v 1.1.1.1 2005/07/18 05:51:56 codeman Exp $
//
// Description:
//     Part of Visitor pattern for TrkGeomTraj class hierarchy.  This is 
//     the base class for any Visitor that will visit a TrkGeomTraj.  As 
//     new GeomTraj's are introduced, they must have an appropriate 
//     visitXXXX function added here and in all derived Visitors.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKGEOMTRAJVISITOR_H
#define TRKGEOMTRAJVISITOR_H

class TrkLineTraj;
class TrkPieceLineTraj;
//SKIP class TrkParabolaTraj;
class BesHelixTraj;

// Class interface //
class TrkGeomTrajVisitor {

public:
  TrkGeomTrajVisitor();
  virtual ~TrkGeomTrajVisitor();

  virtual void visitLine(const TrkLineTraj*) = 0;
  //  virtual void visitParabola(const TrkParabolaTraj*) = 0;
  virtual void visitPieceLine(const TrkPieceLineTraj*) = 0;
  virtual void visitHelix(const BesHelixTraj*) = 0;
  
private:	

  // Preempt
  TrkGeomTrajVisitor&   operator= (const TrkGeomTrajVisitor&);
  TrkGeomTrajVisitor(const TrkGeomTrajVisitor &);
};

#endif







