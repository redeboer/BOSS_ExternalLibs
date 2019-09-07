// $ Header:$
#ifndef FACILITIES_UTIL_H
#define FACILITIES_UTIL_H

#include <string>
#include <vector>
#include <map>

/** @file Util.h
    @author J. Bogart

    This file declares the class Util for basic static utilities 
    and an associated exception class.
    
*/
namespace facilities {
  /// This class provides a home for utility functions with no need for
  /// any context (hence static)

  /// Exception class used by expandEnvVar
  class Untranslatable {
  public:
    Untranslatable(const std::string& toTrans) : m_badVar(toTrans) {}
    std::string m_badVar;
  };

  /// Exception class used when converting from string to numeric type

  class WrongType {
  public:
    WrongType(const std::string& toConvert, const std::string& typeName) : 
      m_toConvert(toConvert), m_typeName(typeName) {}
    std::string getMsg() {
      std::string msg =
        "facilities::WrongType.  Cannot convert '" + m_toConvert + "' to type "
        + m_typeName;
      return msg;
    }
  private:
    std::string m_toConvert;
    std::string m_typeName;
  };

  class Util {
  public:
    /** Given input string @a toExpand expand references to environment
        variables, by default of the form $(varname) and put the 
        expanded version back into the original string.  Alternate
        delimiters for the @a varname may optionally be specified
        @param   toExpand string for which expansion is to be done
        @param   openDel opening delimiter (defaults to "$(")
        @param   closeDel closing delimiter (defaults to ")")
        
        @return  -1 if attempt at expansion failed at least once,
                 else number of successful expansions.

        TODO:  Perhaps add optional arguments to specify alternate
               delimiters.
    */
    static int expandEnvVar(std::string* toExpand, 
                            const std::string& openDel = std::string("$("),
                            const std::string& closeDel = std::string(")"));

    /** Given an input integer @a val to convert and an output string @a outStr
      converts val into a std::string.
      This method duplicates the stdlib.h method itoa, except that it returns
      std::string rather than char*.
      @param   val
      @param   outStr will be modified by this method

      @return  const char* based on the contents of outStr.c_str()
    */
    static const char* itoa(int val, std::string &outStr);

    /// converts an std::string to an integer
    static int atoi(const std::string& InStr);


    /// converts a std::string to a double.  If string contents are not
    /// of proper form, throws facilities::WrongType
    static double stringToDouble(const std::string& InStr);

    /// converts a std::string to an int.  If string contents are not
    /// of proper form, throws facilities::WrongType
    static int stringToInt(const std::string& InStr);



    /** This routine breaks down a string into tokens, based on the
        characters appearing in the string @a delimiters.
        @param input       string to be tokenized
        @param delimiters  string containing one or more delimiter characters
        @param tokens      vector of strings to hold resulting tokens
        @param clear       if true (default) @a tokens will be cleared
                           at the start of processing
    */
    static void stringTokenize(std::string input, const std::string &delimiters,
                               std::vector<std::string> &tokens,
                               bool clear = true);

    /** This routine breaks down a string into key/value token pairs and
        stores them in the user-supplied map. , based on the
        characters appearing in the string @a delimiters and the value
        of @a pairDelimiter.  In a typical example, @a input could be
        "key1=val1,key2=val2,key3=val3".  In this case invoke with
        delimiters=std::string(",") and pairDelimiter=std::string("=")
        (or omit pairDelimiter since it has the default value) 
        @param input         string to be tokenized
        @param delimiters    string containing one or more delimiter 
                             characters
        @param tokenMap      map of strings to hold resulting tokens
        @param pairDelimiter string separating key and value; defaults
                             to "=" 
        @param clear         if true (default) @a tokens will be cleared
                             at the start of processing
    */
    static void keyValueTokenize(std::string input, 
                               const std::string &delimiters,
                               std::map<std::string,std::string> &tokenMap,
			       const std::string& pairDelimiter = 
                                 std::string("="),
                               bool clear = true);

    /** return the "non-directory" part of a (supposed) file identifier, 
        @a path.  Environment variable translation should be done before 
        calling @a basename.
        @sa { Util::expandEnvVar }
        @param path        string assumed to be a file identifier. 
    */
    static std::string basename(const std::string &path);

    /**
       Trim trailing white space characters from the supplied string.
       White space characters for this purpose are blank, carriage return,
       line feed and form feed.  Return # of characters trimmed.
    */
    static unsigned trimTrailing(std::string* toTrim);
  };
}

#endif
