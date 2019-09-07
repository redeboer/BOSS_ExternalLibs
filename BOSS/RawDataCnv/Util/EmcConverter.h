#ifndef EMC_CONVERTER_H
#define EMC_CONVERTER_H

#include "GaudiKernel/StatusCode.h"
#include "RawDataCnv/EventManagement/EmcBuilder.h"
#include "RawDataCnv/Util/BufferHolder.h"
#include "EmcRawEvent/EmcDigi.h"

class EmcRawDataProvider;

class EmcConverter {

   public:

      // static interfaces
      static EmcConverter* instance(int runMode = 2);  //default OfflineMode
      static void destroy();

      // public interfaces
      StatusCode convert(const BufferHolder& src, EmcDigiCol* des);
      StatusCode convert(EmcDigiCol* src, WriteRawEvent*& des);


   private:

      // private methods
      EmcConverter(int runMode);  //get its intance via the static interface!
      ~EmcConverter();

      EmcConverter();  //stop default

      // private data members
      int m_runMode;  //reference to the RunMode of RawDataInputSvc

      /// pointer of EmcRawDataProvider, only OnlineMode  //fucd
      EmcRawDataProvider* m_EmcProvider;

      EmcBuilder m_builder;

      // static data members
      static EmcConverter* s_instance;
};

#endif
