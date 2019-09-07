#include "BesEventAction.hh"
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"

#include "G4DigiManager.hh"
#include "BesMdcDigitizer.hh"
#include "BesTofDigitizer.hh"
#include "BesMucDigitizer.hh"
#include "BesEmcDigitizer.hh"

#include "BesRunAction.hh"
#include "G4RunManager.hh"
#include "BesAsciiIO.hh"
#include "BesMdcHit.hh"
#include "BesRootIO.hh"
#include "BesTDSWriter.hh"
#include "BesTuningIO.hh"

#include "BesSensitiveManager.hh"
#include "BesTruthTrack.hh"
#include "BesTruthVertex.hh"
#include <fstream>

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"

using namespace std;

#include "ReadBoostRoot.hh"
#include "G4UImanager.hh"

BesEventAction::BesEventAction(BesRunAction* runAction)
        :m_runAction(runAction)
{
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IRealizationSvc *tmpReal;
    StatusCode sc = svcLocator->service("RealizationSvc",tmpReal);
    if (!sc.isSuccess())
    {
        std::cout << " Could not initialize Realization Service in BesEventAction" << std::endl;
    } else {
        m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
    }

    m_DM = G4DigiManager::GetDMpointer();

    if (ReadBoostRoot::GetMdc())
    {
        BesMdcDigitizer* mdcDM = new BesMdcDigitizer("BesMdcDigitizer");
        m_DM->AddNewModule(mdcDM);
    }
    if (ReadBoostRoot::GetTof())
    {
        BesTofDigitizer* tofDM = new BesTofDigitizer("BesTofDigitizer");
        m_DM->AddNewModule(tofDM);
    }
    if (ReadBoostRoot::GetEmc())
    {
        BesEmcDigitizer* emcDM = new BesEmcDigitizer("BesEmcDigitizer");
        m_DM->AddNewModule(emcDM);
    }
    if (ReadBoostRoot::GetMuc())
    {
        BesMucDigitizer* mucDM = new BesMucDigitizer("BesMucDigitizer");
        m_DM->AddNewModule(mucDM);
    }

}

BesEventAction::~BesEventAction()
{
  delete m_DM;
}

void BesEventAction::BeginOfEventAction(const G4Event* evt)
{

    G4int eventId = evt->GetEventID();
    //if (eventId%5000==0)
    //    G4cout<<"---> Begin of event: "<<eventId<<G4endl;

    G4UImanager* uiMgr = G4UImanager::GetUIpointer();
    //if(eventId == 46 || eventId == 75)
    // uiMgr->ApplyCommand("/tracking/verbose 1");
    //else
    //  uiMgr->ApplyCommand("/tracking/verbose 0");

    //mc truth
    if (m_runAction)
        if (m_runAction->GetMCTruthFlag()!=0)
        {
            BesSensitiveManager* sensitiveManager = BesSensitiveManager::GetSensitiveManager();
            sensitiveManager->BeginOfTruthEvent(evt);
        }
}

void BesEventAction::EndOfEventAction(const G4Event* evt)
{
    if (G4VVisManager::GetConcreteInstance())
    {
        G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
        G4int n_trajectories = 0;
        if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

        for (G4int i=0; i<n_trajectories; i++)
        {
            G4Trajectory* trj = (G4Trajectory*)
                                ((*(evt->GetTrajectoryContainer()))[i]);

            trj->DrawTrajectory(50);

        }
    }

    //mc truth
    BesSensitiveManager* sensitiveManager = BesSensitiveManager::GetSensitiveManager();
    if (m_runAction)
        if (m_runAction->GetMCTruthFlag()!=0)
        {
            sensitiveManager->EndOfTruthEvent(evt);
        }

    if (ReadBoostRoot::GetHitOut()){
        if (m_runAction)
        {
            //Ascii I/O, output hit collections
            BesAsciiIO* asciiIO = m_runAction->GetAsciiIO();
            if (asciiIO) {
                if (m_RealizationSvc->UseDBFlag() == true) asciiIO->SaveHitAsciiEvents(m_RealizationSvc->getRunId(), evt);
                else asciiIO->SaveHitAsciiEvents(m_runAction->GetRunId(), evt);
            }

            //Root I/O,  output MdcHit collections
            BesRootIO* rootIO = m_runAction->GetRootIO();
            if (rootIO){
                //emc digitization
                if (ReadBoostRoot::GetEmc()) m_DM->Digitize("BesEmcDigitizer");

                if (m_RealizationSvc->UseDBFlag() == true) rootIO->SaveHitRootEvent(m_RealizationSvc->getRunId(), evt);
                else rootIO->SaveHitRootEvent(m_runAction->GetRunId(), evt);
            }
        }
    }else{
        //Tuning I/O
        if (ReadBoostRoot::GetTuning()){
            if (m_runAction)
            {
                BesTuningIO* tuningIO = m_runAction->GetTuningIO();
                if (tuningIO){
                    if (ReadBoostRoot::GetFormatAR())
                    {
                        tuningIO->GetRootEvent(evt->GetEventID());
                    }
                    else{
                        tuningIO->GetNextEvents();
                    }
                }

            }
        }
        //mdc digitization

        if (ReadBoostRoot::GetMdc())
            m_DM->Digitize("BesMdcDigitizer");

        //tof digitization
        if (ReadBoostRoot::GetTof())
            m_DM->Digitize("BesTofDigitizer");

        //emc digitization
        if ((!ReadBoostRoot::GetTuning())&&ReadBoostRoot::GetEmc())
            m_DM->Digitize("BesEmcDigitizer");

        //muc digitization
        if (ReadBoostRoot::GetMuc())
            m_DM->Digitize("BesMucDigitizer");


        if (m_runAction)
        {
        //Ascii I/O, output digi collections
            BesAsciiIO* asciiIO = m_runAction->GetAsciiIO();
            if (asciiIO)
            {
                if (m_RealizationSvc->UseDBFlag() == true) asciiIO->SaveAsciiEvents(m_RealizationSvc->getRunId(), evt);
                else asciiIO->SaveAsciiEvents(m_runAction->GetRunId(), evt);
            }
              
        // Root I/O,output digi collections
            BesTDSWriter* tdsWriter =  m_runAction->GetTDSWriter();
            if (tdsWriter)
            {
                if (m_RealizationSvc->UseDBFlag() == true) tdsWriter->SaveAll(evt, m_RealizationSvc->getRunId() );
                else tdsWriter->SaveAll(evt, m_runAction->GetRunId() );
            }
        }
    }

    sensitiveManager->ClearEvent();
    //HepRandom::saveEngineStatus("EndSimCurrEvt.rndm");
}













