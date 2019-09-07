/*
 *  RangeIssue.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 15.02.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include "ers/RangeIssue.h"
#include <sstream>


const char * const ers::RangeIssue::CLASS_NAME = "ers::RangeIssue" ; 
const char * const ers::RangeIssue::INDEX_REFERENCE_TYPE = "scalar" ; 
const char * const ers::RangeIssue::RANGE_MIN_KEY = "REFERENCE_RANGE_MIN" ; 
const char * const ers::RangeIssue::RANGE_MAX_KEY = "REFERENCE_RANGE_MAX" ;


namespace {
    ers::Issue *create_issue() { return new ers::RangeIssue(); } 
    bool registered = ers::IssueFactory::instance()->register_issue(ers::RangeIssue::CLASS_NAME,create_issue) ;
} // anonymous context

void ers::RangeIssue::check_range(const ers::Context &c, int64_t min, int64_t value, int64_t max, const char* entity_name) {
    if ((value>max) || (value<min)) throw RangeIssue(c,ers::error,min,value,max,entity_name); 
} // check_range


void ers::RangeIssue::set_range(ers::Issue &issue, int64_t index, int64_t min_index, int64_t max_index, const char* entity_name) {
    issue.set_value(REFERENCE_VALUE_KEY,index) ; 
    issue.set_value(RANGE_MIN_KEY,min_index);
    issue.set_value(RANGE_MAX_KEY,max_index);
    std::ostringstream reference_stream ;
    reference_stream << INDEX_REFERENCE_TYPE ; 
    if (entity_name) {
	reference_stream << "(name=" << entity_name << ')' ; 
    } 
    issue.set_value(REFERENCE_TYPE_KEY,reference_stream.str()) ; 
} // set_range

ers::RangeIssue::RangeIssue() : ers::InvalidReferenceIssue() {} 

ers::RangeIssue::RangeIssue(const ers::Context &c, ers::severity_t s) : ers::InvalidReferenceIssue(c,s) {
    set_value(REFERENCE_TYPE_KEY,INDEX_REFERENCE_TYPE) ;
} // RangeIssue

ers::RangeIssue::RangeIssue(const ers::Context &c, ers::severity_t s, int64_t min_index, int64_t index, int64_t max_index, const char* entity_name) : ers::InvalidReferenceIssue(c,s) {
    set_range(*this,index,min_index,max_index,entity_name) ; 
    std::ostringstream message_stream ;
    message_stream << "invalid index: " ; 
    if (entity_name) {
	message_stream << entity_name << '='; 
    } // entity
    message_stream  << index << " is not in range " << min_index << '-' << max_index ;  
    finish_setup(message_stream.str()); 
} // RangeIssue

const char*ers::RangeIssue::get_class_name() const throw() { return CLASS_NAME ; } 

