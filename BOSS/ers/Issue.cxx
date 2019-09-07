/*
 *  Issue.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 26.11.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */


#include <iostream>
#include <sstream>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sysexits.h>

#include "ers/ers.h"
#include "ers/HumanStream.h"
#include <cstring>
#include <typeinfo> 
#define BUFFER_SIZE 256 

#define ISSUE_VALUE_SET_SCALAR(table,key,value) { std::ostringstream stream ; stream << value ;  table[key] = stream.str(); }

/** This block insert the relevant function into the Issue Factory table.
  * This is needed to ensure that serialisation classes work 
  */

namespace {
    ers::Issue *create_issue() { return new ers::Issue(); } 
    bool registered = ers::IssueFactory::instance()->register_issue(ers::Issue::ISSUE_CLASS_NAME,create_issue) ;
} 

using namespace ers ; 

const char* const Issue::CLASS_KEY = "ISSUE_CLASS" ; 
const char* const Issue::COMPILATION_TIME_KEY = "COMPILATION_TIME" ; 
const char* const Issue::COMPILATION_TARGET_KEY = "COMPILATION_TARGET" ; 
const char* const Issue::COMPILATION_DEBUG_LVL_KEY = "COMPILATION_DEBUG_LVL" ; 
const char* const Issue::COMPILER_KEY = "COMPILER" ; 
const char* const Issue::CPP_CLASS_KEY = "ISSUE_CPP_CLASS" ; 
const char* const Issue::ERS_VERSION_KEY = "ERS_VERSION" ;
const char* const Issue::HOST_NAME_KEY = "HOST_NAME" ; 
const char* const Issue::HOST_TYPE_KEY = "HOST_TYPE" ; 
const char* const Issue::HOST_IP_ADDR_KEY = "HOST_IP" ; 
const char* const Issue::MESSAGE_KEY = "MESSAGE" ; 
const char* const Issue::PROCESS_ID_KEY = "PROCESS_ID" ;
const char* const Issue::PROCESS_PWD_KEY = "PROCESS_PWD" ; 
const char* const Issue::PROGRAM_NAME_KEY = "PROGRAM_NAME"; 
const char* const Issue::RESPONSIBILITY_KEY = "RESPONSIBILITY" ; 
const char* const Issue::SEVERITY_KEY = "SEVERITY" ; 
const char* const Issue::SOURCE_POSITION_KEY = "SOURCE_POSITION" ; 
const char* const Issue::SOURCE_PACKAGE_KEY = "SOURCE_PACKAGE" ; 
const char* const Issue::TIME_KEY = "TIME" ; 
const char* const Issue::TRANSIENCE_KEY = "TRANSIENCE" ; 
const char* const Issue::USER_ID_KEY = "USER_ID" ; 
const char* const Issue::USER_NAME_KEY = "USER_NAME" ; 
const char* const Issue::CAUSE_TEXT_KEY = "CAUSE_TEXT"  ;
const char* const Issue::CAUSE_PSEUDO_KEY = "CAUSE" ;
const char* const Issue::QUALIFIER_LIST_KEY = "QUALIFIERS" ; 
const char* const Issue::EXIT_VALUE_KEY = "EXIT_VALUE" ; 

const char* const Issue::ISSUE_CLASS_NAME = "ers::issue" ; 

// Constructors
// ====================================================

/** Empty constructor, should only be used when deserialising issues 
  */

Issue::Issue() {
    m_human_description.clear() ; 
    m_cause = 0 ; 
} // Issue

/** Copy constructor 
  * The \c m_human_description and the \c m_value_table fields are simply copied
  * The The issue in the \c m_cause pointer if present is cloned. 
  * \param issue original for copy
  */ 

Issue::Issue(const Issue &issue) : std::exception() {
    m_value_table = issue.m_value_table ;
    m_human_description = issue.m_human_description ; 
    if (issue.m_cause) {
	this->m_cause = issue.m_cause->clone(); 
    } else {
	this->m_cause = 0 ; 
    } 
} // Issue

/** Builds an Issue out of a value table
  * \param values table of values for the issue
  */

Issue::Issue(const string_map_type &values) {
    cause(); 
    set_values(values); 
    m_human_description.clear();
} // Issue


/** Constructor 
 * \param context the context of the Issue, e.g where in the code did the issue appear 
 * \param s severity_t of the Issue
 * \param m message of the Issue
 */

Issue::Issue(const Context &context, severity_t s, const std::string &m) {
    cause(); 
    setup_common(&context);
    severity(s);
    finish_setup(m); 
} // Issue

/** @overload */

Issue::Issue(const Context &context, severity_t s) {
    cause();   
    setup_common(&context);
    severity(s);
} // Issue

/** Constructor - takes another exceptions as the cause for the current exception.
 * \param context the context of the Issue, e.g where in the code did the issue appear  
 * \param s the severity_t of the exception
 * \param cause_exception the exception that caused the current Issue
 */

Issue::Issue(const Context &context, severity_t s, const std::exception *cause_exception) {
    ERS_PRE_CHECK_PTR(cause_exception);
    cause(cause_exception); 
    setup_common(&context);
    severity(s);
    finish_setup(cause_exception->what()); 
} // Issue

/** Destructor. 
  * If the \c m_cause pointer is not null, the pointed Issue is deleted 
  */

Issue::~Issue() throw() {
    if (m_cause) delete m_cause ; 
    m_cause = 0 ; 
} // ~Issue


// Operators and factory methods 
// ====================================================


/** Builds a clone of the object.
  * The object is allocated on the stack, and should be deleted by the caller
  * \return a new object that is a copy of the current object
  */

Issue *Issue::clone() const { 
    return IssueFactory::instance()->build(this) ; 
} // clone


Issue::operator std::string() const {
    std::string s = human_description();
    return s ;
} // std::string()

/** Copy operator 
  * \param source the original issue
  * \return the copy issue
  * \note The \c m_cause member, if non null, is cloned 
  */

Issue Issue::operator=(const Issue &source) {
    Issue target(source);
    return target ; 
} // operator=

/** Comparison operator 
  * \param other issue to compare to
  * \return \c true if \c this and \c other are equal 
  */

bool Issue::operator==(const Issue &other) const throw() {
    if (m_value_table != other.m_value_table) return false ; 
    if (m_cause == other.m_cause) return true ; 
    return (*m_cause) == *(other.m_cause) ; 
} // operator==

/** Array access operator 
  * \param key the resolve
  * \return string containing value
  * \see get_value(const std::string &)
  */

const std::string & Issue::operator[](const std::string &key) const throw() {
    return get_value(key,"");
} // operator[]



// ====================================================
// Stream Operators
// ====================================================


/** Standard Streaming operator - puts the human description into the Stream. 
 * \param s the destination Stream 
 * \param i the Issue to Stream
 * \see Issue::human_description()
 */

std::ostream& ers::operator<<(std::ostream& s, const Issue& i) {
    return s << i.human_description() ; 
} // operator<<

/** Sends the Issue into a Stream 
 * \param s the Stream to send the Issue into
 * \param i the Issue to send
 * \return the Stream
 * \see serialize_to() 
 */

Stream& ers::operator<<(Stream& s, const Issue& i) {
    s.send(&i); 
    return s ; 
} // operator<< 


// ====================================================
// Manipulation of the m_cause field 
// ====================================================

/**
  * \return the cause of the issue, if there is one, a null pointer otherwise 
  */

const Issue *Issue::cause() const throw() {
    return m_cause ; 
} // cause

/** Sets the cause of the issue 
  * If the cause is an Issue, it is cloned and stored in the \c m_cause pointer.
  * In all cases, the description of the cause is stored in the value table using
  * the \c CAUSE_TEXT_KEY key.
  * If the cause pointer is null, the \c m_cause field is simply cleared. 
  * \param c pointer to the cause exception 
  */

void Issue::cause(const std::exception *c) {
    if (c==0) {
	m_cause = 0 ; 
	return ; 
    } // No cause easy. 
    const Issue *i = dynamic_cast<const Issue *>(c) ; 
    if (i) {
	m_cause = i->clone(); 
    } else {
	m_cause = 0 ; 
    } // if
    set_value(CAUSE_TEXT_KEY,c->what()); 
} // cause


// Value Table manipulation Methods 
// ====================================================

/** Returns a read-only pointer to the value table
  * @return read only pointer to the table
  */

const string_map_type* Issue::get_value_table() const {
    return &m_value_table ; 
} // get_value_table

/** General method for querying properties of the Issue 
 * \param key the key to lookup
 * @return the string value for the key and empty string if the key is not found
 */

const std::string &Issue::get_value(const std::string &key, const std::string &def) const throw() {
    string_map_type::const_iterator pos = m_value_table.find(key);
    if (pos!=m_value_table.end()) {
        return pos->second ; 
    } // if
    return def ; 
} // get_value

/** \overload 
  */

const std::string &Issue::get_value(const std::string &key) const throw() {
    return get_value(key,Core::empty_string) ; 
} // get_value


/** Get a property of an issue as an integer 
  * \param key the key to search for 
  * \param def the value to return if key is not found 
  * \return value of key or \c def
  */

int Issue::get_int_value(const std::string &key, int def) const throw() {
    std::string v = get_value(key) ;      
    if (! v.empty()) { // not empty 
	std::istringstream in(v) ;
	int n ; 
	in >> n ;
	return n ; 
	// return atoi(v.c_str());
    } else { // empty 
	return def ; 
    } // empty 
} // get_int_value


/** Get a property of an issue as an long 
* \param key the key to search for 
* \param def the value to return if key is not found 
* \return value of key or \c def
*/

long Issue::get_long_value(const std::string &key, long def) const throw() {
    std::string v = get_value(key) ;      
    if (! v.empty()) { // not empty 
	std::istringstream in(v) ;
	long n ; 
	in >> n ;
	return n ; 
	// return atoi(v.c_str());
    } else { // empty 
	return def ; 
    } // empty 
} // get_long_value

/** Get a property of an issue as an double 
 * \param key the key to search for 
 * \param def the value to return if key is not found - the default value for this parameter is NaN. 
 * \return value of key or \c def
 */

double Issue::get_double_value(const std::string key, double def=nan("")) const throw() {
    std::string v = get_value(key) ;      
    if (! v.empty()) { // not empty 
	std::istringstream in(v) ;
	double n ; 
	in >> n ;
	return n ; 
	// return atoi(v.c_str());
    } else { // empty 
	return def ; 
    } // empty     
} // get_double_value


/** Sets the value table 
 * \param values the value table to load
 */

void Issue::set_values(const string_map_type &values) throw() {
    m_value_table = values ;
} // load_values

/** Set a numerical value in the value table
 * \param key the key to use for insertion
 * \param value the value to insert
 */

void Issue::set_value(const std::string &key, uint8_t value) throw() {
    ISSUE_VALUE_SET_SCALAR(m_value_table,key,value); 
} // set_value

/** \overload */

void Issue::set_value(const std::string &key, uint16_t value) throw() {
    ISSUE_VALUE_SET_SCALAR(m_value_table,key,value); 
} // set_value

/** \overload */

void Issue::set_value(const std::string &key, uint32_t value) throw() {
    ISSUE_VALUE_SET_SCALAR(m_value_table,key,value); 
} // set_value

/** \overload */

void Issue::set_value(const std::string &key, uint64_t value) throw() {
    ISSUE_VALUE_SET_SCALAR(m_value_table,key,value); 
} // set_value

/** \overload */

void Issue::set_value(const std::string &key, int8_t value) throw() {
    ISSUE_VALUE_SET_SCALAR(m_value_table,key,value); 
} // set_value

/** \overload */

void Issue::set_value(const std::string &key, int16_t value) throw() {
    ISSUE_VALUE_SET_SCALAR(m_value_table,key,value); 
} // set_value

/** \overload */

void Issue::set_value(const std::string &key, int32_t value) throw() {
    ISSUE_VALUE_SET_SCALAR(m_value_table,key,value); 
} // set_value

/** \overload */

void Issue::set_value(const std::string &key, int64_t value) throw() {
    ISSUE_VALUE_SET_SCALAR(m_value_table,key,value); 
} // set_value

/** Set a numerical value in the value table
 * \param key the key to use for insertion
 * \param value the value to insert
 */

void Issue::set_value(const std::string &key, double value) throw() {
    std::ostringstream stream ;
    stream << value ; 
    m_value_table[key] = stream.str();
} // set_value

/** Sets a string value in the value table
  * \param key the key to use for insertion
  * \param value the value to insert 
  */

void Issue::set_value(const std::string &key, const std::string &value) throw() {
    if (! value.empty()) {
	m_value_table[key] = value ;
    }
} // set_value

/** Sets a string value in the value table
  * \param key the key to use for insertion
  * \param value c-string, null pointer is ignored. 
  */

void Issue::set_value(const std::string &key, const char* value) throw() {
    if (value) {
	std::string value_str = std::string(value) ; 
	set_value(key,value_str); 
    } // if 
} // set_value

/** Sets a pointer in the value table 
  * \param key the key to use for insertion
  * \param ptr a pointer 
  * \note the pointer is stored in hex format
  */

void Issue::set_value(const std::string &key, const void* ptr) throw() {
    std::ostringstream stream ;
    stream.setf(std::ios::hex,std::ios::basefield); 
    stream << (unsigned long) ptr ; 
    m_value_table[key] = stream.str();
} // set_value


/** 
 * \return the number of key/value pairs in the issue
 */

int Issue::values_number() const {
    return m_value_table.size(); 
} // values_number

// ====================================================
// Insertions Methods
// ====================================================

/** Inserts the context of the issue into the issue 
* \param context pointer to context object
*/

void Issue::insert(const Context *context) throw() {
    if (context) {
	set_value(SOURCE_POSITION_KEY,context->position()) ; 
	set_value(SOURCE_PACKAGE_KEY,context->package_name()); 
	set_value(COMPILER_KEY,context->compiler()) ; 
	set_value(COMPILATION_TIME_KEY,context->compilation()) ; 
	set_value(COMPILATION_TARGET_KEY,context->host_type()) ; 
	int lvl = ers::Context::debug_level();
	if (lvl>=0) {
	    set_value(COMPILATION_DEBUG_LVL_KEY,lvl); 
	} // if
	int frame_number = context->stack_frames();
	for(int i=0;i<frame_number;i++) {
	    char key_buffer[256] ; 
	    snprintf(key_buffer,sizeof(key_buffer),"SOURCE-STACK-%03x",i);
	    set_value(key_buffer,context->stack_frame(i)); 
	} // for
	std::vector<std::string> qualifs = context->qualifiers() ;
	std::vector<std::string>::const_iterator pos ; 
	for(pos=qualifs.begin();pos!=qualifs.end();pos++) {
	    add_qualifier(*pos) ; 
 	} // for
    } // if context 
} // insert

/** Inserts the current time into the issue 
*/

void Issue::insert_time() throw() {
    time_t now ;
    time(&now); 
    char time_buffer[BUFFER_SIZE] ; 
    ctime_r(&now,time_buffer) ; 
    char *cr = strchr(time_buffer,'\n');
    if (cr) {
	*cr = '\0' ;
    } // carriage return 
    set_value(TIME_KEY,time_buffer); 
} // insert_time

// ====================================================
// Setup Methods
// ====================================================

/** This method sets up common fields for all Issues. 
  * In particular, it inserts all data concerning the Issue's context, this includes
  * \li Source code position (file/line)
  * \li Compiler version
  * \li Compilation time and date
  *
  * \param context context where the exception occured, this should be the ERS_HERE macro. 
  * \note All method used within this method should throw no exceptions to avoid circular problems. 
  */

void Issue::setup_common(const Context *context) throw() {
    const int errno_copy = errno ; // We need to save errno, because it might be changed 
    insert(context);
    insert_time();
    errno = errno_copy ; // we restaure errno 
} // setup_common

/* Cut out stuff to remove dependency
* \li Hostname 
  * \li Process id
  * \li OS and processor of the host
 Process p ; 
    set_value(PROCESS_ID_KEY,p.process_id()); 
    System::User user ; 
    set_value(USER_ID_KEY,user.identity()) ; 
    set_value(USER_NAME_KEY,user.name_safe()) ; 
    set_value(PROCESS_PWD_KEY,System::File::working_directory()); 
    System::LocalHost *localhost = System::LocalHost::instance(); 
    set_value(HOST_NAME_KEY,localhost->full_name()); 
    set_value(HOST_IP_ADDR_KEY,localhost->ip_string()); 
    set_value(HOST_TYPE_KEY,localhost->description()); 

*/


/** Finishes the setting up of the information fields.
  * In particular, in fills in the human message and the class type fields 
  * (those fields are not available until the end of the object construction.
  * @note this method should be called by the sub-class constructor, so that RTTI information is setup and correct. 
  * \param message human readable message 
  * \note All method used within this method should throw no exceptions to avoid circular problems. 
  */

void Issue::finish_setup(const std::string &msg) throw() {
    // set_value(CPP_CLASS_KEY,class_name); 
    set_value(CLASS_KEY, get_class_name()) ;
    set_value(MESSAGE_KEY,msg); 
} // finish_setup



// ====================================================
// Field Access Methods
// ====================================================

/** Returns the key used to describe this particular class when serializing 
  * This method tries to build a meaningfull class name out of C++ RTTI.
  * This depends on the compiler providing information in a format similar to gcc.
  * For more safety. 
  * If the gcc unmangling fails the default (ers::Issue) is used. 
  */

const char*Issue::get_class_name() const throw()  {
    if (m_class_name.empty()) {
	const Issue *p = this ;
	m_class_name = ers::Core::umangle_gcc_class_name((typeid(*p)).name()).c_str(); 
	if (m_class_name.empty()) {
	     m_class_name=ISSUE_CLASS_NAME ; 
	} // fall back 
    } 
    return m_class_name.c_str() ;
} // get_class_name

/** Gets the severity_t of the Issue 
 * @return severity_t of the Issue 
 */

severity_t Issue::severity() const throw() {
    std::string value = get_value(SEVERITY_KEY); 
    return ers::Core::parse_severity(value);
} // severity

/** Set the severity_t of the Issue 
 * \param s the severity_t level 
 */

void Issue::severity(severity_t s) {
    set_value(SEVERITY_KEY,ers::Core::to_string(s)); 
} // severity

/** Is the issue either an error or a fatal error 
  * \return \c true if the issue is either an error or a fatal 
  */

bool Issue::is_error() {
    severity_t s = severity(); 
    return (s==ers::error || s== ers::fatal) ;
} // is_error

/**
  * \return the string representing the severity_t of the issue 
  */

std::string Issue::severity_message() const {
    return get_value(SEVERITY_KEY);  
} // severity_message


/** Gets the responsibility type of the Issue 
 * \return the responsibiliy value of the Issue 
 */

responsibility_t  Issue::responsibility() const throw() {
    std::string value = this->get_value(RESPONSIBILITY_KEY); 
    return ers::Core::parse_responsibility(value);
} // responsability

/** Sets the responsbility of the Issue
 * \param r the responsibility type
 */

void Issue::responsibility(responsibility_t  r) {
    set_value(RESPONSIBILITY_KEY,ers::Core::to_string(r)) ; 
} // responsability


/** Sets the transience of the issue 
  * \param tr true if the issue is transient, false if not
  */

void Issue::transience(bool tr) {
    set_value(TRANSIENCE_KEY,ers::Core::to_string(tr)) ; 
} // transience

/** @return the transience of the issue, 1 = true, 0 = false, -1 = unknown 
  */

int Issue::transience() const throw () {
    std::string value = this->get_value(TRANSIENCE_KEY); 
    return ers::Core::parse_boolean(value.c_str());
} // transience


/** 
 * @return human description of the Issue 
 */

const std::string &Issue::human_description() const throw()  {
    if (m_human_description.empty()) {
	 m_human_description = HumanStream::to_string(this) ; 
    } 
    return m_human_description ; 
}  // human_description

/** This method overides the what method of the std::exception class.
  * As this method is declared const, it has to use a pre-calculated string
  * @return C style string of human_description 
  */

const char *Issue::what() const throw() {
    std::string desr = human_description() ; 
    return desr.c_str(); 
} // what();  

const std::string &Issue::message() const throw() {
    return get_value(MESSAGE_KEY) ; 
} // message

int Issue::exit_value() const throw() {
    int v = 1 ; 
    if (transience()==1) v = EX_TEMPFAIL ; 
    return get_int_value(EXIT_VALUE_KEY,v); 
} // exit_value

/** Add a qualifier to the qualifier list
  * \param qualif the qualifier to add
  */

void Issue::add_qualifier(const std::string &qualif) {
    const std::string &qualif_s = get_value(QUALIFIER_LIST_KEY) ; 
    std::string::size_type pos = qualif_s.find(qualif);
    if (pos!=std::string::npos) return ; // already present
    std::string n_qualif = qualif_s+qualif + " " ; 
    set_value(QUALIFIER_LIST_KEY,n_qualif); 
} // add_qualifier

/** Gets the list of qualifiers 
  * \return list of qualifiers 
  */

std::vector<std::string> Issue::qualifiers() const {
    const std::string &qualif_s = get_value(QUALIFIER_LIST_KEY) ; 
    return ers::Core::tokenize(qualif_s,", \t"); 
} // qualifiers







