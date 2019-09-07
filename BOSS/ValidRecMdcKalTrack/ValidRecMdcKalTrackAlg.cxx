#include "ValidRecMdcKalTrack/ValidRecMdcKalTrackAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "Identifier/MdcID.h"
using namespace std;


ValidRecMdcKalTrackAlg::ValidRecMdcKalTrackAlg(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator),_ntuple(1),m_nt1(0) {

  declareProperty("ntuple",_ntuple=1);
  
}


void ValidRecMdcKalTrackAlg::histDef(void){
  if(_ntuple) {  
    NTuplePtr  nt1(ntupleSvc(),"FILE_KalValid/n101");
    StatusCode status;
    if ( nt1 ) m_nt1 = nt1;                                                    
    else {                                                                     
      m_nt1= ntupleSvc()->book("FILE_KalValid/n101",CLID_ColumnWiseTuple,"KalFitValid");
      if ( m_nt1 )  {
	status = m_nt1->addItem("stat",5,2,m_stat);
        status = m_nt1->addItem("ndf",5,2,m_ndf);
	status = m_nt1->addItem("chisq",5,2,m_chisq);
	status = m_nt1->addItem("length",5,m_length);
	status = m_nt1->addItem("tof",5,m_tof);
	status = m_nt1->addItem("nhits",5,m_nhits);
	status = m_nt1->addItem("zhelix",5,m_zhelix);
	status = m_nt1->addItem("zhelixe",5,m_zhelixe);
	status = m_nt1->addItem("zhelixmu",5,m_zhelixmu);
	status = m_nt1->addItem("zhelixk",5,m_zhelixk);
	status = m_nt1->addItem("zhelixp",5,m_zhelixp);
	status = m_nt1->addItem("zptot",m_zptot);
	status = m_nt1->addItem("zptote",m_zptote);
	status = m_nt1->addItem("zptotmu",m_zptotmu);
	status = m_nt1->addItem("zptotk",m_zptotk);
	status = m_nt1->addItem("zptotp",m_zptotp);
	 
	status = m_nt1->addItem("zpt",m_zpt);
	status = m_nt1->addItem("zpte",m_zpte);
	status = m_nt1->addItem("zptmu",m_zptmu);
	status = m_nt1->addItem("zptk",m_zptk);
	status = m_nt1->addItem("zptp",m_zptp);
	
	status = m_nt1->addItem("fptot",m_fptot);
	status = m_nt1->addItem("fptote",m_fptote);
	status = m_nt1->addItem("fptotmu",m_fptotmu);
	status = m_nt1->addItem("fptotk",m_fptotk);
	status = m_nt1->addItem("fptotp",m_fptotp);
	status = m_nt1->addItem("fpt",m_fpt);
	status = m_nt1->addItem("fpte",m_fpte);
	status = m_nt1->addItem("fptmu",m_fptmu);

	status = m_nt1->addItem("fptk",m_fptk);
	status = m_nt1->addItem("fptp",m_fptp);
	status = m_nt1->addItem("lptot",m_lptot);
	status = m_nt1->addItem("lptote",m_lptote);
	status = m_nt1->addItem("lptotmu",m_lptotmu);
	status = m_nt1->addItem("lptotk",m_lptotk);
	status = m_nt1->addItem("lptotp",m_lptotp);
	status = m_nt1->addItem("lpt",m_lpt);
	status = m_nt1->addItem("lpte",m_lpte);
	status = m_nt1->addItem("lptmu",m_lptmu);
	status = m_nt1->addItem("lptk",m_lptk);
	status = m_nt1->addItem("lptp",m_lptp);

	status = m_nt1->addItem("zsigp",m_zsigp);
	status = m_nt1->addItem("zsigpe",m_zsigpe);
	status = m_nt1->addItem("zsigpmu",m_zsigpmu);
	status = m_nt1->addItem("zsigpk",m_zsigpk);
	status = m_nt1->addItem("zsigpp",m_zsigpp);
	status = m_nt1->addItem("fhelix",5,m_fhelix);
	status = m_nt1->addItem("fhelixe",5,m_fhelixe);
	status = m_nt1->addItem("fhelixmu",5,m_fhelixmu);
	status = m_nt1->addItem("fhelixk",5,m_fhelixk);
	status = m_nt1->addItem("fhelixp",5,m_fhelixp);
	status = m_nt1->addItem("lhelix",5,m_lhelix);
	status = m_nt1->addItem("lhelixe",5,m_lhelixe);
	status = m_nt1->addItem("lhelixmu",5,m_lhelixmu);
	status = m_nt1->addItem("lhelixk",5,m_lhelixk);
	status = m_nt1->addItem("lhelixp",5,m_lhelixp);

	status = m_nt1->addItem("zerror",15,m_zerror);
	status = m_nt1->addItem("zerrore",15,m_zerrore);
	status = m_nt1->addItem("zerrormu",15,m_zerrormu);
	status = m_nt1->addItem("zerrork",15,m_zerrork);
	status = m_nt1->addItem("zerrorp",15,m_zerrorp);
	status = m_nt1->addItem("ferror",15,m_ferror);
	status = m_nt1->addItem("ferrore",15,m_ferrore);
	status = m_nt1->addItem("ferrormu",15,m_ferrormu);
	status = m_nt1->addItem("ferrork",15,m_ferrork);
	status = m_nt1->addItem("ferrorp",15,m_ferrorp);
	status = m_nt1->addItem("lerror",15,m_lerror);
	status = m_nt1->addItem("lerrore",15,m_lerrore);
	status = m_nt1->addItem("lerrormu",15,m_lerrormu);
	status = m_nt1->addItem("lerrork",15,m_lerrork);
	status = m_nt1->addItem("lerrorp",15,m_lerrorp);
	
	if( status.isFailure() ) cout<<"Ntuple m_nt1 add item failed!"<<endl; 
      }
    }

  
    NTuplePtr  nt2(ntupleSvc(),"FILE_KalValid/n102");
    StatusCode status2;
    if ( nt2 ) m_nt2 = nt2;
    else {
      m_nt2= ntupleSvc()->book("FILE_KalValid/n102",CLID_ColumnWiseTuple,"KalFitValid");
      if ( m_nt2 )  {
	if( status2.isFailure() ) cout<<"Ntuple2 add item failed!"<<endl;
      }
    }
  } 
}


StatusCode ValidRecMdcKalTrackAlg::initialize(){
  
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "Booking Histograms" << endreq;
 
  histDef(); 
  
  log << MSG::INFO << "Finished booking Histograms" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode ValidRecMdcKalTrackAlg::execute() {
   MsgStream log(msgSvc(), name());
   SmartDataPtr<RecMdcKalTrackCol> mdcKalTrkCol(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
   if( ! mdcKalTrkCol )
      {
         log << MSG::ERROR << "Unable to retrieve mdcKalTrackCol" << endreq;
         return  StatusCode::FAILURE;
      } else {
	for(RecMdcKalTrackCol::iterator iter_trk=mdcKalTrkCol->begin(); iter_trk!=mdcKalTrkCol->end(); iter_trk++)    {
	 for( int jj=0, iii=0; jj<5; jj++){
	   m_nhits[jj] = (*iter_trk)->getNhits(jj);
	   m_zhelix[jj] = (*iter_trk)->getZHelix()[jj];
	   m_zhelixe[jj] = (*iter_trk)->getZHelixE()[jj];
	   m_zhelixmu[jj] = (*iter_trk)->getZHelixMu()[jj];
	   m_zhelixk[jj] = (*iter_trk)->getZHelixK()[jj];
	   m_zhelixp[jj] = (*iter_trk)->getZHelixP()[jj];
	   m_fhelix[jj] = (*iter_trk)->getFHelix()[jj];
	   m_fhelixe[jj] = (*iter_trk)->getFHelixE()[jj];
	   m_fhelixmu[jj] = (*iter_trk)->getFHelixMu()[jj];
	   m_fhelixk[jj] = (*iter_trk)->getFHelixK()[jj];
	   m_fhelixp[jj] = (*iter_trk)->getFHelixP()[jj];
	   m_lhelix[jj] = (*iter_trk)->getLHelix()[jj];
	   m_lhelixe[jj] = (*iter_trk)->getLHelixE()[jj];
	   m_lhelixmu[jj] = (*iter_trk)->getLHelixMu()[jj];
	   m_lhelixk[jj] = (*iter_trk)->getLHelixK()[jj];
	   m_lhelixp[jj] = (*iter_trk)->getLHelixP()[jj];

	    
	   for(int kk=0; kk<=jj; kk++,iii++) {
	     m_zerror[iii] = (*iter_trk)->getZError()[jj][kk];
	     m_zerrore[iii] = (*iter_trk)->getZErrorE()[jj][kk];
	     m_zerrormu[iii] = (*iter_trk)->getZErrorMu()[jj][kk];
	     m_zerrork[iii] = (*iter_trk)->getZErrorK()[jj][kk];
	     m_zerrorp[iii] = (*iter_trk)->getZErrorP()[jj][kk];
	     m_ferror[iii] = (*iter_trk)->getFError()[jj][kk];
	     m_ferrore[iii] = (*iter_trk)->getFErrorE()[jj][kk];
	     m_ferrormu[iii] = (*iter_trk)->getFErrorMu()[jj][kk];
	     m_ferrork[iii] = (*iter_trk)->getFErrorK()[jj][kk];
	     m_ferrorp[iii] = (*iter_trk)->getFErrorP()[jj][kk];
	     m_lerror[iii] = (*iter_trk)->getLError()[jj][kk];
	     m_lerrore[iii] = (*iter_trk)->getLErrorE()[jj][kk];
	     m_lerrormu[iii] = (*iter_trk)->getLErrorMu()[jj][kk];
	     m_lerrork[iii] = (*iter_trk)->getLErrorK()[jj][kk];
	     m_lerrorp[iii] = (*iter_trk)->getLErrorP()[jj][kk];
	   }
	 }
     
	 
	 m_ndf[0][0] = (*iter_trk)->getNdf(0,0);
	 m_ndf[1][0] = (*iter_trk)->getNdf(0,1);
	 m_ndf[2][0] = (*iter_trk)->getNdf(0,2);
	 m_ndf[3][0] = (*iter_trk)->getNdf(0,3);
	 m_ndf[4][0] = (*iter_trk)->getNdf(0,4);
	 m_ndf[0][1] = (*iter_trk)->getNdf(1,0);
	 m_ndf[1][1] = (*iter_trk)->getNdf(1,1);
	 m_ndf[2][1] = (*iter_trk)->getNdf(1,2);
	 m_ndf[3][1] = (*iter_trk)->getNdf(1,3);
	 m_ndf[4][1] = (*iter_trk)->getNdf(1,4);
	
	 m_stat[0][0] = (*iter_trk)->getStat(0,0);
	 m_stat[1][0] = (*iter_trk)->getStat(0,1);
	 m_stat[2][0] = (*iter_trk)->getStat(0,2);
	 m_stat[3][0] = (*iter_trk)->getStat(0,3);
	 m_stat[4][0] = (*iter_trk)->getStat(0,4);
	 m_stat[0][1] = (*iter_trk)->getStat(1,0);
	 m_stat[1][1] = (*iter_trk)->getStat(1,1);
	 m_stat[2][1] = (*iter_trk)->getStat(1,2);
	 m_stat[3][1] = (*iter_trk)->getStat(1,3);
	 m_stat[4][1] = (*iter_trk)->getStat(1,4);
	
	 
	 m_fptot = sqrt(1+pow(m_fhelix[4],2))/m_fhelix[2];
	 m_fptote = sqrt(1+pow(m_fhelixe[4],2))/m_fhelixe[2];
	 m_fptotmu = sqrt(1+pow(m_fhelixmu[4],2))/m_fhelixmu[2];
	 m_fptotk = sqrt(1+pow(m_fhelixk[4],2))/m_fhelixk[2];
	 m_fptotp = sqrt(1+pow(m_fhelixp[4],2))/m_fhelixp[2];
	
	 m_zpt = 1/m_zhelix[2];
	 m_zpte = 1/m_zhelixe[2];
	 m_zptmu = 1/m_zhelixmu[2];
	 m_zptk = 1/m_zhelixk[2];
	 m_zptp = 1/m_zhelixp[2];
	
	 m_fpt = 1/m_fhelix[2];
	 m_fpte = 1/m_fhelixe[2];
	 m_fptmu = 1/m_fhelixmu[2];
	 m_fptk = 1/m_fhelixk[2];
	 m_fptp = 1/m_fhelixp[2];
										            
	 m_lpt = 1/m_lhelix[2];
	 m_lpte = 1/m_lhelixe[2];
	 m_lptmu = 1/m_lhelixmu[2];
	 m_lptk = 1/m_lhelixk[2];
	 m_lptp = 1/m_lhelixp[2];
	 
	 m_lptot = sqrt(1+pow(m_lhelix[4],2))/m_lhelix[2];
	 m_lptote = sqrt(1+pow(m_lhelixe[4],2))/m_lhelixe[2];
	 m_lptotmu = sqrt(1+pow(m_lhelixmu[4],2))/m_lhelixmu[2];
	 m_lptotk = sqrt(1+pow(m_lhelixk[4],2))/m_lhelixk[2];
	 m_lptotp = sqrt(1+pow(m_lhelixp[4],2))/m_lhelixp[2];
	
	 m_zptot = sqrt(1+pow(m_zhelix[4],2))/m_zhelix[2];
	 m_zptote = sqrt(1+pow(m_zhelixe[4],2))/m_zhelixe[2];
	 m_zptotmu = sqrt(1+pow(m_zhelixmu[4],2))/m_zhelixmu[2];
	 m_zptotk = sqrt(1+pow(m_zhelixk[4],2))/m_zhelixk[2];
	 m_zptotp = sqrt(1+pow(m_zhelixp[4],2))/m_zhelixp[2];

	 StatusCode sc1 = m_nt1->write();
	 if( sc1.isFailure() ) cout<<"Ntuple1 filling failed!"<<endl;
	}
      }
     return StatusCode::SUCCESS;
}


StatusCode ValidRecMdcKalTrackAlg::finalize() {

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "Finalizing..." << endreq;
   return StatusCode::SUCCESS;
}



