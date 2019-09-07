//====================================================================
//      RawDataHltRawCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/EventModel.h"
#include "Identifier/HltID.h"
#include "HltEvent/HltRaw.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataAddress.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataHltRawCnv.h"


extern const CLID& CLID_HltRawCol;

// Constructor.
RawDataHltRawCnv::RawDataHltRawCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
    //init();
}
RawDataHltRawCnv::~RawDataHltRawCnv(){
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataHltRawCnv::classID()
{
   return CLID_HltRawCol;
}

StatusCode RawDataHltRawCnv::initialize()
{
    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataHltRawCnv_Name("RawDataHltRawCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());  
    if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataHltRawCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    
   MsgStream log(messageService(), RawDataHltRawCnv_Name.c_str());
   StatusCode sc = RawDataBaseCnv::initialize();
   if (StatusCode::SUCCESS != sc) {
     
     return sc;
   }

   // Check RawDataCnvSvc
   IService* isvc = 0;
   StatusCode status = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), isvc, true);

   m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (isvc); 
    if(m_RawDataAccess  == 0 ) {
      log<<MSG::ERROR<< "  RawDataCnv: Cant cast to  RawDataCnvSvc " <<endreq; 
      return StatusCode::FAILURE ;
   }
   
   if(m_RawDataAccess)
        log << MSG::INFO << "RawDataCnvSvc retrieved "<< endreq;

    IService* svc ;
    sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
    if(sc != StatusCode::SUCCESS ) {
        log<<MSG::WARNING << " Cant get RawDataInputSvc " <<endreq;
        return sc ;
    }
 
    m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
    if(m_inputSvc == 0 ) {
        log<<MSG::WARNING<< " Cant cast to  RawDataInputSvc " <<endreq;
        return StatusCode::FAILURE ;
    }
 
   return StatusCode::SUCCESS; 
}


StatusCode RawDataHltRawCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    //MsgStream log(msgSvc(), "RawDataHltRawCnv");
    //log << MSG::DEBUG << "RawDataHltRawCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataHltRawCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
   //MsgStream log(msgSvc(), "RawDataHltRawCnv");
   RawDataAddress *pEFAddr;
   uint32_t TEData[2];
   
   // Purpose and Method:  This converter will create an empty HltRawCol on
   //   the TDS.
   HltRawCol *digiCol = new HltRawCol;
   pObj = digiCol;

   RAWEVENT *evt = m_inputSvc->currentEvent(); 
   if (evt == NULL) {
     cout << "RawDataHltRawCnv::createObj can't get event!" << endl; 
     return StatusCode::FAILURE;
   }

   int digiId = 0;
   const BufferHolder& hltBuf = evt->getHltBuf();
   uint32_t nbuf = hltBuf.nBuf();
   
   uint32_t REId = 0, TEId = 0;
   uint32_t nmdc=0,ntof=0,nemc=0,nmuc=0;
   for (uint32_t i = 0; i < nbuf; i++) {
     uint32_t* buf = hltBuf(i);
     uint32_t bufSize = hltBuf.bufSize(i);
     for (uint32_t j = 0; j < bufSize; j++, digiId++) {

	m_hltBuilder.unPack(buf[j], digiId, REId, TEData[0], TEData[1]);
	TEId = m_hltBuilder.getTEID(REId);
	if ( TEId == 0xFFFFFFFF ) continue;

//       log << MSG::NIL << "HLT digit No: " << digiId << "="<<std::hex<<buf[j]
//	   <<"===>"<<TEId<<" : "<<TEData[0]<<","<<TEData[1]<<std::dec<<endreq;
       for (uint32_t k=0; k< 2;k++){
	 // Create new track
	 if(k==0||(k==1&&TEData[k])){
	   if(k==1) TEId+=1;
	   if(HltID::is_number(HltID::convert(TEId))) {
	     nmdc=TEData[k]&0xFF;
	     ntof=(TEData[k]>>8)&0xFF;
	     nemc=(TEData[k]>>16)&0xFF;
	     nmuc=(TEData[k]>>24)&0xFF;
	     //log << MSG::INFO << "number=" << std::hex <<TEData[k] <<std::dec<< endreq;
	   }
	   HltRaw* hltRaw = new HltRaw(HltID::convert(TEId,nmdc,ntof,nemc,nmuc));
	   hltRaw->setIntChannel(TEData[k]);
	   //hltRaw->setFloatChannel(TEData[k]);
	   hltRaw->setChargeChannel(TEData[k]);

	   digiCol->push_back ( hltRaw );
	 }
       }
     }
   }
   return StatusCode::SUCCESS; 
}

StatusCode RawDataHltRawCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
    // convert PixelRaw in the container into ByteStream
    //MsgStream log(messageService(), "RawDataHltRawCnv");

    WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
    if(re == 0) {
      //log << " get RawEvent failed !" << endreq;
      return StatusCode::FAILURE; 
    }

    SmartDataPtr<HltRawCol> digiCol(dataProvider(), EventModel::Hlt::HltRawCol);
    if (digiCol == 0) {
        //log << "no HltRawCol found" << endreq;
        return StatusCode::FAILURE;
    }
    /*
    else {
        log << MSG::INFO << endreq << "Detailed dump of 1st event: " << endreq << endreq;
        int ndigi = 0;
        HltRawCol::const_iterator pHltRaw = digiCol->begin();
        for (pHltRaw; pHltRaw!= digiCol->end(); pHltRaw++) {
            log <<MSG::INFO << "Raw " << ndigi++ << " ";
            // output the digi proper:  " << digi " no longer works because
            // "digi" now returns an integer sort order
            (**pHltRaw).fillStream(log.stream());
            log << endreq;
        }
    }
    */
    return m_hltBuilder.pack(digiCol, re);
}


