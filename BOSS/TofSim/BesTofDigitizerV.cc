//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$Id: BesTofDigitizerV.cc

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

#include "BesTofDigitizerV.hh"
#include "BesTofDigi.hh"
#include "BesTofHit.hh"
#include "G4DigiManager.hh"
#include "BesTofGeoParameter.hh"
#include "Randomize.hh"

bool BesTofDigitizerV::m_booked = false;

NTuple::Tuple* BesTofDigitizerV::m_tupleTof1 = 0;
NTuple::Item<double> BesTofDigitizerV::m_partId;
NTuple::Item<double> BesTofDigitizerV::m_scinNb;
NTuple::Item<double> BesTofDigitizerV::m_edep;
NTuple::Item<double> BesTofDigitizerV::m_nHits; 
NTuple::Item<double> BesTofDigitizerV::m_time1st0;
NTuple::Item<double> BesTofDigitizerV::m_time1st1;
NTuple::Item<double> BesTofDigitizerV::m_timelast0;
NTuple::Item<double> BesTofDigitizerV::m_timelast1;
NTuple::Item<double> BesTofDigitizerV::m_totalPhot0;
NTuple::Item<double> BesTofDigitizerV::m_totalPhot1;
NTuple::Item<double> BesTofDigitizerV::m_NphAllSteps;
NTuple::Item<double> BesTofDigitizerV::m_max0;
NTuple::Item<double> BesTofDigitizerV::m_max1;
NTuple::Item<double> BesTofDigitizerV::m_tdc0;
NTuple::Item<double> BesTofDigitizerV::m_adc0;
NTuple::Item<double> BesTofDigitizerV::m_tdc1;
NTuple::Item<double> BesTofDigitizerV::m_adc1;

NTuple::Tuple* BesTofDigitizerV::m_tupleTof2 = 0;
NTuple::Item<double> BesTofDigitizerV::m_eTotal;
NTuple::Item<double> BesTofDigitizerV::m_nDigi;
NTuple::Item<double> BesTofDigitizerV::m_partIdMPV;
NTuple::Item<double> BesTofDigitizerV::m_scinNbMPV;
NTuple::Item<double> BesTofDigitizerV::m_edepMPV;
NTuple::Item<double> BesTofDigitizerV::m_nDigiOut;

NTuple::Tuple* BesTofDigitizerV::m_tupleTof3 = 0;
NTuple::Item<double> BesTofDigitizerV::m_forb;
NTuple::Item<double> BesTofDigitizerV::m_timeFlight;
NTuple::Item<double> BesTofDigitizerV::m_ddT;
NTuple::Item<double> BesTofDigitizerV::m_scinSwim;
NTuple::Item<double> BesTofDigitizerV::m_scinTime;
NTuple::Item<double> BesTofDigitizerV::m_transitTime;
NTuple::Item<double> BesTofDigitizerV::m_endTime;

NTuple::Item<double> BesTofDigitizerV::m_edepHit;


//NTuple::Tuple* BesTofDigitizerV::m_tupleTof4 = 0;//Write  out the basic information for the Mrpc Digitizer 
//NTuple::Item<double> BesTofDigitizerV::m_partIdmrpc;
//NTuple::Item<double> BesTofDigitizerV::m_stripidentifier;
//NTuple::Item<double> BesTofDigitizerV::m_trackindex;
//NTuple::Item<double> BesTofDigitizerV::m_signal_pc;
//NTuple::Item<double> BesTofDigitizerV::m_time_threshold;
//NTuple::Item<double> BesTofDigitizerV::m_time_1sthit;
//NTuple::Item<double> BesTofDigitizerV::m_time_1;
//NTuple::Item<double> BesTofDigitizerV::m_time_2;
//NTuple::Item<double> BesTofDigitizerV::m_firedstrip;
//NTuple::Item<double> BesTofDigitizerV::m_numberions;








BesTofDigitizerV::BesTofDigitizerV()
{
    IMessageSvc* msgSvc;
    Gaudi::svcLocator()->service("MessageSvc", msgSvc);
    MsgStream log(msgSvc, "BesTofDigitizerBrV1");

    ISvcLocator* svcLocator = Gaudi::svcLocator();
//    StatusCode scc = svcLocator->service("TofCaliSvc", m_tofCaliSvc);
//    if (scc ==  StatusCode::SUCCESS) {
//        log << MSG::INFO << "TofSim Gets TofCaliSvc Service Sucessfully!! " << endreq;
//    }
//    else {
//        log << MSG::ERROR << "TofSim Gets TofCaliSvc Service Failed !! " << endreq;
//    }
    
    StatusCode sim_scc = svcLocator->service("TofSimSvc", m_tofSimSvc);
    if (sim_scc ==  StatusCode::SUCCESS) {
        log << MSG::INFO << "TofSim Gets TofSimSvc Service Sucessfully!! " << endreq;
    }
    else {
        log << MSG::ERROR << "TofSim Gets TofSimSvc Service Failed !! " << endreq;
    }
    StatusCode qelec_scc = svcLocator->service("TofQElecSvc", m_tofQElecSvc);
    if (qelec_scc ==  StatusCode::SUCCESS) {
        log << MSG::INFO << "TofRec Gets tofQElecSvc Sucessfully!! " << endreq;
    }
    else {
        log << MSG::ERROR << "TofRec Gets tofQElecSvc Failed !! " << endreq;
    }
  

  
  //retrieve G4Svc
    IG4Svc* tmpSvc;
    StatusCode sc = svcLocator->service("G4Svc", tmpSvc);
    m_G4Svc = dynamic_cast<G4Svc *>(tmpSvc);

  

  //get Tof Ntuple from G4Svc
    if (m_G4Svc->TofRootFlag() && (!m_booked))
    {


      m_tupleTof1 = m_G4Svc->GetTupleTof1();

      sc = m_tupleTof1->addItem("partId",m_partId);
      sc = m_tupleTof1->addItem("scinNb",m_scinNb);
      sc = m_tupleTof1->addItem("edep",m_edep);
      sc = m_tupleTof1->addItem("nHits",m_nHits);
      sc = m_tupleTof1->addItem("time1st0",m_time1st0);
      sc = m_tupleTof1->addItem("time1st1",m_time1st1);
      sc = m_tupleTof1->addItem("timelast0",m_timelast0);
      sc = m_tupleTof1->addItem("timelast1",m_timelast1);
      sc = m_tupleTof1->addItem("totalPhot0",m_totalPhot0);
      sc = m_tupleTof1->addItem("totalPhot1",m_totalPhot1);
      sc = m_tupleTof1->addItem("NphAllSteps",m_NphAllSteps);
      sc = m_tupleTof1->addItem("max0",m_max0);
      sc = m_tupleTof1->addItem("max1",m_max1);
      sc = m_tupleTof1->addItem("tdc0",m_tdc0);
      sc = m_tupleTof1->addItem("adc0",m_adc0);
      sc = m_tupleTof1->addItem("tdc1",m_tdc1);
      sc = m_tupleTof1->addItem("adc1",m_adc1);

      

      m_tupleTof2 = m_G4Svc->GetTupleTof2();
      sc = m_tupleTof2->addItem("eTotal",m_eTotal);
      sc = m_tupleTof2->addItem("nDigi",m_nDigi);
      sc = m_tupleTof2->addItem("partIdMPV",m_partIdMPV);
      sc = m_tupleTof2->addItem("scinNbMPV",m_scinNbMPV);
      sc = m_tupleTof2->addItem("edepMPV",m_edepMPV);
      sc = m_tupleTof2->addItem("nDigiOut",m_nDigiOut);
      sc = m_tupleTof2->addItem("edepPerHit",m_edepHit);

      m_tupleTof3 = m_G4Svc->GetTupleTof3();
      //sc = m_tupleTof3->addItem("forb",m_forb);
      sc = m_tupleTof3->addItem("timeFlight",m_timeFlight);
      sc = m_tupleTof3->addItem("ddT",m_ddT);
      sc = m_tupleTof3->addItem("scinSwim",m_scinSwim);
      sc = m_tupleTof3->addItem("scinTime",m_scinTime);
      sc = m_tupleTof3->addItem("transitTime",m_transitTime);
      sc = m_tupleTof3->addItem("endTime",m_endTime);

      
      //m_tupleTof4 = m_G4Svc->GetTupleTof4();
      //sc = m_tupleTof4->addItem("partID",m_partIdmrpc);
      //sc = m_tupleTof4->addItem("stripidentifier",m_stripidentifier);
      //sc = m_tupleTof4->addItem("trackindex",m_trackindex);
      //sc = m_tupleTof4->addItem("signal_pc",m_signal_pc);
      //sc = m_tupleTof4->addItem("time_threshold",m_time_threshold);
      //sc = m_tupleTof4->addItem("time_1sthit",m_time_1sthit);
      //sc = m_tupleTof4->addItem("time_1",m_time_1);
      //sc = m_tupleTof4->addItem("time_2",m_time_2);
      //sc = m_tupleTof4->addItem("firedstrip",m_firedstrip);
      //sc = m_tupleTof4->addItem("numberions",m_numberions);


      m_booked = true;
    }

}

BesTofDigitizerV::~BesTofDigitizerV()
{;}

void BesTofDigitizerV::Initialize()
{
    for (G4int i=0;i<2;i++)
    {
        m_ADC[i] = -999;
        m_TDC[i] = -999;
    }
    m_trackIndex = -999;
    m_globalTime = 9999;
}

