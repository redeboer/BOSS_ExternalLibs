/*
 *  EntityNotFoundIssue.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 09.02.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include "ers/Issue.h"

namespace ers {
    
    /** This class represents the issue where a entity was searched for but not found.
      * Typically when a search fails, instances of this class (or subclasses) should be thrown or
      * returned 
      * \author Matthias Wiesmann
      * \version 1.0 
      */
    
    class EntityNotFoundIssue : public Issue {
	
protected:
	EntityNotFoundIssue(const ers::Context &c, ers::severity_t s) ; 
public:
	static const char* const CLASS_NAME ;            /**< \brief Class name */
	static const char* const ENTITY_NAME_KEY ; 
	static const char* const ENTITY_TYPE_KEY ; 
	static const char* const ENTITY_CONTEXT_KEY ;
	EntityNotFoundIssue();
	EntityNotFoundIssue(const ers::Context &c, ers::severity_t s, 
			    const std::string &entity_name, 
			    const std::string &entity_type,
			    const std::string &entity_context) ; 
	virtual const char*get_class_name() const throw() ;
	
    } ; // EntityNotFoundIssue
    
} // ers


