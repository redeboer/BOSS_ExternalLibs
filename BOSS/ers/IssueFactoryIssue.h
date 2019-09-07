/*
 *  IssueFactoryIssue.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 04.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#ifndef ERS_ISSUE_FACTORY_ISSUE
#define ERS_ISSUE_FACTORY_ISSUE

#include "ers/Issue.h"

namespace ers {
    
    /** This class represents an issue that occurs in the Issue factory.
      * Those issue typically mean that a Issue object could not be deserialized. 
      * \author Matthias Wiesmann
      * \version 1.0
      * \brief Issue in the Issue factory mechanism. 
      */
    
    class IssueFactoryIssue : public Issue {
	
protected:
	IssueFactoryIssue(const Context &context, severity_t s); 
public:
	static const char* const CLASS_NAME ; 
	
	IssueFactoryIssue() ; 
	IssueFactoryIssue(const Context &context, severity_t s, const std::string&name, const std::string &message); 
	
	const char *get_class_name() const throw() ; 
    } ; // IssueFactoryIssue
    
    
} // ers

/** \def ERS_ISSUE_FACTORY_ERROR(name,message) Macro that inserts the ERS_HERE and the error value for IssueFactoryIssue constructor
  */

#define ERS_ISSUE_FACTORY_ERROR(name,message) IssueFactoryIssue(ERS_HERE,ers::error,name,message) 

#endif

