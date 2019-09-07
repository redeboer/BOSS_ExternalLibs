#ifndef TOF_CONVERTER_H
#define TOF_CONVERTER_H

#include "GaudiKernel/StatusCode.h"
#include "RawDataCnv/EventManagement/TofBuilder.h"
#include "RawDataCnv/Util/BufferHolder.h"
#include "TofRawEvent/TofDigi.h"
#include "LumiDigi/LumiDigi.h"

class TofConverter {

   public:

      // static interfaces
      static TofConverter* instance();
      static void destroy();

      // public interfaces
      StatusCode convert(const BufferHolder& src, TofDigiCol* des, LumiDigiCol* des2 = 0);

      StatusCode convert(const BufferHolder& tof,
                         const BufferHolder& etf,
                         TofDigiCol* des,
                         LumiDigiCol* des2 = 0);

      StatusCode convert(TofDigiCol* src, WriteRawEvent*& des);


   private:

      // private methods
      TofConverter();  //get its instance via the static interface!
      ~TofConverter();

      void unPackETF(uint32_t reDigi, uint32_t& TEId, uint32_t& TEData,
                     uint32_t& TEOverflow, uint32_t& TETorQ);

      // private data members
      TofBuilder m_builder;

      // static data methods
      static TofConverter* s_instance;
};

#endif
