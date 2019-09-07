#include "ValidRecMucTrack/ValidRecMucTrackAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/IHistogramSvc.h"

#include "MucRecEvent/RecMucTrack.h"


ValidRecMucTrackAlg::ValidRecMucTrackAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   Algorithm(name, pSvcLocator) {
  
    m_tuple1 = 0;  
}

StatusCode ValidRecMucTrackAlg::initialize(){

   StatusCode status;
   MsgStream log( msgSvc(), name() );
   log << MSG::INFO << "Booking Histograms" << endreq;

   //  Book N-tuple 1
   NTuplePtr nt1(ntupleSvc(), "FILE105/T");
   if ( nt1 ) m_tuple1 = nt1;
   else {
     m_tuple1 = ntupleSvc()->book ("FILE105/T", CLID_RowWiseTuple, "MucTrack parameter");
     if ( m_tuple1 )    {
       status = m_tuple1->addItem("depth",m_depth);
       status = m_tuple1->addItem("distance",m_distance);
       status = m_tuple1->addItem("deltaPhi",m_deltaPhi);
       status = m_tuple1->addItem("maxHits",m_maxHits);
       status = m_tuple1->addItem("numLayers",m_numLayers);
       status = m_tuple1->addItem("numHits",m_numHits);
       status = m_tuple1->addItem("chi2",m_chi2);
       status = m_tuple1->addItem("xPos",m_xPos);
       status = m_tuple1->addItem("yPos",m_yPos);
       status = m_tuple1->addItem("zPos",m_zPos);
     }
     else    {   // did not manage to book the N tuple....
       log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
       //return StatusCode::FAILURE;
     }
   }
   log << MSG::INFO << "Finished booking NTuples" << endmsg;


  return StatusCode::SUCCESS;


}


StatusCode ValidRecMucTrackAlg::execute() {
   MsgStream log(msgSvc(), name());
   SmartDataPtr<RecMucTrackCol> mucTracks(eventSvc(),"/Event/Recon/RecMucTrackCol");
   if( ! mucTracks )
      {
         log << MSG::ERROR << "Unable to retrieve RecMucTrackCol" << endreq;
         return  StatusCode::FAILURE;
      } else {
         log << MSG::DEBUG << "RecMucTrackCol retrieved of size "<< mucTracks->size() << endreq;
         for(RecMucTrackCol::iterator it=mucTracks->begin(); it!=mucTracks->end(); it++)
            {

               m_depth    = (*it)->depth();
               m_distance = (*it)->distance();
               m_deltaPhi = (*it)->deltaPhi();
               m_maxHits  = (*it)->maxHitsInLayer();
               m_numLayers= (*it)->numLayers();
               m_numHits  = (*it)->numHits();
               m_chi2     = (*it)->chi2();
               m_xPos     = (*it)->xPos();
               m_yPos     = (*it)->yPos();
               m_zPos     = (*it)->zPos();

               StatusCode status1 = m_tuple1->write();
               if ( status1.isFailure() )  {
                 log << MSG::ERROR << "Cannot fill Ntuple1" << endreq;
               }
            }
      }
     return StatusCode::SUCCESS;
}


StatusCode ValidRecMucTrackAlg::finalize() {

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "Finalizing..." << endreq;

}



