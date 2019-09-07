//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkContext.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//     Holds information about the "environment" in which a track is created --
// BField, track id manager.  One of these objects must be passed 
// to the track when it is created.  The idea is to decouple the tracks 
// from the sources of this information.  (See TrkFitter/TrkContextEv for 
// semi-automatic creation of an object of this class.)
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKCONTEXT_HH
#define TRKCONTEXT_HH

class BField;
class TrkIdManager;
class TrkId;

// Class interface //
class TrkContext {

public:
  TrkContext(const BField*);
  TrkContext(const TrkContext &);
  virtual ~TrkContext();
  TrkContext&   operator= (const TrkContext&);

  const BField*       bField()    const                           {return _bf;}
  virtual TrkId       getId()     const = 0;

  void setBField(const BField* bf);
  bool operator==(const TrkContext&) const;

protected:
  
private:	
  const BField* _bf;
};

#endif







