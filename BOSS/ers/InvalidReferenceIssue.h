/*
 *  InvalidReferenceIssue.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 04.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#ifndef ERS_INVALID_REFERENCE
#define ERS_INVALID_REFERENCE

#include "ers/Issue.h"

namespace ers {

    /** This issue represents an invalid reference. 
      * This class can be used for any type of reference, 
      * but most of the present mechanisms are designed for C/C++ memory references,
      * aka. pointers.
      * \author Matthias Wiesmann
      * \version 1.0
      * \brief Invalid pointer issue. 
      */
    
    class InvalidReferenceIssue : public Issue {
protected:
	void pointer(const void *p); 
	InvalidReferenceIssue(const Context &context, severity_t s) ; 
public:
	static const char* const CLASS_NAME ; 
	static const char* const REFERENCE_VALUE_KEY ;
	static const char* const REFERENCE_TYPE_KEY ; 
	static const char* const REFERENCE_NAME_KEY ; 
	static const char* const REFERENCE_C_PTR_TYPE ; 
	InvalidReferenceIssue() ;
	InvalidReferenceIssue(const Context &context, severity_t s, const void* pointer, const std::string &message, responsibility_t  r = resp_unknown) ; 
	virtual const char*get_class_name() const throw() ;
	static void check_reference(const Context &context, severity_t s, const void* pointer, const char* ptr_name, responsibility_t  r); 
    } ; // InvalidReferenceIssue
    
} // ers

/** \def ERS_PRE_CHECK_PTR(p) checks that C/C++ pointer \c p is valid (non null) and throws an ers::InvalidReferenceIssue if not
  * If the pointer is invalid, this is considered as a client responsibility. 
  * \note Do not use this to check constant pointers, as the check is compiled out if the pointer is constant 
  */

#if (!defined(N_ERS_ASSERT))
#if (defined(__GNUC__))
#define ERS_PRE_CHECK_PTR(p) if (! __builtin_constant_p(p)) ers::InvalidReferenceIssue::check_reference(ERS_HERE,ers::error,p,#p,ers::resp_client) ;
#else 
#define ERS_PRE_CHECK_PTR(p) ers::InvalidReferenceIssue::check_reference(ERS_HERE,ers::error,p,#p,ers::resp_client)
#endif
#else
#define ERS_PRE_CHECK_PTR(p) 
#endif

/** \def ERS_CHECK_PTR(p) checks that C/C++ pointer \c p is valid (non null) and throws an ers::InvalidReferenceIssue if not
 * If the pointer is invalid, this is considered as a server responsibility. 
 * \note Do not use this to check constant pointers, as the check is compiled out if the pointer is constant 
 */

#if (!defined(N_ERS_ASSERT))
#if (defined(__GNUC__))
#define ERS_CHECK_PTR(p) if (! __builtin_constant_p(p)) ers::InvalidReferenceIssue::check_reference(ERS_HERE,ers::error,p,#p,ers::resp_server)
#else 
#define ERS_CHECK_PTR(p) ers::InvalidReferenceIssue::check_reference(ERS_HERE,ers::error,p,#p,ers::resp_server)
#endif
#else 
#define ERS_CHECK_PTR(p) 
#endif

#endif

