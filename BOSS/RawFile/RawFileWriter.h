#ifndef BESIII_RAWFILE_WRITER_H
#define BESIII_RAWFILE_WRITER_H

#include "RawFile/raw_ofstream.h"

class RawFileWriter
{
   public :

      // all public interfaces should be thread safe
      RawFileWriter(const std::string& fname);
      virtual ~RawFileWriter();

      int writeEvent(const uint32_t* pevt);


   private :

      RawFileWriter();  //stop default


   private :

      raw_ofstream*  m_wfs;
};

#endif
