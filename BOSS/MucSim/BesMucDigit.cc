//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author:  Youzy      Peking University      mail: youzy@hep.pku.cn
//Created:  Dec, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

//
// $Id: BesMucDigit

#include "BesMucDigit.hh"
#include "G4ios.hh"
#include "G4Box.hh"
#include "G4VSolid.hh"
#include "strstream"
class G4VSolid;

BesMucDigit::BesMucDigit() 
{
}

BesMucDigit::~BesMucDigit() 
{
}

void BesMucDigit::SetHit(BesMucHit* hit)
{
  m_pHit = hit;
  
  G4int part = m_pHit->GetPart();
  G4int seg  = m_pHit->GetSeg();
  G4int gap  = m_pHit->GetGap();
  m_Pos = m_pHit->GetPosLocal().x();
  if ( (part == 1 && gap%2 != 0) || (part != 1 && gap%2 == 0) ) {
    m_Pos = m_pHit->GetPosLocal().y();
  }

  if(part == 1 && seg == 2 &&gap%2 != 0) m_Pos2 = m_pHit->GetPosLocal().x();

  G4VPhysicalVolume* pvGasChamber = m_pHit->GetVolume();
  if(m_pHit->GetTrackID()!=-1){
    m_lvGap     = pvGasChamber->GetMotherLogical();  //after update gdml, mother of gaschamber change to be aluminumbox, but i still use the name m_lvGap
  m_lvGapName = m_lvGap->GetName();

  //G4cout<<"in digit----"<<m_lvGapName<<"  "<<m_lvGap->GetDaughter(1)->GetLogicalVolume()->GetName()<<G4endl;
  }
}

G4VPhysicalVolume* 
BesMucDigit::GetNearestStrip() 
{
  m_pvStripDigit = GetStrip( GetNearestStripNo() );
  if(!m_pvStripDigit) {
    G4cerr << "Nearest Strip is not found." << G4endl;
  }

  return m_pvStripDigit;
}

G4int BesMucDigit::GetNearestStripNo() 
{
 if(m_pHit->GetTrackID()!=-1){
  G4float posStrip0 = GetStripPos( GetStrip(0) );
  G4float posStrip1 = GetStripPos( GetStrip(1) );
  G4float width = posStrip1 - posStrip0;
  //G4cout << "posStrip0 " << posStrip0 << " posStrip1 " << posStrip1 << G4endl;

  G4int iGuessStrip = G4int( (m_Pos - posStrip0)/width + 0.5 );
  // confine iGuessStrip to an extend before check IsNearest.
  if(iGuessStrip < 0) {
    iGuessStrip = 0;
  } 
  else if(iGuessStrip >= 112) {
    iGuessStrip = 112;
  }
  //G4cout << "m_pos=" << m_Pos << "firstly, I guess: " << iGuessStrip << G4endl;
  
  G4int step = IsNearest(iGuessStrip);
  while(step != 0) {
    //G4cout << "guess " << iGuessStrip << " wrong , iGuess " << step << G4endl;
    iGuessStrip += step;
    step = IsNearest(iGuessStrip);
  }
  //G4cout << " guess " << iGuessStrip << " right " << G4endl;

  //deal with barrel segment 2 gap%2!=0
  G4int part = m_pHit->GetPart();
  G4int seg  = m_pHit->GetSeg();
  G4int gap  = m_pHit->GetGap();
  //G4cout<<" in Digit   iGuessStrip = "<< iGuessStrip <<G4endl;
  if(part == 1 && seg == 2 &&gap%2 != 0 && iGuessStrip > 79 ){
    if(iGuessStrip > 79 && iGuessStrip <= 95 && m_Pos2 < 0) iGuessStrip += 16 ;
    if(iGuessStrip > 95 && iGuessStrip <= 111 && m_Pos2 > 0) iGuessStrip -= 16 ;
    //G4cout<<" in barrel segment 2 and gap%2!=0, and strip > 79  strip="<< iGuessStrip<<" posx="<<m_Pos2<<" "<<" posy="<<m_Pos<<G4endl;
  }

  return iGuessStrip;
 }
 else return m_pHit->GetStrip();
}

G4VPhysicalVolume* 
BesMucDigit::GetStrip(G4int i)
{
  // If no strip get, return 0.
  // If i > daughtersNo, there will be run error.
  G4LogicalVolume *lvStripPlane = m_lvGap->GetDaughter(0)->GetLogicalVolume();
  G4int daughtersNo = lvStripPlane->GetNoDaughters();
  
  if(i >= 0 && i < daughtersNo) {
    G4VPhysicalVolume*  pvStrip = lvStripPlane->GetDaughter(i);
    G4String strStrip = pvStrip->GetLogicalVolume()->GetName();
    //G4cout << "length " << strStrip.length() << G4endl;
    //if(strStrip.length() == 30) { //use name length to determine whether it is a strip

    if (strStrip.find("s") == 10) { //"Strip" at 23 of "logicalMucPart1Seg0Gap0Strip111" 
      //"s" at 10 of lMucP0S0G0s001
      return pvStrip;
    }
    //return pvStrip;
  }
  
  return 0;
}

G4float BesMucDigit::GetStripPos(G4VPhysicalVolume* pvStrip)
{
  G4float pos = 1.0e38;
  if (!pvStrip) return pos;
  
  G4int part = m_pHit->GetPart();
  G4int gap  = m_pHit->GetGap();

  pos = pvStrip->GetObjectTranslation().x();
  if ( (part == 1 && gap%2 != 0) || (part != 1 && gap%2 == 0) ) {
    pos = pvStrip->GetObjectTranslation().y();
  }

  G4String striptype= pvStrip->GetLogicalVolume()->GetSolid()->GetEntityType();
//  G4String striplenght= pvStrip->GetLogicalVolume()->GetName();
  G4Box *temp;
  temp=(G4Box *)pvStrip->GetLogicalVolume()->GetSolid();
  G4double length=temp->GetXHalfLength();
//  std::ostream length;
//  pvStrip->GetLogicalVolume()->GetSolid()->StreamInfo( length);

  G4float  box1,box2,box3;
//  length>>box1>>box2>>box3;
  //G4cout<<length<<" "<<striptype<<"         in GetStripPos   "<<pvStrip->GetObjectTranslation().x()<<" "<<pvStrip->GetObjectTranslation().y()<<" "<<pvStrip->GetObjectTranslation().z()<<G4endl;
  return pos; 
} 



G4int BesMucDigit::IsNearest(G4int i)
{
  G4float posMax = 1.0e38;
  G4float posLeftStrip;
  G4float posRightStrip;
  G4float posCurrentStrip;

  G4VPhysicalVolume* pvLeftStrip    = GetStrip(i-1);
  G4VPhysicalVolume* pvRightStrip   = GetStrip(i+1);
  G4VPhysicalVolume* pvCurrentStrip = GetStrip(i);

  if(i < 0) {
    return i;
  }
  else if(i == 0) {
    posLeftStrip    = -1.0*posMax;
    posRightStrip   = GetStripPos( pvRightStrip );
  }
  else if(!pvCurrentStrip) {
    // i > Max.
    return -1;
  }
  else if(!pvRightStrip) {
    // i = Max.
    posLeftStrip  = GetStripPos( pvLeftStrip );
    posRightStrip = posMax;
  }
  else {
    // 0 < i < Max.
    posLeftStrip  = GetStripPos( pvLeftStrip );
    posRightStrip = GetStripPos( pvRightStrip );
  }
  posCurrentStrip = GetStripPos( pvCurrentStrip );

   G4float distCurrent = std::abs(m_Pos - posCurrentStrip);
   G4float distLeft    = std::abs(m_Pos - posLeftStrip);
   G4float distRight   = std::abs(m_Pos - posRightStrip);

  //G4cout << "left " << distLeft << " current " << distCurrent << " right " << distRight << G4endl;

  if( (distCurrent <= distLeft) && (distCurrent <= distRight) ) {
    return 0;
  }
  else if(distCurrent > distLeft) {
    return -1;
  }
  else {
    return 1;
  }

}
