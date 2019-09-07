#include <ctype.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <string.h>
#include "ers/Core.h"
#include <cstdlib> 
#include <assert.h>

const char* const ers::Core::SEVERITY_NAMES[] = { "none", "debug_0", "debug_1", "debug_2", "debug_3", "information", "notification", "warning", "error", "fatal", "maximum (illegal)" } ;
const char* const ers::Core::BOOLEAN_NAMES[] = { "false", "true" } ; 
const char* const ers::Core::RESPONSIBILITY_NAMES[] = { "precondition", "internal", "subSystem" } ; 
const std::string ers::Core::empty_string = "" ;

/** 
 * \brief Transforms a severity_t type into the corresponding string
 * \param s severity
 * \return pointer to string with associated text 
 */

const char *ers::Core::to_string(ers::severity_t s) throw() {
    const unsigned int index = (unsigned int) s ;
    assert(index<=severity_max); 
    return ers::Core::SEVERITY_NAMES[index] ; 
} // getSeverityText

/** Parses a string and extracts a severity_t 
 * \param s the string to parse 
 * \return a severity_t value
 */

ers::severity_t ers::Core::parse_severity(const char *s) throw() {
    for(int i=0;i<severity_max;i++) {
        if (strcmp(s,ers::Core::SEVERITY_NAMES[i])==0) return (ers::severity_t) i ; 
    }// for
    return severity_none ; 
} // parse_severity

/** Parses a string and extracts a severity_t 
 * \param s the string to parse 
 * \return a severity_t value
 */

ers::severity_t ers::Core::parse_severity(const std::string &s) throw() {
    return parse_severity(s.c_str());
} // parse_severity

/** Transforms a responsibility value into a string 
  * \param r the responsibility
  * \return string with text for responsibility 
  */

const char* ers::Core::to_string(ers::responsibility_t  r) throw() {
    const unsigned int index = (unsigned int) r ;
    assert(index<=resp_max);
    return RESPONSIBILITY_NAMES[index] ;
} // to_string

/** Parses a string and extracts a responsibility 
 * \param s the string to parse 
 * \return a responsibility value
 */

ers::responsibility_t  ers::Core::parse_responsibility(const char *s) throw() {
    for(int i=0;i<resp_max;i++) {
        if (strcmp(s,RESPONSIBILITY_NAMES[i])==0) return (ers::responsibility_t) i ; 
    } // for 
    return resp_unknown ;
} // parse_responsability

/** Parses a string and extracts a responsibility 
  * \param s the string to parse
  * \return a responsibility value
  */

ers::responsibility_t  ers::Core::parse_responsibility(const std::string &s) throw() {
    return parse_responsibility(s.c_str()) ; 
} // parse_responsability

/** Parse a string and extract a boolean 
  * \param s the string to parse
  * \return 1 if true
  * \return 0 if false
  * \return -1 if undefined 
  */

int ers::Core::parse_boolean(const char* s) throw() {
    if (! s) return -1 ; 
    if (! *s) return -1 ; 
    if (strcasecmp(s,BOOLEAN_NAMES[1])==0) return 1 ;
    if (strcasecmp(s,BOOLEAN_NAMES[0])==0) return 0 ; 
    return -1 ; 
} // parse_boolean

/** Convert a boolean to a string 
  * \param b the boolean
  * \return either the string "true" or "false"
  */

const char* ers::Core::to_string(bool b) throw () {
    int index = b ? 1 : 0 ;
    return BOOLEAN_NAMES[index] ;
} // to_string


/** This method parses a string in the format used by gcc class names
  * The string begins with the length of the string expressed in ascii encoded integer, 
  * followed by the character data (no 0 character at the end). 
  * \param ptr pointer to the character data pointer, this pointer is incremented by the parsing.
  * \return a string containing the string data
  */

std::string ers::Core::parse_prefix_string(const char **ptr) throw() {
    if (ptr==0 || *ptr==0 || **ptr=='\0') return ers::Core::empty_string ; 
    int l = 0 ; 
    while(isdigit(**ptr)) { // we parse the integer 
	l*=10 ;
	l+=(**ptr)-'0' ; 
	(*ptr)++ ; 
    } // 
    std::string s(*ptr,l); // we create the string 
    (*ptr)+=l ; 
    return s ; 
} // parse_gcc_string


/** This method tries to unmangle GCC class names given by the RTTI system
  * This works for GCC 3.2 and 3.4.
  * It should not be used for anything else than human display or fallback mechanism.
  * \param name the mangled name of the object 
  * \return an unmangled name 
  */

std::string ers::Core::umangle_gcc_class_name(const char* name) throw() {
    if (name==0 || strlen(name)==0) return ers::Core::empty_string ; 
    const char *ptr = name ; 
    std::ostringstream stream ; 
    while (*ptr=='P') { // pointers are denoted with P
	stream << "*" ;
	ptr++ ; 
    } // if
    while (*ptr=='N') { // namespace are denoted by N+string 
	ptr++ ; 
	stream << parse_prefix_string(&ptr) << "::" ; 
    } // 
    stream << parse_prefix_string(&ptr);
    return stream.str(); 
} // umangle_gcc_class_name

std::vector<std::string> ers::Core::tokenize(const std::string &text, const std::string &separators) {
    std::vector<std::string> result_vector ; 
    std::string::size_type start_p, end_p ; 
    start_p = text.find_first_not_of(separators) ; 
    while(start_p != std::string::npos) {
	end_p = text.find_first_of(separators,start_p) ;
	if (end_p == std::string::npos) {
	    end_p = text.length(); 
	}
	const std::string sub_str = text.substr(start_p,end_p-start_p);
	result_vector.push_back(sub_str) ; 
	start_p = text.find_first_not_of(separators,end_p) ;
    } // while
    return result_vector ; 
} // tokenize




