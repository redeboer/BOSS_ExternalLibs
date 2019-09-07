/*
 *  StreamFactory.cxx
 *  ERS
 *
 *  Created by Matthias Wiesmann on 21.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include <iostream>
#include <cstdlib> 

#include "ers/StreamFactory.h"
#include "ers/DefaultStream.h"
#include "ers/ers.h"


/** This variable contains the default keys for building the default streams.
  * The default is to use the default stream, in verbose mode for errors and fatals. 
  */

const char* ers::StreamFactory::DEFAULT_STREAMS[] = {
    "null:",                                                          // none
    "default:",  "default:",  "default:", "default:",                 // debug levels
    "default:",  "default:",  "default:",                             // information, notification, warning
    "default:verbose",  "default:verbose",                            // Errors and Fatals
    "null:"  } ;                                                      // max


/** Pointer to the singleton instance 
  */

ers::StreamFactory *ers::StreamFactory::s_instance = 0 ; 


// Constructors & Instance access methods
// --------------------------------------

/** Constructor - should never be called by user code, use the \c instance() method instead
  * \see instance() 
  */

ers::StreamFactory::StreamFactory() {
   for(int i= static_cast<int> (severity_none);i< static_cast<int> (severity_max);i++) {	
	severity_t s = static_cast<severity_t> (i) ; 
       m_streams[s]=0 ;
   } // for*/
} // StreamFactory

/** Copy constructor - disabled, use the \c instance() method instead
  * \see instance() 
  * \throw ers::NotImplemented in all cases
  */

ers::StreamFactory::StreamFactory(const StreamFactory &other) {
    (void) other ; // shut up the compiler 
    ERS_NOT_IMPLEMENTED(); 
} // StreamFactory

/** Destructor - basic cleanup
  * \note in practice this method should never be called, the singleton should never be deallocated
  */

ers::StreamFactory::~StreamFactory() {
    for(int i= static_cast<int> (severity_none);i< static_cast<int> (severity_max);i++) {	
	severity_t s = static_cast<severity_t> (i) ; 
	if(m_streams[s]) {
	    delete(m_streams[s]);
	    m_streams[s]=0 ; 
	} // if stream 
    } // for*/
} // ~StreamFactory


/** This method returns the singleton instance. 
  * It should be used for every operation on the factory. 
  * \return a pointer to the singleton instance 
  */

ers::StreamFactory* ers::StreamFactory::instance() {
    if (0==s_instance) {
	s_instance = new StreamFactory(); 
    } // if
    return s_instance ; 
} // instance

/** Dumps all registered types of streams */

void ers::StreamFactory::print_registered() {
    const StreamFactory *factory = instance();
    std::cerr << *factory ; 
} // print_registered

// Static methods
// --------------------------------------


/** Finds the default stream for a given severity.
  * The stream is searched in the default instance
  * \param s the severity_t 
  * \return the default stream for the severity_t 
  */

ers::Stream* ers::StreamFactory::get_default_stream(severity_t s) {
    return instance()->get_stream(s) ; 
} // get_default_stream

/** Searches for the textual key for a given severity. 
  * This key is first searched in the environnement variables,
  * if this fails, the default values are loaded. 
  * The environnement variable should have the same name than the severity
  * with the prefix ERS_ in front. The whole name should be in uppercases.
  * For instance for severity_t warning, the environnement variable should be
  * \c ERS_WARNING. 
  * \param s the severity_t 
  * \return the key describing the stream. 
  */

const char* ers::StreamFactory::key_for_severity(severity_t s) {
    char key_buffer[64] ; 
    snprintf(key_buffer,sizeof(key_buffer),"ERS_%s",ers::Core::to_string(s)) ;
    char *c = &(key_buffer[0]);
    while(*c) {
	*c = toupper(*c) ;
	c++ ;
    } // while
    const char *env = ::getenv(key_buffer) ; 
    if (env!=0) return env ;
    const char* static_key = DEFAULT_STREAMS[s] ; 
    if (static_key) return static_key ; 
    return ers::DefaultStream::KEY ; 
} // key_for_severity

/** Builds a stream from a textual key 
  * The key should have the format \c protocol:path.extension
  * In certain cases, the path will be empty. 
  * For instance to write in XML format to the error stream, the key is: 
  * \c cerr:.xml 
  * \param key the textual key 
  * \note the stream is allocated on the stack, it is the caller's responsibility to delete it.
  */

ers::Stream *ers::StreamFactory::create_stream(const std::string &key) const {
    std::string protocol ; 
    std::string uri ; 

    std::string::size_type colon = key.find(':') ; 
    if (colon==std::string::npos) {
    	protocol = key ; 
    } else {
        protocol = key.substr(0,colon) ; 
	std::string::size_type uri_start = colon+1 ; 
	if (uri_start<key.size()) {
	    uri = key.substr(uri_start) ; 
	} // if
    } // colon present 
    for(stream_factory_collection::const_iterator pos=m_factories.begin();pos!=m_factories.end();++pos) {
	create_stream_callback callback = pos->second; 
	Stream *s = callback(protocol,uri); 
	if (s!=0) return s ; 
    } // for
    fprintf(stderr,"Warning, could not find stream for key protocol=\"%s\" uri=\"%s\" (%s)\n",protocol.c_str(),uri.c_str(),key.c_str()); 
    return new DefaultStream(); 
} // create_stream

/** Builds a stream for a given severity. 
  * The actual key for the severity_t is found using \c key_for_severity,
  * then the appropriate stream is constructred using \c create_stream
  * \see key_for_severity()
  * \see create_stream()
  * \param s the severity_t of the requested stream
  * \return the newly created stream
  * \note the stream is allocated on the stack, it is the caller's responsibility to delete it.
  */

ers::Stream *ers::StreamFactory::create_stream(severity_t s) {
    const char* key = key_for_severity(s); 
    return create_stream(key); 
} // get_stream


/** Sends an Issue to the fatal error stream 
 * \param issue_ptr 
 */

void ers::StreamFactory::fatal(Issue *issue_ptr) {
    ERS_PRE_CHECK_PTR(issue_ptr); 
    issue_ptr->severity(ers::fatal); 
    dispatch(issue_ptr,false); 
} // fatal

/** Sends an Issue to the error stream 
 * \param issue_ptr 
 */

void ers::StreamFactory::error(Issue *issue_ptr) {
    ERS_PRE_CHECK_PTR(issue_ptr); 
    issue_ptr->severity(ers::error); 
    dispatch(issue_ptr,false); 
} // error

/** Sends an Issue to the warning stream 
* \param issue_ptr the issue to send
*/

void ers::StreamFactory::warning(Issue *issue_ptr) {
    ERS_PRE_CHECK_PTR(issue_ptr); 
    issue_ptr->severity(ers::warning); 
    dispatch(issue_ptr,false); 
} // warning

/** Sends an issue to the debug stream 
 * \param issue_ptr the Issue to send
 * \param s the severity_t of the associated stream. 
 *          Accepted values: \li \c ers_debug_0 \li \c ers_debug_1 \li \c ers_debug_2 \li \c ers_debug_3 
 */

void ers::StreamFactory::debug(Issue *issue_ptr, severity_t s) {
    ERS_PRE_CHECK_PTR(issue_ptr); 
    ERS_PRECONDITION(s<ers::information,"severity_t is not debug : %s (%d) %d",ers::Core::to_string(s),s,ers::information);
    issue_ptr->severity(s) ; 
    dispatch(issue_ptr,false);
} //  debug

/** Sends a message to the debug stream 
* \param c the Context of the message
* \param message the text of the message 
* \param s the severity_t of the associated stream. Accepted values: \li \c ers_debug_0 \li \c ers_debug_1 \li \c ers_debug_2 \li \c ers_debug_3 
*/

void ers::StreamFactory::debug(const Context &c, const std::string &message, severity_t s) {
    LogIssue log_issue(c,s,message); 
    debug(&log_issue,s); 
} // debug

/** Sends a message to the warning stream 
 * \param c the context of the message
 * \param message the message to send 
 */

void ers::StreamFactory::warning(const Context &c, const std::string &message) {
    LogIssue log_issue(c,ers::warning,message);
    warning(&log_issue); 
} // warning


/** Dispatches an issue to the appropriate stream.
 * The stream is decided based upon the severity_t specified in the Issue.
 * If \c throw_error is true errors and fatal errors are not sent to a stream, but thrown in the context of the caller.
 * \param issue_ptr the Issue to dispatch
 * \param throw_error should errors and fatals are thrown 
 */

void ers::StreamFactory::dispatch(Issue *issue_ptr, bool throw_error) {
    ERS_PRE_CHECK_PTR(issue_ptr); 
    if (throw_error && issue_ptr->is_error()) { throw *issue_ptr  ; } 
    const severity_t s = issue_ptr->severity() ;
    Stream *stream_ptr =  instance()->get_stream(s) ; 
    ERS_CHECK_PTR(stream_ptr);
    stream_ptr->send(issue_ptr); 
} // dispatch


void ers::StreamFactory::dispatch(Issue &issue_ref, bool throw_error) {
	dispatch(&issue_ref,throw_error); 
} // dispatch


void ers::StreamFactory::set_stream(severity_t s, const std::string &key) {
    instance()->set(s,key.c_str()) ; 
} // set



// Member methods
// --------------------------------------

/** Gets stream for severity_t 
  * \param s the severity_t of the requested stream
  * \return the stream 
  */

ers::Stream * ers::StreamFactory::get_stream(severity_t s)  {
    if (m_streams[s]==0) {
	m_streams[s]=create_stream(s) ; 
    } // if 
    return m_streams[s] ; 
} // get_stream


/** Sets the stream for a given severity_t 
  * \param severity_t  severity_t of the stream 
  * Accepted values: \li \c ers_debug_0 \li \c ers_debug_1 \li \c ers_debug_2 \li \c ers_debug_3 
  * \li \c ers_information \li \c ers_notification \li \c ers_warning
  * \li \c ers::error \li \c ers_fatal
  * \param s the new stream 
  */

void ers::StreamFactory::set(severity_t severity, Stream *s) {
    ERS_PRE_CHECK_PTR(s); 
    ERS_PRECONDITION(severity_none < severity && severity < severity_max,"illegal severity_t %d",(int) severity);
    if (m_streams[severity]) {
	delete m_streams[severity] ;
    } // if there is a stream defined 
    m_streams[severity] = s ; 
} // stream

/** Builds a stream using a string key and sets it up for a given severity
  * \param severity_t  severity_t of the stream 
  * Accepted values: \li \c ers_debug_0 \li \c ers_debug_1 \li \c ers_debug_2 \li \c ers_debug_3 
  * \li \c ers_information \li \c ers_notification \li \c ers_warning
  * \li \c ers::error \li \c ers_fatal
  * \param key the key used to build the new stream 
  */

void ers::StreamFactory::set(severity_t severity, const char* key) {
    ERS_PRE_CHECK_PTR(key); 
    Stream *s = create_stream(key);
    set(severity,s); 
} // 

/** 
  * \return the current fatal stream 
  */

ers::Stream *ers::StreamFactory::fatal()  { return get_stream(ers::fatal) ; } // fatal 

/**
 * \return the current error stream 
 */

ers::Stream* ers::StreamFactory::error()  { return get_stream(ers::error) ; } // error

/** 
 * \return the current warning stream 
 */

ers::Stream* ers::StreamFactory::warning()  { return get_stream(ers::warning); } // warning


/** Finds the debug stream 
 * \param s the severity_t of the associated stream. 
 * Accepted values: \li \c ers_debug_0 \li \c ers_debug_1 \li \c ers_debug_2 \li \c ers_debug_3 
 * \return debug stream 
 */

ers::Stream* ers::StreamFactory::debug(severity_t s)  {
    ERS_PRECONDITION(s<ers::information && s>ers::severity_none,"severity_t is not debug : %s (%d)",ers::Core::to_string(s),s);
    return get_stream(s) ; 
} // debug




/** Registers a factory function with the stream factory.
  * The callback is function that takes two parameters <ol>
  * <li>a string describing the protocol, for instance \c file </li>
  * <li>a string describing a uri, can be a path, a suffix or anything</li>
  * </ol>
  * The function should return a heap allocated stream, or null if it does not
  * understand the protocol. 
  * \param name name of the stream type (human display only).
  * \param callback the callback function
  * \return \c true if the registration was sucessfull 
  */

bool ers::StreamFactory::register_factory(const std::string &name, create_stream_callback callback) {
    // std::cerr << "registering " << name << std::endl ; 
    m_factories[name] = callback ;
    return true  ;
} // register_factory

/** Writes a description of the factory to a stream. 
  * This method gives a list of all registered stream types. 
  * \param stream the stream to write into
  */

void ers::StreamFactory::write_to(std::ostream& stream) const {
    stream << "Stream factory - registered streams\n" ; 
    stream << "-----------------------------------\n" ; 
    int i = 0 ; 
    for(stream_factory_collection::const_iterator pos=m_factories.begin();pos!=m_factories.end();++pos) {
	std::string name = pos->first; 
	stream << i << ")\t" << name << std::endl; 
	i++ ; 
    } // for
    stream << "-----------------------------------\n" ; 
} // write_to


/** Streaming operator
  * \param stream destination stream
  * \param factory the factory object to display
  * \return the stream passed as first parameter
  * \see ers::StreamFactory::write_to()
  */

std::ostream& ers::operator<<(std::ostream& stream, const ers::StreamFactory& factory) {
    factory.write_to(stream);
    return stream ; 
} // operator




