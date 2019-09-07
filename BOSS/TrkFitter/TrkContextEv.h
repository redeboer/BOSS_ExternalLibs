//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkContextEv.h,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
//
// Description:
//     Derived class of TrkContext.  Adds precisely one feature: the 
// ctor to fill in default values from AbsEvent and AbsEnv.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef TRKCONTEXTEV_HH
#define TRKCONTEXTEV_HH
#include "TrkBase/TrkContext.h"
//class AbsEvent;

class BField;

// Class interface //
class TrkContextEv : public TrkContext {

public:
                 TrkContextEv(const BField *);
  virtual        ~TrkContextEv();

  virtual TrkId  getId()    const;


private:	
  TrkIdManager* _idman;

  TrkIdManager* idManager() const                     {return _idman;}
  // Preempt 
  TrkContextEv&   operator= (const TrkContextEv&);
  TrkContextEv(const TrkContextEv &);
};

#endif
