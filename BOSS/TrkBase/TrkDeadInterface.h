//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkDeadInterface.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKDEADINTERFACE_HH
#define TRKDEADINTERFACE_HH
#include "TrkBase/TrkExtInterface.h"
#include "ProxyDict/IfdIntKey.h"
// Class interface //

class TrkDeadInterface : public TrkExtInterface {
public:
  TrkDeadInterface() : _mykey(0) {;}
  virtual ~TrkDeadInterface() {;}
  virtual const IfdKey& myKey() const { return _mykey; }
private:
  IfdIntKey _mykey;
};

#endif







