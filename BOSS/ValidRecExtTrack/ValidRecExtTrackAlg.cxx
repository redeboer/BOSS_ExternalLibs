#include "ValidRecExtTrack/ValidRecExtTrackAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/IHistogramSvc.h"

#include "ExtEvent/RecExtTrack.h"


ValidRecExtTrackAlg::ValidRecExtTrackAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   Algorithm(name, pSvcLocator) {
  
    m_tuple1 = 0;  
}

StatusCode ValidRecExtTrackAlg::initialize(){

   StatusCode status;
   MsgStream log( msgSvc(), name() );
   log << MSG::INFO << "Booking Histograms" << endreq;

   //  Book N-tuple 1
   NTuplePtr nt1(ntupleSvc(), "FILE208/ExtTrk");
   if ( nt1 ) m_tuple1 = nt1;
   else {
     m_tuple1 = ntupleSvc()->book ("FILE208/ExtTrk", CLID_RowWiseTuple, "ExtTrack parameter");
     if ( m_tuple1 )    {
       status = m_tuple1->addItem("tof1",m_tof1);
       status = m_tuple1->addItem("tof1Path",m_tof1Path);
       status = m_tuple1->addItem("tof1PosSigmaAlongX",m_tof1PosSigmaAlongX);
       status = m_tuple1->addItem("xTof1Pos",m_xTof1Pos);
       status = m_tuple1->addItem("yTof1Pos",m_yTof1Pos);
       status = m_tuple1->addItem("zTof1Pos",m_zTof1Pos);
       status = m_tuple1->addItem("emTof1_11",m_emTof1_11);
       status = m_tuple1->addItem("emTof1_33",m_emTof1_33);
       status = m_tuple1->addItem("emTof1_55",m_emTof1_55);
       status = m_tuple1->addItem("emTof1_66",m_emTof1_66);
       status = m_tuple1->addItem("emTof1_23",m_emTof1_23);
       status = m_tuple1->addItem("emTof1_35",m_emTof1_35);
     }
     else    {   // did not manage to book the N tuple....
       log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
       //return StatusCode::FAILURE;
     }
   }
   log << MSG::INFO << "Finished booking NTuples" << endmsg;


  return StatusCode::SUCCESS;


}


StatusCode ValidRecExtTrackAlg::execute() {
   MsgStream log(msgSvc(), name());
   SmartDataPtr<RecExtTrackCol> extTracks(eventSvc(),"/Event/Recon/RecExtTrackCol");
   if( ! extTracks )
      {
         log << MSG::ERROR << "Unable to retrieve RecExtTrackCol" << endreq;
         return  StatusCode::FAILURE;
      } else {
         log << MSG::DEBUG << "RecExtTrackCol retrieved of size "<< extTracks->size() << endreq;
         for(RecExtTrackCol::iterator it=extTracks->begin(); it!=extTracks->end(); it++)
            {
               
	       m_tof1=(*it)->tof1();
	       m_tof1Path=(*it)->tof1Path();
	       m_tof1PosSigmaAlongX=(*it)->tof1PosSigmaAlongX();
	       m_xTof1Pos = (*it)->tof1Position().x();
	       m_yTof1Pos = (*it)->tof1Position().y();
	       m_zTof1Pos = (*it)->tof1Position().z();
	       m_emTof1_11 = (*it)->tof1ErrorMatrix().fast(1,1);
	       m_emTof1_33 = (*it)->tof1ErrorMatrix().fast(3,3);
	       m_emTof1_55 = (*it)->tof1ErrorMatrix().fast(5,5);
	       m_emTof1_66 = (*it)->tof1ErrorMatrix().fast(6,6);
	       m_emTof1_23 = (*it)->tof1ErrorMatrix().fast(2,3);
	       m_emTof1_35 = (*it)->tof1ErrorMatrix().fast(3,5);
                
               StatusCode status1 = m_tuple1->write();
               if ( status1.isFailure() )  {
                 log << MSG::ERROR << "Cannot fill Ntuple1" << endreq;
               }
            }
      }
     return StatusCode::SUCCESS;
}


StatusCode ValidRecExtTrackAlg::finalize() {

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "Finalizing..." << endreq;

}



