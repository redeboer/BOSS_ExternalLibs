// $Header: /bes/bes/BossCvs/Event/RootCnvSvc/src/Util.cxx,v 1.10 2011/02/17 01:20:06 maqm Exp $

#include "RootCnvSvc/Util.h"

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
        
    //int envStart = toExpand->find_first_of(openDel.c_str());
    int envStart = toExpand->find(openDel.c_str());  //by fucd
    while (envStart != -1) {
      //int envEnd = toExpand->find_first_of(closeDel.c_str());
      int envEnd = toExpand->find(closeDel.c_str(),envStart);  //by fucd
            
      // add  characters to account for opening delimiter
      int afterBracket = envStart + opLen;
            
      if (!( (envStart==-1) || (envEnd==-1) ))
      {
        std::string envVariable = 
          toExpand->substr(afterBracket,(envEnd-afterBracket));
        const char * path = ::getenv(envVariable.c_str());
        if (path) {
	  //toExpand->replace(envStart,(envEnd+clLen), path);
          toExpand->replace(envStart,(envEnd+clLen-envStart), path); //by fucd
          if (nSuccess > -1) nSuccess++;
        }
        else {
          std::cerr << "Util::expandEnvVar unable to translate " 
                    << envVariable << std::endl;
          throw Untranslatable(envVariable);
        }
      }
      //envStart = toExpand->find_first_of(openDel.c_str());
      envStart = toExpand->find(openDel.c_str());  //by fucd
    }
    return nSuccess;
  }

  int Util::catchOptionVal(std::string* toCatch,
			   const int pos,
			   const std::string& openDel,
			   const std::string& closeDel) {
    unsigned opLen = openDel.size();
    unsigned clLen = closeDel.size();
 
    int valStart = toCatch->find(openDel.c_str(),pos);
    while (valStart != -1) {
      int valEnd = toCatch->find(closeDel.c_str(),valStart);
 
      // add  characters to account for opening delimiter                                                        
      int afterBracket = valStart + opLen;
 
      if (valEnd!=-1){
	std::string valStr = toCatch->substr(afterBracket,(valEnd-afterBracket));
	toCatch->erase(valStart,(valEnd+clLen-valStart));
	return atoi(valStr);
      }
      else{
	std::cerr << "Util::can't find the close delimiter "
		  << closeDel << std::endl;
	throw Untranslatable(*toCatch);
      }
    }
    return -1;
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
  }

  std::string Util::basename(const std::string& path) {
     std::vector<std::string> names;
     stringTokenize(path, "\\/", names);
     return *(names.end() - 1);
  }
  
}






