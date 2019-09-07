/*
 *  FilterStream.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 31.03.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include "ers/FilterStream.h"
#include "ers/InvalidReferenceIssue.h"
#include "ers/StreamFactory.h"
#include "ers/NotImplemented.h"

/** Tag used to identify the stream
  */

const char* const ers::FilterStream::FILTER_STREAM_TAG = "filter" ; 
const char* const ers::FilterStream::INCLUDE_TAG = "?" ; 
const char* const ers::FilterStream::EXCLUDE_TAG = "!"; 
const char* const ers::FilterStream::TARGET_TAG = "@" ; 
const char* const ers::FilterStream::SEPARATORS = "," ; 

namespace {
    ers::Stream *create_stream(const std::string &protocol, const std::string &uri) { 
	if (protocol==ers::FilterStream::FILTER_STREAM_TAG) {
	    ers::Stream *stream = ers::FilterStream::factory(uri);
	    // ERS_DEBUG_0("build stream %x",stream); 
	    return stream ; 
	} // if
	return 0 ;
    } // create_stream
    bool registered = ers::StreamFactory::instance()->register_factory(ers::FilterStream::FILTER_STREAM_TAG,create_stream) ;
} // anonymous namespace 

/** Builds a stream out of a set of parameters 
  * \param include_str string containing all the coma separated include qualifiers 
  * \param exclude_str string containing all the coma separated exclude qualifiers 
  * \param target_str string containing the key for the target stream, i.e the stream where accepted Issues are sent. 
  * \brief factory method for partially parse information
  */

ers::FilterStream * ers::FilterStream::factory(const std::string &include_str, const::std::string &exclude_str, const std::string &target_str) {
    ers::Stream *target_stream = ers::StreamFactory::instance()->create_stream(target_str); 
    std::vector<std::string> include_list = ers::Core::tokenize(include_str,SEPARATORS);
    std::vector<std::string> exclude_list = ers::Core::tokenize(exclude_str,SEPARATORS); 
    return new FilterStream(target_stream,include_list,exclude_list); 
} // factory

/** Builds a stream out of a key string.
  * This method parses the string and calls a more specialised version of the \c factory method. 
  * \param params single string describing the requested filter stream
  * \return pointer to newly heap allocated instance of FilterStream
  * \brief factory method for unparsed information 
  */

ers::FilterStream * ers::FilterStream::factory(const std::string &params) {
    std::string::size_type include_start = params.find(INCLUDE_TAG); 
    std::string::size_type exclude_start = params.find(EXCLUDE_TAG);
    std::string::size_type target_start =  params.find(TARGET_TAG);
    std::string include_str = params.substr(include_start+1,exclude_start-1);
    std::string exclude_str = params.substr(exclude_start+1,target_start-exclude_start-1);
    std::string target_str = params.substr(target_start+1); 
    // ERS_DEBUG_0("include: '%s' exclude: '%s' target '%s'",include_str.c_str(),exclude_str.c_str(), target_str.c_str()); 
    return factory(include_str,exclude_str,target_str); 
} // factory


/** Disabled empty constructor 
*/

ers::FilterStream::FilterStream() : ers::Stream() {
} // FilterStream


/** Disabled copy constructor 
*/

ers::FilterStream::FilterStream(const FilterStream &other) : ers::Stream(other) {
} // FilterStream


/** Constructor 
  * \param target_ptr pointer to the target stream, the target pointer is assumed to be allocated
  * on the stack and owned by the current object, i.e it will be deleted upon destruction
  * \param include_list collection of strings, at least one of these strings need to be present
  * in the qualifiers in order for the Issue to be accepted.
  * \param exclude_list collection of strings, no qualifier of the Issue must match those in this 
  * collection in order for the Issue to be accepted. 
  */

ers::FilterStream::FilterStream(ers::Stream *target_ptr, const std::vector<std::string> &
include_list,  const std::vector<std::string> & exclude_list) : ers::Stream() {
    ERS_CHECK_PTR(target_ptr); 
    m_target_stream_ptr = target_ptr ; 
    m_include = include_list ; 
    m_exclude = exclude_list ; 
} // FilterStream



/** Destructor
 */

ers::FilterStream::~FilterStream() {
    delete(m_target_stream_ptr) ; 
} // FilterStream

/** Filtering method 
  * This method checks if an Issue is to be accepted or not. 
  * \param issue_ptr the issue to check 
  * \return \c true if the Issue passes filtering, \c false otherwise.
  */

bool ers::FilterStream::is_accept(const ers::Issue* issue_ptr) {
    ERS_CHECK_PTR(issue_ptr);
    std::string qualifiers = issue_ptr->get_value(ers::Issue::QUALIFIER_LIST_KEY) ;
    if (! qualifiers.empty()) { // we check if qualifiers are in exclude list
	std::vector<std::string>::const_iterator e_pos ; 
	for(e_pos = m_exclude.begin();e_pos!=m_exclude.end();e_pos++) {
	    std::string::size_type conflict = qualifiers.find(*e_pos) ; 
	    if (conflict!=std::string::npos) {
		return false ;   
	    } // found conflict 
	} // for 
    } // there are some qualifiers 
    if (! m_include.empty()) {
	std::vector<std::string>::const_iterator i_pos ; 
	for(i_pos = m_include.begin();i_pos!=m_include.end();i_pos++) {
	    std::string::size_type match = qualifiers.find(*i_pos) ; 
	    if (match!=std::string::npos) {
		return true ; 
	    } // found match 
	} // for
	return false ; 
    } // include list contains items 
    return true ; 
} // is_accept

/** Send method 
  * basically calls \c is_accept to check if the issue is accepted. 
  * If this is the case, the \c send method on the target is called with 
  * \c issue_ptr. 
  * \param issue_ptr pointer to the issue to send. 
  */

void ers::FilterStream::send(const ers::Issue *issue_ptr) {
    ERS_CHECK_PTR(issue_ptr);
    ERS_CHECK_PTR(m_target_stream_ptr); 
    if (is_accept(issue_ptr)) {
	// ERS_DEBUG_3("accepted %s:",issue_ptr->what()); 
	m_target_stream_ptr->send(issue_ptr); 
    } else {
	// ERS_DEBUG_3("rejected %s:",issue_ptr->what()); 
    } 
} // send

/** Prints stream description to stream 
  * \param stream STL target stream
  */

void ers::FilterStream::print_to(std::ostream& stream) const {
    stream << FILTER_STREAM_TAG << ':' ; 
    std::vector<std::string>::const_iterator i_pos ; 
    stream << INCLUDE_TAG ; 
    for(i_pos = m_include.begin();i_pos!=m_include.end();i_pos++) {
	if (i_pos!=m_include.begin()) {
	    stream << SEPARATORS ; 
	} // if not first 
	stream << *i_pos ; 
    } // for include 
    stream << EXCLUDE_TAG ; 
    std::vector<std::string>::const_iterator e_pos ; 
    for(e_pos = m_exclude.begin();e_pos!=m_exclude.end();e_pos++) {
	if (e_pos!=m_exclude.begin()) {
	    stream << SEPARATORS ; 
	} // if not first 
	stream << *e_pos ; 
    } // for exclude 
    stream << TARGET_TAG ; 
    stream << *m_target_stream_ptr ; 
} // print_to



