#include <iostream>
#include <sstream>
#include "RawFile/RawFileTools.h"
#include <stdio.h>

std::vector<std::string> RawFileTools::wildcard_correct(const std::string& fname)
{
   static char pbuf[8192];
   static const std::string lsCom("/bin/ls ");

   std::vector<std::string> newfnames;

   if ( ( fname.find('*', 0) != std::string::npos ) ||
	 ( fname.find('?', 0) != std::string::npos ) )
   {
      FILE* ftmp = popen((lsCom+fname).c_str(), "r");

      std::stringstream  fnstream;
      while ( fgets(pbuf, 8192, ftmp) != NULL ) {
	 fnstream << pbuf;
      }

      std::string tfn;
      while ( ! (fnstream>>tfn).eof() ) {
	 newfnames.push_back(tfn);
      }
   }
   else {
      newfnames.push_back(fname);
   }

   return newfnames;
}

std::vector<std::string> RawFileTools::wildcard_correct(const std::vector<std::string>& fnames)
{
   static char pbuf[8192];
   static const std::string lsCom("/bin/ls ");

   std::vector<std::string> newfnames;
   std::string fname;
   std::stringstream  fnstream;
   std::vector<std::string>::const_iterator it = fnames.begin();

   while ( it != fnames.end() ) {
      if ( ( it->find('*', 0) != std::string::npos ) ||
	   ( it->find('?', 0) != std::string::npos ) ) {
	 // list and get the wildcard files
	 std::string com = lsCom + (*it);
	 FILE* ftmp = popen(com.c_str(), "r");

	 fnstream.clear();
	 while ( fgets(pbuf, 8192, ftmp) != NULL ) {
	    fnstream << pbuf;
	 }

	 while ( ! (fnstream>>fname).eof() ) {
	    newfnames.push_back(fname);
	 }
      }
      else {
	 newfnames.push_back(*it);
      }
      ++it;
   }

   return newfnames;
}

std::string RawFileTools::fname2idxname(const std::string& fname)
{
   std::string::size_type pathend = fname.rfind('/', fname.length());
   std::string idxname = (pathend != std::string::npos ) ? fname.substr(pathend+1) : fname;

   idxname += ".idx";

   return idxname;
}

std::string RawFileTools::itoa(int i)
{
   std::stringstream  sstr;
   sstr << i;

   std::string str;
   sstr >> str;

   while ( str.length() < 3 ) {
      str = std::string("0") + str;
   }

   return str;
}
