//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author:  Youzy      Peking University      mail: youzy@hep.pku.cn
//Created:  Nov, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

//
// $Id: BesMucHit 

#include "BesMucHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"
#include "strstream"

G4Allocator<BesMucHit> BesMucHitAllocator;

BesMucHit::BesMucHit()
{ }

BesMucHit::BesMucHit(int prt, int seg, int lay, int str, int trId, int trIndex)
{ 
  m_Part = prt; 
  m_Seg  = seg; 
  m_Gap  = lay;
  m_Strip = str;
  m_TrackID = trId; 
  m_TrackIndex = trIndex;
}

BesMucHit::~BesMucHit() 
{ }

BesMucHit::BesMucHit(const BesMucHit& right)
  :G4VHit()
{
  m_TrackID    = right.m_TrackID;
  m_TrackIndex = right.m_TrackIndex;
  m_PDGCode    = right.m_PDGCode;
  m_Edep       = right.m_Edep;
  m_Energy     = right.m_Energy;
  m_Time       = right.m_Time;
  m_Pos        = right.m_Pos;
  m_PosLocal   = right.m_PosLocal;
  m_Dir        = right.m_Dir;
  m_Momentum   = right.m_Momentum;
  m_Volume     = right.m_Volume;
  m_VolumeName = right.m_VolumeName;
  m_Part       = right.m_Part;
  m_Seg        = right.m_Seg;
  m_Gap        = right.m_Gap;
  m_Panel      = right.m_Panel;
  m_GasChamber = right.m_GasChamber;     //now it means up or down RPC! 2006.10.17
  m_Strip      = right.m_Strip;
}

const BesMucHit& BesMucHit::operator=(const BesMucHit& right)
{
  m_TrackID    = right.m_TrackID;
  m_TrackIndex = right.m_TrackIndex;
  m_PDGCode    = right.m_PDGCode;
  m_Edep       = right.m_Edep;
  m_Energy     = right.m_Energy;
  m_Time       = right.m_Time;
  m_Pos        = right.m_Pos;
  m_PosLocal   = right.m_PosLocal;
  m_Dir        = right.m_Dir;
  m_Momentum   = right.m_Momentum;
  m_Volume     = right.m_Volume;
  m_VolumeName = right.m_VolumeName;
  m_Part       = right.m_Part;
  m_Seg        = right.m_Seg;
  m_Gap        = right.m_Gap;
  m_Panel      = right.m_Panel;
  m_GasChamber = right.m_GasChamber;
  m_Strip      = right.m_Strip;
  return *this;
}

int BesMucHit::operator==(const BesMucHit& right) const
{
  return (this==&right) ? 1 : 0;
}

void BesMucHit::SetVolume(G4VPhysicalVolume* pv)
{
  m_Volume     = pv; 
  m_VolumeName = pv->GetLogicalVolume()->GetName();

  G4LogicalVolume *lvGap = pv->GetMotherLogical();
  G4String GapName = lvGap->GetName();

  //G4cout<<" in BesMucHit ---- "<<m_VolumeName<<" "<<GapName<<G4endl;
//    //  "logicalMucPart1Seg0Gap4Panel1GasChamber0"
//    //  "logicalMucPart1Seg4Gap0"
//    G4String strPart       = m_VolumeName.substr(14,1);
//    G4String strSeg        =      GapName.substr(18,1);
//    G4String strGap        = m_VolumeName.substr(22,1);
//    G4String strPanel      = m_VolumeName.substr(28,1);
//    G4String strGasChamber = m_VolumeName.substr(39,1);

  //   "logicalMucPart0Seg0Gap0RpcUpDown0Panel0GasChamber"   2006.10.17 liangyt
  //   "logicalMucPart0Seg0Gap0" 

  //   "lMucP0S0G0R0Pn0C"           2006.12.05 liangyt 
  //   "lMucP0S0G0"
  G4String strPart       = m_VolumeName.substr(5,1);
  G4String strSeg        =      GapName.substr(7,1);
  G4String strGap        = m_VolumeName.substr(9,1);
  G4String strGasChamber = m_VolumeName.substr(11,1);    //same as rpcupdown 
  G4String strPanel      = m_VolumeName.substr(14,1);
//   G4String strPart       = m_VolumeName.substr(14,1);
//   G4String strSeg        =      GapName.substr(18,1);
//   G4String strGap        = m_VolumeName.substr(22,1);
//   G4String strGasChamber = m_VolumeName.substr(32,1);    //same as rpcupdown 
//   G4String strPanel      = m_VolumeName.substr(38,1);

  std::istrstream partBuf(strPart.c_str(),             strlen(strPart.c_str()));
  std::istrstream segBuf(strSeg.c_str(),               strlen(strSeg.c_str()));
  std::istrstream gapBuf(strGap.c_str(),               strlen(strGap.c_str()));
  std::istrstream panelBuf(strPanel.c_str(),           strlen(strPanel.c_str()));
  std::istrstream gasChamberBuf(strGasChamber.c_str(), strlen(strGasChamber.c_str()));

  partBuf       >> m_Part ;
  segBuf        >> m_Seg;
  gapBuf        >> m_Gap;
  panelBuf      >> m_Panel;
  gasChamberBuf >> m_GasChamber; 
}


void BesMucHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(m_Pos);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(0.,0.,1.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
    
    //re-visualize a physical volume where a hit is detected
    //
    //
  /*  const G4ThreeVector& thrVector=G4ThreeVector(1.,0.,0.);
    const G4RotationMatrix &rot = 
	    G4RotationMatrix(thrVector,0.*deg);
    
    G4Transform3D trans(rot, pos);
    G4VisAttributes att;
    const G4VPhysicalVolume* pPhys= detector->GetPhysicalTrap();
    const G4LogicalVolume* logVol=
	    pPhys->GetLogicalVolume();
    const G4VisAttributes* pVA=logVol->GetVisAttributes();
    if(pVA) att=*pVA;
    G4Colour col(1.,0.,0.);
    att.SetColour(col);
    att.SetForceSolid(true);

    pVVisManager->Draw(*pPhys,att,trans);*/
  }
}

void BesMucHit::Print()
{
  
  G4cout << "  Track id: "       << m_TrackID
	 << "  pdg code: "       << m_PDGCode
	 << G4endl
         << "  energy deposit: " << G4BestUnit(m_Edep, "Energy")
	 << "  pos: "            << G4BestUnit(m_Pos,  "Length") 
	 << "  posLocal: "       << G4BestUnit(m_PosLocal,  "Length") 
	 << G4endl
	 << "  energy: "         << G4BestUnit(m_Energy, "Energy")
	 << "  direction: "      << m_Dir 
	 << "  time: "           << G4BestUnit(m_Time,"Time")
	 << "  volume name: "    << m_VolumeName
	 << G4endl;
}




