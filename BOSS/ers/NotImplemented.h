/*
 *  NotImplemented.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 08.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#ifndef ERS_NOT_IMPLEMENTED

#include "ers/Assertion.h"

namespace ers {
    
    /** This special 'assertion' is used to mark a non implemented function or method. 
     *  Typically, this is marked using the NOT_IMPLEMENTED() macro. 
     *  \author Matthias Wiesmann
     *  \version 1.0
     *  \brief Macro to mark unimplemented code. 
     */
    
    class NotImplemented : public Assertion {
	
protected:
	virtual std::string build_message(const char* condition_text, const std::string &message, bool constant_expression = false) throw() ; 
public:
	static const char* const CLASS_NAME ; 
	NotImplemented();
	NotImplemented(const Context &context, severity_t s) ;
	virtual const char*get_class_name() const throw();
    } ;  // Precondition
} // ers

#define ERS_NOT_IMPLEMENTED() throw ers::NotImplemented(ERS_HERE,ers::error)
#define ERS_NOT_IMPLEMENTED_FATAL() throw ers::NotImplemented(ERS_HERE,ers::ers_fatal)

#endif

