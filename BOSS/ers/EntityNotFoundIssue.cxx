/*
 *  EntityNotFoundIssue.cxx
 *  Test
 *
 *  Created by Matthias Wiesmann on 09.02.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include "ers/EntityNotFoundIssue.h"

#include <sstream>

const char* const ers::EntityNotFoundIssue::CLASS_NAME = "ers::EntityNotFoundIssue" ; 
const char* const ers::EntityNotFoundIssue::ENTITY_NAME_KEY = "ENTITY_NAME" ; 
const char* const ers::EntityNotFoundIssue::ENTITY_TYPE_KEY = "ENTITY_TYPE" ; 
const char* const ers::EntityNotFoundIssue::ENTITY_CONTEXT_KEY = "ENTITY_KEY" ;

namespace {
    ers::Issue *create_issue() { return new ers::EntityNotFoundIssue(); } 
    bool registered = ers::IssueFactory::instance()->register_issue(ers::EntityNotFoundIssue::CLASS_NAME,create_issue) ;
} 

const char * ers::EntityNotFoundIssue::get_class_name() const throw() { return CLASS_NAME ; } 

/** \overload */
ers::EntityNotFoundIssue::EntityNotFoundIssue() : ers::Issue() {} 

/** \overload */
ers::EntityNotFoundIssue::EntityNotFoundIssue(const ers::Context &c, ers::severity_t s) : ers::Issue(c,s) {}

/** Constructor 
  * \brief entity not found constructor
  * \param c the ers context where the issue occured
  * \param s the ers severity_t of the issue 
  * \param entity_name the name of the entity that was not found 
  * \param entity_type the type of the entity that was not found 
  * \param entity_context the name of the context where the entity was searched for
  */

ers::EntityNotFoundIssue::EntityNotFoundIssue(const ers::Context &c, ers::severity_t s, const std::string &entity_name, const std::string &entity_type, const std::string &entity_context) : Issue(c,s) {
    set_value(ENTITY_NAME_KEY,entity_name);
    set_value(ENTITY_TYPE_KEY,entity_type);
    set_value(ENTITY_CONTEXT_KEY,entity_context) ; 
    std::ostringstream stream ; 
    stream << "Cannot find " << entity_type << "\"" << entity_name << "\" in " << entity_context ;
    finish_setup(stream.str()); 
} // EntityNotFoundIssue
