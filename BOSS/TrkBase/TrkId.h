//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkId.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//     Holds an ID number and a pointer to an Id manager that knows how 
// provide the next number in the sequence; designed to provided tracks 
// with unique ids.  It does _not_ own the Id manager.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//------------------------------------------------------------------------
#ifndef TRKID_HH
#define TRKID_HH

class TrkIdManager;

// Class interface //
class TrkId {

public:
  TrkId(long idNo, TrkIdManager* idMan);    // creates with input id number
  TrkId(TrkIdManager* idMan);              // gets id number from idMan
  TrkId(const TrkId &);                    // copies existing value
  TrkId&   operator= (const TrkId&);       // copies existing value
  bool operator<(const TrkId &) const;
  ~TrkId();

  void setNewValue(const TrkId&);    // gets next Id number and copies manager
  operator long() const {return _value;}  // automatic conversion to long
  TrkIdManager* idManager() const;
  void setIdManager(TrkIdManager* idMan);  // hack for making trks from db
  
private:	

  int _value;
  TrkIdManager* _idman;
};

#endif


