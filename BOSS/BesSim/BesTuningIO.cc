//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description:
////Author:   Yuan Ye
////Created:  Dec, 2007
////Modified:
////Comment:
//
#include "RootEventData/TMcHitTof.h"
#include "RootEventData/TMcHitEvent.h"
#include "RootEventData/TMcHitMdc.h"
#include "RootEventData/TMcDigiEmc.h"
#include "BesMdcHit.hh"
#include "BesTofHit.hh"
#include "BesEmcHit.hh"
#include "BesEmcDigi.hh"
#include "BesMucHit.hh"
#include "ReadBoostRoot.hh"
#include "BesTuningIO.hh"
#include "AsciiDmp/AsciiData.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ThreeVector.hh"

#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"   

#include "ReadBoostRoot.hh"
#include "TVector3.h"

using namespace std;
BesTuningIO::BesTuningIO(std::vector<std::string> name)
        :m_tuningFile(name),m_evt(0)
{
    m_DigiMan = G4DigiManager::GetDMpointer();
    m_inputFileStream = new std::ifstream();

    if (ReadBoostRoot::GetFormatAR()){
        //tuning  input root file
        //TFile *f = new TFile(m_tuningFile);
        //HitTree = (TTree*)f->Get("HitTree");
        //m_TMcHitEvent = new TMcHitEvent();
	//TBranch *branch = HitTree->GetBranch("TMcHitEvent");
	// begin change from TTree to TChain
	HitChain = new TChain("HitTree");
	if (m_tuningFile.size()==0){
	  std::cout << "there is no tuning file" << std::endl;
	}
	std::cout << "file number: " << m_tuningFile.size() << std::endl;
	for (int i = 0 ; i < m_tuningFile.size(); i++){
	  
	  //std::cout << "________________________________________________________________________________fileName : " << m_tuningFile[i] << std::endl;
	  //HitChain->Add(&m_tuningFile[i]);
	  HitChain->Add(m_tuningFile[i].c_str());
        }
        m_TMcHitEvent = new TMcHitEvent();
	TBranch *branch = HitChain->GetBranch("TMcHitEvent");
	
	// end   change from TTree to TChain
        branch->SetAddress(&m_TMcHitEvent);
	std::cout << "HitChain entries: " << HitChain->GetEntries() << std::endl;
    }
    else{// tuning input ascii file
        //m_inputFileStream->open(m_tuningFile.c_str());
        //if ((*m_inputFileStream).good()) {
        //    try {
        //        (*m_inputFileStream) >> m_version;
        //    } catch (AsciiDumpException& ) {
        //        std::cerr << "BesTuningIO::Got AsciiDumpException eror while reading VERSION block !!!" << std::endl;
        //    }
        //}else{
        //    std::cerr << "BesTuningIO::Open tuning input file error!!!" << std::endl;
        //}
    }
}

BesTuningIO::~BesTuningIO(){
    if (m_inputFileStream)  delete m_inputFileStream;
    if (m_evt) delete m_evt;
}

void BesTuningIO::GetNextEvents(){
    if (m_evt) delete m_evt;

    m_evt = new HitEVENT;
    try {
        (*m_inputFileStream) >> *m_evt;
    } catch (AsciiWrongTag& ex) {
        std::cerr << "wrong tag, got " << ex.got()
        << " expected: " << ex.expected()
        << std::endl;
        delete m_evt;
        m_evt=0;
        return;
    } catch (AsciiDumpException&) {
        std::cerr<<"BesTuningIO: Reach file end!"<<std::endl;
        delete m_evt;
        m_evt=0;
        return;
    }

    if (ReadBoostRoot::GetMdc())GetMdcHits();

    if (ReadBoostRoot::GetTof())GetTofHits();

    if (ReadBoostRoot::GetEmc())GetEmcDigi();

    if (ReadBoostRoot::GetMuc())GetMucHits();
}

void BesTuningIO::GetMdcHits(){
    G4int mdcHitCollID = -1;
    mdcHitCollID = m_DigiMan->GetHitsCollectionID("BesMdcHitsCollection");
    if (mdcHitCollID>=0){
        BesMdcHitsCollection* mdcDC = (BesMdcHitsCollection*)m_DigiMan->GetHitsCollection(mdcHitCollID);
        if (mdcDC){
            G4int nHit = mdcDC->entries();
            if (nHit>0){
                for (G4int i=0;i<nHit;i++)
                {
                    delete (*mdcDC)[i];
                }
                mdcDC->GetVector()->clear();
            }

            std::vector<MdcHitType>::iterator iter;
            iter = (m_evt->mdcHit).hitCol.begin();
            // Loop over mdc hits
            for (; iter != (m_evt->mdcHit).hitCol.end(); iter++) {
                BesMdcHit* newHit = new BesMdcHit();
                newHit->SetTrackID ((*iter).trackIndex);
                newHit->SetLayerNo ((*iter).layerNo);
                newHit->SetCellNo ((*iter).cellNo);
                newHit->SetEdep ((*iter).energyDeposit);
                newHit->SetPos (G4ThreeVector((*iter).posX,(*iter).posY,(*iter).posZ));
                newHit->SetDriftD ((*iter).driftDistance);
                newHit->SetTheta((*iter).theta);
                newHit->SetPosFlag((*iter).posFlag);
                newHit->SetEnterAngle((*iter).enterAngle);
                newHit->SetDriftT (0.);
                newHit->SetGlobalT((*iter).globalT);
                mdcDC->insert(newHit);

            }
            //mdcDC->PrintAllHits();
        }else{
            std::cerr << "BesTuningIO::can't get mdcHitsCollection"<<std::endl;
        }
    }else{
        std::cerr << "BesTuningIO::can't get mdcHitCollID"<<std::endl;
    }
}

void BesTuningIO::GetEmcDigi(){
  G4int THCID = -1;
  THCID = m_DigiMan->GetDigiCollectionID("BesEmcDigitsCollection");
  if (THCID>=0) {
    BesEmcDigitsCollection* emcDC = new BesEmcDigitsCollection("BesEmcDigitizer","BesEmcDigitsCollection");
   m_DigiMan->SetDigiCollection(THCID,emcDC);
  }
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//Below use root format tuning not ascii format
void BesTuningIO::GetRootEvent(int evtID){
    //std::cout << "evtID: " << evtID << std::endl;
    //HitTree->GetEntry(evtID);
    HitChain->GetEntry(evtID);
    //std::cout << "HitChain" << std::endl;
    if (ReadBoostRoot::GetMdc())GetMdcRootHits();
    if (ReadBoostRoot::GetTof())GetTofRootHits();
    if (ReadBoostRoot::GetEmc())GetEmcRootDigi();
    if (ReadBoostRoot::GetMuc())GetMucHits();
}


void BesTuningIO::GetEmcRootDigi(){
  G4int THCID = -1;
  THCID = m_DigiMan->GetDigiCollectionID("BesEmcDigitsCollection");
  //cout << "THCID " << THCID << endl; 
  if (THCID>=0) {
    BesEmcDigitsCollection* emcDC = new BesEmcDigitsCollection("BesEmcDigitizer","BesEmcDigitsCollection");

  //std::cout << "GetEmcRootDigi "  << emcDC << std::endl;
    if(emcDC){
      int nHit = emcDC->entries();
      //std::cout << "nHit: " << nHit << std::endl;
      if(nHit > 0){
        for(int i = 0; i < nHit; i++){
          delete (*emcDC)[i];
        }
	emcDC->GetVector()->clear();
      }
    }
    
   int nHits = m_TMcHitEvent->getMcDigiEmcCol()->GetEntries();
   //std::cout << "nHits: " << nHits << std::endl;
   for(int i = 0; i < nHits; i++){
     m_TMcDigiEmc = m_TMcHitEvent->getMcDigiEmc(i);
       
     BesEmcDigi* emcDigi = new BesEmcDigi();
     
     emcDigi->SetPartId(m_TMcDigiEmc->GetPartId());    
     emcDigi->SetThetaNb(m_TMcDigiEmc->GetThetaNb());
     emcDigi->SetPhiNb(m_TMcDigiEmc->GetPhiNb());   
     emcDigi->SetEnergy(m_TMcDigiEmc->GetEnergy());
     emcDigi->SetTime(m_TMcDigiEmc->GetTime());
     emcDigi->SetTrackIndex(m_TMcDigiEmc->GetTrackIndex());
     //emcDigi->Print();
     //std::cout << "SetEnergy" <<  emcDigi->GetEnergy() << std::endl;

     emcDC->insert(emcDigi);
     delete m_TMcDigiEmc;
 
   }

   //std::cout << "insert" << std::endl;
   m_DigiMan->SetDigiCollection(THCID,emcDC);


  }
 
}

void BesTuningIO::GetMdcRootHits(){

  G4int THCID = -1;
  THCID = m_DigiMan->GetHitsCollectionID("BesMdcHitsCollection");
  if (THCID>=0) {
    BesMdcHitsCollection* mdcDC = (BesMdcHitsCollection*) (m_DigiMan->GetHitsCollection(THCID));
    if(mdcDC){
      int nHit = mdcDC->entries();
      if(nHit > 0){
        for(int i = 0; i < nHit; i++){
          delete (*mdcDC)[i];
        }
	mdcDC->GetVector()->clear();
      }
    }
    
   int nHits = m_TMcHitEvent->getMcHitMdcCol()->GetEntries();
   //std::cout << "nHits: " << nHits << std::endl;
   for(int i = 0; i < nHits; i++){
     m_TMcHitMdc = m_TMcHitEvent->getMcHitMdc(i);
       
     BesMdcHit* mdcHit = new BesMdcHit();
     
     mdcHit->SetTrackID(m_TMcHitMdc->GetTrackID());       
     mdcHit->SetLayerNo(m_TMcHitMdc->GetLayerNo()); 
     mdcHit->SetCellNo(m_TMcHitMdc->GetCellNo());
     mdcHit->SetEdep(m_TMcHitMdc->GetEdep()); 
     mdcHit->SetDriftD(m_TMcHitMdc->GetDriftD());
     mdcHit->SetDriftT(m_TMcHitMdc->GetDriftT());
     mdcHit->SetGlobalT(m_TMcHitMdc->GetGlobalT());
     mdcHit->SetTheta(m_TMcHitMdc->GetTheta());
     mdcHit->SetEnterAngle(m_TMcHitMdc->GetEnterAngle());
     mdcHit->SetPosFlag(m_TMcHitMdc->GetPosFlag());

     TVector3 tTemp = m_TMcHitMdc->GetPos();
     G4ThreeVector gTemp = G4ThreeVector(tTemp.X(), tTemp.Y(), tTemp.Z());
     mdcHit->SetPos(gTemp);
     //mdcHit->Print(); 

     mdcDC->insert(mdcHit);
     delete  m_TMcHitMdc;
 
   }


  }
 
}

void BesTuningIO::GetTofRootHits(){

  //retrieve G4Svc
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IG4Svc* tmpSvc;
  StatusCode sc = svcLocator->service("G4Svc", tmpSvc);
  G4Svc* m_G4Svc = dynamic_cast<G4Svc *>(tmpSvc);

  const double m_beamTime = m_TMcHitEvent->getBeamTime();
  m_G4Svc->SetBeamTime(m_beamTime);
  //std::cout << "beamtime: " << m_beamTime << std::endl;

  G4int THCID = -1;
  THCID = m_DigiMan->GetHitsCollectionID("BesTofHitsCollection");
  if (THCID>=0) {
    BesTofHitsCollection* tofDC = (BesTofHitsCollection*) (m_DigiMan->GetHitsCollection(THCID));
    if(tofDC){
      int nHit = tofDC->entries();
      if(nHit > 0){
        for(int i = 0; i < nHit; i++){
          delete (*tofDC)[i];
        }
	tofDC->GetVector()->clear();
      }
    }
    
   int nHits = m_TMcHitEvent->getMcHitTofCol()->GetEntries();
   //std::cout << "nHits: " << nHits << std::endl;
   for(int i = 0; i < nHits; i++){
     m_TMcHitTof = m_TMcHitEvent->getMcHitTof(i);
       
     BesTofHit* tofHit = new BesTofHit();

     tofHit->SetTrackIndex(m_TMcHitTof->GetTrackIndex());
     tofHit->SetG4Index(m_TMcHitTof->GetG4Index());
     tofHit->SetPartId(m_TMcHitTof->GetPartId());
     tofHit->SetScinNb(m_TMcHitTof->GetScinNb());
     tofHit->SetEdep(m_TMcHitTof->GetEdep());
     tofHit->SetStepL(m_TMcHitTof->GetStepL());
     tofHit->SetTrackL(m_TMcHitTof->GetTrackL());
     tofHit->SetTime(m_TMcHitTof->GetTime());
     tofHit->SetDeltaT(m_TMcHitTof->GetDeltaT());
     tofHit->SetCharge(m_TMcHitTof->GetCharge());
     
     TVector3 tTemp = m_TMcHitTof->GetPos();
     G4ThreeVector gTemp(tTemp.X(), tTemp.Y(), tTemp.Z());
     tofHit->SetPos(gTemp);

     tTemp = m_TMcHitTof->GetPDirection();
     gTemp = G4ThreeVector(tTemp.X(), tTemp.Y(), tTemp.Z());
     tofHit->SetPDirection(gTemp);

     tTemp = m_TMcHitTof->GetMomentum();
     gTemp = G4ThreeVector(tTemp.X(), tTemp.Y(), tTemp.Z());
     tofHit->SetMomentum(gTemp);
     
     //tofHit->Print(); 

     tofDC->insert(tofHit);
     delete  m_TMcHitTof;
 
   }

   //std::cout << "tofDC: " <<  tofDC->entries() << std::endl;

  }
  
}
