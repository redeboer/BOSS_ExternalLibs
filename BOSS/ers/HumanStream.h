/*
 *  HumanStream.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 01.11.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */


#include <ostream>
#include <sstream>

#include "ers/Stream.h"

namespace ers {

/** This class streams an issue into an human readable string. 
  * It is uses by the issue class to display itself. 
  * This stream can be specified for other uses, but the user is then responsible
  * for reading data from the stream and clearing it. 
  * \author Matthias Wiesmann
  * \version 1.0
  * \brief Single line, human readable format stream. 
  */
    
class HumanStream  : public Stream {
protected:
    std::ostringstream m_out_stream ; 
public:
    static std::string to_string(const Issue *issue) throw() ; 
    static const char* const KEY ; 
    HumanStream() ;
    HumanStream(const HumanStream &other); 
    ~HumanStream() ; 
    std::string to_string() ; 
    void clear() ; 
    virtual void send(const Issue *ptr); 
    virtual void print_to(std::ostream& stream) const ; 

} ; // human_stream 

} // namespace ers

