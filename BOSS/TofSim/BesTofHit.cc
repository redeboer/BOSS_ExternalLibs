//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofHit.cc

#include "BesTofHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"

G4Allocator<BesTofHit> BesTofHitAllocator;

BesTofHit::BesTofHit() {}

BesTofHit::~BesTofHit() {}

BesTofHit::BesTofHit(const BesTofHit& right)
        :G4VHit()
{
    m_trackIndex=right.m_trackIndex;
    m_g4Index=right.m_g4Index;
    m_partId=right.m_partId;
    m_scinNb=right.m_scinNb;
    m_gapNb=right.m_gapNb;
    m_edep=right.m_edep;
    m_stepL=right.m_stepL;
    m_trackL=right.m_trackL;
    m_pos=right.m_pos;
    m_time=right.m_time;
    m_deltaT=right.m_deltaT;
    m_pDirection=right.m_pDirection;
    m_momentum=right.m_momentum;
    //m_scinMaterial = right.m_scinMaterial;
    m_charge = right.m_charge;
    m_number_of_ions=right.m_number_of_ions;
    m_strip = right.m_strip;
    m_locPos = right.m_locPos;
}


const BesTofHit& BesTofHit::operator=(const BesTofHit& right)
{
    m_trackIndex=right.m_trackIndex;
    m_g4Index=right.m_g4Index;
    m_partId=right.m_partId;
    m_scinNb=right.m_scinNb;
    m_gapNb=right.m_gapNb;
    m_edep=right.m_edep;
    m_stepL=right.m_stepL;
    m_trackL=right.m_trackL;
    m_pos=right.m_pos;
    m_time=right.m_time;
    m_deltaT=right.m_deltaT;
    m_pDirection=right.m_pDirection;
    m_momentum=right.m_momentum;
    //m_scinMaterial = right.m_scinMaterial;
    m_charge = right.m_charge;
    m_number_of_ions = right.m_number_of_ions;
    m_strip = right.m_strip;
    m_locPos = right.m_locPos;

    return *this;
}



int BesTofHit::operator==(const BesTofHit& right) const
{
    return (this==&right) ? 1 : 0;
}

void BesTofHit::AddHit(const BesTofHit& newHit)
{
    m_edep += newHit.m_edep;
    if (m_edep>newHit.m_edep)
    {
        m_time = newHit.m_time;
        m_pos = newHit.m_pos;
    }
}

void BesTofHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if (pVVisManager)
    {
        G4Circle circle(m_pos);
        circle.SetScreenSize(1.);
        circle.SetFillStyle(G4Circle::filled);
        G4Colour colour(0.,0.,1.);
        G4VisAttributes attribs(colour);
        circle.SetVisAttributes(attribs);
        pVVisManager->Draw(circle);
    }
}


void BesTofHit::Print()
{
    G4cout<<m_trackIndex<<" "<<m_g4Index<<" "<<m_partId<<" "<<m_scinNb<<" ";
    G4double posx = m_pos.x();
    G4double posy = m_pos.y();
    G4double posr = sqrt(posx*posx + posy*posy);
    G4cout<<posr<<" "<<m_edep<<" "<<m_stepL<<G4endl<<G4endl;
}


