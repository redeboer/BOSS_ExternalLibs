/*
 *  Context.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 26.11.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */
#include <iostream>
#include <sstream>
#include "ers/Context.h"
#include <cstdlib> 

#if defined(__GNU_LIBRARY__)
#include <execinfo.h>
#endif

ers::Context *ers::Context::empty_instance = 0 ; 
std::string ers::Context::s_host_type  ; 

std::vector<std::string>  ers::Context::default_qualifiers ;

/** Returns the empty instance 
  * \return a pointer to the empty instance.
  */ 

const ers::Context *ers::Context::empty() {
    if (! empty_instance) {
	std::string empty = "" ; 
	empty_instance = new ers::Context(empty,0,empty,empty,empty,empty,empty,empty) ; 
    } // if 
    return empty_instance ; 
} // empty 

/** Tries to gues the host name 
* \return a string describing the host, in the form <var>architecture</var>-<var>operating-system</var>
*/

std::string & ers::Context::host_type() {
    if (s_host_type.empty()) build_host_type() ; 
    return s_host_type ; 
} // plateform

/** Gives the current debug level 
  * \return debug level, or -1 if it cannot be determined 
  */

int ers::Context::debug_level() {
#if defined(DEBUG_LEVEL) 
    return DEBUG_LEVEL ; 
#else 
    return -1 ; 
#endif
} // debug_level

void ers::Context::add_qualifier(const std::string &qualif) {
    default_qualifiers.push_back(qualif) ;
} // add_qualifier

/** Constructor - defines an Issue context.
  * This constructor should generally not be called directly, instead use the macro \c ERS_HERE. 
  * \param filename name of the source code file
  * \param line_number line_number in the source code 
  * \param function_name name of the function - either pretty printed or not
  * \param compiler_name name of the compiler 
  * \param compiler_version version of the compiler 
  * \param compilation_time time of the compilation 
  * \param compilation_date date of the compilation 
  */

ers::Context::Context(const std::string &filename, int line_number, const std::string &function_name, 
                      const std::string &compiler_name, const std::string &compiler_version, 
                      const std::string &compilation_time, const std::string &compilation_date, 
		      const std::string &package_name) {
    this->m_file_name = filename ; 
    this->m_line_number = line_number ; 
    this->m_function_name = function_name ; 
    this->m_compiler_name = compiler_name ; 
    this->m_compiler_version = compiler_version ; 
    this->m_compilation_date = compilation_date ; 
    this->m_compilation_time = compilation_time ; 
    this->m_package_name = package_name ; 
#if defined(__GNU_LIBRARY__)
    void * array[128] ; 
    const int n_size =  backtrace (array,128) ; 
    char ** symbols = backtrace_symbols(array, n_size);
    for (int i = 1; i < n_size; i++) { // we start at 1, current position is noise 
	this->m_stack_frames.push_back(symbols[i]);
    } // for
    free(symbols);
#endif
} // Context

/** The source code file name 
  * \return path of the source file 
  */

const std::string & ers::Context::file() const throw() {
    return m_file_name ; 
} // file

/** The line number in the source code 
  * \return line number 
  */

int ers::Context::line() const throw() {
    return m_line_number ; 
} // line 

/** Name of the function containing the context 
  * \return name of the function
  */

const std::string & ers::Context::function() const throw() {
    return m_function_name ; 
} // function

/** Pretty printed code position 
  * format: file_name:line_number (function_name)
  * \return reference to string containing format
  * \note the file name is truncated from the last slash 
  */

const std::string & ers::Context::position() const {
    if (m_position.empty()) {
	std::ostringstream position_s ;
	if (! m_package_name.empty()) {
	    position_s << m_package_name << ":" ; 
	} 
	if (! m_file_name.empty()) {
	    const std::string::size_type p = m_file_name.rfind('/') ; 
	    if (std::string::npos == p) {
		position_s << m_file_name ; 
	    } else {
		position_s << (m_file_name.substr(p+1)) ; 
	    } //
	    position_s << ":" << m_line_number << " ";
	} 
	if (! m_function_name.empty()) {
	    position_s << "(" << m_function_name << ")" ;
	} // if
	m_position = position_s.str();
    } // cached version not calculated 
    return m_position ; 
} // position

/** Pretty printed compiler name 
  * \return reference to string containing format 
  */

const std::string & ers::Context::compiler() const {
    if (m_compiler.empty()) {
	if (! m_compiler_name.empty()) {
	    std::ostringstream compiler_s ;
	    compiler_s << m_compiler_name << " " << m_compiler_version ; 
	    m_compiler = compiler_s.str(); 
	} else {
	    m_compiler = "unknown" ; 
	} 
    } // build cache
    return m_compiler ; 
} // compiler

/** Pretty printed compilation time description
  * \return reference to string containing description
  */

const std::string & ers::Context::compilation() const {
    if (m_compilation.empty()) {
	std::ostringstream compilation_s ;
	if (! m_compilation_time.empty()) {
	    compilation_s << m_compilation_time << " " ; 
	} // compilation time
	if (! m_compilation_date.empty()) {
	    compilation_s << m_compilation_date ; 
	} // compilation date
	m_compilation = compilation_s.str(); 
    } // if
    return m_compilation ; 
} // compilation

/** \return package name 
  */

const std::string & ers::Context::package_name() const throw() {
    return m_package_name ; 
} // package_name

void ers::Context::build_host_type() {
    std::ostringstream plateform_s ;
#if defined(__linux__) 
    plateform_s << "linux" ; 
#endif
#if defined(__OpenBSD__)
    plateform_s << "OpenBSD" ;
#endif
#if defined(__FreeBSD__) 
    plateform_s << "FreeBSD" ;
#endif
#if defined(__APPLE__) && defined(__MACH__)
    plateform_s << "Darwin" ; 
#endif
#if defined(__SOLARIS__)
    plateform_s << "Solaris" ;
#endif    
    plateform_s << "/" ; 
#if defined(__POWERPC__)  || defined(__ppc__ ) || defined( __PPC__ ) || defined( powerpc ) || defined( ppc )
    plateform_s << "PowerPC" ; 
#endif
#if defined(__i386__) || defined(__INTEL__) || defined( intel ) || defined( _M_IX86 ) 
    plateform_s << "i386" ;
#endif
#if defined(sparc) || defined(__sparc)   
    plateform_s << "Sparc" ; 
#endif
   s_host_type=  plateform_s.str();
} // build_host_type

int ers::Context::stack_frames() const throw() {
   return m_stack_frames.size();
} // stack_frames

const std::string & ers::Context::stack_frame(int i) const {
   return m_stack_frames[i] ; 
} // stack_frame

/** Returns the set of qualifiers associated with context
  * At the moment, this includes the default qualifiers plus the package 
  * \return array of strings represnting the qualifiers 
  */

std::vector<std::string>  ers::Context::qualifiers() const throw() {
    std::vector<std::string> qualif = default_qualifiers ;
    if (! m_package_name.empty()) {
	qualif.push_back(m_package_name) ; 
    } // if 
    return qualif ; 
} // qualifiers


