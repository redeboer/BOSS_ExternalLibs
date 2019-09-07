// File and Version Information:
//      $Id: AbsEvtObj.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//      implementation for AbsEvtObj class
//
//
// Author List:
//      Bob Jacobsen
//
// 
//------------------------------------------------------------------------

//#include "BaBar/BaBar.hh"

//-----------------------
// This Class's Header --
//-----------------------
#include <assert.h>
#include "TrkBase/AbsEvtObj.h"
#include <iostream>
using std::cout;
using std::endl;
using std::ostream;

//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

//--------------
// Destructor --
//--------------
AbsEvtObj::~AbsEvtObj() {}

void AbsEvtObj::print(ostream& ) const {
  // single line print; this is a default behavior
  cout << "(AbsEvtObj print() called with no override; this is an error!)";
}

void AbsEvtObj::printAll(ostream& ) const {
  // single line print; this is a default behavior
  cout << "AbsEvtObj printAll() called with no override; this is an error!"<<endl;
}


ostream&  operator << (ostream& o, const AbsEvtObj& a) {a.print(o); return o;}


