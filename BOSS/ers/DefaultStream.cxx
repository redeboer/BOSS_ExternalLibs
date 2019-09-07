/*
 *  DefaultStream.cxx
 *  ERS
 *
 *  Created by Matthias Wiesmann on 21.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */
#include <iostream>

#include "ers/DefaultStream.h"
#include "ers/StreamFactory.h"
#include "ers/InvalidReferenceIssue.h"

/** The key for this stream is \c default the URI field is ignored */
const char* const ers::DefaultStream::KEY = "default" ; 
const char* const ers::DefaultStream::VERBOSE_KEY = "verbose" ; 

namespace {
    ers::Stream *create_stream(const std::string &protocol, const std::string &uri) { 
	
        if (protocol==ers::DefaultStream::KEY) {
	    bool verbose = (uri == ers::DefaultStream::VERBOSE_KEY) ; 
	    return new ers::DefaultStream(verbose) ;  
        } else {
	} 
	return 0 ; 
    } // 
    bool registered = ers::StreamFactory::instance()->register_factory(ers::DefaultStream::KEY,create_stream) ;
} // anonymous namespace

ers::DefaultStream::DefaultStream(bool verbose) {
    m_verbose = verbose ;
} // DefaultStream


ers::DefaultStream::~DefaultStream() {} 

/** We print the stream in a semi-tabbed mode. 
  * The message is first printed out, then a list of all properties. 
  * \param issue_ptr reference to the issue to print
  * \note The format produced by this class is subject to change! 
  */

void ers::DefaultStream::send(const Issue *issue_ptr) {
    ERS_PRE_CHECK_PTR(issue_ptr); 
    try {
	const string_map_type *table = issue_ptr->get_value_table(); 
	const std::string & message_str = issue_ptr->get_value(Issue::MESSAGE_KEY) ;
	const std::string & severity_str = issue_ptr->get_value(Issue::SEVERITY_KEY) ;
	const std::string & position_str = issue_ptr->get_value(Issue::SOURCE_POSITION_KEY) ; 
	const std::string & date_str = issue_ptr->get_value(Issue::TIME_KEY); 
	
        std::ostream & out = issue_ptr->severity() < warning ? std::cout : std::cerr;
        
        out << severity_str << " at " << position_str << " (" << date_str << "): " << message_str << std::endl ; 
	if (m_verbose) {
	    out << "-----------" << std::endl ; 
            for(string_map_type::const_iterator pos = table->begin();pos!=table->end();++pos) {
                const std::string &key = pos->first ; 
                const std::string &value = pos->second ; 
                out << key << "=\t\"" << value << '\"' << std::endl ;
            } // for
		out << "-----------" << std::endl ; 
	} // if long format
    } catch (std::ios_base::failure &ex) {
	throw ers::Issue(ERS_HERE,ers::error,&ex);
    } // catch generic I/O errors
   
} // send

