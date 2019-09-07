/*
 *  InvalidReferenceIssue.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 04.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include "ers/InvalidReferenceIssue.h"
#include <sstream>
#include <cstdio>

const char * const ers::InvalidReferenceIssue::CLASS_NAME = "ers::InvalidReferenceIssue" ; 

const char * const ers::InvalidReferenceIssue::REFERENCE_VALUE_KEY = "REFERENCE_VALUE" ; 
const char * const ers::InvalidReferenceIssue::REFERENCE_TYPE_KEY = "REFERENCE_TYPE" ; 
const char * const ers::InvalidReferenceIssue::REFERENCE_NAME_KEY = "REFERENCE_NAME" ; 
const char * const ers::InvalidReferenceIssue::REFERENCE_C_PTR_TYPE = "C Pointer" ; 

namespace {
    ers::Issue *create_issue() { return new ers::InvalidReferenceIssue(); } 
    bool registered = ers::IssueFactory::instance()->register_issue(ers::InvalidReferenceIssue::CLASS_NAME,create_issue) ;
} // anonymous context

ers::InvalidReferenceIssue::InvalidReferenceIssue() : Issue() {} 

ers::InvalidReferenceIssue::InvalidReferenceIssue(const Context &context, severity_t s) : Issue(context,s) {} 

ers::InvalidReferenceIssue::InvalidReferenceIssue(const Context &context, severity_t s, const void* p, const std::string &msg, responsibility_t  r) : Issue(context,s) {
    severity(s);
    pointer(p); 
    responsibility(r) ; 
    finish_setup(msg); 
} // InvalidReferenceIssue


const char* ers::InvalidReferenceIssue::get_class_name() const throw() {
    return ers::InvalidReferenceIssue::CLASS_NAME ; 
} // get_class_name


void ers::InvalidReferenceIssue::pointer(const void *p) {
    char buffer[sizeof(void *)+1] ; // space needed to print a pointer
    snprintf(buffer,sizeof(buffer),"%p",p); 
    set_value(REFERENCE_VALUE_KEY,buffer) ; 
    set_value(REFERENCE_TYPE_KEY,REFERENCE_C_PTR_TYPE) ; 
} // pointer

void ers::InvalidReferenceIssue::check_reference(const Context &context, severity_t s, const void* p, const char* ptr_name, responsibility_t  r) {
    if (p==0) {
	std::ostringstream message ;
	message << "pointer '" << ptr_name << "' is null" ; 
	ers::InvalidReferenceIssue issue(context,s,p,message.str(),r) ;
	issue.m_value_table[REFERENCE_NAME_KEY] = ptr_name ; 
	throw issue ; 
    } // if
} // check
    



