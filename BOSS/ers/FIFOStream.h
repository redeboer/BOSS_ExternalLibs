/*
 *  FIFOStream.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 02.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#include <deque>
#include "ers/Stream.h"

namespace ers {

    /** This streams is used to transmit Issues inside a process.
      * \author Matthias Wiesmann
      * \version 1.0
      * \brief Queue stream 
      * \note This stream is \b not thread safe - it cannot depend on thread library. 
      */
    
    class FIFOStream : public Stream {
	
protected:
	std::deque<Issue*> m_issue_queue ; 
public:
	static const char* const FIFO_STREAM_KEY ;     /**< Key for FIFO stream */
	FIFOStream();
	FIFOStream(const FIFOStream &other); 
	~FIFOStream() ;
	virtual void send(const Issue *i) ;
	virtual Issue *receive() ; 
	virtual void print_to(std::ostream& stream) const ;
    } ; //  FIFOStream
} // ers
	

