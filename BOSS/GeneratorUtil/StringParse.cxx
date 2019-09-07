// --------------------------------------------------
// 
// File:  GeneratorUtil/StringParse.cxx
// Description:
//    This code is used to parse a string .
//    it breaks it into components which are seperated by spaces 
//    in the original. The components can be accessed and converted to doubles or ints
// AuthorList:
//         Ian Hinchliffe April 2000
#include "GeneratorUtil/StringParse.h"
#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>

StringParse::StringParse(std::string input) {
  m_past_end="?!?";
  m_lstring=input;
  m_nword=0;
  std::string::size_type pos=0;
  std::string::size_type prev_pos=0;
  while((pos=input.find_first_of(' ',pos)) != std::string::npos){
      m_lsubstring.push_back(input.substr(prev_pos,pos-prev_pos));
      prev_pos=++pos;
    }
    // special handling for last substring
    m_lsubstring.push_back(input.substr(prev_pos,pos-prev_pos));
  m_nword=m_lsubstring.size();
}
std::string StringParse::piece(const int &num){
  if(num <= m_nword){
    return m_lsubstring[num-1];
  }
  else { 
    return m_past_end;
  }
}
int StringParse::intpiece(const int & num){  
if(num <= m_nword){
     int i= atoi(m_lsubstring[num-1].c_str());
     return i;
  }
  else { 
    return -1;
  }
}
long StringParse::longpiece(const int & num){  
if(num <= m_nword){
     int i= atol(m_lsubstring[num-1].c_str());
     return i;
  }
  else { 
    return -1;
  }
}
float StringParse::numpiece(const int & num){  
if(num <= m_nword){
     float x= atof(m_lsubstring[num-1].c_str());
     return x;
  }
  else { 
    return -1.1;
  }
}
StringParse::~StringParse(){
}


