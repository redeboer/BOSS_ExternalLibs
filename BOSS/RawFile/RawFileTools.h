#ifndef BESIII_RAW_FILE_TOOLS_H
#define BESIII_RAW_FILE_TOOLS_H

#include <vector>
#include <string>

namespace RawFileTools
{
   std::vector<std::string> wildcard_correct(const std::string& fname);
   std::vector<std::string> wildcard_correct(const std::vector<std::string>& fnames);

   std::string fname2idxname(const std::string& fname);

   std::string itoa(int i);
}

#endif
