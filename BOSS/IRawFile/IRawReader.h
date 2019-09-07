#ifndef BESIII_INTERFACE_RAWREADER_H
#define BESIII_INTERFACE_RAWREADER_H

#include <stdint.h>
#include <string>

class IRawReader
{
   public :

      virtual ~IRawReader() {}

      virtual const uint32_t*  nextEvent() = 0;

      virtual const uint32_t*  currentEvent() const = 0;

      virtual uint32_t         runNo() = 0;

      virtual std::string      currentFile() = 0;

      virtual uint32_t         stat() = 0;
};

#endif
