#include "ValidRecMdcTrack/ValidRecMdcTrackAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"
#include "EventModel/EventHeader.h"
//#include "ReconEvent/ReconEvent.h"


#include "MdcRecEvent/RecMdcTrack.h"
#include "Identifier/MdcID.h"

using namespace Event;


ValidRecMdcTrackAlg::ValidRecMdcTrackAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   Algorithm(name, pSvcLocator) {
   
    
    
   // m_tuple1 = 0;
   // m_tuple2 = 0;
    

}

StatusCode ValidRecMdcTrackAlg::initialize(){
   MsgStream log( msgSvc(), name() );
   log << MSG::INFO << "Booking Histograms" << endreq;


   NTuplePtr nt1(ntupleSvc(),"FILE101/mdcTrack");
   if( nt1 ) m_tuple1 = nt1;
   else{
         m_tuple1 = ntupleSvc()->book("FILE101/mdcTrack",CLID_RowWiseTuple,"ValidRecMdcTrackNTuple1");

         if( m_tuple1 ) {
             m_tuple1->addItem ("eventNo",g_eventNo);
             m_tuple1->addItem ("runNo", g_runNo);
	     m_tuple1->addItem("nhits",g_hits);
	     m_tuple1->addItem ("Id",g_track_id);
             m_tuple1->addItem ("Phi0",g_phi0);
             m_tuple1->addItem ("Cpa",g_kappa);
             m_tuple1->addItem ("Tanl",g_tanl);
             m_tuple1->addItem ("p",g_p);

             m_tuple1->addItem ("Dr",g_dr);
	     m_tuple1->addItem ("Dz",g_dz);
             m_tuple1->addItem ("Chisq",g_chisq_of_fit);
	     m_tuple1->addItem ("FiTerm",g_phi_terminal);
	     m_tuple1->addItem ("Nhits",g_number_of_hits);
       	     m_tuple1->addItem ("Nster",g_number_of_stereo_hits);
    
             m_tuple1->addItem ("px",g_px);
             m_tuple1->addItem ("py",g_py);
             m_tuple1->addItem ("pz",g_pz);
             m_tuple1->addItem ("pxy",g_pxy);
             m_tuple1->addItem ("charge",g_charge);             
             m_tuple1->addItem ("stat",g_stat);
             m_tuple1->addItem ("x",g_x);
             m_tuple1->addItem ("y",g_y);
             m_tuple1->addItem ("z",g_z);
             m_tuple1->addItem ("r",g_r);
             m_tuple1->addItem ("chi2",g_chi2);
             m_tuple1->addItem ("ndof",g_ndof);
             m_tuple1->addItem ("firstLayer",g_firstLayer);
             m_tuple1->addItem ("lastLayer",g_lastLayer);
             m_tuple1->addItem ("theta",g_theta);
             m_tuple1->addItem ("phi",g_phi);    

	 } else { 
		 log << MSG::ERROR << "----- Cannot book or register histograms -----" << endreq;
		 return StatusCode::FAILURE;
	 }
   }
   NTuplePtr nt2(ntupleSvc(),"FILE101/mdcHit");
   if( nt2 ) m_tuple2 = nt2;
   else{
         m_tuple2 = ntupleSvc()->book("FILE101/mdcHit",CLID_RowWiseTuple,"ValidRecMdcHitNTuple1");

         if( m_tuple2 ) {
             m_tuple2->addItem ("Hits_id",g_hits_id);
	     m_tuple2->addItem ("Hits_ddl",g_hits_ddl);
	     m_tuple2->addItem ("Hits_ddr",g_hits_ddr);

	     m_tuple2->addItem ("doca",g_doca);
	     m_tuple2->addItem ("Hits_mdc_id",g_hits_mdc_id);
	     m_tuple2->addItem ("layer", g_layer);
 	     m_tuple2->addItem ("wire",  g_wire );

	     m_tuple2->addItem ("Hits_tdc",g_hits_tdc);
	     m_tuple2->addItem ("Hits_adc",g_hits_adc);
	 } else { 
		 log << MSG::ERROR << "----- Cannot book or register histograms -----" << endreq;
		 return StatusCode::FAILURE;
	 }
   }
  
    log << MSG::INFO << "Finished booking Histograms" << endreq;
    return StatusCode::SUCCESS;

/*
  
   NTuplePtr nt2(ntupleSvc(),"FILE101/mdcHit");
   if( nt2 ) m_tuple2 = nt2;
   else{
         m_tuple2 = ntupleSvc()->book("FILE101/mdcHit",CLID_ColumnWiseTuple,"ValidRecMdcTrackNTuple2");

         if( m_tuple2 ) {
             m_tuple1->addItem ("eventNo",g_eventNo);
             m_tuple1->addItem ("runNo", g_runNo);
             m_tuple1->addItem("nhits",g_hits,0,5000);
             m_tuple1->addItem ("Id",g_track_id);
             m_tuple1->addItem ("Phi0",g_phi0);
             m_tuple1->addItem ("Cpa",g_kappa);
             m_tuple1->addItem ("Tanl",g_tanl);
             m_tuple1->addItem ("p",g_p);

             m_tuple1->addItem ("Dr",g_dr);
             m_tuple1->addItem ("Dz",g_dz);
             m_tuple1->addItem ("Chisq",g_chisq_of_fit);
             m_tuple1->addItem ("FiTerm",g_phi_terminal);
             m_tuple1->addItem ("Nhits",g_number_of_hits);
             m_tuple1->addItem ("Nster",g_number_of_stereo_hits);

             m_tuple2->addItem ("Hits_id",g_hits_id);
             m_tuple2->addItem ("Hits_ddl",g_hits_ddl);
             m_tuple2->addItem ("Hits_ddr",g_hits_ddr);

             m_tuple2->addItem ("doca",g_doca);
             m_tuple2->addItem ("Hits_mdc_id",g_hits_mdc_id);
             m_tuple2->addItem ("layer",g_layer);
             m_tuple2->addItem ("wire",g_wire );

             m_tuple2->addItem ("Hits_tdc",g_hits_tdc);
             m_tuple2->addItem ("Hits_adc",g_hits_adc);
     }
     else { 
    log << MSG::ERROR << "----- Cannot book or register histograms -----" << endreq;
    return StatusCode::FAILURE;
      }

    log << MSG::INFO << "Finished booking Histograms" << endreq;
    return StatusCode::SUCCESS;

  }*/

 }

StatusCode ValidRecMdcTrackAlg::execute() {
   MsgStream log(msgSvc(), name());


   int ntrk=0;
  
    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
    if (!eventHeader) {
      log << MSG::FATAL << "Could not find Event Header" << endreq;
     return( StatusCode::SUCCESS);
      }

    log << MSG::INFO << "retrieved event: " << eventHeader->eventNumber()  << "  run: " <<  eventHeader->runNumber() << endreq;
    int eventNo=eventHeader->eventNumber();
    int runNo=eventHeader->runNumber();
    if(m_tuple1){
      g_eventNo = eventNo; 
      g_runNo = runNo; 
    }

   SmartDataPtr<RecMdcTrackCol> mdcTracks(eventSvc(),"/Event/Recon/RecMdcTrackCol");
   if( ! mdcTracks )
      {
         log << MSG::ERROR << "Unable to retrieve RecMdcTrackCol" << endreq;
         return  StatusCode::FAILURE;
      } else {
         log << MSG::DEBUG << "RecMdcTrackCol retrieved of size "<< mdcTracks->size() << endreq;
         for(RecMdcTrackCol::iterator it=mdcTracks->begin(); it!=mdcTracks->end(); it++)
            {  

/*               g_hits=0;

               HitRefVec gothits = (*it)->getVecHits();
               HitRefVec::iterator it_gothit = gothits.begin();
               for( ; it_gothit != gothits.end(); it_gothit++){
                 g_hits_id  = (*it_gothit)->getId();
                 g_hits_ddl = (*it_gothit)->getDriftDistLeft();
                 g_hits_ddr = (*it_gothit)->getDriftDistRight();
                 g_doca        = (*it_gothit)->getDoca();
                 g_hits_mdc_id = (*it_gothit)->getMdcId();
                 g_layer       = MdcID::layer((*it_gothit)->getMdcId());
                 g_wire        = MdcID::wire((*it_gothit)->getMdcId());



                 g_hits_tdc    = (*it_gothit)->getTdc();
                 g_hits_adc    = (*it_gothit)->getAdc();

                 g_hits++;
                 m_tuple2->write();
           }*/

               g_track_id       = (*it)->trackId();
               g_phi0           = (*it)->helix()[1];
               g_kappa          = (*it)->helix()[2];
               g_tanl           = (*it)->helix()[4];
    
               g_dr             = (*it)->helix()[0];
               g_dz             = (*it)->helix()[3];
               g_chisq_of_fit   = (*it)->chi2();
               g_phi_terminal   = (*it)->getFiTerm();
               g_number_of_hits = (*it)->getNhits();
               g_number_of_stereo_hits=(*it)->nster();
               g_p              = (*it)->p();

               g_px             = (*it)->px();
               g_py             = (*it)->py();
               g_pz             = (*it)->pz();
               g_pxy            = (*it)->pxy();
               g_charge         = (*it)->charge();
               g_stat           = (*it)->stat();
               g_x              = (*it)->x();
               g_y              = (*it)->y();
               g_z              = (*it)->z();
               g_r              = (*it)->r();
               g_chi2           = (*it)->chi2();
               g_ndof           = (*it)->ndof();
               g_firstLayer     = (*it)->firstLayer();
               g_lastLayer      = (*it)->lastLayer();             
               
               
               g_hits=0;
               HitRefVec gothits = (*it)->getVecHits();
               HitRefVec::iterator it_gothit = gothits.begin();
               for( ; it_gothit != gothits.end(); it_gothit++){
                 g_hits_id  = (*it_gothit)->getId();
		 
	         g_hits_ddl = (*it_gothit)->getDriftDistLeft();
	         g_hits_ddr = (*it_gothit)->getDriftDistRight();

	         g_doca        = (*it_gothit)->getDoca();
	         g_hits_mdc_id = (*it_gothit)->getMdcId();
                 g_layer       = MdcID::layer((*it_gothit)->getMdcId());
                 g_wire        = MdcID::wire((*it_gothit)->getMdcId());
               
               
              

                 g_hits_tdc    = (*it_gothit)->getTdc();
	         g_hits_adc    = (*it_gothit)->getAdc();

                 g_hits++;
		 m_tuple2->write(); 
           } 

           m_tuple1->write(); 
              

            
             
              

             

          
       
            
      }
     
   }
   return StatusCode::SUCCESS;
}


StatusCode ValidRecMdcTrackAlg::finalize() {

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "Finalizing..." << endreq;
   return StatusCode::SUCCESS;

}



