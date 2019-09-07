//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: AbsEvtObj.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//      Class AbsEvtObj header file
//
//      An AbsEvtObj is an abstract base class for holding common
//      method definitions for all objects in an AbsEvent
//
//      The most important of these virtual methods is the destructor,
//      used to break dependencies on teh various class definitions
//
//      Three forms of print are also declared.  
//        stream << operator:
//           summarize object as a string, without using endl or newlines
//        print():
//           equivalent to the << operator
//        printAll():
//           output the entire contents of the object, using multiple
//           lines if needed.  Should end with endl
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Bob Jacobsen            LBNL
//      Ed Frank                University of Pennsylvania
//
// Copyright Information:
//      Copyright (C) 1995      LBNL
//
// History
//    ??xxx??  BobJ             First versions
//    02Feb97  Ed Frank         Inherit from ProxyDict/DictData
//
// BUGS:
//    Any given object should have exactly one DictData base class.
//    There is a question of whether to make AbsEvtObj have DictData
//    as a public base or as a virtual public base.  I've left it as
//    the former, thus leaving the design requirment of
//    "just one DictData base" as a responsibility of the user.  I'm
//    not sure if this is the right thing to do.  Comments are welcome.
//    If you are confused: the context is multiple inheritance
//    usage cases.
//------------------------------------------------------------------------
#ifndef ABSEVTOBJ_H
#define ABSEVTOBJ_H
//#include "BaBar/BaBar.hh"

#include <assert.h>
#include <iosfwd>
//class AbsEvent;
//class ToyCacheMgr;

//
// Class Definition
//

class AbsEvtObj {

public:

  virtual ~AbsEvtObj();

  // There are three different print forms:
  // 'print(cout)', used by and equivalent to <<, occupies exactly one line
  virtual void print(std::ostream& o) const;

  // 'printAll(cout)', is allowed to take multiple lines and should
  // dump entire contents
  virtual void printAll(std::ostream& o) const;

  // used to setup any needed internal pointers for I/O
  // right now, this is not used. Its intended as a post-load dbio setup.
//  virtual void loadPtrs(const AbsEvent* ) {};
//  virtual void savePtrs(const AbsEvent* ) {};

  //This is a fix for ToyPrototype
//    virtual void bindCacheMgr(ToyCacheMgr*) {};
  
  // op == is needed so these can be held in Tools.h++ collections
  inline bool operator== (const AbsEvtObj& a) const { return this==&a; }

protected:
  // default constructor is protected to make it harder to 
  // instantiate by accident
  
  AbsEvtObj() {};

  // assignment operator protected to avoid use except by subclasses
  AbsEvtObj& operator = (const AbsEvtObj&) {return *this;};

};

// The third print form is the stand alone:
std::ostream&  operator << (std::ostream& o, const AbsEvtObj&);

#endif
