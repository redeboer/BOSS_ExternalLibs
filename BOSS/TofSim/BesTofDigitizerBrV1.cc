//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$Id: BesTofDigitizerBrV1.cc

#include "BesTofDigitizerBrV1.hh"
#include "BesTofDigi.hh"
#include "BesTofHit.hh"
#include "G4DigiManager.hh"
#include "BesTofGeoParameter.hh"
#include "Randomize.hh"
#include <TMath.h>
#include <math.h>

BesTofDigitizerBrV1::BesTofDigitizerBrV1()
{
    BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();
    m_scinLength = tofPara->GetBr1L();
}

BesTofDigitizerBrV1::~BesTofDigitizerBrV1()
{
}

void BesTofDigitizerBrV1::Digitize(ScintSingle* scint, BesTofDigitsCollection* DC)
{
    G4cout<<"BesTofDigitizerBrV1::Digitize"<<G4endl;
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

void BesTofDigitizerBrV1::TofPmtInit()
{
    Initialize();
    m_t1st = 9999.;
}

void BesTofDigitizerBrV1::TofPmtAccum(BesTofHit* hit)
{
    G4int trackIndex = hit->GetTrackIndex();
    G4int partId = hit->GetPartId();
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
    G4double posz = pos.z();

    G4double atten;
    atten = m_tofCaliSvc->BAtten(scinNb);
    G4double pathL[2];
    pathL[0]=m_scinLength/2-posz;
    pathL[1]=posz+m_scinLength/2;
    for (G4int j=0;j<2;j++)
    {
        //G4cout<<"atten:"<<atten<<G4endl;
        m_ADC[j] += edep*exp(-pathL[j]/atten);
    }
    if (time<m_t1st)
    {
        m_t1st = time;
        m_z = posz;
        m_TDC[0] = m_TDC[1] = m_t1st;
    }
}

void BesTofDigitizerBrV1::Smear(G4int scinNb)
{
    G4cout<<"m_t1st:"<<m_t1st<<"  m_z:"<<m_z<<G4endl;

    /*G4double tofRes = 0.08;
    G4double atten;
    for(G4int i=0;i<2;i++)
    {
      m_TDC[i] += tofRes * G4RandGauss::shoot();
    }*/

    double pp1[10];
    double pp2[10];
    for (int i=0;i<10;i++)
    {
        pp1[i]=m_tofCaliSvc->BTof(scinNb)->getP1(i);
        pp2[i]=m_tofCaliSvc->BTof(scinNb)->getP2(i);
        //G4cout<<"pp1["<<i<<"]:"<<pp1[i]<<"  "<<"pp2["<<i<<"]:"<<pp2[i]<<G4endl;
    }
    double pp[10];
    for (int i=0;i<2;i++)
    {
        m_ADC[i] *= 7.;
        for (int j=0;j<10;j++)
        {
            if (i==0)
                pp[j]=pp1[j];
            else
                pp[j]=pp2[j];
        }
        m_TDC[i] += (pp[0]+pp[1]*m_z)/TMath::Sqrt(m_ADC[i])+
                    pp[2]*m_ADC[i]+
                    pp[3]*m_ADC[i]*m_ADC[i]+
                    pp[4]*m_ADC[i]*m_ADC[i]*m_ADC[i]+
                    pp[5]/(84.2*84.2+m_z*m_z)+
                    pp[6]*m_z+
                    pp[7]*m_z*m_z+
                    pp[8]*m_z*m_z*m_z+
                    pp[9];
    }
}
