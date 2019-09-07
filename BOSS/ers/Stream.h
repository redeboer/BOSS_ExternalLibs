/*
 *  Stream.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 02.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */


#ifndef __ERS_ISSUE_STREAM__
#define __ERS_ISSUE_STREAM__

#include <string>
#include <iostream>

#include "ers/Core.h"
#include "ers/Context.h"

namespace ers {
    
    class Issue ; 

/** Root issue stream.
  * An ERS stream is a mean to send and receive issues. 
  * The two core method to do so are \c send and \c receive.
  * Certain subclasses of stream might implement only sending, or only receiving. 
  * The root stream class implements a null stream, i.e a stream where no issue can be read from 
  * and silently discards sent issues. 
  * \author Matthias Wiesmann
  * \version 1.0
  * \brief Root/Null issue stream
  */
    
class Stream {
    friend class Issue ; 
protected:
public:    
    static const char* const NULL_STREAM_KEY ;          /**< Key for discard stream */
    Stream();
    Stream(const Stream &other); 
    operator std::string() const ; 
    virtual ~Stream(); 
    virtual void send(const Issue *i) ;                /**< \brief Sends an issue into the stream */
    virtual Issue *receive() ;                         /**< \brief Receives an issue from the stream */
    virtual void print_to(std::ostream& stream) const ; 
} ; 
std::ostream& operator<<(std::ostream& target, const ers::Stream & s);

} // ers 




#endif

