#include "RawDataCnv/Util/MucConverter.h"

MucConverter* MucConverter::s_instance = 0;

MucConverter* MucConverter::instance()
{
   if ( s_instance == 0 ) {
      s_instance = new MucConverter();
   }

   return s_instance;
}

void MucConverter::destroy()
{
   if ( s_instance != 0 ) {
      delete s_instance;
      s_instance = 0;
   }
}

StatusCode MucConverter::convert(const BufferHolder& src, MucDigiCol* des)
{
   uint32_t REId, TEId, FecData, StrSqc;
   uint32_t nbuf = src.nBuf();

   for (uint32_t i = 0; i < nbuf; ++i) {
      uint32_t* buf = src(i);
      uint32_t  bufSize = src.bufSize(i);
      for (uint32_t j = 0; j < bufSize; ++j) {
	 m_builder.unPack(buf[j], REId, FecData, StrSqc);
	 TEId = m_builder.getTEID( REId );
	 if ( TEId == 0xFFFFFFFF )  continue;

	 uint32_t teidBase = TEId & 0xFF0FFFFF;
	 uint32_t teid;
	 for (uint32_t k = 0; FecData != 0 && k < 16; FecData >>= 1, ++k) {
	    if ((FecData & 1) == 0) continue;
	    if (StrSqc == 0) {
	       teid = teidBase + 15 -k;
	    }
	    else {
	       teid = teidBase + k;
	    }
	    MucDigi* mucDigi = new MucDigi(teid);
	    des->push_back( mucDigi );
	 }
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode MucConverter::convert(MucDigiCol* src, WriteRawEvent*& des)
{
   return m_builder.pack(src, des);
}

MucConverter::MucConverter()
{
}

MucConverter::~MucConverter()
{
}
