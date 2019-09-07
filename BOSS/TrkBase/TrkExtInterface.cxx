//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkExtInterface.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
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

//#include "BaBar/BaBar.h"
#include "TrkBase/TrkExtInterface.h"
#include "TrkBase/TrkRep.h"
#include "ProxyDict/IfdIntKey.h"
//#include "ErrLogger/ErrLog.h"

//------------------------------------------------------------------------
TrkExtInterface::TrkExtInterface() : _myRep(0), _nonconst(false){
//------------------------------------------------------------------------
}
 
//------------------------------------------------------------------------
TrkExtInterface::~TrkExtInterface() {
//------------------------------------------------------------------------
}


//------------------------------------------------------------------------
bool
TrkExtInterface::attach(TrkRep* newRep) {
//------------------------------------------------------------------------
  bool retval(false);
  if (myKey() == newRep->myKey()) {
    setRep(newRep);
    retval = true;
  }
  return retval;
}

//------------------------------------------------------------------------
bool
TrkExtInterface::attach(const TrkRep* newRep) {
//------------------------------------------------------------------------
  bool retval(false);
  if (myKey() == newRep->myKey()) {
    setRep(newRep);
    retval = true;
  }
  return retval;
}

void
TrkExtInterface::setRep(const TrkRep* newRep) {
  _myRep = (TrkRep*) newRep;   
  _nonconst = false;
}

void
TrkExtInterface::setRep(TrkRep* newRep) {
  _myRep = newRep;   
  _nonconst = true;
}

//------------------------------------------------------------------------
TrkRep* 
TrkExtInterface::myRep() {
//------------------------------------------------------------------------
  if(_myRep != 0 && _nonconst)
    return _myRep;
  else if (0 == _myRep) {
    std::cout<<"ErrMsg(error) "<< "Cannot use interface without attached rep." 
		  << std::endl;
    return 0;
  } else
    std::cout<<"ErrMsg(error) "<< "Cannot return non-const rep after const attachment" 
		  << std::endl;
    return 0;
}

//------------------------------------------------------------------------
const TrkRep* 
TrkExtInterface::myConstRep() const {
//------------------------------------------------------------------------
  if (0 == _myRep) {
    std::cout<<"ErrMsg(error) "<< "Cannot use interface without attached rep." 
		  << std::endl;
    return 0;
  }
  return _myRep;
}

