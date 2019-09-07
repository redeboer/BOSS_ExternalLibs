#include "RootEventData/TMcHitTof.h"
#include "RootEventData/TMcHitEvent.h"
#include "RootEventData/TMcDigiEmc.h"
#include "BesMdcHit.hh"
#include "BesTofHit.hh"
#include "BesEmcDigi.hh"
#include "BesEventAction.hh"
#include "G4RunManager.hh"
#include "BesTruthTrack.hh"
#include "BesTruthVertex.hh"
#include "BesSensitiveManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

#include "TFile.h"
#include "TTree.h"
#include "BesRootIO.hh"

#include "G4ThreeVector.hh"

#include "AsciiDmp/AsciiData.hh"
#include "G4DigiManager.hh"
#include <iostream>
//#include <vector>
using namespace std;
//using std::vector;

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"   

BesRootIO::BesRootIO(
    G4int mdcTruFlag,G4int mdcDigiFlag,
    G4int tofTruFlag, G4int tofDigiFlag,
    G4int emcTruFlag, G4int emcDigiFlag,
    G4int mucTruFlag, G4int mucDigiFlag,G4String name)
        :m_mdcTruFlag(mdcTruFlag),m_mdcDigiFlag(mdcDigiFlag),
        m_tofTruFlag(tofTruFlag),m_tofDigiFlag(tofDigiFlag),
        m_emcTruFlag(emcTruFlag),m_emcDigiFlag(emcDigiFlag),
        m_mucTruFlag(mucTruFlag),m_mucDigiFlag(mucDigiFlag),m_rootFile(name)
{
    m_DigiMan = G4DigiManager::GetDMpointer();

    f = new TFile(m_rootFile,"RECREATE");
    //create a MdcHit TTree
    m_TMcHitEvent = new TMcHitEvent();
    HitTree = new TTree("HitTree","staff data from HitCollection");

    HitTree->Branch("TMcHitEvent", "TMcHitEvent", &m_TMcHitEvent, 32000, 99);
}

BesRootIO::~BesRootIO()
{
    f->Write();
}

void BesRootIO::SaveHitRootEvent(G4int runId, const G4Event* evt)
{
    //std::cout << "Save Hit Root Event" << std::endl;
    if (m_mdcDigiFlag)
    {
        SaveMdcHitRoot();
    }
    if (m_tofDigiFlag)
    {
        SaveTofHitRoot();
    }
    if (m_emcDigiFlag){
        SaveEmcDigiRootEvent();
    }

    HitTree->Fill();
    
    m_TMcHitEvent->Clear();
}

void BesRootIO::SaveEmcDigiRootEvent(){
  //std::cout << "SaveEmcDigiRootEvent" << std::endl;
  G4int emcDigiCollID = -1;
  emcDigiCollID = m_DigiMan->GetDigiCollectionID("BesEmcDigitsCollection");
  if(emcDigiCollID>=0)
  {
    BesEmcDigitsCollection* emcDC = (BesEmcDigitsCollection*)m_DigiMan->GetDigiCollection(emcDigiCollID);
    G4int nHits = emcDC->entries();
    //std::cout << "nHits: " << nHits << std::endl;

    if(nHits>0){
      BesEmcDigi* emcDigi;
      for (int i = 0; i < nHits; i++) {
        emcDigi=(*emcDC)[i];
        TMcDigiEmc* tEmcDigi = new TMcDigiEmc();

        tEmcDigi->SetPartId(emcDigi->GetPartId());    
        tEmcDigi->SetThetaNb(emcDigi->GetThetaNb());
        tEmcDigi->SetPhiNb(emcDigi->GetPhiNb());   
        tEmcDigi->SetEnergy(emcDigi->GetEnergy());
        tEmcDigi->SetTime(emcDigi->GetTime());
        tEmcDigi->SetTrackIndex(emcDigi->GetTrackIndex());
	//emcDigi->Print();
        //std::cout << "SetEnergy" <<  emcDigi->GetEnergy() << std::endl;

        m_TMcHitEvent->addMcDigiEmc(tEmcDigi);
       //std::cout << "addMcDigiEmc" << std::endl;
      }
    }
  }
}

void BesRootIO::SaveMdcHitRoot()
{   
  //std::cout << "SaveMdcHitRoot" << std::endl;
    G4int THCID = -1;
    THCID = m_DigiMan->GetHitsCollectionID("BesMdcHitsCollection");
    if (THCID>=0)
    {
        BesMdcHitsCollection* mdcDC = (BesMdcHitsCollection*) (m_DigiMan->GetHitsCollection(THCID));
        G4int nHits = mdcDC->entries();
	//std::cout << "nHits: " << nHits << std::endl;
        if (nHits>0)
        {
            BesMdcHit* mdcHit;
            for (int i=0;i<nHits;i++)
            {
                mdcHit=(*mdcDC)[i];
                TMcHitMdc* tMdcHit = new TMcHitMdc();
                tMdcHit->SetTrackID(mdcHit->GetTrackID());       
                tMdcHit->SetLayerNo(mdcHit->GetLayerNo()); 
                tMdcHit->SetCellNo(mdcHit->GetCellNo());
                tMdcHit->SetEdep(mdcHit->GetEdep()); 
                tMdcHit->SetDriftD(mdcHit->GetDriftD());
                tMdcHit->SetDriftT(mdcHit->GetDriftT());
                tMdcHit->SetGlobalT(mdcHit->GetGlobalT());
                tMdcHit->SetTheta(mdcHit->GetTheta());
                tMdcHit->SetEnterAngle(mdcHit->GetEnterAngle());
                tMdcHit->SetPosFlag(mdcHit->GetPosFlag());

                G4ThreeVector gTemp = mdcHit->GetPos();
                TVector3 tTemp = TVector3(gTemp.x(), gTemp.y(), gTemp.z());
                tMdcHit->SetPos(tTemp);

		//mdcHit->Print();
                m_TMcHitEvent->addMcHitMdc(tMdcHit);
            }
        }
    }


}

void BesRootIO::SaveTofHitRoot()
{
   //retrieve G4Svc
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IG4Svc* tmpSvc;
    StatusCode sc = svcLocator->service("G4Svc", tmpSvc);
    G4Svc*  m_G4Svc = dynamic_cast<G4Svc *>(tmpSvc);
    double m_beamTime = m_G4Svc->GetBeamTime() * ns;
    m_TMcHitEvent->setBeamTime(m_beamTime);
    //std::cout << "beamtime: " << m_beamTime << std::endl;

    G4int THCID = -1;
    THCID = m_DigiMan->GetHitsCollectionID("BesTofHitsCollection");
    if (THCID>=0)
    {
        BesTofHitsCollection* tofDC = (BesTofHitsCollection*) (m_DigiMan->GetHitsCollection(THCID));
        G4int nHits = tofDC->entries();
	//std::cout << "nHits: " << nHits << std::endl;
        if (nHits>0)
        {
            BesTofHit* tofHit;
            for (int i=0;i<nHits;i++)
            {
                tofHit=(*tofDC)[i];
                TMcHitTof* tTofHit = new TMcHitTof();
                tTofHit->SetTrackIndex(tofHit->GetTrackIndex());
                tTofHit->SetG4Index(tofHit->GetG4Index());
                tTofHit->SetPartId(tofHit->GetPartId());
                tTofHit->SetScinNb(tofHit->GetScinNb());
                tTofHit->SetEdep(tofHit->GetEdep());
                tTofHit->SetStepL(tofHit->GetStepL());
                tTofHit->SetTrackL(tofHit->GetTrackL());
                tTofHit->SetTime(tofHit->GetTime());
                tTofHit->SetDeltaT(tofHit->GetDeltaT());
                tTofHit->SetCharge(tofHit->GetCharge());

                G4ThreeVector gTemp = tofHit->GetPos();
                TVector3 tTemp(gTemp.x(), gTemp.y(), gTemp.z());
                tTofHit->SetPos(tTemp);

                gTemp = tofHit->GetPDirection();
                tTemp = TVector3(gTemp.x(), gTemp.y(), gTemp.z());
                tTofHit->SetPDirection(tTemp);

                gTemp = tofHit->GetMomentum();
                tTemp = TVector3(gTemp.x(), gTemp.y(), gTemp.z());
                tTofHit->SetMomentum(tTemp);

		//tofHit->Print();
                m_TMcHitEvent->addMcHitTof(tTofHit);
            }
        }
    }

}
