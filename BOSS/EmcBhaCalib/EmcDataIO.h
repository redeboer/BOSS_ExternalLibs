//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//	Copyright (C) 2005         IHEP
//
//------------------------------------------------------------------------

#ifndef EMCDATAIO_H
#define EMCDATAIO_H

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <iostream>
//---------------
// Gaudi Headers --
//---------------
#include "GaudiKernel/MsgStream.h"

using namespace std;


//              ---------------------
//              -- Class Interface --
//              ---------------------

/**
  *  Data input / output of Emc calibration using root.,
  *
  *  @see 
  *
  *  @author Chunxiu Liu	       (originator/contributor etc.);
  */

class EmcDataIO { 

//--------------------
// Instance Members --
//--------------------

public:

  //Constructors
  EmcDataIO();

  //copy
  EmcDataIO(const EmcDataIO &m1 );


  // Destructor
  ~EmcDataIO();



protected:

 

private:

  // Friends

  // Data members
 
  IMessageSvc* _msgSvc() const;
  std::string name() const { return "EmcDataIO"; }

};

#endif /* EMCDATAIO_H */

