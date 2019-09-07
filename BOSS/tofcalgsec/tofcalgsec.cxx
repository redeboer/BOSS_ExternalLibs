#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartDataLocator.h"
#include "EventModel/Event.h"
#include "EventModel/EventModel.h"
#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"
#include "tofcalgsec/TofCalibManager.h"
#include "tofcalgsec/TofCalib.h"
#include "tofcalgsec/calib_barrel_left.h"
#include "tofcalgsec/calib_barrel_right.h"
#include "tofcalgsec/calib_barrel_left_offset1_bunch0_3.h"
#include "tofcalgsec/calib_barrel_left_offset1_bunch1_3.h"
#include "tofcalgsec/calib_barrel_left_offset1_bunch2_3.h"
#include "tofcalgsec/calib_barrel_left_offset1_bunch3_3.h"
#include "tofcalgsec/calib_barrel_left_offset2_bunch0_3.h"
#include "tofcalgsec/calib_barrel_left_offset2_bunch1_3.h"
#include "tofcalgsec/calib_barrel_left_offset2_bunch2_3.h"
#include "tofcalgsec/calib_barrel_left_offset2_bunch3_3.h"
#include "tofcalgsec/calib_barrel_right_offset1_bunch0_3.h"
#include "tofcalgsec/calib_barrel_right_offset1_bunch1_3.h"
#include "tofcalgsec/calib_barrel_right_offset1_bunch2_3.h"
#include "tofcalgsec/calib_barrel_right_offset1_bunch3_3.h"
#include "tofcalgsec/calib_barrel_right_offset2_bunch0_3.h"
#include "tofcalgsec/calib_barrel_right_offset2_bunch1_3.h"
#include "tofcalgsec/calib_barrel_right_offset2_bunch2_3.h"
#include "tofcalgsec/calib_barrel_right_offset2_bunch3_3.h"
#include "tofcalgsec/calib_barrel_left_offset1_bunch0_4.h"
#include "tofcalgsec/calib_barrel_left_offset1_bunch1_4.h"
#include "tofcalgsec/calib_barrel_left_offset1_bunch2_4.h"
#include "tofcalgsec/calib_barrel_left_offset1_bunch3_4.h"
#include "tofcalgsec/calib_barrel_left_offset2_bunch0_4.h"
#include "tofcalgsec/calib_barrel_left_offset2_bunch1_4.h"
#include "tofcalgsec/calib_barrel_left_offset2_bunch2_4.h"
#include "tofcalgsec/calib_barrel_left_offset2_bunch3_4.h"
#include "tofcalgsec/calib_barrel_right_offset1_bunch0_4.h"
#include "tofcalgsec/calib_barrel_right_offset1_bunch1_4.h"
#include "tofcalgsec/calib_barrel_right_offset1_bunch2_4.h"
#include "tofcalgsec/calib_barrel_right_offset1_bunch3_4.h"
#include "tofcalgsec/calib_barrel_right_offset2_bunch0_4.h"
#include "tofcalgsec/calib_barrel_right_offset2_bunch1_4.h"
#include "tofcalgsec/calib_barrel_right_offset2_bunch2_4.h"
#include "tofcalgsec/calib_barrel_right_offset2_bunch3_4.h"
#include "tofcalgsec/calib_barrel_atten.h"
#include "tofcalgsec/calib_barrel_veff.h"
#include "tofcalgsec/calib_endcap_left.h"
#include "tofcalgsec/calib_endcap_veff.h"
#include "tofcalgsec/TofCalibFit.h"
#include "tofcalgsec/calib_barrel_q0.h"
#include "tofcalgsec/calib_barrel_common.h"
#include "tofcalgsec/calib_barrel_sigma.h"
#include "tofcalgsec/calib_endcap_sigma.h"
#include "tofcalgsec/calib_endcap_atten.h"
#include "tofcalgsec/calib_etf_left.h"
#include "tofcalgsec/calib_etf_right.h"
#include "tofcalgsec/calib_etf_combine.h"
#include "tofcalgsec/calib_mc_etf_left.h"
#include "tofcalgsec/calib_mc_etf_right.h"
#include "tofcalgsec/calib_mc_etf_combine.h"
#include "tofcalgsec/calib_etf_veff.h"
#include "tofcalgsec/calib_etf_bunch.h"
#include "tofcalgsec/tofcalgsec.h"
#include "TSystem.h"
#include <stdlib.h>


tofcalgsec::tofcalgsec(const std::string& name, ISvcLocator* pSvcLocator):Algorithm(name,pSvcLocator){
  //set default values
  m_calibItem="111111111";
  //declares
  declareProperty("Online",                m_online                );
  declareProperty("Calibration",           m_calibration           );
  declareProperty("WorkDir",               m_workdir               );
  declareProperty("InputDataDir",          m_datafile_dir          );
  declareProperty("InputDataFiles_Barrel", m_datafile_barrel       );
  declareProperty("InputDataFiles_Endcap", m_datafile_endcap       );
  declareProperty("InputDataFiles_Etf",    m_datafile_etf          );
  declareProperty("CalibItem",             m_calibItem             );
  declareProperty("TimeCorrelationZBin",   m_tcorrzbin = 23        );
  declareProperty("BarrelSigmaZBin",       m_barrelzbin = 5        );
  declareProperty("EndcapSigmaRBin",       m_endcaprbin = 5        );
  declareProperty("EndcapQRBin",           m_endcapQrbin = 5       );
}


StatusCode tofcalgsec::initialize(){

  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<< "tofcalgsec::initialize()!" <<endreq;

  if( m_online ) {
    NTuplePtr nt1(ntupleSvc(),"TofCalib/btrk");
    NTuplePtr nt2(ntupleSvc(),"TofCalib/etrk");
    NTuplePtr nt3(ntupleSvc(),"TofCalib/etf");
    if ( nt1 || nt2 || nt3 ) {
      mtuple_barrel = nt1;
      mtuple_endcap = nt2;
      mtuple_etf    = nt3;
    }
    else {
      mtuple_barrel = ntupleSvc()->book("TofCalib/btrk",CLID_ColumnWiseTuple,"TofCalib");
      mtuple_endcap = ntupleSvc()->book("TofCalib/etrk",CLID_ColumnWiseTuple,"TofCalib");
      mtuple_etf    = ntupleSvc()->book("TofCalib/etf", CLID_ColumnWiseTuple,"TofCalib");

      if( mtuple_barrel ) {
	m_checkbarrel = new TofCalibCheck( mtuple_barrel );
      }
      else{
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(mtuple_barrel) <<endmsg;
	return StatusCode::FAILURE;
      }
      if( mtuple_endcap ) {
	m_checkendcap = new TofCalibCheck( mtuple_endcap );
      }
      else{
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(mtuple_endcap) <<endmsg;
	return StatusCode::FAILURE;
      }
      if( mtuple_etf ) {
	m_checketf = new TofCalibCheck( mtuple_etf );
      }
      else{
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(mtuple_etf) <<endmsg;
	return StatusCode::FAILURE;
      }
    }
  }
  else {
    m_calibration = true; // offline
  }

  if( !m_online || m_calibration ) {
    // check workdir
    std::cout<<" tofcalgsec checks your configurations ..."<<std::endl;
    void *pdir=0;
    if((pdir=gSystem->OpenDirectory(m_workdir.c_str()))==0){//dir exists?
      std::cerr<<"WorkDir "<<m_workdir<<" doesn't exist "<<std::endl;
      if(gSystem->MakeDirectory(m_workdir.c_str())==-1){//mkdir,fail to create?
	std::cerr<<" Fail to create directory : "<<m_workdir
		 <<" Specify  ./ as current work directory"<<std::endl;
	m_workdir="./";
      }
    }
    else{
      gSystem->FreeDirectory(pdir);
      pdir=0;
    }
  }

  if( !m_online ) {
    void *pdir=0;
    if( (pdir=gSystem->OpenDirectory(m_datafile_dir.c_str())) ) {
      FileStat_t fs;
      std::vector<string>::iterator it = m_datafile_barrel.begin();
      for( ; it!=m_datafile_barrel.end(); it++ ) {
	if( (*it).size()>0 ) {
	  std::string thefile = (m_datafile_dir+"/"+(*it));
	  if( gSystem->GetPathInfo(thefile.c_str(),fs)==0 ) {
 	    m_barrelfile.push_back(thefile);
	  }
	  else{
	    std::cerr<<" File "<<thefile<<" doesn't exist! "<<std::endl;
	  }
	}
      }
      it = m_datafile_endcap.begin();
      for( ; it!=m_datafile_endcap.end(); it++ ) {
	if( (*it).size()>0 ) {
	  std::string thefile=(m_datafile_dir+"/"+(*it));
	  if( gSystem->GetPathInfo(thefile.c_str(),fs)==0 ) {
 	    m_endcapfile.push_back(thefile);
	  }
	  else{
	    std::cerr<<" File "<<thefile<<" doesn't exist! "<<std::endl;
	  }
	}
      }
      it = m_datafile_etf.begin();
      for( ; it!=m_datafile_etf.end(); it++ ) {
	if( (*it).size()>0 ) {
	  std::string thefile=(m_datafile_dir+"/"+(*it));
	  if( gSystem->GetPathInfo(thefile.c_str(),fs)==0 ) {
 	    m_etffile.push_back(thefile);
	  }
	  else{
	    std::cerr<<" File "<<thefile<<" doesn't exist! "<<std::endl;
	  }
	}
      }
      gSystem->FreeDirectory(pdir);
      pdir=0;
    }
    else {
      std::cerr<<" Error : Please specify the InputDataDir which contains input data files for calibration "<<std::endl;
      exit(0);
    }
  }

  if( !m_online || m_calibration ) {
    std::cout<<"******************* Report of before Calibration ********************"<<std::endl
	     <<"* WorkDir   = "<<m_workdir<<" [ results are save in this directory ]   "<<std::endl
	     <<"* WorkMode  = "<<(m_online?"OnLine":"OffLine")<<" [ run alone or with other algorithms ] "<<std::endl
	     <<"* CalibItem = "<<m_calibItem<<" [ which calibraions you perform ]"<<std::endl;
  }

  if( !m_online ) {
    std::cout <<"* ==> InputDataDir : "<<m_datafile_dir<<std::endl
	      <<"*   ===>Files of Barrel : ";
    for( std::vector<std::string>::iterator it=m_barrelfile.begin(); it!=m_barrelfile.end(); it++ ) {
      std::cout << "'" << (*it) << "' ";
    }
    std::cout << std::endl
	      <<"*   ===>Files of Endcap : ";
    for( std::vector<std::string>::iterator it=m_endcapfile.begin(); it!=m_endcapfile.end(); it++ ) {
      std::cout << "'" << (*it) << "' ";
    }
    std::cout << std::endl
	      <<"*   ===>Files of Etf : ";
    for( std::vector<std::string>::iterator it=m_etffile.begin(); it!=m_etffile.end(); it++ ) {
      std::cout << "'" << (*it) << "' ";
    }
    std::cout << std::endl;
  }  
  std::cout<<"*********************************************************************"<<std::endl;

  return StatusCode::SUCCESS;
}


StatusCode tofcalgsec::beginRun(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "tofcalgsec::beginRun()" << endreq;
  return StatusCode::SUCCESS; 
}


StatusCode tofcalgsec::execute(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "start "<<"tofcalgsec::execute()" << endreq;

  if( m_online ) {
    SmartDataPtr<RecBTofCalHitCol> bhitcol(eventSvc(),"/Event/Recon/RecBTofCalHitCol");
    if( bhitcol ) {
      m_checkbarrel->FillBarrelCol( bhitcol );
      if( m_calibration ) {
	TofCalibManager* manager = TofCalibManager::getManager();
	manager->dataSet()->setBarrelData( bhitcol );
      }
    }
    else{
      //      log << MSG::ERROR << " Can't get /Event/Recon/RecBTofCalHitCol in TDS!" << endreq;
      //      return StatusCode::SUCCESS;
    }

    SmartDataPtr<RecETofCalHitCol> ehitcol(eventSvc(),"/Event/Recon/RecETofCalHitCol");
    if( ehitcol ) {
      m_checkendcap->FillEndcapCol( ehitcol );
      if( m_calibration ) {
	TofCalibManager::getManager()->dataSet()->setEndcapData( ehitcol );
      }
    }
    else{
      //      log << MSG::ERROR << " Can't get /Event/Recon/RecETofCalHitCol in TDS!" <<endreq;
      //      return StatusCode::SUCCESS;
    }

    if( bhitcol ) {
      m_checketf->FillEtfCol( bhitcol );
      if( m_calibration ) {
	TofCalibManager::getManager()->dataSet()->setEtfData( bhitcol );
      }
    }
    else{
      //      log << MSG::ERROR << " Can't get /Event/Recon/RecBTofCalHitCol in TDS!" << endreq;
      //      return StatusCode::SUCCESS;
    }

  }

  log << MSG::INFO << "end "<<"tofcalgsec::execute()" << endreq;
  return StatusCode::SUCCESS; 
}


StatusCode tofcalgsec::endRun(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "tofcalgsec::endRun()" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode tofcalgsec::finalize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "tofcalgsec::finalize()" << endreq;

  if( m_online && !m_calibration ) {
    if( m_checkbarrel ) { delete m_checkbarrel; }
    if( m_checkendcap ) { delete m_checkendcap; }
    if( m_checketf )    { delete m_checketf;    }
    return StatusCode::SUCCESS;
  }

  TofCalibManager* manager = TofCalibManager::getManager();
  if( !m_online ) {
    if( m_barrelfile.empty() ) {
      std::cout << " No barrle data files are valid, exit " << std::endl;
      exit(0);
    }
    else {
      manager->dataSet()->setBarrelDataFiles( m_barrelfile );
    }
    if( m_endcapfile.empty() ) {
      std::cout << " No endcap data files are valid, exit " << std::endl;
      exit(0);
    }
    else {
      manager->dataSet()->setEndcapDataFiles( m_endcapfile );
    }
    if( m_etffile.empty() ) {
      std::cout << " No etf data files are valid, exit " << std::endl;
      exit(0);
    }
    else {
      manager->dataSet()->setEtfDataFiles( m_etffile );
    }
  }

  // barrel attenuation length calibration
  if( testbit(5) ) {
    manager->addCalib( new calib_barrel_atten(), 1  );
    manager->addCalib( new calib_barrel_q0(),    1  );
  }
  // barrel effective velocity calibration
  if( testbit(4) ) {
    manager->addCalib( new calib_barrel_veff(),  1  );
  }
  // barrel time calibration
  if( testbit(7) ) {
    manager->addCalib( new calib_barrel_left(),  1  );
    if (testbit(8)){
       manager->addCalib( new calib_barrel_left_offset1_bunch0_4(), true );
       manager->addCalib( new calib_barrel_left_offset1_bunch1_4(), true );
       manager->addCalib( new calib_barrel_left_offset1_bunch2_4(), true );
       manager->addCalib( new calib_barrel_left_offset1_bunch3_4(), true );
       manager->addCalib( new calib_barrel_left_offset2_bunch0_4(), true );
       manager->addCalib( new calib_barrel_left_offset2_bunch1_4(), true );
       manager->addCalib( new calib_barrel_left_offset2_bunch2_4(), true );
       manager->addCalib( new calib_barrel_left_offset2_bunch3_4(), true );
    }
    if( !testbit(8) ) {
       manager->addCalib( new calib_barrel_left_offset1_bunch0_3(), true );
       manager->addCalib( new calib_barrel_left_offset1_bunch1_3(), true );
       manager->addCalib( new calib_barrel_left_offset1_bunch2_3(), true );
       manager->addCalib( new calib_barrel_left_offset1_bunch3_3(), true );
       manager->addCalib( new calib_barrel_left_offset2_bunch0_3(), true );
       manager->addCalib( new calib_barrel_left_offset2_bunch1_3(), true );
       manager->addCalib( new calib_barrel_left_offset2_bunch2_3(), true );
       manager->addCalib( new calib_barrel_left_offset2_bunch3_3(), true );
    }
    manager->addCalib( new calib_barrel_right(), 1 );
    if( testbit(8) ) {
       manager->addCalib( new calib_barrel_right_offset1_bunch0_4(), true );
       manager->addCalib( new calib_barrel_right_offset1_bunch1_4(), true ); 
       manager->addCalib( new calib_barrel_right_offset1_bunch2_4(), true );
       manager->addCalib( new calib_barrel_right_offset1_bunch3_4(), true );
       manager->addCalib( new calib_barrel_right_offset2_bunch0_4(), true );
       manager->addCalib( new calib_barrel_right_offset2_bunch1_4(), true );
       manager->addCalib( new calib_barrel_right_offset2_bunch2_4(), true );
       manager->addCalib( new calib_barrel_right_offset2_bunch3_4(), true );
    }
    if( !testbit(8) ) {
       manager->addCalib( new calib_barrel_right_offset1_bunch0_3(), true );
       manager->addCalib( new calib_barrel_right_offset1_bunch1_3(), true );
       manager->addCalib( new calib_barrel_right_offset1_bunch2_3(), true );
       manager->addCalib( new calib_barrel_right_offset1_bunch3_3(), true );
       manager->addCalib( new calib_barrel_right_offset2_bunch0_3(), true );
       manager->addCalib( new calib_barrel_right_offset2_bunch1_3(), true );
       manager->addCalib( new calib_barrel_right_offset2_bunch2_3(), true );
       manager->addCalib( new calib_barrel_right_offset2_bunch3_3(), true );
    } 
  }
  // barrel sigma calibration
  if( testbit(6) ) {
    manager->addCalib( new calib_barrel_common(m_tcorrzbin), true  );
    manager->addCalib( new calib_barrel_sigma(m_barrelzbin), true  );
  }

  // endcap attenuation length calibration
  if( testbit(1) ) {
    manager->addCalib( new calib_endcap_atten(m_endcapQrbin), false );
  }
  // endcap effective velocity calibration
  if( testbit(0) ) {
    manager->addCalib( new calib_endcap_veff(), false );
  }
  // endcap time calibration
  if( testbit(3) ) {
    manager->addCalib( new calib_endcap_left(), 0 );
  }
  // endcap sigma calibration
  if( testbit(2) ) {
    manager->addCalib( new calib_endcap_sigma(m_endcaprbin), false );
  }
  // etf effective velocity calibration
  if( testbit(9) ) {
    manager->addCalib( new calib_etf_veff(), 2 );
  }
  // monte carlo etf time calibration
  if( testbit(11) ) {
    manager->addCalib( new calib_mc_etf_combine(), 2 );
    manager->addCalib( new calib_mc_etf_left(),    2 );
    manager->addCalib( new calib_mc_etf_right(),   2 );
  }
  // etf time calibration
  if( testbit(10) ) {
    manager->addCalib( new calib_etf_combine(), 2 );
    manager->addCalib( new calib_etf_left(),    2 );
    manager->addCalib( new calib_etf_right(),   2 );
  }
  // etf bunch offset
  if( testbit(12) ) {
    if (testbit(8)){
      manager->addCalib( new calib_etf_bunch(4), false );
    }
    if (!testbit(8)){
      manager->addCalib( new calib_etf_bunch(3), false );
    }
  }

  manager->doCalibration();

  manager->fillTxt( m_workdir );
  manager->fillRoot( m_workdir );

  return StatusCode::SUCCESS;
}


tofcalgsec::~tofcalgsec() {}
