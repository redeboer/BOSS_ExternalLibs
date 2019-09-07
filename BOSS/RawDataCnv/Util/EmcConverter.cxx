#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataCnv/Util/EmcConverter.h"

EmcConverter* EmcConverter::s_instance = 0;

EmcConverter* EmcConverter::instance(int runMode)
{
   if ( s_instance == 0 ) {
      s_instance = new EmcConverter(runMode);
   }

   return s_instance;
}

void EmcConverter::destroy()
{
   if ( s_instance != 0 ) {
      delete s_instance;
      s_instance = 0;
   }
}

StatusCode EmcConverter::convert(const BufferHolder& src, EmcDigiCol* des)
{
   uint32_t REId, TEId, TETDC, TEADC, measure;
   uint32_t nbuf = src.nBuf();

   for (uint32_t i = 0; i < nbuf; ++i) {  //loop the buffer
      uint32_t* buf = src(i);
      uint32_t* bufend = buf + src.bufSize(i);
      for (; buf < bufend; ++buf) {  //loop the digit

	 m_builder.unPack(*buf, REId, TETDC, TEADC, measure);

	 TEId = m_builder.getTEID(REId);
	 if ( TEId == 0xFFFFFFFF ) continue;

	 if ( m_EmcProvider ) {
	    // by fucd: if not good digi, reject. Only for online mode now
	    if(!m_EmcProvider->isGoodEmcDigi(EmcRawDataProvider::DropLow|EmcRawDataProvider::CutTime
		     |EmcRawDataProvider::DropHot,TEId,measure,TEADC,TETDC)) continue;
	    Identifier id(TEId);
	    m_EmcProvider->doCalib(id, measure, TEADC);
	 }

	 // create and set the DataObject
	 EmcDigi* emcDigi = new EmcDigi(Identifier(TEId), TETDC, TEADC);
	 emcDigi->setMeasure(measure);
	 des->push_back(emcDigi);
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode EmcConverter::convert(EmcDigiCol* src, WriteRawEvent*& des)
{
   return m_builder.pack(src, des);
}

EmcConverter::EmcConverter(int runMode)
   : m_runMode(runMode),
     m_EmcProvider(0)
{
   if ( m_runMode == 1 ) {
      // by fucd: only when online mode, use raw data provider here 
      IService* svc ;
      StatusCode sc = Gaudi::svcLocator()->getService("RawDataProviderSvc", svc);
      //if ( sc.isFailure() ) {
      //   log << MSG::ERROR << "Cannot get Raw Data Provider Service!" << endreq;
      //}
      IRawDataProviderSvc* rawSvc = dynamic_cast<IRawDataProviderSvc*> (svc);
      if ( rawSvc != 0 ) {
	 m_EmcProvider = rawSvc->getEmcProvider();
      }
      else {
	 std::cerr << "Failed to get the RawDataProviderSvc" << std::endl;
      }
   }
}

EmcConverter::~EmcConverter()
{
}
