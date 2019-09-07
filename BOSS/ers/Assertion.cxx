/*
 *  Assertion.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 26.11.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */


#include <iostream>
#include <sstream>
#include <sysexits.h>

#include "ers/Assertion.h"
#include "ers/Issue.h"


namespace {
    ers::Issue *create_issue() { return new ers::Assertion(); } 
    bool registered = ers::IssueFactory::instance()->register_issue(ers::Assertion::CLASS_NAME,create_issue) ;
} 

/** Key for assertion condition description 
 */

const char * const ers::Assertion::ASSERT_CONDITION_KEY = "ASSERTION_CONDITION" ; 
const char * const ers::Assertion::CLASS_NAME = "ers::Assertion" ;
const  char * const ers::Assertion::MESSAGE_ELEMENTS[] = { "Assertion '", "' failed: ", " (this condition is constant)" } ;  

/** Empty constructor for deserialisation / factory 
  */

ers::Assertion::Assertion() : Issue() {} 

/** Constructor for subclasses 
  */

ers::Assertion::Assertion(const Context &context, severity_t s) : Issue(context,s) {} 

/** Constructor 
  * \param condition_text the text of the assertion's condition
  * \param msg message describing the assertion's condition 
  * \param s severity_t of the assertion 
  * \param context position in the code where the assertion failed, should be the MRS_HERE macro 
  * \param constant_expression is the expression constant (as detected by the compiler).
  */

ers::Assertion::Assertion(const Context &context, severity_t s, const char* condition_text, const std::string &msg, bool constant_expression) : Issue(context,s) {
    setup(condition_text,msg,constant_expression);
} // Assertion

/** Setup method, should be called at the end of the constructor by class and subclasses 
  */

void ers::Assertion::setup(const char *condition_text, const std::string &msg, bool constant_expression)  {
    m_value_table[ASSERT_CONDITION_KEY] = *condition_text ;
    responsibility(resp_server); 
    set_value(EXIT_VALUE_KEY,EX_SOFTWARE);
    transience( ! constant_expression); 
    finish_setup(build_message(condition_text,msg,constant_expression));
} // setup

const char* ers::Assertion::get_class_name() const throw (){
    return CLASS_NAME ; 
} // get_class_name

/** Builds the message associated with an assertion 
  * \return message describing the failure of the assertion 
  * \param condition_text the condition in textual form 
  * \param msg the message associated with the condition
  * \param constant_expression is the assertion a constant expression as detected by the compiler. 
  * \return an human readable message 
  */

std::string ers::Assertion::build_message(const char* condition_text, const std::string &msg, bool constant_expression) throw() {
    std::ostringstream m ;
    m << MESSAGE_ELEMENTS[0] << condition_text << MESSAGE_ELEMENTS[1]  << msg ;
    if (constant_expression) {
	m << MESSAGE_ELEMENTS[2]  ; 
    } // constant expression 
    return m.str();
} // build_message

