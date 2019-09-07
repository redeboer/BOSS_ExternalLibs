//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkIdManDumb.h,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
//
// Description:
//     Class for generating dummy id numbers (always same value).  Users 
// can create instances, or use a singleton instance (value = -1).
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef TRKIDMANDUMB_HH
#define TRKIDMANDUMB_HH
#include "TrkBase/TrkIdManager.h"

// Class interface //
class TrkIdManDumb : public TrkIdManager {

public:
  TrkIdManDumb(long val);
  virtual ~TrkIdManDumb();
  static TrkIdManDumb* instance();

  virtual long nextId();
  virtual long lastId() const;
  virtual void setMax(long maxid);
private:	
  long _theValue;

  long theValue()       const                             {return _theValue;}
  // Preempt 
  TrkIdManDumb&   operator= (const TrkIdManDumb&);
  TrkIdManDumb(const TrkIdManDumb &);
};

#endif







