/*
 *  LogIssue.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 06.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include "ers/LogIssue.h"

#include <sstream>

const char* const ers::LogIssue::CLASS_NAME = "ers::LogIssue" ; 

namespace {
    ers::Issue *create_issue() { return new ers::LogIssue(); } 
    bool registered = ers::IssueFactory::instance()->register_issue(ers::LogIssue::CLASS_NAME,create_issue) ;
} 

ers::LogIssue::LogIssue() : Issue() {} 
ers::LogIssue::LogIssue(const ers::Context &c, severity_t s) : ers::Issue(c,s) {}

ers::LogIssue::LogIssue(const ers::Context &c, severity_t s, const std::string &msg) : Issue(c,s) {
    finish_setup(msg); 
} // 

const char* ers::LogIssue::get_class_name() const throw() { return ers::LogIssue::CLASS_NAME ; }
