//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHistory.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:  TrkHistory; simple class to record the history
// of a track fit
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Dave Brown
//
//------------------------------------------------------------------------

#ifndef TRKHISTORY_HH
#define TRKHISTORY_HH

#include "TrkBase/TrkErrCode.h"
#include <iostream>

class TrkHistory {
public:
  TrkHistory();
  TrkHistory(const TrkErrCode& err,const char* modname);
  TrkHistory(const TrkHistory& other);
  TrkHistory& operator =(const TrkHistory& other);
  ~TrkHistory();
// accessors
  const TrkErrCode& status() const { return _err; }
  const char* module() const { return _mod; }
  bool operator == (const TrkHistory& other) const;
  void print(std::ostream& os=std::cout) const;
private:
  TrkErrCode _err;
  const char* _mod;
};
// stream operator
std::ostream& operator<<(std::ostream& os, const TrkHistory& history);

#endif

