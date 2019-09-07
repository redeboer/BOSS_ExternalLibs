//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkIdManager.h,v 1.2 2005/05/18 11:50:22 zhangy Exp $
//
// Description:
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef TRKIDMANAGER_HH
#define TRKIDMANAGER_HH
#include "GaudiKernel/DataObject.h"

// Class interface //
class TrkIdManager : public DataObject {
public:
  TrkIdManager();
  virtual ~TrkIdManager();

  virtual long nextId() = 0;       // Get next id number and update manager
  virtual long lastId() const = 0;
  virtual void setMax(long maxid) = 0;

private:	
  // Preempt 
  TrkIdManager&   operator= (const TrkIdManager&);
  TrkIdManager(const TrkIdManager &);
};

#endif
