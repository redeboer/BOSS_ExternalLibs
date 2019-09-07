#ifndef BESIII_INTERFACE_RAWWRITER_H
#define BESIII_INTERFACE_RAWWRITER_H


class IRawWriter
{
   public :

      virtual ~IRawWriter() {}

      virtual int writeEvent(void* pevt, int size) = 0;

      virtual int stat() = 0;
};

#endif
