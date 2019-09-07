//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Youzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$Id: BesMucDigitizer.cc

#include "BesMucDigitizer.hh"
#include "BesMucDigit.hh"
#include "BesMucDigi.hh"
#include "BesMucHit.hh"
#include "G4DigiManager.hh"
#include "Randomize.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"


const G4int BesMucDigitizer::m_nPartMax;
const G4int BesMucDigitizer::m_nSegMax;
const G4int BesMucDigitizer::m_nGapMax;
const G4int BesMucDigitizer::m_nStripMax;

BesMucDigitizer::BesMucDigitizer(G4String modName)
:G4VDigitizerModule(modName)
{
  collectionName.push_back("BesMucDigisCollection");
  m_besMucDigisCollection=0;

  //Standard unit: length in mm; time in ns; energy in MeV; att in mm;
  //Here: att in mm; vel in m/s; threshold in MeV; mucRes in ps; 
  

  //retrieve G4Svc
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IG4Svc* tmpSvc;
  StatusCode sc=svcLocator->service("G4Svc", tmpSvc);
  m_G4Svc=dynamic_cast<G4Svc *>(tmpSvc);

  //get Muc Ntuple from G4Svc
  if(m_G4Svc->MucRootFlag())
  {
    m_tupleMuc = m_G4Svc->GetTupleMuc();
    sc = m_tupleMuc->addItem("part",m_part);
    sc = m_tupleMuc->addItem("seg",m_seg);
    sc = m_tupleMuc->addItem("gap",m_gap);
    sc = m_tupleMuc->addItem("strip",m_strip);

  }




  Initialize();  
}

BesMucDigitizer::~BesMucDigitizer()
{
}

void BesMucDigitizer::Initialize()
{
  for(G4int i = 0; i < m_nPartMax; i++) {
    for(G4int j = 0; j < m_nSegMax; j++) {
      for(G4int k = 0; k < m_nGapMax; k++) {
	for(G4int l = 0; l < m_nStripMax; l++) {
	  m_stripID[i][j][k][l] = 0;
	}
      }
    }
  }
}


void BesMucDigitizer::Digitize()
{
  Initialize();
  
  m_besMucDigisCollection = new BesMucDigisCollection(moduleName, collectionName[0]);
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  
  //hits collection ID
  G4int THCID;
  THCID = DigiMan->GetHitsCollectionID("BesMucHitsCollection");
  
  //hits collection
  BesMucHitsCollection* THC = 0;
  THC = (BesMucHitsCollection*) (DigiMan->GetHitsCollection(THCID));

  if(THC) {
    G4int trackIndex, part, seg, gap, strip;
    BesMucDigit *aDigit = new BesMucDigit();

    G4int n_hit = THC->entries();
    for(G4int i = 0; i < n_hit; i++) {
      aDigit->SetHit((*THC)[i]);
      trackIndex = aDigit->GetTrackIndex();
      part  = aDigit->GetPart();
      seg   = aDigit->GetSeg();
      gap   = aDigit->GetGap();
      strip = aDigit->GetNearestStripNo();
     
      //fill muc Ntuple
      if(m_G4Svc->MucRootFlag())
	{
	  m_part = part;
	  m_seg = seg;
	  m_gap = gap;
	  m_strip = strip;
	  
	  m_tupleMuc->write();
	}

      (*THC)[i]->SetPart(part);
      (*THC)[i]->SetSeg(seg);
      (*THC)[i]->SetGap(gap);
      (*THC)[i]->SetStrip(strip);
      if(m_stripID[part][seg][gap][strip] == 0) {
	NewDigi(trackIndex, part, seg, gap, strip);
      }
      else { 
	OldDigi();
      }
    } 
    //Smear();  
    StoreDigiCollection(m_besMucDigisCollection);	
    delete aDigit;
  }
}

void BesMucDigitizer::NewDigi(G4int trackIndex, G4int part, G4int seg, G4int gap, G4int strip)
{
  BesMucDigi *digi = new BesMucDigi();
  digi->SetTrackIndex(trackIndex);
  digi->SetPartId(part);
  digi->SetSegId(seg);
  digi->SetGapId(gap);
  digi->SetStripId(strip);
  m_besMucDigisCollection->insert(digi);    	

  m_stripID[part][seg][gap][strip] = 1;
}

void BesMucDigitizer::OldDigi()
{ }

void BesMucDigitizer::Smear()
{ }
