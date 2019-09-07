//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$Id: BesTofDigitizerEcV1.cc

#include "BesTofDigitizerEcV1.hh"
#include "BesTofDigi.hh"
#include "BesTofHit.hh"
#include "G4DigiManager.hh"
#include "BesTofGeoParameter.hh"
#include "Randomize.hh"
#include "TMath.h"
#include <math.h>

BesTofDigitizerEcV1::BesTofDigitizerEcV1()
{
    BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();
    m_bucketPosR = tofPara->GetBucketPosR();  // 445 ???
}

BesTofDigitizerEcV1::~BesTofDigitizerEcV1()
{
}

void BesTofDigitizerEcV1::Digitize(ScintSingle* scint, BesTofDigitsCollection* DC)
{
    G4cout<<"BesTofDigitizerEcV1::Digitize"<<G4endl;
    m_besTofDigitsCollection = DC;
    G4DigiManager* digiManager = G4DigiManager::GetDMpointer();
    G4int THCID = digiManager->GetHitsCollectionID("BesTofHitsCollection");
    m_THC = (BesTofHitsCollection*) (digiManager->GetHitsCollection(THCID));
    if (m_THC)
    {
        G4int partId, scinNb, nHits;
        BesTofHit* hit;
        partId=scint->GetPartId();
        scinNb=scint->GetScinNb();
        nHits=scint->GetHitIndexes()->size();
        TofPmtInit();
        for (G4int j=0;j<nHits;j++)
        {
            hit= (*m_THC)[( *(scint->GetHitIndexes()) )[j]];
            TofPmtAccum(hit);
        }

        Smear(scinNb);
        if ( m_TDC[0]>0 )
        {
            BesTofDigi* digi = new BesTofDigi;
            digi->SetTrackIndex(m_trackIndex);
            digi->SetPartId(partId);
            digi->SetScinNb(scinNb);
            digi->SetForwADC( m_ADC[0]) ;
            digi->SetForwTDC( m_TDC[0]) ;
            digi->SetBackADC( m_ADC[1]) ;
            digi->SetBackTDC( m_TDC[1]) ;
            m_besTofDigitsCollection->insert(digi);
        }
    }
}

void BesTofDigitizerEcV1::TofPmtInit()
{
    Initialize();
    m_t1st = 9999.;
}

void BesTofDigitizerEcV1::TofPmtAccum(BesTofHit* hit)
{
    G4int trackIndex = hit->GetTrackIndex();
    G4int scinNb = hit->GetScinNb();
    G4double time = hit->GetTime();
    if (time<m_globalTime)
    {
        m_globalTime = time;
        m_trackIndex = trackIndex;
    }
    G4double edep = hit->GetEdep();
    G4ThreeVector pos = hit->GetPos();
    G4double posx = pos.x();
    G4double posy = pos.y();
    G4double pathL=abs(m_bucketPosR-sqrt(posx*posx+posy*posy));
    G4double atten;
    atten = m_tofCaliSvc->EAtten(scinNb);
    m_ADC[0] += edep*exp(-pathL/atten);

    if (time<m_t1st)
    {
        m_t1st = time;
        m_r = sqrt(posx*posx+posy*posy);
        m_TDC[0] = m_t1st;
    }
}

void BesTofDigitizerEcV1::Smear(G4int scinNb)
{
    /*G4double tofRes = 0.08;
    for(G4int i=0;i<2;i++)
    {
      m_TDC[i] += tofRes * G4RandGauss::shoot();
    }*/

    double pp[8];
    for (int i=0;i<8;i++)
    {
        pp[i]=m_tofCaliSvc->ETof(scinNb)->getP(i);
    }
    m_ADC[0] *= 7.;
    m_TDC[0] += (pp[0]+pp[1]*m_r)/TMath::Sqrt(m_ADC[0])+
                pp[2]/m_ADC[0]+
                pp[3]*m_r/m_ADC[0]+
                pp[4]*m_r+
                pp[5]*m_r*m_r+
                pp[6]*m_r*m_r*m_r+
                pp[7];
}
