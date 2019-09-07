#ifndef MDC_CONVERTER_H
#define MDC_CONVERTER_H

#include "GaudiKernel/StatusCode.h"
#include "RawDataCnv/EventManagement/MdcBuilder.h"
#include "RawDataCnv/Util/BufferHolder.h"
#include "MdcRawEvent/MdcDigi.h"

class MdcConverter {

   public:

      // static interfaces
      static MdcConverter* instance(int runMode = 2);  //default OfflineMode
      static void destroy();

      // public interfaces
      void init(int runFrom, int runTo);
      void setRunId(int runId) { m_runId = runId; }
      StatusCode convert(const BufferHolder& src, MdcDigiCol* des);
      StatusCode convert(MdcDigiCol* src, WriteRawEvent*& des);


   private:

      // private methods
      MdcConverter(int runMode);  //get its intance via the static interface!
      ~MdcConverter();

      MdcConverter();  //stop default

      // private data members
      int m_runMode;  //reference to the RunMode of RawDataInputSvc

      int m_runId;
      int m_runFrom;
      int m_runTo;

      uint32_t  m_mdcTag[16384][4];

      MdcBuilder m_builder;

      // static data members
      static MdcConverter* s_instance;
};

#endif
