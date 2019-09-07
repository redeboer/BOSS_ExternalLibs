/*
 *  DefaultIssue.h
 *  Test
 *
 *  Created by Matthias Wiesmann on 14.02.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#ifndef ERS_DEFAULT_ISSUE
#define ERS_DEFAULT_ISSUE

#include "ers/Issue.h"
#include <cstdlib> 

namespace ers {
    
    /** This class acts as a placeholder for issues whose name could 
      * not be resolved at runtime. 
      * Because of this, it does not have the usual constructor 
      * and also does not have an associated class name, instead the 
      * class simply keeps the class name stored in the field \c m_class_name.
      * \author Matthias Wiesmann
      * \brief Place holder Issue class
      * \version 1.0
      * \note To force the resolution of an issue (i.e try again to cast it to the correct run-time type),
      *       One simply needs to call the \c IssueFactory::build method with the instance as a parameter. 
      */
    
    class DefaultIssue : public Issue {
protected:
public:
	DefaultIssue(const std::string &name) ; 
	~DefaultIssue() throw() ; 
	virtual const char *get_class_name() const throw() ;           /**< \brief Get key for class (used for serialisation)*/
    } ; // Issue
}  // ers

#endif
