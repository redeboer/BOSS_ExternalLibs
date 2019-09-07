#include "MdcFastTrkAlg/MdcFastTrkAlg.h"
#include "MdcFastTrkAlg/FTFinder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/Event.h"
#include "MdcRawEvent/MdcDigi.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"

#ifndef OnlineMode
#include "MdcFastTrkAlg/ntupleItem.h"
#include "BesTimerSvc/BesTimerSvc.h"
#include "McTruth/McKine.h"
#endif

#include <vector>
#include <iostream>

using namespace std;

//...Globals...

#ifndef OnlineMode
//MCTruth

int num_2Dtrk, num_3Dtrk; 
int num_finaltrk;

extern NTuple::Item<long>   g_ntrkMC, g_eventNo;
extern NTuple::Array<float> g_theta0MC, g_phi0MC;
extern NTuple::Array<float> g_pxMC, g_pyMC, g_pzMC, g_ptMC;

//recon
extern NTuple::Item<long>   g_ntrk;
extern NTuple::Item<float>  g_eventtime;
extern NTuple::Array<float> g_px, g_py, g_pz, g_pt, g_p;
extern NTuple::Array<float> g_phi, g_theta;
extern NTuple::Array<float> g_vx, g_vy, g_vz;
extern NTuple::Array<float> g_dr, g_phi0, g_kappa, g_dz, g_tanl;
extern NTuple::Item<float>  g_estime;

extern IHistogram1D*   g_sigmaxy;
extern IHistogram1D*   g_sigmaz;
extern IHistogram1D*   g_chi2xy;
extern IHistogram1D*   g_chi2sz;
extern IHistogram1D*   g_ncellMC;
extern IHistogram1D*   g_ncell;
extern IHistogram1D*   g_naxialhit;
extern IHistogram1D*   g_nstereohit;
extern IHistogram1D*   g_nhit;
extern IHistogram2D*   g_hitmap[20];
#endif

/////////////////////////////////////////////////////////////////////////////

MdcFastTrkAlg::MdcFastTrkAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
	declareProperty("bunchtime",      m_bunchtime_MC=8.0);
	declareProperty("T0cal",          m_mdc_tcal=200.0);
  m_ftFinder = 0;
  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcFastTrkAlg::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

#ifndef OnlineMode
  NTuplePtr nt(ntupleSvc(),"FILE102/n1");
  if ( nt ) m_tuple = nt;
  else {
      m_tuple=ntupleSvc()->book("FILE102/n1",CLID_ColumnWiseTuple,"MdcRecEvent");
      if( m_tuple ) {
         m_tuple->addItem ("eventNo",  g_eventNo);
         m_tuple->addItem ("NtrackMC", g_ntrkMC,0,50);
         m_tuple->addItem ("MCtheta0", g_ntrkMC,  g_theta0MC);
         m_tuple->addItem ("MCphi0",   g_ntrkMC, g_phi0MC);
         m_tuple->addItem ("pxMC",  g_ntrkMC, g_pxMC);
         m_tuple->addItem ("pyMC",  g_ntrkMC, g_pyMC);
         m_tuple->addItem ("pzMC",  g_ntrkMC, g_pzMC);
         m_tuple->addItem ("ptMC",  g_ntrkMC, g_ptMC);
         m_tuple->addItem ("Ntrack",g_ntrk, 0, 50);
         m_tuple->addItem ("px",    g_ntrk, g_px);
         m_tuple->addItem ("py",    g_ntrk, g_py);
         m_tuple->addItem ("pz",    g_ntrk, g_pz);
         m_tuple->addItem ("pt",    g_ntrk, g_pt);
         m_tuple->addItem ("p",     g_ntrk, g_p);
         m_tuple->addItem ("phi",   g_ntrk, g_phi);
         m_tuple->addItem ("theta", g_ntrk, g_theta);
         m_tuple->addItem ("vx",    g_ntrk, g_vx);
         m_tuple->addItem ("vy",    g_ntrk, g_vy);
         m_tuple->addItem ("vz",    g_ntrk, g_vz); 
         m_tuple->addItem ("dr",    g_ntrk, g_dr);
         m_tuple->addItem ("phi0",  g_ntrk, g_phi0);
         m_tuple->addItem ("kappa", g_ntrk, g_kappa);
         m_tuple->addItem ("dz",    g_ntrk, g_dz);
         m_tuple->addItem ("tanl",  g_ntrk, g_tanl);
         m_tuple->addItem ("eventtime", g_eventtime);
	 m_tuple->addItem ("Testime", g_estime);
      }
      else    {   // did not manage to book the N tuple....
         log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple) << endmsg;
         //return StatusCode::FAILURE;
      }
   }

   g_sigmaxy   = histoSvc()->book( "sigmaxy", "1D sigmaxy", 100, -0.2, 0.2 );
   g_sigmaz = histoSvc()->book( "sigmaz", "1D sigmaz", 100, -3.5, 3.5);
   g_chi2xy = histoSvc()->book( "chi2xy", "1D chi2xy", 100, -0.01, 0.01 );
   g_chi2sz = histoSvc()->book( "chi2sz", "1D chi2sz", 100, -8.0, 8. );
   g_ncellMC = histoSvc()->book( "ncellmc", "1D ncellMC", 100, 0.0, 7000. );
   g_ncell = histoSvc()->book( "ncell", "1D ncell", 100, 0.0, 7000. );
   g_naxialhit = histoSvc()->book( "naxialhit", "2D axial hit", 30, 0.0, 30. );
   g_nstereohit = histoSvc()->book( "nstereohit", "2D stereo hit", 30, 0.0, 30. );
   g_nhit = histoSvc()->book( "nhit", "2D hit", 50, 0.0, 50. );
   g_hitmap[0] = histoSvc()->book( "hm0", "2d hitmap1", 80, -80., 80., 80, -80., 80. );
   g_hitmap[1] = histoSvc()->book( "hm1", "2d hitmap2", 80, -80., 80., 80, -80., 80. );
   g_hitmap[2] = histoSvc()->book( "hm2", "2d hitmap3", 80, -80., 80., 80, -80., 80. );
   g_hitmap[3] = histoSvc()->book( "hm3", "2d hitmap4", 80, -80., 80., 80, -80., 80. );
   g_hitmap[4] = histoSvc()->book( "hm4", "2d hitmap5", 80, -80., 80., 80, -80., 80. );
   g_hitmap[5] = histoSvc()->book( "hm5", "2d hitmap6", 80, -80., 80., 80, -80., 80. );
   g_hitmap[6] = histoSvc()->book( "hm6", "2d hitmap7", 80, -80., 80., 80, -80., 80. );
   g_hitmap[7] = histoSvc()->book( "hm7", "2d hitmap8", 80, -80., 80., 80, -80., 80. );
   g_hitmap[8] = histoSvc()->book( "hm8", "2d hitmap9", 80, -80., 80., 80, -80., 80. );
   g_hitmap[9] = histoSvc()->book( "hm9", "2d hitmap10", 80, -80., 80., 80, -80., 80. );
   g_hitmap[10] = histoSvc()->book( "hm10", "2d hitmap11", 80, -80., 80., 80, -80., 80. );

#endif

  /*NTuplePtr nth(ntupleSvc(),"FILE1/Hit/2");
  if ( nth ) m_htuple = nth;
  else {
      m_htuple=ntupleSvc()->book("FILE1/Hit/2",CLID_ColumnWiseTuple,"MdcRecHit");
      if( m_htuple ) {
         m_htuple->addItem ("sigmaxy", g_sigmaxy);
         m_htuple->addItem ("sigmaz", g_sigmaz);
         m_htuple->addItem ("chi2xy", g_chi2xy);
         m_htuple->addItem ("chi2sz", g_chi2sz);
         m_htuple->addItem ("ncellMC", g_ncellMC);
         m_htuple->addItem ("ncell", g_ncell);
      }
      else    {   // did not manage to book the N tuple....
         log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple) << endmsg;
         return StatusCode::FAILURE;
      }
   }*/
#ifndef OnlineMode   
   num_2Dtrk=0;
   num_3Dtrk=0; 
   num_finaltrk=0;
#endif

   //m_ftFinder->init();
   m_ftFinder = new FTFinder();
   m_ftFinder->init(); 
   m_ftFinder->setBunchtime(m_bunchtime_MC);
   m_ftFinder->setT0cal(m_mdc_tcal);
   m_ftFinder->setAlgorithmPointer(this);


#ifndef OnlineMode
  StatusCode sc = service( "BesTimerSvc", m_timersvc);
  if( sc.isFailure() ) {
     log << MSG::WARNING << name() << ": Unable to locate BesTimer Service" << endreq;
     return StatusCode::FAILURE;
  }

  m_timer[1] = m_timersvc->addItem("Execution");
  m_timer[1]->propName("nExecution");
#endif
 
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcFastTrkAlg::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;

#ifndef OnlineMode
  m_timer[1]->start();
#endif

  m_ftFinder->event();

#ifndef OnlineMode
  m_timer[1]->stop();

  //cout << "m_timer[1]->elapsed()::" << m_timer[1]->elapsed() << endl;
  //cout << "m_timer[1]->mean()::" << m_timer[1]->mean() << endl;
  g_eventtime = m_timer[1]->elapsed();

  if(m_tuple){     
    StatusCode status = m_tuple->write();
    if (!status.isSuccess()) {
      log << MSG::ERROR << "Can't fill ntuple!" << endreq;
    }
  }
#endif

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcFastTrkAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  m_ftFinder->term();
#ifndef OnlineMode
  log << MSG::DEBUG <<"num_2Dtrk: " << num_2Dtrk 
      <<" num_3Dtrk: " << num_3Dtrk 
      <<" num_finaltrk: " << num_finaltrk 
      << endmsg;
  m_timersvc->print();
#endif

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode MdcFastTrkAlg::beginRun() {
 
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in beginRun()" << endreq;

  m_ftFinder->begin_run();
 
  return StatusCode::SUCCESS;
}
