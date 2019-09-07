/*
 *  IssueFactory.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 30.11.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#ifndef ERS_ISSUE_FACTORY
#define ERS_ISSUE_FACTORY

#include <string>
#include <map>
#include "ers/Core.h"

namespace ers {

class Issue ; 
    
    /** This class implements the factory pattern for Issues.
      * The main responsability of this class is to keep track of the existing types of Issues
      * Each issue should register one factory method for instances of this class.
      * This is needed for deserializing of Issues. 
      * 
      * \author Matthias Wiesmann
      * \version 1.0
      * \brief Factory for all Issues 
      */
    
class IssueFactory {
public:
    typedef Issue* (*CreateIssueCallback)();
    typedef std::map<std::string,CreateIssueCallback> CallbackMap ; 
protected:
    IssueFactory(); 
    CallbackMap m_factory_map ; 
    static IssueFactory  *s_factory ; 
     
public:
    static IssueFactory *instance() ;                                             /**< \brief method to access singleton */
    static void print_registered();                                               /**< \brief prints all registered issue types */
    bool register_issue(const std::string &name, CreateIssueCallback creator) ;   /**< \brief register an issue factory */
    Issue *build(const std::string &name) const ;                                 /**< \brief build an empty issue out of a name */
    Issue *build(const std::string &name, const string_map_type *values) const ;  /**< \brief build a issue out of name and set values */
    Issue *build(const Issue *original);                                          /**< \brief build a clone of an issue */
    void write_to(std::ostream& stream) const ;                                   /**< \brief writes description to stream */
    } ; // IssueFactory

    std::ostream& operator<<(std::ostream&, const IssueFactory& factory);         /**< \brief streaming operator */

} // ers

#endif

