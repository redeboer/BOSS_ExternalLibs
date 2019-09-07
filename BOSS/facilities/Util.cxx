// $Header: /bes/bes/BossCvs/Calibration/facilities/src/Util.cxx,v 1.2 2011/02/23 15:35:54 maqm Exp $

#include "facilities/Util.h"

#ifdef DEFECT_NO_STRINGSTREAM
#include <strstream>
#else
#include <sstream>
#endif

#include <iostream>
#include <cstdio>
#include <stdlib.h> 
#include <cstring> 
/** @file Util.cxx 
@author J. Bogart
*/

namespace facilities {
    
  int Util::expandEnvVar(std::string* toExpand, 
                         const std::string& openDel,
                         const std::string& closeDel) {
    unsigned opLen = openDel.size();
    unsigned clLen = closeDel.size();
    int nSuccess = 0;
        
    int envStart = toExpand->find(openDel.c_str());
    while (envStart != -1) {
      int envEnd = toExpand->find(closeDel.c_str());
            
      // add  characters to account for opening delimiter
      int afterBracket = envStart + opLen;
            
      if (!( (envStart==-1) || (envEnd==-1) ))
      {
        std::string envVariable = 
          toExpand->substr(afterBracket,(envEnd-afterBracket));
        const char * path = getenv(envVariable.c_str());
        if (path) {
          toExpand->replace(envStart,(envEnd+clLen), path);
          if (nSuccess > -1) nSuccess++;
        }
        else {
          std::cerr << "Util::expandEnvVar unable to translate " 
                    << envVariable << std::endl;
          throw Untranslatable(envVariable);
        }
      }
      envStart = toExpand->find(openDel.c_str());
    }
    return nSuccess;
  }
    
  const char* Util::itoa(int val, std::string &outStr) {
    // Purpose and Method:  Provide a standard routine to convert integers
    //    into std::string.  The method used depends upon the availability of
    //    the stringstream classes.  The stringstream classes are the
    //    standard, but some compilers do yet support them.
    //    The method used is determined by the DEFECT_NO_STRINGSTREAM
    //    macro, defined in the facilities requirements file.

    static char outCharPtr[20];

#ifdef DEFECT_NO_STRINGSTREAM
    // Using static buffer to avoid problems with memory allocation
    char a[100]=""; 
    std::ostrstream locStream(a,100);
#else
    std::ostringstream locStream;
    locStream.str("");
#endif
    locStream << val;
#ifdef DEFECT_NO_STRINGSTREAM
    locStream << std::ends;
#endif
    outStr = locStream.str();
    strcpy(outCharPtr, outStr.c_str());
    return outCharPtr;
  }
    
  int Util::atoi(const std::string& inStr) {
    // Purpose and Method:  Provide a standard routine to convert std::strings
    //    into integers.  The method used depends upon the availability of
    //    the stringstream classes.  The stringstream classes are the
    //    standard, but some compilers do yet support them.
    //    The method used is determined by the DEFECT_NO_STRINGSTREAM
    //    macro, defined in the facilities requirements file.
    // Output: returns an integer
    //    if string cannot be converted to an integer, returns zero

    int val;

#ifdef DEFECT_NO_STRINGSTREAM
    std::istrstream locStream(inStr.c_str());
#else
    std::istringstream locStream(inStr);
#endif
    locStream >> val;
    if (!locStream) {return 0;}
    return val;
  }  

  double Util::stringToDouble(const std::string& inStr) {
    double val;
    char  junk[3];
    int nItem = sscanf(inStr.c_str(), "%lg %1s", &val, junk);
    if (nItem != 1) {
      throw WrongType(inStr, "double");
    }
    return val;
  }


  int Util::stringToInt(const std::string& inStr) {
    int  val;
    char junk[3];
    int nItem = sscanf(inStr.c_str(), "%d %1s", &val, junk);
    if (nItem != 1) {
      throw WrongType(inStr, "int");
    }
    return val;
  }

  void Util::stringTokenize(std::string input, const std::string& delimiters,
                            std::vector<std::string>& tokens, bool clear) {
    if (clear) tokens.clear();

    std::string::size_type j;
    while ( (j = input.find_first_of(delimiters)) != std::string::npos ) {
      if (j != 0) tokens.push_back(input.substr(0, j));
      input = input.substr(j+1);
    }
    tokens.push_back(input);
    if (tokens.back() == "") tokens.pop_back();
  }


  void Util::keyValueTokenize(std::string input, 
                              const std::string& delimiters,
                              std::map<std::string,std::string>& tokens, 
                              const std::string& pairDelimiter,
                              bool clear) {
    if (clear) tokens.clear();

    std::vector<std::string> strvec;
    stringTokenize(input,delimiters,strvec,true);
    unsigned advance = pairDelimiter.size();

    std::vector<std::string>::const_iterator input_itr = strvec.begin();
    while(input_itr!=strvec.end())
      {
	std::string current = *input_itr++;
	std::string::size_type j = current.find(pairDelimiter);
	std::string key   = current.substr(0, j);
	std::string value = current.substr(j+advance);
	tokens[key] = value;
      } 
  }
  
  std::string Util::basename(const std::string& path) {
     std::vector<std::string> names;
     stringTokenize(path, "\\/", names);
     return *(names.end() - 1);
  }

  unsigned Util::trimTrailing(std::string* toTrim) {
    static const char blank=' ';
    static const char LF=0xA;    // new line
    static const char FF=0xC;    // form feed
    static const char CR=0xD;    // carriage return

    unsigned orig = toTrim->size();
    unsigned last = orig - 1;
    bool notDone = true;
    unsigned nTrimmed = 0;


    while (notDone) {
      char lastChar = (*toTrim)[last];
      switch (lastChar) {
      case blank:
      case LF:
      case FF:
      case CR:
        last--;
        nTrimmed++;
        break;
      default:
        notDone=false;
        break;
      }
    }
    if (nTrimmed)  toTrim->resize(orig - nTrimmed);

    return nTrimmed;
  }

}






