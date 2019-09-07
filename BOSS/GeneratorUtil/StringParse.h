// --------------------------------------------------
// 
// File:  GeneratorModule/StringParse.h
// Description:
//    This code is used to parse a string.
//    It breaks it into components which are seperated by spaces  
//    in the original.
//    Three methods can then be called to 
//    Return the nth component as a string (piece method)
//    Return the nth component as an integer (intpiece method) if conversion is possible, returns -1 if it is off the end and 0 if it cannot be converted
//    Return the nth component as a double (numpiece method)  if conversion is possible, returns -1.1 if it is off the end and 0 if it cannot be converted
// AuthorList:
//         Ian Hinchliffe April 2000

#ifndef STRINGPARSE_H
#define STRINGPARSE_H
#include <string>
#include <vector>
class StringParse {
public:
    StringParse(std::string input); //constructor
  StringParse(); //constructor
  ~StringParse(); //constructor
  std::string piece(const int & num);
  int intpiece(const int & num); 
  long longpiece(const int & num); 
  float numpiece(const int & num);
    unsigned string_size(void) const;
    
private:
  std::string m_lstring;
  std::vector<std::string> m_lsubstring;
  int m_nword;
  std::string m_past_end;
};


inline unsigned
StringParse::string_size(void) const
{ return m_nword; }

#endif
