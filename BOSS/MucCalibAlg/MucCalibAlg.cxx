//------------------------------------------------------------------------------|
//      [File  ]:                       MucCalibAlg.cxx                         |
//      [Brief ]:       Algorithom of MUC calibration                           |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#include<iostream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/INTupleSvc.h"

#include "MucCalibAlg/MucCalibAlg.h"

using namespace std;

// -----------------------------------Declaration----------------------------------------
MucCalibAlg::MucCalibAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), 
  m_vJobInfo(0), 
  m_vConfigInfo(0),
  m_sOutputFile("MucCalibConst.root")
{
  // Declare the properties
  declareProperty("JobInfo",    m_vJobInfo);
  declareProperty("ConfigInfo", m_vConfigInfo);
  declareProperty("Output"    , m_sOutputFile);
}

// ----------------------------------Initialize-----------------------------------------
StatusCode MucCalibAlg::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << endreq << "In initialize() " << endreq;
  std::string recMode[3]={"ExtTrk","SlfTrk","Combine"};
  log << MSG::INFO << " InputTag   =    " << m_vJobInfo[4] << endreq;
  log << MSG::INFO << " RecMode    =    " << recMode[m_vConfigInfo[0]] << endreq;
  log << MSG::INFO << " UsePad     =    " << m_vConfigInfo[1] << endreq;
  log << MSG::INFO << " EffWindow  =    " << m_vConfigInfo[2] << endreq;
  log << MSG::INFO << " ClstMode   =    " << m_vConfigInfo[3] << endreq;
  log << MSG::INFO << " ClstSave   =    " << m_vConfigInfo[4] << endreq;
  log << MSG::INFO << " CheckEvent =    " << (m_vConfigInfo[5]?"Yes":"No") << endreq;
  log << MSG::INFO << " DimuSelect =    " << (m_vConfigInfo[6]?"Yes":"No") << endreq;
  log << MSG::INFO << " DimuOnly   =    " << (m_vConfigInfo[7]?"Yes":"No") << endreq;
  //log << MSG::INFO << " Output     =    " << m_sOutputFile << endreq;

  m_pMucCalibMgr = new MucCalibMgr( m_vJobInfo, m_vConfigInfo, m_sOutputFile );

  return StatusCode::SUCCESS;
}

// ----------------------------------Execute--------------------------------------------
StatusCode MucCalibAlg::execute()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << endreq << "In execute()" << endreq;
  StatusCode sc = StatusCode::FAILURE;
 
  m_pMucCalibMgr->ReadEvent();
  if( m_vConfigInfo[5] == 1 ) m_pMucCalibMgr->CheckEvent();
  m_pMucCalibMgr->FillEvent();
  
  return StatusCode::SUCCESS;
}

// ----------------------------------Finalize------------------------------------------
StatusCode MucCalibAlg::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << endreq << "In finalize()" << endreq << endreq;
  m_pMucCalibMgr->AnalyseEffAndNoise();
  //m_pMucCalibMgr->AnalyseCluster();
  m_pMucCalibMgr->AnalyseRes();
  
  m_pMucCalibMgr->SaveConst();

  m_pMucCalibMgr->EndRun();
  //delete m_pMucCalibMgr;
  log << MSG::INFO << endreq << "MucCalibAlg finalizes successfully!" << endreq << endreq;
  return StatusCode::SUCCESS;
}

//--------------------------------------------END-----------------------------------------

