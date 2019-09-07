/*
 *  IssueFactoryIssue.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 04.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include "ers/IssueFactoryIssue.h"
#include <sstream>


const char* const ers::IssueFactoryIssue::CLASS_NAME = "ers::IssueFactoryIssue" ; 

namespace {
    ers::Issue *create_issue() { return new ers::IssueFactoryIssue(); } 
    bool registered = ers::IssueFactory::instance()->register_issue(ers::IssueFactoryIssue::CLASS_NAME,create_issue) ;
} 

/** \overload 
  */

ers::IssueFactoryIssue::IssueFactoryIssue() : Issue() {} 

/** \overload
  */

ers::IssueFactoryIssue::IssueFactoryIssue(const Context &context, severity_t s): Issue(context,s) {}

/** Constructs an Issue factory Issue
  * \param context the context of the issue, use \c MRS_HERE 
  * \param s the severity_t of the issue
  * \param name the name of the class requested to the factory 
  * \param msg the error message associated with the issue 
  */


ers::IssueFactoryIssue::IssueFactoryIssue(const Context &context, severity_t s, const std::string&name, const std::string &msg) : Issue(context,s) {
    std::ostringstream m ;
    m << "Unable to build Issue for name '" << name << "': " << msg ;
    finish_setup(m.str());
} // IssueFactoryIssue


const char *ers::IssueFactoryIssue::get_class_name() const throw() { return CLASS_NAME ; }


