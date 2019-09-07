#ifndef MUC_CONVERTER_H
#define MUC_CONVERTER_H

#include "GaudiKernel/StatusCode.h"
#include "RawDataCnv/EventManagement/MucBuilder.h"
#include "RawDataCnv/Util/BufferHolder.h"
#include "MucRawEvent/MucDigi.h"

class MucConverter {

   public:

      // static interfaces
      static MucConverter* instance();
      static void destroy();

      // public interfaces
      StatusCode convert(const BufferHolder& src, MucDigiCol* des);
      StatusCode convert(MucDigiCol* src, WriteRawEvent*& des);


   private:

      // private methods
      MucConverter();  //get its intance via the static interface!
      ~MucConverter();

      // private data members
      MucBuilder m_builder;

      // static data members
      static MucConverter* s_instance;
};

#endif
