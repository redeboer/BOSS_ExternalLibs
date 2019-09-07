#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "DQAEvent/DQAEvent.h"
#include "DQACtrlSamples/DQACtrlSamples.h"

DECLARE_ALGORITHM_FACTORY(DQACtrlSamples)

/////////////////////////////////////////////////////////////////////////////

DQACtrlSamples::DQACtrlSamples(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
   
    //Declare the properties
    declareProperty("SelectBhabha", m_selBhabha = true);
    declareProperty("SelectDimu", m_selDimu = true);
    declareProperty("SelectHadron", m_selHadron = false);
    declareProperty("SelectRhopi", m_selRhopi = false);
    declareProperty("SelectPpbar", m_selPpbar = false);
    declareProperty("SelectKstark", m_selKstark = false);
    declareProperty("SelectLambdalambda", m_selLambdalambda = false);
    declareProperty("SelectPPpipi", m_selPPpipi = false);
    declareProperty("SelectKsKpiDedx", m_selKsKpiDedx = false);

    declareProperty("SelectpipiJpsi", m_selpipiJpsi = false);

    declareProperty("OutputBhabha", m_OutBhabha = false);
    declareProperty("OutputDimu", m_OutDimu = false);
    declareProperty("OutputHadron", m_OutHadron = false);
    declareProperty("OutputRhopi", m_OutRhopi = false);
    declareProperty("OutputPpbar", m_OutPpbar = false);
    declareProperty("OutputKstark", m_OutKstark = false);
    declareProperty("OutputLambdalambda", m_OutLambdalambda = false);
    declareProperty("OutputPPpipi", m_OutPPpipi = false);
    declareProperty("OutputKsKpiDedx", m_OutKsKpiDedx = false);

    declareProperty("OutputpipiJpsi", m_OutpipiJpsi = false);
    
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQACtrlSamples::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode sc;

  if ( m_selBhabha ) {
      sc =  createSubAlgorithm( "DQASelBhabha", "DQASelBhabha", m_dqaBhabha);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm DQASelBhabha" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutBhabha ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutBhabha", m_dqaoutBhabha);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutBhabha" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }

  if ( m_selDimu ) {
      sc =  createSubAlgorithm( "DQASelDimu", "DQASelDimu", m_dqaDimu);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm DQASelDimu" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutDimu ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutDimu", m_dqaoutDimu);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutDimu" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }

  if ( m_selHadron ) {
      sc =  createSubAlgorithm( "DQASelHadron", "DQASelHadron", m_dqaHadron);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm DQASelHadron" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutHadron ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutHadron", m_dqaoutHadron);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutHadron" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }

  if ( m_selRhopi ) {
      sc =  createSubAlgorithm( "DQARhopi", "DQARhopi", m_dqaRhopi);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm DQARhopiAlg" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutRhopi ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutRhopi", m_dqaoutRhopi);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutRhopi" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }

  if ( m_selPpbar ) {
      sc =  createSubAlgorithm( "DQAJpsi2PPbarAlg", "DQAJpsi2PPbarAlg", m_dqaPpbar);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm Jpsi2PPbarAlg" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutPpbar ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutPpbar", m_dqaoutPpbar);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutPpbar" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }
  
  if ( m_selKstark ) {
      sc =  createSubAlgorithm( "DQAKsKpi", "DQAKsKpi", m_dqaKstark);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm DQAKsKpi" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutKstark ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutKstark", m_dqaoutKstark);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutKstark" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }
  
  if ( m_selLambdalambda ) {
      sc =  createSubAlgorithm( "JsiLL", "JsiLL", m_dqaLambdalambda);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm JsiLL" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutLambdalambda ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutLambdalambda", m_dqaoutLambdalambda);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutLambdalambda" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }

  if ( m_selPPpipi ) {
      sc = createSubAlgorithm( "DQAPi2p2", "DQAPi2p2", m_dqaPPpipi);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm DQAPi2p2" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutPPpipi ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutPPpipi", m_dqaoutPPpipi);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAPi2p2" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }
  
   if ( m_selKsKpiDedx ) {
      sc =  createSubAlgorithm( "DQAKsKpiDEDX", "DQAKsKpiDEDX", m_dqaKsKpiDedx);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm DQAKsKpiDedx" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutKsKpiDedx ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutKsKpiDedx", m_dqaoutKsKpiDedx);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutKsKpiDedx" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }

   if ( m_selpipiJpsi ) {
      sc =  createSubAlgorithm( "PipiJpsi", "PipiJpsi", m_dqapipiJpsi);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Error creating Sub-Algorithm DQApipiJpsi" << endreq;
          return StatusCode::FAILURE;
      }
      if ( m_OutpipiJpsi ) {
          sc = createSubAlgorithm( "EventWriter", "DQAOutpipiJpsi", m_dqaoutpipiJpsi);
          if (sc.isFailure()) {
              log << MSG::ERROR << "Error creating Sub-Algorithm DQAOutpipiJpsi" << endreq;
              return StatusCode::FAILURE;
          }
      }
  }
 
  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQACtrlSamples::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<DQAEvent::DQAEvent> dqaevt(eventSvc(), "/Event/DQATag");
  if( !dqaevt ) {
      dqaevt = new DQAEvent::DQAEvent;
      StatusCode sc;
      sc = eventSvc()->registerObject("/Event/DQATag", dqaevt);
      if (sc.isFailure()) {
          log << MSG::ERROR << "Could not register DQAEvent to TDS" << endreq;
          return StatusCode::FAILURE;
          return sc;
      }
  }

  if ( m_selBhabha ) {
      m_dqaBhabha->execute();
      if ( m_dqaBhabha->filterPassed() ) {
          dqaevt->setBhabha(1);
          if ( m_OutBhabha ) m_dqaoutBhabha->execute();
      }
  }

  if ( m_selDimu ) {
      m_dqaDimu->execute();
      if ( m_dqaDimu->filterPassed() ) {
          dqaevt->setDimu(1);
          if ( m_OutDimu ) m_dqaoutDimu->execute();
      }
  }

  if ( m_selHadron ) {
      m_dqaHadron->execute();
      if ( m_dqaHadron->filterPassed() ) {
          dqaevt->setHadron(1);
          if ( m_OutHadron ) m_dqaoutHadron->execute();
      }
  }
  
  if ( m_selRhopi ) {
      m_dqaRhopi->execute();
      if ( m_dqaRhopi->filterPassed() ) {
          dqaevt->setRhopi(1);
          if ( m_OutRhopi ) m_dqaoutRhopi->execute();
      }
  }
  
  if ( m_selPpbar ) {
      m_dqaPpbar->execute();
      if ( m_dqaPpbar->filterPassed() ) {
          dqaevt->setPpbar(1);
          if ( m_OutPpbar ) m_dqaoutPpbar->execute();
      }
  }
  
  if ( m_selKstark ) {
      m_dqaKstark->execute();
      if ( m_dqaKstark->filterPassed() ) {
          dqaevt->setKstark(1);
          if ( m_OutKstark ) m_dqaoutKstark->execute();
      }
  }
  
  if ( m_selLambdalambda ) {
      m_dqaLambdalambda->execute();
      if ( m_dqaLambdalambda->filterPassed() ) {
          dqaevt->setLambdalambdabar(1);
          if ( m_OutLambdalambda ) m_dqaoutLambdalambda->execute();
      }
  }

  if ( m_selPPpipi ) {
      m_dqaPPpipi->execute();
      if ( m_dqaPPpipi->filterPassed() ) {
          dqaevt->setPpbarpipi(1);
          if ( m_OutPPpipi ) m_dqaoutPPpipi->execute();
      }
  }

   if ( m_selKsKpiDedx ) {
      m_dqaKsKpiDedx->execute();
      if ( m_dqaKsKpiDedx->filterPassed() ) {
          dqaevt->setKstark(1);
          if ( m_OutKsKpiDedx ) m_dqaoutKsKpiDedx->execute();
      }
  }
  
    if ( m_selpipiJpsi ) {
      m_dqapipiJpsi->execute();
      if ( m_dqapipiJpsi->filterPassed() ) {
          dqaevt->setHadron(1);
          if ( m_OutpipiJpsi ) m_dqaoutpipiJpsi->execute();
      }
  }
  
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQACtrlSamples::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


