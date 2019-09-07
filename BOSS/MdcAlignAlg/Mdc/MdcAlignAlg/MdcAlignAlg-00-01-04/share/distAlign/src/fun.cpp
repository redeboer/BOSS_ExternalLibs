#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"

#include "include/fun.h"

using namespace std;

vector<string> getHistList()
{
   vector<string> fnames;

   string command(
	 "JobOutputDir=`/bin/ls -dt1 joboutput-* 2>/dev/null | head -1`\n"
	 "if [ -d \"${JobOutputDir}\" ]; then\n"
	 "  find ${JobOutputDir} -name hist.root\n"
	 "fi\n"
	 );

   stringstream fnstream;

   char* fnbuf   = new char[1024];
   FILE* fstream = popen(command.c_str(), "r");

   while ( fgets(fnbuf, 1024, fstream) != NULL ) {
      fnstream << fnbuf;
   }

   string fname;
   while ( ! (fnstream>>fname).eof() ) {
      fnames.push_back(fname);
   }

   pclose(fstream);
   delete [] fnbuf;

   if ( fnames.empty() ) {
      cout << "WARNING: Failed to retrieve hist files in the current directory!" << endl;
//       exit(1);
   }
   return fnames;
}

vector<string> getHistList(string path)
{
   vector<string> fnames;
   string newpath = path;
   string::size_type strl = newpath.length();
   if((strl>1) && ('/'==newpath[strl-1])) newpath.erase(strl-1);

   char com1[500];
   sprintf(com1, "JobOutputDir=`/bin/ls -dt1 %s/joboutput-* 2>/dev/null | head -1`\n", newpath.c_str());
   string command1(com1);
   string command2(
	 "if [ -d \"${JobOutputDir}\" ]; then\n"
	 "  find ${JobOutputDir} -name hist.root\n"
	 "fi\n"
	 );
   string command = command1 + command2;
   stringstream fnstream;

   char* fnbuf   = new char[1024];
   FILE* fstream = popen(command.c_str(), "r");

   while ( fgets(fnbuf, 1024, fstream) != NULL ) {
      fnstream << fnbuf;
   }

   string fname;
   while ( ! (fnstream>>fname).eof() ) {
      fnames.push_back(fname);
   }

   pclose(fstream);
   delete [] fnbuf;

   if ( fnames.empty() ) {
      cout << "ERROR: Failed to retrieve hist files!" << endl;
      exit(1);
   }
   return fnames;
}
