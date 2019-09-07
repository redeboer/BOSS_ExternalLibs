/*
 *  IssueFactory.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 30.11.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#include <iostream>
#include <cstdlib> 
#include "ers/IssueFactory.h"
#include "ers/Issue.h"
#include "ers/DefaultIssue.h"
#include "ers/Precondition.h"
#include "ers/IssueFactoryIssue.h"
#include "ers/InvalidReferenceIssue.h"

ers::IssueFactory  *ers::IssueFactory::s_factory = 0 ; 

ers::IssueFactory::IssueFactory() {}  

/** Finds the singleton instance of the factory.
  * If the instance is not allocated yet, it is. 
  * \return a pointer to the singleton instance 
  */

ers::IssueFactory *ers::IssueFactory::instance()  {
    if (0==s_factory) {
	s_factory = new IssueFactory();
    } // creation needed
    return s_factory ; 
} // instance

void ers::IssueFactory::print_registered() {
    IssueFactory *factory = instance() ; 
    std::cerr << *factory; 
} //print_registered_issues


/** Register an issue type with the factory 
  * \param name the name that will be used to lookup new instances 
  * \param creator a pointer to the function used to create new instance for that particular type of function
  * \return \c true if the name was already present in the table
  */

bool ers::IssueFactory::register_issue(const std::string &name, CreateIssueCallback creator) {
    return m_factory_map.insert(CallbackMap::value_type(name,creator)).second; 
} // register_Issue

/** Builds an issue out of the name it was registered with 
  * \param name the name used to indentify the class 
  * \return an newly allocated instance of type \c name or DefaultIssue 
  * \note If the requested type cannot be resolved an instance of type DefaultIssue 
  */

ers::Issue *ers::IssueFactory::build(const std::string &name) const {
    CallbackMap::const_iterator i = m_factory_map.find(name); 
    if (i == m_factory_map.end()) {
	// throw ERS_ISSUE_FACTORY_ERROR(name,"issue not registred") ; 
	return new DefaultIssue(name); 
    } // Not found
    ers::Issue *issue = (i->second)() ; 
    if (0==issue) {
	throw ERS_ISSUE_FACTORY_ERROR(name,"factory function returned null"); 
    } // issue null 
    return issue ; 
} // build

/** Builds an issue out of a name and a value table 
  * @param name the name used to indentify the class 
  * @param values the value-table containing the state for the Issue
  * @return an newly allocated instance of this type or null if the name is not registered 
  */

ers::Issue *ers::IssueFactory::build(const std::string &name, const ers::string_map_type *values) const {
    Issue *i = build(name);
    if (i) {
	i->set_values(*values); 
    } // if i
    return i ; 
} // build

/** Clones an Issue.
  * This method creates (using the other build methods) an new Issue with the same value table
  * @param original the Issue to be cloned 
  * @return a new cloned Issued
  * @note The problem of cause exception is not handled yet. 
  */

ers::Issue *ers::IssueFactory::build(const Issue *original) {
    ERS_PRE_CHECK_PTR(original); 
    const std::string name = original->get_class_name() ; 
    const ers::string_map_type *values = original->get_value_table();
    ERS_ASSERT(values!=0,"null value table for original"); 
    ers::Issue *clone_issue = build(name,values);
    return clone_issue ; 
}// build

void ers::IssueFactory::write_to(std::ostream& stream) const {
    stream << "Issue factory - registered issues\n" ; 
    stream << "---------------------------------\n" ; 
    int i = 0 ; 
    for(CallbackMap::const_iterator pos=m_factory_map.begin();pos!=m_factory_map.end();++pos) {
	std::string name = pos->first; 
	stream << i << ")\t" << name << std::endl; 
	i++ ; 
    } // for
    stream << "---------------------------------\n" ; 
} // 

std::ostream& ers::operator<<(std::ostream& stream, const IssueFactory& factory) {
    factory.write_to(stream);
    return stream ; 
} // operator



