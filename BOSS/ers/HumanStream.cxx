/*
 *  HumanStream.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 01.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#include <assert.h>
#include "ers/ers.h"
#include "ers/HumanStream.h"


const char* const ers::HumanStream::KEY = "human" ; 

namespace {
    ers::Stream *create_stream(const std::string &protocol, const std::string &uri) { 
        (void) uri ;
        if (protocol==ers::HumanStream::KEY) {
	    return new ers::HumanStream() ;  
        } 
	return 0 ; 
    } // 
    bool registered = ers::StreamFactory::instance()->register_factory(ers::HumanStream::KEY,create_stream) ;
} // anonymous namespace


/** Dumps the content of an issue into a string 
  * This class is used internally by the Issue class to produce then description field. 
  * \param issue_ptr the Issue to serialise
  * \return string description 
  */

std::string ers::HumanStream::to_string(const Issue *issue_ptr) throw() {
    try { 
	HumanStream ers_stream ;
	ers_stream.send(issue_ptr) ; 
	std::string str = ers_stream.to_string();
	return str ; 
    } catch (std::exception &ex) {
	std::string msg = "error while building message " + std::string(ex.what()) ; 
	return msg; 
    } // 
} // to_string

ers::HumanStream::HumanStream() : ers::Stream() {} // HumanStream
ers::HumanStream::HumanStream(const HumanStream &other) : ers::Stream(other) {} 
ers::HumanStream::~HumanStream() {} 

/** \return the content of the stream as a string 
  */

std::string ers::HumanStream::to_string() { 
     return m_out_stream.str(); 
} // to_string

/** Clears the content of the stream 
  */

void ers::HumanStream::clear() {
    m_out_stream.str(""); 
} // clear

void ers::HumanStream::print_to(std::ostream& stream) const {
    stream << ers::HumanStream::KEY << ':' ; 
} // print_to

void ers::HumanStream::send(const Issue *issue_ptr) {
    ERS_PRE_CHECK_PTR(issue_ptr); 
    if (m_out_stream) {
	try {
	    const string_map_type *table = issue_ptr->get_value_table(); 
	    const std::string message_str = issue_ptr->get_value(Issue::MESSAGE_KEY) ;
    	    const std::string severity_str = issue_ptr->get_value(Issue::SEVERITY_KEY) ;
            m_out_stream << "issue: " << message_str << "(" << severity_str << ")[" ; 
	    bool first = true ; 
	    for(string_map_type::const_iterator pos = table->begin();pos!=table->end();++pos) {
	    	const std::string &key = pos->first ; 
		const std::string &value = pos->second ; 
		if (first) {
		    first = false ;
		} else {
		    m_out_stream << ", " ;
		} // first or not
		m_out_stream  << key << "=\"" << value << '\"';
	    } // for
	    m_out_stream << "]" ; 
	} catch (std::ios_base::failure &ex) {
	    throw ers::Issue(ERS_HERE,ers::error,&ex); 
	} // catch generic I/O errors 
    } // if m_out_stream
} // send




