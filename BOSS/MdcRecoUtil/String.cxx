//--------------------------------------------------------------------------
// File and Version Information:
//
// $Id: String.cxx,v 1.1.1.1 2005/04/21 01:15:12 zhangy Exp $
//
// Description:
//      The functions, within this namespace, are needed by BaBar to replace
//      those lost in the rw to STL string migration.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//       A. De Silva
//
// Copyright Information:
//
//------------------------------------------------------------------------


#include "MdcRecoUtil/String.h"

#include <string>
#include <iostream>

int 
bes::String::compare_nocase(const std::string& s, const std::string& s2) {
  std::string::const_iterator p = s.begin();
  std::string::const_iterator p2 = s2.begin();

  while (p != s.end() && p2 != s2.end() ) {
    if (toupper(*p) != toupper(*p2)) 
      return (toupper(*p) < toupper(*p2)) ? -1 : 1;
    ++p;
    ++p2;
  }
  return (s2.size() == s.size()) ? 0 : (s.size() < s2.size()) ? -1 : 1;
}

int 
bes::String::find_nocase(const std::string& s, const std::string& s2) {
  std::string str1 = s;
  transformToUpper(str1);
  std::string str2 = s2;
  transformToUpper(str2);
  return str1.find(str2);
}

std::string 
bes::String::toLower(const std::string& str) {
  std::string result(str);
  transformToLower(result);
  return result;
}

std::string 
bes::String::toUpper(const std::string& str) {
  std::string result(str);
  transformToUpper(result);
  return result;
}

unsigned int
bes::String::rwHash(const std::string& str){
  const char* data = str.c_str();
  int length = str.size(), total=0;
  for ( int i=0; i<length; i++ ) total += data[i];
  return total;
}


