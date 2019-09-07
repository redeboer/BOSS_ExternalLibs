/*
 *  Assertion.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 26.11.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

/** \file Assertion.h
  * This file defines the assertion class and the associated macros. 
  */

#ifndef ERS_ASSERTION
#define ERS_ASSERTION

#include <stdio.h>
#include "ers/Issue.h"

namespace ers {
    
/** This class represents an assertion in the code, it is an issue with two fields: 
  * \li A condition 
  * \li A message describing the condition 
  *
  * If the condition is not verified, an Issue is trown. 
  * \author Matthias Wiesmann
  * \version 1.0
  * \brief This Issue represents a basic assertion
  */
    
    class Assertion: public ers::Issue {
protected:
    virtual std::string build_message(const char* condition_text, const std::string &msg, bool constant_expression=false) throw() ; 
    Assertion(const Context &context, severity_t s); 
    void setup(const char *condition, const std::string &message, bool constant_expression); 
    static const char* const MESSAGE_ELEMENTS[] ; 
public:
    static const char* const ASSERT_CONDITION_KEY ;
    static const char* const CLASS_NAME ; 
     
    virtual const char*get_class_name() const throw() ;
    Assertion(); 
    Assertion(const Context &context, severity_t s, const char*condition, const std::string &message,  bool constant_expression=false) ;
} ; 

/** This structure is simply used to trigger compile-time assertion errors 
  * The \c true template instanciation is implemented, but not the false, this means that if the template
  * is instanciated with \c false, we get a compile-time error.
  * \brief compile time error structure. 
  */

template <bool> struct Compile_time_error ; 

/** \brief compile time error structure */

template<> struct Compile_time_error<true> { };

} // ers 



/** \def ERS_STATIC_ASSERT(expr) This macro inserts a compile-time assertion into the code. 
 * Compile time assertion can only be done on constant factors (i.e compile-time known quantities).  
 *
 */

#ifndef N_ERS_STATIC_ASSERT
#define ERS_STATIC_ASSERT(expr) { ers::Compile_time_error <((expr) != 0)> ERROR_ASSERTION_FAILED ;   (void) ERROR_ASSERTION_FAILED ; }
#else 
#define ERS_STATIC_ASSERT(expr) 
#endif 


/** \def ERS_ASSERT(expr,msg,...) This macro inserts an assertion than checks condition e, 
 * if e is not true, then an issue of type ers::Asertion is thrown with message msg. 
 * The msg is actually a formatting string, like printf, that can be used with subsequent parameters. 
 * If the compiler is gcc, then the transient field of the assertion is set according to the transcience of the expression. 
 * This means that if the expression is detected by the compiler as being constant, 
 * \note This macro is disabled if the \c N_ERS_ASSERT macro is defined 
 */
#ifndef N_ERS_ASSERT
#ifdef __GNUC__
#define ERS_ASSERT(expr,...) { if(!(expr)) { char assertion_buffer[256] ; snprintf(assertion_buffer,sizeof(assertion_buffer), __VA_ARGS__) ; ers::Assertion failed_assertion(ERS_HERE, ers::error, #expr,assertion_buffer,__builtin_constant_p(expr)) ; throw failed_assertion ; } }
#else 
#define ERS_ASSERT(expr,...) { if(!(expr)) { char assertion_buffer[256] ; snprintf(assertion_buffer,sizeof(assertion_buffer), __VA_ARGS__) ; ers::Assertion failed_assertion(ERS_HERE, ers::error,#expr,assertion_buffer,false) ; throw failed_assertion ; } }
#endif
#else 
#define ERS_ASSERT(expr,...) ((void) (expr))
#endif

#endif

