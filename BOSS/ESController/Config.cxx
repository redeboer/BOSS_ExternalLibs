//Dear emacs, this is -*- c++ -*-

#include "ESController/Config.h"
#include "ESController/error.h"
#include <cstdlib>

efpsc::Config::Config(const std::string& objname)
{
  // Default Parameters
  m_jobOptionsPath = "jobOptions.txt";
  m_messageSvcType = "MessageSvc";
  m_evtSel         = "NONE";
  m_dllName        = "";
  m_factoryName    = "";
  m_pythonSetupFile = "ESController/ESControllerPythonSetup.py";

  // Set Parameters
  std::string tmp_string = objname;
  //cout<<"objname: "<<objname<<endl;
 
  // parameter 1 : jobOptionsPath
  m_jobOptionsPath = tmp_string.substr(0, tmp_string.find(";"));
 //tmp_string = tmp_string.substr(tmp_string.find(";")+1,tmp_string.size());
  if (tmp_string.substr(0, tmp_string.find(";")) != "") {
    m_jobOptionsPath = tmp_string.substr(0, tmp_string.find(";"));
    //    cout<<"m_jobOptionsPAth: "<<endl;
  }

  // parameter 2 : MessageSvcType
  tmp_string = tmp_string.substr(tmp_string.find(";")+1,tmp_string.size());
  if (tmp_string.substr(0, tmp_string.find(";")) != "") {
    m_messageSvcType = tmp_string.substr(0, tmp_string.find(";"));
  }

  // parameter 3 : Event selector
  tmp_string = tmp_string.substr(tmp_string.find(";")+1,tmp_string.size());
  if (tmp_string.substr(0, tmp_string.find(";")) != "") {
    m_evtSel = tmp_string.substr(0, tmp_string.find(";"));
  }

  // parameter 4 : ApplicationMgr DLL
  tmp_string = tmp_string.substr(tmp_string.find(";")+1,tmp_string.size());
  if (tmp_string.substr(0, tmp_string.find(";")) != "") {
    m_dllName = tmp_string.substr(0, tmp_string.find(";"));
  }

  // parameter 5 : ApplicationMgr Factory Name
  tmp_string = tmp_string.substr(tmp_string.find(";")+1,tmp_string.size());
  if (tmp_string.substr(0, tmp_string.find(";")) != "") {
    m_factoryName = tmp_string.substr(0, tmp_string.find(";"));
  }

  // parameter 6 : Basic Python Setup File
  tmp_string = tmp_string.substr(tmp_string.find(";")+1,tmp_string.size());
  if (tmp_string.substr(0, tmp_string.find(";")) != "") {
    m_pythonSetupFile = tmp_string.substr(0, tmp_string.find(";"));
  }

  // Print Parameters

  EFPSC_DBG1("efpsc::Config::Config"," -> jobOptions Path             = " << m_jobOptionsPath); 
  EFPSC_DBG1("efpsc::Config::Config"," -> MessageSvcType              = " << m_messageSvcType);
  EFPSC_DBG1("efpsc::Config::Config"," -> Event Selector              = " << m_evtSel);
  EFPSC_DBG1("efpsc::Config::Config"," -> ApplicationMgr DLL name     = " << m_dllName);
  EFPSC_DBG1("efpsc::Config::Config"," -> ApplicationMgr Factory name = " << m_factoryName);
  EFPSC_DBG1("efpsc::Config::Config"," -> Basic Python setup file     = " << m_pythonSetupFile);
}

efpsc::Config::~Config(void)
{
}


