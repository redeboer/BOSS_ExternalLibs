/*
 *  FIFOStream.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 02.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#include "ers/FIFOStream.h"
#include "ers/Precondition.h"
#include "ers/NotImplemented.h"
#include "ers/InvalidReferenceIssue.h"
#include "ers/StreamFactory.h"

const char* const ers::FIFOStream::FIFO_STREAM_KEY = "fifo" ; 

namespace {
    ers::Stream *create_stream(const std::string &protocol, const std::string &uri) { 
	(void) uri ; // to shut up the compiler 
	if (protocol==ers::FIFOStream::FIFO_STREAM_KEY) return new ers::FIFOStream() ; 
	return 0 ;
    } 
    bool registered = ers::StreamFactory::instance()->register_factory(ers::FIFOStream::FIFO_STREAM_KEY,create_stream) ;
} 


ers::FIFOStream::FIFOStream() : Stream() {} 

ers::FIFOStream::FIFOStream(const FIFOStream &other) : Stream(other) {
    for(unsigned int i=0;i<other.m_issue_queue.size();i++) {
	Issue *cloned = other.m_issue_queue[i]->clone();
	m_issue_queue.push_back(cloned);
    } // for
} // FIFOStream

ers::FIFOStream::~FIFOStream() {}

/** Sends the issue into the stream.
  * This method should put the issue into a FIFO queue and be non blocking
  * @param i pointer to the issue to send
  * @note This method is not implemented, pending some form of common TDAQ thread library. 
  */

void ers::FIFOStream::send(const ers::Issue *issue_ptr) {
    ERS_PRE_CHECK_PTR(issue_ptr); 
    Issue *cloned = issue_ptr->clone();
    m_issue_queue.push_back(cloned); 
} // send

/** Blocking read into the stream. 
  * @return a reference to the next Issue
  * @note This method is not implemented, pending some form of common TDAQ thread library. 
  */

ers::Issue *ers::FIFOStream::receive() {
    if (m_issue_queue.empty()) return 0 ; 
    Issue *issue = m_issue_queue[0] ; 
    m_issue_queue.pop_front();
    return issue ; 
} // receive

void ers::FIFOStream::print_to(std::ostream& stream) const {
    stream << ers::FIFOStream::FIFO_STREAM_KEY << ":[" ;
    for(unsigned int i=0;i<m_issue_queue.size();i++) {
	stream << m_issue_queue[i]->what() ; 
	if (i!=m_issue_queue.size()-1) {
	    stream << ", " ; 
	} // if not last 
    } // for elements
    stream << "]" ; 
} // print_to


