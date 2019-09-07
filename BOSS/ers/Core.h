/*
 *  Core.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 13.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

/** \file Core.h This files contains small utility functions and general definitions for ers.
  * \version 1.0 
  */


#ifndef __ERS_UTIL__
#define __ERS_UTIL__  "0.3"

#include <string>
#include <map>
#include <vector>

namespace ers {
    
    typedef enum _severity_t { severity_none, debug_0, debug_1, debug_2, debug_3, information, notification, warning, error, fatal,  severity_max } severity_t  ; 
    typedef enum _responsibility_t  { resp_client, resp_server, resp_dependency , resp_unknown, resp_max } responsibility_t ; 
    typedef std::map<std::string, std::string> string_map_type ;   
    typedef std::vector<std::string> string_vector ;
    
    /** This class contains some general constants.
      * \brief Miscalenous constants
      */
    class Core {
	static const char* const SEVERITY_NAMES[] ;
	static const char* const BOOLEAN_NAMES[] ; 
	static const char* const RESPONSIBILITY_NAMES[] ; 
protected:
public:
	static const std::string empty_string ;
	static severity_t parse_severity(const char *s) throw() ;                     /**< \brief string to severity_t */
	static severity_t parse_severity(const std::string &s) throw() ;              /**< \brief string to severity_t */
	static const char* to_string(severity_t s) throw() ;                          /**< \brief severity_t to string */
	static responsibility_t parse_responsibility(const char *s) throw() ;         /**< \brief string to responsibility */
	static responsibility_t parse_responsibility(const std::string &s) throw() ;  /**< \brief string to responsibility */
	static const char* to_string(responsibility_t s) throw() ;                    /**< \brief responsibility to string */
	static int parse_boolean(const char* s) throw() ;                               /**< \brief string to boolean */
	static const char* to_string(bool b) throw ();                                  /**< \brief boolean to string */
	static std::string parse_prefix_string(const char **ptr) throw() ;                    /**< \brief prefix string data to string */
	static std::string umangle_gcc_class_name(const char* name) throw() ;           /**< \brief unmangles gcc RTTI names */
	static std::vector<std::string> tokenize(const std::string &text, const std::string &separators) ; 
    } ; // Core
}

#endif
