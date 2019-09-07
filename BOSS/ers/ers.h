/*
 *  ers.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 26.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

/** \file ers.h This file includes all the main headers of the Error Reporting System.
  * It does not declare anything <em>per se</em>, it does contains the doxygen code to
  * generate all the general documentation for the package. 
  * \author Matthias Wiesmann
  * \version 1.1 Removed dependency on System package
  * \brief ers header and documentation file 
  */

#include "ers/Core.h"
#include "ers/Context.h"

#include "ers/Issue.h"
#include "ers/IssueFactory.h"
#include "ers/Stream.h"
#include "ers/StreamFactory.h"

#include "ers/Assertion.h"
#include "ers/Precondition.h"
#include "ers/InvalidReferenceIssue.h"
#include "ers/NotImplemented.h"
#include "ers/LogIssue.h"
#include "ers/RangeIssue.h"

/** \page ERSHowTo How to use the ERS package
  The goal of the Error Reporting System is to offer tool to simplify and unify error handling and error reporting 
  in TDAQ applications. 
  This package can be used at different levels. At the lowest level, one can simply use the existing checking macros. 
  A more advanced way of using this package is to define specific Issue subclasses. 
  \section Macros Basic macros
  Basic ERS functionality can be accessed by using simple macros. 
  Those macros are available simply by including the ers/ers.h header file. 
  \subsection AssertionMacros Assertion Macros
  The ERS package offers a set of macros to do basic checking. 
  Those macros generally behave like the standard C assert macros. 
  They have the following features:<ul>
  <li>Different types of checks:
      <ul>
      <li>Compile time checks (\c ERS_STATIC_ASSERT) those should be used to verity certain assertions at compile-time.
          For instance if you code rely on certain data structures having certain memory sizes, this can be checked using
          static assertions. 
      <li>Preconditions (\c ERS_PRECONDITION) those should be used to check condition when entering functions. 
          For instance if you only accept certain values for parameters precondtions should verify that those conditions 
          are resepected. 
      <li>Pointer precondition (\c ERS_PRE_CHECK_PTR) 
          this special type of pre-condition checks that a pointer is valid (not null), 
          you should this macro to check all pointers that a function or method receives as parameters. 
      <li>Pointer checking (\c ERS_CHECK_PTR)
          this type of checks is used to verify general pointers internally. 
      <li>General assertion (\c ERS_ASSERT)
	  this macro can be used for checks that do not fit into any other category. 
      </ul></li>
  <li>Macros are compiled out if macro \c N_DEBUG is defined</li>
  <li>Throws complete ERS issues, they can be caught streamed, modified etc.</li>
  <li>Preconditions and general assertions support variable number of parameters and \c fprintf like formatting</li>
  <li>Runtime assertion can detect if they are invariant conditions</li>
  </ul>
  All macros throw Issues object as exceptions with a severity_t of \c ers::error. 
  The main difference between the different macros is that the Issue object they generate in case of violated condition
  contains different information fields. For instance all precondition macros generate issue that specify that the
  responsiblity for the Issue lies in the caller, not the callee. 
  \see ers::Assertion
  \see ers::Precondition
  \see ers::InvalidReferenceIssue

  \subsection LoggingMacros Logging Macros
  The ERS package offers a set of macros to do logging. Those macros construct an issue and send them to the relevant stream. 
  They all support multiple number of parameters with \c fprintf like patterns.
  For each debug and warning severity_t level there is an associated macro:
  \li ERS_DEBUG_0
  \li ERS_DEBUG_1
  \li ERS_DEBUG_2
  \li ERS_DEBUG_3
  \li ERS_WARN

  The actual behaviour of the macro can be set up either at compile or runtime. 
  Debug macros with levels larger than 0 can be disabled at compile time simply by defining the DEBUG_LEVEL.
  For instance, if DEBUG_LEVEL is 1, then ERS_DEBUG_2 and ERS_DEBUG_3 are compiled out. 
  At runtime the stream associated with a given severity can be disabled by associated a null stream with the severity. 
  A filtering stream can also be associated with the severity level. 
  The different types of streams supported by the package and the debug and warning macros 
  are presented in the documentation for class ers::StreamFactory 
  \see ers::StreamFactory 

  \section Issues Building Custom Issues
  For a discussion about building custom issue, see the documentation for the Issue class. 
  An example custom issue is also given in the example directory. 
  \see ers::Issue 
  \see ExampleIssue 
  \see ExampleIssue.h

  \section Selecting Streams
  The ERS system use the abstraction of streams to handle issues. 
  Conceptualy, a stream is simply an object that can the user can use to send (or receive) Issues. 
  There is one stream associated with each severity level,
  those streams can be set or retrieved using methods of the ers::StreamFactory class. 
  \see ers::StreamFactory   

  \section Exit values 
  The ERS system offers some facilities to give out standard compliant exit values for programs. 
  That is, if a program exists because of an ers Issue, it can call the \c exit_value method on the issue
  to possibly get an appropriate exit value (as defined in sysexits.h).
  A typical main program would look like this:
  \code 
  int main(int argc, char** argv) {
      try {
	  do_stuff(); 
      } catch (ers::Issue &e) {
	  ers::StreamFactory::dispatch(e); // we send the issue to the appropriate stream 
	  exit(e.exit_value());            // we get the actual exit value from the issue 
      } // try / catch 
  } // main 
  \endcode
  \see http://www.gsp.com/cgi-bin/man.cgi?section=3&topic=sysexits

  \section FAQ FAQ
  \subsection Assertion-Precondition What is the difference between an assertion and a precondition?
  An precondition is a condition that needs to be respected when \e entering a function, 
  an assertion is a condition that needs to be respected \e inside a function. 
  A failed precondition indicates that the problem lies outside of the function, a failed assertion
  indicates that the problem lies inside the function. 
  \see ers::Assertion
  
  \subsection ERS_HERE What is the macro ERS_HERE?
  The macro ERS_HERE is used to insert all the context information to a ers issue. 
  When constructing an issue one should always give the macro ERS_HERE as a first parameter. 
  \see ers::Context

  \subsection factory Do I need to implement the registration and factory methods for each of my Issues?
  The registration and factory mechanism as implemented in the ExampleIssue class is needed for some functionality of ERS. 
  If it is not present, the system will work but certain features will be disabled. 
  In particular the absence of factory method means that ERS cannot dynamically build instance with the precise C++ type. 
  For instance if you instanciate an CustomIssue that does not declare a factory method, if 
  you serialise this issue to a file and then deserialise it, it will not have the type CustomIssue, but instead will have
  the type DefaultIssue. The same happens if an Issue is caused by another issue, the cause issue is cloned into the 
  second issue, so if there is no factory method, the clone issue will have the generic type. 

*/
