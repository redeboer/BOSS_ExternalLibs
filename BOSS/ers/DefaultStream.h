/*
 *  DefaultStream.cxx
 *  Test
 *
 *  Created by Matthias Wiesmann on 21.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */
 
#include <ostream>
#include <sstream>

#include "ers/Stream.h"

namespace ers {

/** This class displas an Issue on the standard error stream
  * This class is meant to be used as default stream for simple on screen display.
  * More advanced streams are offered in the \c System package 
  * This stream only sends output to the standard error stream.
  * To select this stream, simply use the \c default string key. 
  * You can get a verbose output by 
  * \author Matthias Wiesmann
  * \version 1.0
  * \brief Default Issue display 
  */
    
class DefaultStream  : public Stream {
protected:
    bool m_verbose ; 
public:
    static const char * const KEY ;
    static const char * const VERBOSE_KEY ; 
    DefaultStream(bool verbose=false) ;
    ~DefaultStream() ; 
    void send(const Issue *ptr); 
} ; // DefaultStream 

} // namespace ers

