#include "RawFile/RawFileWriter.h"

RawFileWriter::RawFileWriter(const std::string& fname)
{
   m_wfs = raw_ofstream::instance(fname);
}

RawFileWriter::~RawFileWriter()
{
   raw_ofstream::release();
}

int RawFileWriter::writeEvent(const uint32_t* pevt)
{
   const char* pbuf = reinterpret_cast<const char*>(pevt);
   int  sizeBytes = pevt[1] * 4;  //unit of size is word

   raw_ofstream::lock();
   int nfile = m_wfs->write_event(pbuf, sizeBytes);
   raw_ofstream::unlock();

   return nfile;
}
