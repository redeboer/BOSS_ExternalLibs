#define COMMON_MAIN_CPP


// Include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include <iostream>
#include <fstream>

// RCS Id for identification of object version
static const char* rcsid = "$Id: BossMain.cxx,v 1.4 2013/11/21 01:07:52 dengzy Exp $";


//------------------------------------------------------------------------------
//
//  Package    : Boss Common
//
//  Description: Main Program
//
//------------------------------------------------------------------------------


int main( int argc, char* argv[] ) {

  

  std::cout<<std::endl
     << "**************************************************" << std::endl
     << "               BOSS version: "<<getenv("BES_RELEASE") << std::endl
     << "************** BESIII Collaboration **************" << std::endl
     << std::endl;
  // Create an instance of an application manager
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty> propMgr (  iface );
  SmartIF<IAppMgrUI> appMgr  (  iface );
  if( !appMgr.isValid() || !propMgr.isValid() ) {
    std::cout << "Fatal error while creating the ApplicationMgr " << std::endl;
    return 1;
  }

  // Get the input configuration file from arguments (defaulting to "jobOptions.txt")
  std:: string opts = (argc>1) ? argv[1] : "jobOptions.txt";
  std::cout<<"the jobOptions file is : " <<opts<<std::endl;
  std::ifstream fin(opts.c_str());
  if(fin.peek()==std::ifstream::traits_type::eof())
  {
    std::cout<<"ERROR! the jobOptions file is empty!"<<std::endl;
    exit(1);
  }
  std::string cont;
  std::string temp;
  while(!fin.eof())
  {
    fin>>temp;
    cont+=temp;
  }
  if(cont.size()<10)
  {
    std::cout<<"Please check your jobOptions."<<std::endl;
    exit(1);
  }
      

  propMgr->setProperty( "JobOptionsPath", opts );
  if( opts.substr( opts.length() - 3, 3 ) == ".py" ) {

    // Python script specified - override defaults
    propMgr->setProperty( "JobOptionsType", "NONE" );
    propMgr->setProperty( "DLLs",
       "['BossServices', 'GaudiPython', 'McEventSelector' ,'GaudiSvc']" );
    propMgr->setProperty( "ExtSvc",
       "[ 'McEventSelector/EventSelector']" );
    propMgr->setProperty( "Runable",        "PythonScriptingSvc" );
    propMgr->setProperty( "EventLoop",      "BossEventLoopMgr" );
    propMgr->setProperty( "OutStreamType",  "BossOutputStream" );
  }

  // Run the application manager and process events
  return (appMgr->run().isSuccess()) ? 0 : 1;
}
