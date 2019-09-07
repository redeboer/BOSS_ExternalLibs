/*
 *  ParseIssue.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 17.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#ifndef ERS_PARSE_ISSUE
#define ERS_PARSE_ISSUE

#include "ers/Issue.h"

namespace ers {
    
    /** This class represents an error while parsing data. 
      * \author Matthias Wiesmann
      * \version 1.0
      * \brief Data parsing error
      */
    
class ParseIssue: public Issue {
protected:
    ParseIssue(const Context &c, severity_t s) ; 
public:
    static const char * const OFFENDING_LINE_KEY ;
    static const char * const OFFENDING_LINE_NUMBER_KEY ; 
    static const char * const PARSE_ISSUE_CLASS_NAME ; 
    void offending_line(const std::string &line) ; 
    void offending_line_number(int line); 
    void file_name(std::string filename); 
    ParseIssue() ; 
    ParseIssue(const Context &c,severity_t s, const std::string &message); 
    ParseIssue(const Context &c,severity_t s, const std::string &message, const std::string &line); 
    virtual const char* get_class_name() const throw() ;
} ; // ParseIssue

} // ers

#define ERS_PARSE_ERROR(message,...) ers::ParseIssue(ERS_HERE,ers::error,__VA_ARGS__) 

#endif

