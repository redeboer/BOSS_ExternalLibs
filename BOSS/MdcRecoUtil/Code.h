//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: Code.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class Header for |Code|
//      Tell about success and failure
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A. Snyder
//
// Copyright Information:
//	Copyright (C) 1996	SLAC
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef Code_H
#define Code_H

#include <assert.h>
#include <stdlib.h>

//DEL class ostream;

class Code {

public:

  //constructors
  
  //default to success, default success code is 1
  Code(int s=1,int f=0):_fail(0),_success(0) 
  {
    if(f==0) {setSuccess(s);}
    else if(s==0) {setFail(f);}
  }

  //copy
  Code(const Code &c) 
    :_fail(c.fail()),_success(c.success())
  {}

  //access

  inline int fail()const {return _fail;}
  inline int success()const {return _success;}

  //set
  inline void setFail(int i) 
  {assert(i); _fail=i; _success=0;}
  inline void setSuccess(int i) 
  {assert(i); _success=i; _fail=0;}


private:

  //data

  int _fail;			// failure code
  int _success;			// success code

};

#endif
