/*
 *  ParseIssue.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 17.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#include "ers/ParseIssue.h"

const char * const ers::ParseIssue::OFFENDING_LINE_KEY = "PARSE_ISSUE_TEXT" ; 
const char * const ers::ParseIssue::OFFENDING_LINE_NUMBER_KEY = "PARSE_ISSUE_LINE_NUMBER" ; 
const char * const ers::ParseIssue::PARSE_ISSUE_CLASS_NAME = "ers::ParseIssue" ; 

namespace {
    ers::Issue *create_parse_issue() { return new ers::ParseIssue(); } 
    bool registered = ers::IssueFactory::instance()->register_issue(ers::ParseIssue::PARSE_ISSUE_CLASS_NAME,create_parse_issue) ;
} 

ers::ParseIssue::ParseIssue() : ers::Issue() {} 

ers::ParseIssue::ParseIssue(const Context &c, severity_t s) : ers::Issue(c,s) {} 

ers::ParseIssue::ParseIssue(const Context &c, severity_t s,const std::string &msg ) : ers::Issue(c,s) {
    finish_setup(msg); 
} // ParseIssue

ers::ParseIssue::ParseIssue(const Context &c, severity_t s,const std::string &msg, const std::string &line )  : ers::Issue(c,s) {
    this->offending_line(line); 
    finish_setup(msg); 
} // ParseIssue
 
const char* ers::ParseIssue::get_class_name() const throw() {
    return PARSE_ISSUE_CLASS_NAME ; 
} // get_class_name

void ers::ParseIssue::offending_line(const std::string &line) {
    m_value_table[OFFENDING_LINE_KEY] = line ; 
}// offending_line

void ers::ParseIssue::offending_line_number(int line) {
    set_value(OFFENDING_LINE_NUMBER_KEY,line); 
} // offending_line_number
    
void ers::ParseIssue::file_name(std::string filename) {
    m_value_table["ParseFileName"] = filename ; 
} // file_name



