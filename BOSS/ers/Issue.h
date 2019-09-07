/*
 *  Issue.h
 *  ers
 *
 *  Created by Matthias Wiesmann on 08.12.04.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#ifndef ERS_Issue_
#define ERS_Issue_

#include <map>
#include <string>
#include <stdint.h>

#include "ers/Core.h"
#include "ers/Stream.h"
#include "ers/Context.h"
#include "ers/IssueFactory.h"

namespace ers {
    
    /** This class is the root Issue object.  
    *  The class does not contain any fields with information, instead everything is stored in a hashmap 
    *  as key - value paris (both strings). 
    *  The object contains utility methods to allow the manipulation of those key / values and
    *  code to insert common values into it, like time, compilation information, host information etc.
    *  For an example of how to build an actual subclass of issue look at the source of ExampleIssue.
    *  \author Matthias Wiesmann
    *  \version 1.1
    *  \note Issue chaining is handled by copying the cause issue on the stack and keeping a pointer to it.
    *  When the object is destroyed, it destroys the pointed issue. This means we can only chain issues that 
    *  correctly implement the factory methods required by the IssueFactory class
    *  \see ers::IssueFactory
    *  \see ExampleIssue
    *  \brief Root Issue class
    */
    
    class Issue : public std::exception  {  
	friend class Stream ; 
	friend class IssueFactory ; 
public:	
        static const char *const CLASS_KEY ;                               /**< \brief key for class information */
	static const char *const COMPILATION_TIME_KEY ;                    /**< \brief key for compilation time */
	static const char *const COMPILATION_TARGET_KEY ;                  /**< \brief key for compilation target */
	static const char *const COMPILER_KEY ;                            /**< \brief key for compilator type */
	static const char *const COMPILATION_DEBUG_LVL_KEY ;            
	static const char *const CPP_CLASS_KEY ;                           /**< \brief key for c++ class (might be mangled) */
	static const char *const ERS_VERSION_KEY ;                         /**< \brief key for ERS version */
	static const char *const HOST_NAME_KEY ;                           /**< \brief key for hostname */
	static const char *const HOST_TYPE_KEY ;                           /**< \brief key for host type (architecture / os) */
	static const char *const HOST_IP_ADDR_KEY ;                        /**< \brief key for host ip address */
	static const char *const MESSAGE_KEY ;                             /**< \brief key for human readable */
	static const char *const PROCESS_ID_KEY ;                          /**< \brief key for the process id (number)*/
	static const char *const PROCESS_PWD_KEY ;                         /**< \brief key for the process working directory */
	static const char *const PROGRAM_NAME_KEY ;                        /**< \brief key for the name of the program */
	static const char *const RESPONSIBILITY_KEY ;                      /**< \brief key for the responsibility of the issue (text) */
	static const char *const SEVERITY_KEY ;                            /**< \brief key for the severity_t of the issue */
	static const char *const SOURCE_POSITION_KEY ;                     /**< \brief key for position in the source code */
	static const char *const SOURCE_PACKAGE_KEY ;                      /**< \brief package name associated with source code */
	static const char *const TIME_KEY ;                                /**< \brief key for the time of the issue (text) */
	static const char *const TRANSIENCE_KEY ;                          /**< \brief key for the transience of the issue (text) */
	static const char *const USER_ID_KEY ;                             /**< \brief key for the user-id of the owner of the process */
	static const char *const USER_NAME_KEY ;                           /**< \brief key for the user-name of the owner of the process */
	static const char *const CAUSE_PSEUDO_KEY ;                        /**< \brief key used when serializing the cause issue, this key is not used in the value table */
	static const char *const CAUSE_TEXT_KEY ;                          /**< \brief key used to store the cause issue's message */
	static const char *const QUALIFIER_LIST_KEY ;                      /**<Ê\brief key used to store the qualifier list */
	static const char *const EXIT_VALUE_KEY ;                          /**< \brief key used to store the exit value */
	static const char *const ISSUE_CLASS_NAME ;                        /**< \brief name of the class, used for serialisation */
	
protected:
	    Issue *m_cause ;                                               /**< \brief Issue that caused the current issue */
	mutable std::string m_class_name ;                             /**< \brief class name */
	mutable std::string m_human_description ;                      /**< \brief Human readable description (cache)*/
	string_map_type m_value_table  ;                               /**< \brief Optional properties. */
	void insert(const Context *context) throw() ;                  /**< \brief Inserts the context */
	void insert_time() throw() ;                                   /**< \brief Inserts current time */
        void setup_common(const Context *context) throw() ;            /**< \brief Sets up the common fields. */
        void finish_setup(const std::string &message) throw() ;        /**< \brief Finishes the setup of the Issue */
        Issue(const Context &context, severity_t s);                 /**< \brief Constructor for subclasses */
        void set_values(const string_map_type &values) throw();        /**< \brief sets the value table */
public:
	    Issue();  
	Issue(const Issue &issue); 
        Issue(const string_map_type &values); 
	Issue(const Context &context, severity_t s, const std::string &message);   
        Issue(const Context &context, severity_t s, const std::exception *cause); 
	virtual ~Issue() throw() ;
	Issue *clone() const ; 
	
	const Issue *cause() const throw() ;                           /**< \brief return the cause Issue of this Issue */
	void cause(const std::exception *cause=0);                     /**< \brief Initialises the cause field */
	operator std::string() const ;                                 /**< \brief Converts the issue into a string */
	
	Issue operator=(const Issue &issue);                           /**< \brief Affectation operator */
	bool operator==(const Issue &other) const throw();             /**< \brief Equality operator */
	const std::string& operator[](const std::string &key) const throw(); 
	
	const std::string &get_value(const std::string &key, const std::string &def) const throw() ;       /**< \brief Reads the property list. */
	const std::string &get_value(const std::string &key) const throw(); 
	int get_int_value(const std::string &key, int def=0) const throw() ;       /**< \brief Get a value of the table as an integer */
	long get_long_value(const std::string &key, long def=0) const throw() ;    /**< \brief Get a value of the table as a long integer */
	double get_double_value(const std::string key, double def) const throw() ; /**< \brief Get a value of the table as double */
	void set_value(const std::string &key, uint8_t value) throw() ;            /**< \brief Sets a value 8 bit unsigned */
	void set_value(const std::string &key, uint16_t value) throw() ; 
	void set_value(const std::string &key, uint32_t value) throw() ;    
	void set_value(const std::string &key, uint64_t value) throw() ;           
	void set_value(const std::string &key, int8_t value) throw() ;            
	void set_value(const std::string &key, int16_t value) throw() ; 
	void set_value(const std::string &key, int32_t value) throw() ;    
	void set_value(const std::string &key, int64_t value) throw() ;           	
	void set_value(const std::string &key, double value) throw() ;             /**< \brief Sets a value (double float) */
	void set_value(const std::string &key, const std::string &value) throw() ; /**< \brief Sets a value (string) */
	void set_value(const std::string &key, const char* value) throw() ;        /**< \brief Sets a value (c-string) */
	void set_value(const std::string &key, const void* ptr) throw() ; 
	int values_number() const ;                                                /**< \brief How many key / values */
	
	virtual const char *get_class_name() const throw() ;           /**< \brief Get key for class (used for serialisation)*/
	const string_map_type* get_value_table() const ;               /**< \brief extract value table */
	severity_t severity() const throw()  ;                         /**< \brief severity_t of the issue */
	void severity(severity_t s) ;                                  /**< \brief sets the severity_t of the issue */
	bool is_error();                                               /**< \brief is the issue an error (or fatal). */
	std::string severity_message() const ;                         /**< \brief message associated with the severity_t of the issue */
	void responsibility(responsibility_t r) ;                      /**< \brief set the responsability of the issue */
	responsibility_t responsibility() const throw() ;              /**< \brief get the responsability level of the issue */
	void transience(bool tr);                                      /**< \brief sets if the issue is transient */
	int transience() const throw() ;                               /**< \brief is the issue transient */
	const std::string &human_description() const throw()  ;        /**< \brief Human description message. */
	const char* what() const throw() ;                             /**< \brief Human description message. */
	const std::string &message() const throw() ;                   /**< \brief Message */
	virtual int exit_value() const throw();                        /**< \brief value to pass to exit */
	void add_qualifier(const std::string &qualif) ;                /**< \brief adds a qualifier to the issue */
	std::vector<std::string> qualifiers() const ;                  /**< \brief return array of qualifiers */
    } ; // Issue

std::ostream& operator<<(std::ostream&, const Issue&);
Stream& operator<<(Stream&, const Issue&); 
} // ers

// This macro suppresses assertion if N_DEBUG is defined 
// 

#if defined(N_DEBUG)
#define N_ERS_STATIC_ASSERT
#define N_ERS_ASSERT
#endif

#endif

