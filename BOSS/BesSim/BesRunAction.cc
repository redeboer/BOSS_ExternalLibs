//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: class for BesRunAction (simple)
//Author: Liuhm
//Created: May 25, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#include "BesRunAction.hh"
#include "G4Run.hh"
//#include "BesRunActionMessenger.hh"
#include "BesAsciiIO.hh"
#include "BesRootIO.hh"
#include "BesTDSWriter.hh"
#include "BesTuningIO.hh"

#include "ReadBoostRoot.hh"

BesRunAction::BesRunAction()
        :m_runId(0), m_MCTruthFlag(1),
        m_asciiFlag(""),m_asciiFile(""),m_asciiIO(0),m_rootFlag(true),m_rootFile(""),m_rootIO(0),
        m_tuningIO(0),m_TDSFlag(false),m_TDSWriter(0)
{
    //m_runMessenger = new BesRunActionMessenger(this);
}

BesRunAction::~BesRunAction()
{
    //delete m_runMessenger;
}

void BesRunAction::BeginOfRunAction(const G4Run* aRun)
{
    //set run Id, use old BES convention, -9 as default
    //aRun->SetRunID(runNumber);
    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
    m_runId= aRun->GetRunID();

    G4int flag[8];
    G4int allflag=0;
    for (G4int i=0;i<8;i++)
    {
        flag[i]=m_asciiFlag[i]-'0';
        allflag += flag[i];
    }
    if (allflag)
        m_asciiIO = new BesAsciiIO(flag[0],flag[1],flag[2],flag[3],flag[4],flag[5],flag[6],flag[7],m_asciiFile);

    if (m_rootFlag)
        m_rootIO = new BesRootIO(1,1,1,1,1,1,1,1,m_rootFile);

    if (m_TDSFlag)
        m_TDSWriter = new BesTDSWriter;

    if (ReadBoostRoot::GetTuning()){
      std::vector<std::string>::iterator pd;
       for (pd = m_tuningFile.begin(); pd != m_tuningFile.end(); pd++){
          std::cout << "BesRunAction tuning File: " << *pd << std::endl;
       }

        m_tuningIO = new BesTuningIO(m_tuningFile);
    }
}


void BesRunAction::EndOfRunAction(const G4Run* )
{
    if (m_asciiIO)
        delete m_asciiIO;
    if (m_rootIO)
        delete m_rootIO;
    if (m_TDSFlag)
        delete m_TDSWriter;
    if (m_tuningIO)
        delete m_tuningIO;
}
