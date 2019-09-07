// Dear emacs, this is -*- c++ -*-

#include "ESController/SC.h"
#include "ESController/error.h"
#include "EFServices/IEFEventLoopMgr.h"
#include "HltTools/PathResolver.h"

//#include <Python.h>
#include <stdint.h>
#include <cmath>
#include <cstdio>
#include <string>

//hxt #include <df_allocator>
//#include "RawDataFormat/FragmentType.h"
//#include "RawDataFormat/AbstractFragment.h"
//#include "RawDataFormat/minimal.h"

// Include files for Gaudi
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IClassManager.h"

//#include "HltConfig/MyFrame.h"

#include <list>

efpsc::SC::SC () :
  m_eventLoopMgr(0),
  m_config(0),
  m_pesaAppMgr(0),
  m_isCreated(false),
  m_outLev(5),
  m_nameEventLoopMgr("EFEventLoopMgr")
{
   ::setenv("SCOutputLevel","3",1);
   char* ss=::getenv("SCOutputLevel");
   //if(ss!=NULL)std::cout<<ss<<std::endl;
   //else std::cout<<"****************"<<std::endl;
    //char* outlev=::getenv("SCOutputLevel");
    //std::cout<<outlev<<std::endl;
    //m_outLev=atoi(outlev);    
    //std::cout<<m_outLev<<std::endl;
    //Hlt::MyFrame::instance();
}

efpsc::SC::~SC ()
{
  //delete Hlt::MyFrame::instance();
  m_pesaAppMgr->release() ;
}

bool efpsc::SC::configure (const std::string& config)
{ 
  static bool first_call_configure = true;
  if(first_call_configure){
    first_call_configure = false;
  }
  else{
    return true;
  }
  //if(0==Hlt::MyFrame::instance()){
  //    std::cout<<"Error while create MyFrame"<<std::endl;
      //exit(1);
  //}

  //EFPSC_DBG1("efpsc::SC::configure", 
    // "SC configuration started.");
  if(m_outLev<=3) std::cout<<"efpsc::SC::configure  "<<"SC configuration started."<<std::endl;
  m_config = new efpsc::Config(config);

  // Create an instance of the Pesa application manager
  //EFPSC_DBG1("efpsc::SC::configure", 
//	     "---> Create Pesa Application Manager");
  if(m_outLev<=2) std::cout<<"efpsc::SC::configure  "<<"---> Create Pesa Application Manager:"
	  <<m_isCreated<<std::endl;
  if(!m_isCreated){
    if ((m_config->dllName().size() != 0) &&  (m_config->factoryName().size() != 0)) {
      m_pesaAppMgr = Gaudi::createApplicationMgr(m_config->dllName(),m_config->factoryName());
      if(m_outLev<=3)std::cout<<"Gaudi::createApplicationMgr(m_config->dllName(),m_config->factoryName())"
	      		           <<std::endl;
    } else {
      m_pesaAppMgr = Gaudi::createApplicationMgr();
      if(m_outLev<=3)std::cout<<"Gaudi::createApplicationMgr()"<<std::endl;
    }
  }
 
  // configure Pesa
//  EFPSC_DBG1("efpsc::SC::configure", 
//	     "---> Configure Pesa Property Manager");
  if(m_outLev<=2) std::cout<<"efpsc::SC::configure  "<<"---> Configure Pesa Property Manager"<<std::endl;
  StatusCode sc ;
  SmartIF<IProperty> propMgr ( m_pesaAppMgr );
  SmartIF<IAppMgrUI> appMgr  ( m_pesaAppMgr );
  if( !appMgr.isValid() || !propMgr.isValid() ) {
    //EFPSC_ERROR("efpsc::SC::configure",
    //		"While creating the ApplicationMgr");
    if(m_outLev<=5) std::cout<<"efpsc::SC::configure  "<<"While creating the ApplicationMgr"<<std::endl;
    return false;
  }

  // Get the input configuration file from arguments (default "jobOptions.txt")
  std::string opts = "jobOptions.txt";
  ::setenv("efJobPath","",1);
  if (m_config->jobOptionsPath().size() != 0) {
    opts = m_config->jobOptionsPath() ;
    if (opts.rfind("/") < opts.size()) {
      ::setenv("efJobPath",opts.substr(0,opts.rfind("/")).c_str(),1);
    }
  }
  //EFPSC_DBG1("efpsc::SC::configure", "---> Pesa JobOptions file is = " << opts);
  //EFPSC_DBG1("efpsc::SC::configure", "---> Pesa JobOptions path    = " << ::getenv("efJobPath"));
  if(m_outLev<=2) std::cout<<"efpsc::SC::configure  "<<"---> Pesa JobOptions file is = " << opts<<std::endl
				<<"                      "<<"---> Pesa JobOptions path    = " 
				<< ::getenv("efJobPath")<<std::endl;
  propMgr->setProperty( "JobOptionsPath", opts );

  propMgr->setProperty( "EventLoop",m_nameEventLoopMgr );     

  

  // Select the MessageSvc Type
  if (m_config->messageSvcType().size() != 0) {
    propMgr->setProperty( "MessageSvcType", m_config->messageSvcType());
  }

  // No EventSelector will be used
  if (m_config->evtSel().size() != 0) {
    propMgr->setProperty( "EvtSel", m_config->evtSel());
  } else {
    propMgr->setProperty( "EvtSel", "NONE" );
  }
  //EFPSC_DBG1("efpsc::SC::configure", "finish property set--->start configure ");
  //EFPSC_DBG1("efpsc::SC::configure", "Configuring ApplicationMgr: "
//	     << appMgr->stateName() << " Status : " << sc.getCode());
  if(m_outLev<=3) std::cout<<"efpsc::SC::configure  "<<"finish property set--->start configure"<<std::endl;
  if(m_outLev<=2) std::cout<<"efpsc::SC::configure  "<<"Configuring ApplicationMgr: "
			   << appMgr->FSMState() << " Status : " << sc.getCode()<<std::endl;
  // Configure the application manager
  if( opts.substr( opts.length() - 3, 3 ) == ".py" ) {

     //EFPSC_ERROR("efpsc::SC::configure", 
//	         "Error: Don't support python config files");
     if(m_outLev<=5) std::cout<<"efpsc::SC::configure  "
	                           <<"Error: Don't support python config files"<<std::endl;
     return false;
    // ----------------------------
    // Configuration with .py files
    // ----------------------------

    // Try to initialize the Python interpreter
    /*if ( ! Py_IsInitialized() ) {
      Py_Initialize();

      // check
      if ( ! Py_IsInitialized() ) {
        EFPSC_ERROR("efpsc::SC::configure",
                    "Error: Python could not been intialized.");
        Py_Finalize();
        return false;
      }

      // import basic python modules (used throughout)
      std::string pyBasicFile = "SC/SCPythonSetup.py" ;
      if (m_config->pythonSetupFile().size() != 0) {
        pyBasicFile = m_config->pythonSetupFile() ;
      }

      pyBasicFile = PathResolver::find_file(pyBasicFile, "PYTHONPATH");
      FILE* infile = fopen( pyBasicFile.c_str() , "r" );
      if ( PyRun_SimpleFile(infile,const_cast<char*>(pyBasicFile.c_str())) != 0 ) {
        EFPSC_ERROR("efpsc::SC::configure",
                    "Error: Basic Python configuration failed.");
        Py_Finalize();
        return false;
      }

      std::string pyExecFile = "include( \""+std::string(opts)+"\" )" ;
      if ( PyRun_SimpleString(const_cast<char*>(pyExecFile.c_str())) != 0 ) {
        EFPSC_ERROR("efpsc::SC::configure",
                    "Error: Python job configuration script failed.");
        Py_Finalize();
        return false;
      }
    }*/
  } else {
    // -----------------------------
    // Configuration with .txt files
    // -----------------------------
    sc = appMgr->configure();
    if( sc.isFailure() ) {
      //EFPSC_DBG1("efpsc::SC::configure",
//		  "While configuring the ApplicationMgr");
      if(m_outLev<=5) std::cout<<"efpsc::SC::configure  "<<"While configuring the ApplicationMgr"<<std::endl;
      return false;
    }
  }
 // EFPSC_DBG1("efpsc::SC::configure", "Configuring ApplicationMgr: "
//	     << appMgr->stateName() << " Status : " << sc.getCode());
  if(m_outLev<=2) std::cout<<"efpsc::SC::configure  "<<"Configuring ApplicationMgr: "
			   << appMgr->FSMState() << " Status : " << sc.getCode()<<std::endl;
  // Initialize the application manager
  sc = appMgr->initialize();
  //EFPSC_DBG1("efpsc::SC::configure", "Initialize ApplicationMgr : " 
//	     << appMgr->stateName() 
//	     << " Status : " << sc.getCode());
  if(m_outLev<=2) std::cout<<"efpsc::SC::configure  "<<"Initialize ApplicationMgr : " 
			   << appMgr->FSMState() << " Status : " << sc.getCode()<<std::endl;
  if( sc.isFailure() ) {
    EFPSC_ERROR("efpsc::SC::configure", "While intializing the ApplicationMgr");
    return false;
  }

  // Find the basename for the EventLoopMgr  
  std::string value ;
  //std::cout<<"value = "<<value<<std::endl;
  sc = propMgr->getProperty( "EventLoop", value );
  //std::cout<<"value = "<<value<<std::endl;
  if( sc.isFailure() ) {
    //EFPSC_DBG1("efpsc::SC::configure", "While retrieving Property EventLoop ");
    if(m_outLev<=4) std::cout<<"efpsc::SC::configure  "<<"While retrieving Property EventLoop "<<std::endl;
  } else {
    m_nameEventLoopMgr.assign(value, value.find_first_of("\"")+1,
			      value.find_last_of("\"")-value.find_first_of("\"")-1) ;
  }

  SmartIF<ISvcLocator> svcLoc( m_pesaAppMgr );
  if (svcLoc.isValid()) {
    //EFPSC_DBG1("efpsc::SC::configure","Name for EventLoopManager:"<<m_nameEventLoopMgr);
    if(m_outLev<=2) std::cout<<"efpsc::SC::configure  "<<"Name for EventLoopManager:"
	                          <<m_nameEventLoopMgr<<std::endl;
    StatusCode sc ;
    IEventProcessor* processingMgr = 0 ;
    
    sc = svcLoc->service( m_nameEventLoopMgr , processingMgr);
    m_eventLoopMgr=processingMgr;

    if( !sc.isSuccess() )  {
      //EFPSC_ERROR("efpsc::SC::configure", " Error Retrieving Processing manager!") ;
      if(m_outLev<=5) std::cout<<"efpsc::SC::configure  "<<"Error Retrieving Processing manager!"<<std::endl;
      return false;
    }

  } else {
    //EFPSC_ERROR("efpsc::SC::configure", " Error Retrieving Service Locator!") ;
    if(m_outLev<=5) std::cout<<"efpsc::SC::configure  "<<"Error Retrieving Service Locator!"<<std::endl;
    return false;
  }

  m_isCreated=true;
  //EFPSC_DBG1("efpsc::SC::configure","SC configuration finished.");
  if(m_outLev<=3) std::cout<<"efpsc::SC::configure  "<<"SC configuration finished."<<std::endl;
  return true;
}

bool efpsc::SC::unconfigure (void)
{
  return true;
  //EFPSC_DBG1("efpsc::SC::unconfigure",
//	     "SC UNconfiguration started.");
  if(m_outLev<=3) std::cout<<"efpsc::SC::unconfigure  "<<"SC UNconfiguration started."<<std::endl;
  StatusCode sc ;
  SmartIF<IAppMgrUI> appMgr  ( m_pesaAppMgr );
  if( !appMgr.isValid() ) {
//    EFPSC_ERROR("efpsc::SC::unconfigure",
//		"While unconfiguring the ApplicationMgr, no valid interface");
      if(m_outLev<=5) std::cout<<"efpsc::SC::unconfigure  "
	                            <<"While unconfiguring the ApplicationMgr, no valid interface"<<std::endl;
      return false;
  }
  SmartIF<ISvcLocator> svcLoc( m_pesaAppMgr );

  //const std::list<IService*>* svclist=svcLoc->getServices();
  //for(std::list<IService*>::iterator it = svclist->begin(); it != svclist->end(); it++ ) {
  //     std::cout<<(*it)->name()<<std::endl;
  //}

  // Finalize the application manager
  sc = appMgr->finalize();

//  EFPSC_DBG1("efpsc::SC::unconfigure", "Finalize ApplicationMgr: " 
//	     << appMgr->stateName() << " Status : " << sc.getCode());
  if(m_outLev<=2) std::cout<<"efpsc::SC::unconfigure  "<<"Finalize ApplicationMgr: " 
			   << appMgr->FSMState() << " Status : " << sc.getCode()<<std::endl;
  if( sc.isFailure() ) {
//    EFPSC_ERROR("efpsc::SC::unconfigure",
//		"---> While finalizing the ApplicationMgr ");
    if(m_outLev<=5) std::cout<<"efpsc::SC::unconfigure  "<<"While finalizing the ApplicationMgr"<<std::endl;
    return false;
  }

  // Terminate the application manager
  sc = appMgr->terminate();
//  EFPSC_DBG1("efpsc::SC::unconfigure", "Terminate ApplicationMgr : " 
//	     << appMgr->stateName() << " Status : " << sc.getCode());
  if(m_outLev<=2) std::cout<<"efpsc::SC::unconfigure  "<<"Terminate ApplicationMgr : " 
			   << appMgr->FSMState() << " Status : " << sc.getCode()<<std::endl;
  if( sc.isFailure() ) {
    //EFPSC_ERROR("efpsc::SC::unconfigure","While terminating the ApplicationMgr");
    if(m_outLev<=5) std::cout<<"efpsc::SC::unconfigure  "<<"While terminating the ApplicationMgr"<<std::endl;
    return false;
  } 

  //IService* msgSvc=0;
  //svclist=svcLoc->getServices();
  //svclist->erase(svclist.begin(), svclist.end() );
  //std::cout<<svclist->size()<<std::endl;
  //if(svcLoc->existsService("MessageSvc")){
  //     svcLoc->getService("MessageSvc",msgSvc);
  //     std::cout<<"111"<<msgSvc->name()<<std::endl;
  //     std::list<IService*> svclist=svcLoc->getServices();
  //     for(std::list<IService*>::iterator it = svclist.begin(); it != svclist.end(); it++ ) {
  //         //std::cout<<(*it)->name()<<std::endl;
  //     }
  //     int a1=svclist.size();
  //     svclist->remove(msgSvc);
  //     int a2=svclist.size();
  //	if(svcLoc->existsService("MessageSvc"))std::cout<<"exist"<<a1<<","<<a2<<std::endl;
  //}
  //appMgr->release();	 
  //if(msgSvc) msgSvc->release();
  //svcMgr->removeService("MessageSvc");

  delete m_config;
  m_config = 0;
  //EFPSC_DBG2("efpsc::SC::unconfigure", "SC UNconfiguration finished.");
  if(m_outLev<=3) std::cout<<"efpsc::SC::unconfigure  "<<"SC UNconfiguration finished."<<std::endl;
  return true;
}

bool efpsc::SC::prepareForRun(int run_number=0)
{
  // This is a placeholder method
  if(m_eventLoopMgr){
    SmartIF<IEFEventLoopMgr> processor_psc( m_eventLoopMgr);
    StatusCode sc = processor_psc->prepareForRun(run_number);
    std::cout << "=================" << m_eventLoopMgr << std::endl;
    if(sc.isSuccess()) return true;
  }
  return false;
}

bool efpsc::SC::process(const uint32_t *&feStore, const uint32_t *&sdStore,std::string &efDecision_s,std::vector<uint32_t> &efBitPattern)
{
  //std::cout<<"jak:"<<__FILE__<<std::endl;
  //EFPSC_DBG1("efpsc::SC::process", "***********************************************");
  //EFPSC_DBG1("efpsc::SC::process", "****************** NEW EVENT ******************");
  //EFPSC_DBG1("efpsc::SC::process", "***********************************************");
  if(m_outLev<=3)std::cout<<"efpsc::SC::process"<<" **************** NEW EVENT ****************"<<std::endl;
  //Test if FullEventFragment is valid
 // if(!fe->is_valid()){
 //   EFPSC_ERROR("efpsc::SC::process","FullEventFragment NOT valid! This event is not going to be processed!");
 //   return false;
 // }  else {
 //   EFPSC_DBG1("efpsc::SC::process", " FullEventFragment is valid." );
 // }


  // ExecuteEvent from the application manager
  StatusCode sc ;
  if ( 0 != m_eventLoopMgr ) {
    SmartIF<IEventProcessor> processor( m_eventLoopMgr);
    
    if ( processor.isValid() ) {
      SmartIF<IService> processor_s( m_eventLoopMgr);
      //EFPSC_DBG1("efpsc::SC::process", " ---> Executing events for processor"<<processor_s->name());
      //std::cout<<"jak:"<<__FILE__<<std::endl;

      sc = processor->executeEvent((void*)feStore);
      
      //EFPSC_DBG1("efpsc::SC::Process","after executeEvent(fe)");

      //std::cout<<"jak:"<<__FILE__<<std::endl;
      if(m_outLev<=2)std::cout<<"efpsc::SC::process  "<<"after executeEvent(fe)"<<std::endl;
      if( sc.isFailure() ) {
	//EFPSC_DBG1("efpsc::SC::process", "Error for executeEvent in the ApplicationMgr " << processor_s->name());
	if(m_outLev<=4)std::cout<<"efpsc::SC::process  "<<"Error for executeEvent in the ApplicationMgr "
	                             << processor_s->name()<<std::endl;
      }
      else {
	SmartIF<IEFEventLoopMgr> processor_psc( m_eventLoopMgr);
	//
	//--- ef Decision and Result from Pesa
	//
	//std::cout<<"jak:"<<__FILE__<<std::endl;
        efDecision_s = processor_psc->EF_Decision();

	//std::cout<<"jak:"<<__FILE__<<std::endl;
	//EFPSC_DBG1("efpsc::SC::Process","after EF_Decision. EFdecision is: "<<efDecision_s);
	if(m_outLev<=3)std::cout<<"efpsc::SC::process  "<<"after EF_Decision. EFdecision is: "
		                     <<efDecision_s<<std::endl;
	sdStore=(uint32_t*)processor_psc->EF_Fragment_Address();
	//std::cout<<"jak:"<<__FILE__<<std::endl;
	if(sdStore==NULL){
	  //EFPSC_DBG1("efpsc::SC::process","EF_Fragment_Address() returns NULL pointer");
          if(m_outLev<=2)std::cout<<"efpsc::SC::process  "
		                       <<"EF_Fragment_Address() returns NULL pointer"<<std::endl;
	}
	//Hard coded (One must change it in the future):
	//EF_Decision==Accepted--> efDecision=2
	//EF_Decision==Rejected--> efDecision=1
	//EF_Decision==Error--> efDecision=0
	/*
	if(efDecision_s=="Accepted"){
	  //efDecision=2;
	  //Append EFResult to the Event Fragment
	  EFPSC_DBG1("efpsc::SC::process", "EFResult is going to be appended to the Event Fragment");
	}else if(efDecision_s=="Rejected"){ 
	  //efDecision=1;
	  EFPSC_DBG1("efpsc::SC::process", "No EFResult is going to be appended to the Event Fragment");

	}else{
	  //efDecision=0;
	  EFPSC_DBG1("efpsc::SC::process", "an Error was obtained when running algorithms on the Event.");
	  EFPSC_DBG1("efpsc::SC::process", "No EFResult is going to be appended to the Event Fragment");
	}
	*/
	efBitPattern.resize(4, 0);  //zoujh: Just a place holder
	if(efDecision_s=="Junk") efBitPattern[0]=1<<0;
	//else if(efDecision_s=="GJunk") efBitPattern[0]=(1<<0)+(1<<1);
	else if(efDecision_s=="GJunk") efBitPattern[0]=(1<<1);
	else if(efDecision_s=="Beamgas") efBitPattern[0]=1<<2;
	//else if(efDecision_s=="GBeamgas") efBitPattern[0]=(1<<2)+(1<<3);
	else if(efDecision_s=="GBeamgas") efBitPattern[0]=(1<<3);
	else if(efDecision_s=="Cosmic") efBitPattern[0]=1<<4;
	//else if(efDecision_s=="GCosmic") efBitPattern[0]=(1<<4)+(1<<5);
	else if(efDecision_s=="GCosmic") efBitPattern[0]=(1<<5);
	else if(efDecision_s=="EBhabha") efBitPattern[0]=1<<6;
	//else if(efDecision_s=="GEBhabha") efBitPattern[0]=(1<<6)+(1<<7);
	else if(efDecision_s=="GEBhabha") efBitPattern[0]=(1<<7);
	else if(efDecision_s=="BBhabha") efBitPattern[0]=1<<8;
	//else if(efDecision_s=="GBBhabha") efBitPattern[0]=(1<<8)+(1<<9);
	else if(efDecision_s=="GBBhabha") efBitPattern[0]=(1<<9);
	else if(efDecision_s=="Dimuon") efBitPattern[0]=1<<10;
	//else if(efDecision_s=="GDimuon") efBitPattern[0]=(1<<10)+(1<<11);
	else if(efDecision_s=="GDimuon") efBitPattern[0]=(1<<11);
	else if(efDecision_s=="Diphoton") efBitPattern[0]=1<<12;
	//else if(efDecision_s=="GDiphoton") efBitPattern[0]=(1<<12)+(1<<13);
	else if(efDecision_s=="GDiphoton") efBitPattern[0]=(1<<13);
	else if(efDecision_s=="Hadron") efBitPattern[0]=1<<14;
	//else if(efDecision_s=="GHadron") efBitPattern[0]=(1<<14)+(1<<15);
	else if(efDecision_s=="GHadron") efBitPattern[0]=(1<<15);
	else if(efDecision_s=="Twophoton") efBitPattern[0]=1<<16;
	//else if(efDecision_s=="GTwophoton") efBitPattern[0]=(1<<16)+(1<<17);
	else if(efDecision_s=="GTwophoton") efBitPattern[0]=(17);
	else if(efDecision_s=="RandomTrg") efBitPattern[0]=1<<31;
	else if(efDecision_s=="Error")     efBitPattern[0]=1<<30;
	else if(efDecision_s=="Other")     efBitPattern[0]=1<<29;
	else  efBitPattern[0]=1<<28;
	efBitPattern[1]=processor_psc->EF_Algorithm_Tag();
	efBitPattern[2]=processor_psc->EF_Version();
	efBitPattern[3]=processor_psc->EF_Total_Energy();
	//EFPSC_DBG1("efpsc::SC::process", "**************end of psc process!**************"<<std::endl);
        if(m_outLev<=2)std::cout<<"efpsc::SC::process  "
		                     <<"**************end of psc process!**************"<<std::endl;
      }
    } 
    else {
      //EFPSC_DBG1("efpsc::SC::process", "executeEvent ApplicationMgr : no valid event processor ");
      if(m_outLev<=5)std::cout<<"efpsc::SC::process  "
	                           <<"executeEvent ApplicationMgr : no valid event processor "<<std::endl;
      return false;
    }
    
  } 
  else {
    return false;
  }
  return true;
}

