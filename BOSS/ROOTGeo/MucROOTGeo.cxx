//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Muc Geometry General for EventDisplay
 *
 *    2004/12/11   Zhengyun You      Peking University
 *                 named from MucGeo to MucROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 */

using namespace std;

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

#include <TGeoManager.h>

#include "ROOTGeo/MucROOTGeo.h"

const int MucROOTGeo::m_kSeg[m_kPart]  = {4, 8, 4};
const int MucROOTGeo::m_kAbsorber[m_kPart] = {9, 9, 9};
const int MucROOTGeo::m_kGap[m_kPart]      = {8, 9, 8};
const int MucROOTGeo::m_kPanel[m_kPart]    = {4, 3, 4};

MucROOTGeo::MucROOTGeo()
{ 
  // Default constructor.
  for (int part = 0; part < m_kPart; part++) {
    for (int seg = 0; seg < m_kSegMax; seg++) {
      for (int gap = 0; gap < m_kGapMax; gap++) {
	m_NodeGap[part][seg][gap] = 0;
	m_PhysicalGap[part][seg][gap] = 0;
	//	m_NodeBox[part][seg][gap] = 0;
	//	m_PhysicalBox[part][seg][gap] = 0;
	for (int strip = 0; strip < m_kStripMax; strip++) {
	  m_NodeStrip[part][seg][gap][strip] = 0;
	  m_PhysicalStrip[part][seg][gap][strip] = 0;
	}
      }
    }
  }

  string GdmlManagementPath = getenv("GDMLMANAGEMENTROOT");
  //cout << "MucGeoGeneral::InitFromXML(), GdmlManagementPath not found" << endl;
  string GdmlFile = GdmlManagementPath + string("/dat/Muc.gdml");
  //string GdmlFile = string("Muc.gdml");
  cout << "GdmlFile " << GdmlFile << endl;
  InitFromGdml( GdmlFile.c_str(), "Muc" );
}

MucROOTGeo::~MucROOTGeo()
{ }

void
MucROOTGeo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  SetNode();
  //SetDefaultVis();

  m_ROOTGeoInit = 1;  
}

void 
MucROOTGeo::SetNode()
{
  m_Muc = GetTopVolume();
  if(!m_Muc) {
    m_Muc = GetLogicalVolume("logicalMuc");
  }
  if(!m_Muc) std::cout << "m_Muc = 0" << std::endl;

  int gapCount = 0;
  for (int part = 0; part < m_kPart; part++) {
    for (int seg = 0; seg < m_kSeg[part]; seg++) {  
      for (int gap = 0; gap < m_kGap[part]; gap++) {
	m_StripNum[part][seg][gap] = GetVolumeStripPlane(part, seg, gap)->GetNdaughters();
        //std::cout << part << " " << seg << " " << gap << " " << m_StripNum[part][seg][gap] <<" "<<GetVolumeStripPlane(part, seg, gap)->GetName()<< std::endl;

	std::stringstream osnameGap;
	osnameGap << "pv_" << "l" << "Muc" << "P" << part << "S" << seg << "G" << gap << "_" << gapCount; 
	m_NodeGap[part][seg][gap] = GetNode( osnameGap.str() );
	gapCount++;

	//cout<<"in MucROOTGeo "<<m_NodeGap[part][seg][gap]->GetName()<<" "<<m_NodeGap[part][seg][gap]->GetDaughter(0)->GetName()<<endl;

	std::stringstream osnameBox;
	osnameBox << "pv_" << "l" << "Muc" << "P" << part << "S" << seg << "G" << gap << "Al" << "_" << "0"; 
	//	m_NodeAluminumBox[part][seg][gap] = GetNode( osnameBox.str() );
	TGeoNode *temp = GetNode( osnameBox.str() );

	int segment = 0; if(part==1&&seg==2) segment = 2;
        std::stringstream osnameStripPlane;
        osnameStripPlane << "pv_" << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "SP" << "_" << "0";
        m_NodeStripPlane[part][seg][gap] = GetNode( osnameStripPlane.str() );
	
	for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
	  std::stringstream osnameStrip;
	  if(strip < 10) {
	    osnameStrip << "pv_" << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << "00" << strip << "_" << strip; 
	  }
	  else if(strip < 100) {
	    osnameStrip << "pv_" << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << "0" << strip << "_" << strip; 
	  }
	  else {
	    osnameStrip << "pv_" << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << strip << "_" << strip; 
	  }
	  
	  m_NodeStrip[part][seg][gap][strip] = GetNode( osnameStrip.str() );
	}
      }
    }
  }
}

void 
MucROOTGeo::SetDefaultVis()
{
  //std::cout << "begin of set defaultvis" << std::endl;
  int mucColor        = 2;
  int absorberColor   = 2;
  int gapColor        = 8;
  int gasChamberColor = 8;
  int bakeliteColor   = 8;
  int stripColor      = 6;

  m_Muc->SetLineColor(mucColor);
  m_Muc->SetVisibility(0);
  
  for (int part = 0; part < m_kPart; part++) {
    for (int seg = 0; seg < m_kSeg[part]; seg++) {
      for (int absorber = 0; absorber < m_kAbsorber[part]; absorber++) {
	GetVolumeAbsorber(part, seg, absorber)->SetLineColor(absorberColor);
	GetVolumeAbsorber(part, seg, absorber)->SetVisibility(0);
	if (part != 1) {
	  for (int panel = 0; panel < m_kPanel[part]; panel++) {
	    GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetLineColor(absorberColor);
	    GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetVisibility(0);
	  }
	}
      }
      
      for (int gap = 0; gap < m_kGap[part]; gap++) {
	GetVolumeGap(part, seg, gap)->SetLineColor(gapColor);
	GetVolumeGap(part, seg, gap)->SetVisibility(0);
        GetVolumeStripPlane(part, seg, gap)->SetLineColor(gapColor);
        GetVolumeStripPlane(part, seg, gap)->SetVisibility(0);
 
	for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
	  GetVolumeStrip(part, seg, gap, strip)->SetLineColor(stripColor);
	  GetVolumeStrip(part, seg, gap, strip)->SetVisibility(1);
	}
	
	for (int panel = 0; panel < m_kPanel[part]; panel++) {
	  for(int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
	    GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetLineColor(bakeliteColor);
	    GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetVisibility(0);
	  }
	  for (int gasChamber = 0; gasChamber < m_kGasChamber; gasChamber++) {
	    GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetLineColor(gasChamberColor);
	    GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetVisibility(0);
	  }
	}
      }
    }
  }

  for (int part = 0; part < m_kPart; part++) {
    for (int seg = 0; seg < m_kSeg[part]; seg++) {
      for (int gap = 0; gap < m_kGap[part]; gap++) {
	GetGap(part, seg, gap)->SetVisibility(0);
	for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
	  GetStrip(part, seg, gap, strip)->SetVisibility(0);
	}
      }
    }
  }

  //std::cout << "end of set defaultvis" << std::endl;
}

void
MucROOTGeo::SetAllVisible()
{
  for (int part = 0; part < m_kPart; part++) {
    for (int seg = 0; seg < m_kSeg[part]; seg++) {
      for (int absorber = 0; absorber < m_kAbsorber[part]; absorber++) {
	if(part == 1) {
	  GetVolumeAbsorber(part, seg, absorber)->SetVisibility(1);
	}
	else {
	  GetVolumeAbsorber(part, seg, absorber)->SetVisibility(0);
	  for (int panel = 0; panel < m_kPanel[part]; panel++) {
	    GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetVisibility(1);
	  }
	}
      }
	
      for (int gap = 0; gap < m_kGap[part]; gap++) {
	GetVolumeGap(part, seg, gap)->SetVisibility(0);
	for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
	  GetVolumeStrip(part, seg, gap, strip)->SetVisibility(0);
	}
	
	for (int panel = 0; panel < m_kPanel[part]; panel++) {
	  for(int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
	    GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetVisibility(1);
	  }
	  for (int gasChamber = 0; gasChamber < m_kGasChamber; gasChamber++) {
	    GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetVisibility(1);
	  }
	}
      }
    }
  }

  for (int part = 0; part < m_kPart; part++) {
    for (int seg = 0; seg < m_kSeg[part]; seg++) {
      for (int gap = 0; gap < m_kGap[part]; gap++) {
	GetGap(part, seg, gap)->SetVisibility(1);
	for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
	  GetStrip(part, seg, gap, strip)->SetVisibility(0);
	}
      }
    }
  }
}

void
MucROOTGeo::SetQuarterVisible()
{
  for (int part = 0; part < m_kPart; part++) {
    for (int seg = 0; seg < m_kSeg[part]; seg++) {
      for (int absorber = 0; absorber < m_kAbsorber[part]; absorber++) {
	if(part == 1) {
	  if(seg > 2) GetVolumeAbsorber(part, seg, absorber)->SetVisibility(1);
	}
	else {
	  GetVolumeAbsorber(part, seg, absorber)->SetVisibility(0);
	  for (int panel = 0; panel < m_kPanel[part]; panel++) {
	    if(seg > 0 && seg < 3) GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetVisibility(1);
	  }
	}
      }
      
      for (int gap = 0; gap < m_kGap[part]; gap++) {
	if (part == 1 && seg > 2) {
	  GetVolumeGap(part, seg, gap)->SetVisibility(1);
	  GetGap(part, seg, gap)->SetVisibility(1);
	}
	else {
          GetVolumeGap(part, seg, gap)->SetVisibility(0);
	  GetGap(part, seg, gap)->SetVisibility(0);
	}
	for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
	  GetVolumeStrip(part, seg, gap, strip)->SetVisibility(0);
	}
	
	for (int panel = 0; panel < m_kPanel[part]; panel++) {
	  for (int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
	    if ((part != 1 && seg > 0 && seg < 3) || (part == 1 && seg > 2)) GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetVisibility(1);
	  }
	  for (int gasChamber = 0; gasChamber < m_kGasChamber; gasChamber++) {
	    if ((part != 1 && seg > 0 && seg < 3) || (part == 1 && seg > 2)) GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetVisibility(1);
	  }
	}
      }
    }
  }

}

void 
MucROOTGeo::SetPhysicalNode()
{
  int stripColor = 2;

  if (gGeoManager == 0) std::cout << "Create gGeoManager first" << std::endl; 
  TGeoNode *bes = gGeoManager->GetTopNode();
  TGeoNode *muc = bes->GetDaughter(m_childNo);
  
  for (int part = 0; part < m_kPart; part++) {
    for (int seg = 0; seg < m_kSeg[part]; seg++) {
      for (int gap = 0; gap < m_kGap[part]; gap++) {
	TGeoNode *nodeGap = GetGap(part, seg, gap);
	m_PhysicalGap[part][seg][gap] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() + 
								       TString("/") + muc->GetName() +
								       TString("/") + nodeGap->GetName() +
								       TString("/") + nodeGap->GetDaughter(0)->GetName());
	m_PhysicalGap[part][seg][gap]->SetVisibility(0);
	m_PhysicalGap[part][seg][gap]->SetIsVolAtt(kFALSE);

	TGeoNode *nodeStripPlane = GetStripPlane(part, seg, gap);
        //std::cout << nodeGap->GetName() << "  " << nodeStripPlane->GetName() << endl;
	
	//TGeoNode *nodeBox = GetAluminumBox(part, seg, gap);
// 	m_PhysicalAluminumBox[part][seg][gap] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() + 
// 									 TString("/") + muc->GetName() +
// 									 TString("/") + nodeGap->GetName() +
// 									 TString("/") + nodeGap->GetDaughter(0)->GetName() );


	for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
	  TGeoNode *nodeStrip = GetStrip(part, seg, gap, strip);
	  m_PhysicalStrip[part][seg][gap][strip] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() + 
										  TString("/") + muc->GetName() +
                                                                                  TString("/") + nodeGap->GetName() +
										  TString("/") + nodeGap->GetDaughter(0)->GetName() +
										  TString("/") + nodeStripPlane->GetName() +
										  TString("/") + nodeStrip->GetName() );
	  m_PhysicalStrip[part][seg][gap][strip]->SetVisibility(0);
	  m_PhysicalStrip[part][seg][gap][strip]->SetIsVolAtt(kFALSE);
	  m_PhysicalStrip[part][seg][gap][strip]->SetLineColor(stripColor);
	}
      }
    }
  }
}

int
MucROOTGeo::GetPartNum()
{
  int part = m_kPart;
  return part;
}

int 
MucROOTGeo::GetSegNum(int part)
{
  int seg = m_kSeg[part];
  return seg;
}
  
int 
MucROOTGeo::GetGapNum(int part)
{
  int gap = m_kGap[part];
  return gap;
}

int 
MucROOTGeo::GetStripNum(int part, int seg, int gap)
{
  int strip = m_StripNum[part][seg][gap];
  return strip;
}

TGeoVolume*
MucROOTGeo::GetVolumeAbsorber( int part, int seg, int absorber )
{
  std::stringstream osname;
  osname << "l" << "Muc" << "P" << part << "S" << seg << "Ab" << absorber;
  
  return GetLogicalVolume( osname.str() );
}
  
float
MucROOTGeo::GetAbsorberThickness( int part, int seg, int absorber )
{
  float thickness = 0.0;
  TGeoVolume *vol = GetVolumeAbsorber(part, seg, absorber);
  if (vol) {
    thickness = 2.0*((TGeoBBox*)vol->GetShape())->GetDZ();
  }
 
  return thickness;
}

TGeoVolume*
MucROOTGeo::GetVolumeAbsorberPanel( int part, int seg, int absorber, int panel )
{
  std::stringstream osname;
  osname << "l" << "Muc" << "P" << part << "S" << seg << "Ab" << absorber << "Pn" << panel;
  
  return GetLogicalVolume( osname.str() );
}
  
TGeoVolume*
MucROOTGeo::GetVolumeGap( int part, int seg, int gap )
{
  std::stringstream osname;
  osname << "l" << "Muc" << "P" << part << "S" << seg << "G" << gap;
  
  return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeAluminumBox( int part, int seg, int gap )
{
  std::stringstream osname;
  osname <<"l" << "Muc" << "P" << part << "S" << seg << "G" << gap <<"Al";
  
  return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeStripPlane( int part, int seg, int gap )
{
  int segment = 0; if(part==1&&seg==2) segment = 2;
  std::stringstream osname;
  osname << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "SP";

  return GetLogicalVolume( osname.str() );
}
  
TGeoVolume*
MucROOTGeo::GetVolumeStrip( int part, int seg, int gap, int strip )
{
  std::stringstream osname;
  if(strip < 10) {
    osname << "l" << "Muc" << "P" << part << "S" << "0" << "G" << gap << "s" << "00" << strip; 
  }
  else if(strip<100){
    osname << "l" << "Muc" << "P" << part << "S" << "0" << "G" << gap << "s" << "0" << strip; 
  }
  else {
    osname << "l" << "Muc" << "P" << part << "S" << "0" << "G" << gap << "s" << strip; 
  }
  
  return GetLogicalVolume( osname.str() );
}
  
TGeoVolume* 
MucROOTGeo::GetVolumeGasChamber( int part, int seg, int gap, int panel, int gasChamber )
{
  std::stringstream osname;
  osname << "l" << "Muc" << "P" << part << "S" << "0" << "G" << gap << "R" << gasChamber << "Pn" << panel << "C";

  return GetLogicalVolume( osname.str() );
}

TGeoVolume* 
MucROOTGeo::GetVolumeBakelite( int part, int seg, int gap, int panel, int bakelite )
{
  std::stringstream osname;
  osname << "l" << "Muc" << "P" << part << "S" << "0" << "G" << gap << "R" << panel << "B" << bakelite;
  
  return GetLogicalVolume( osname.str() );
}

TGeoNode*
MucROOTGeo::GetGap( int part, int seg, int gap )
{
  if (m_NodeGap[part][seg][gap] != 0) {
    return m_NodeGap[part][seg][gap];
  }
  else {
    std::cout << "Node: " << "Part" << part << "Seg" << seg << "Gap" << gap << " not found" << std::endl;
    return 0;
  } 
}

// TGeoNode*
// MucROOTGeo::GetAluminumBox( int part, int seg, int gap )
// {
//   if (m_NodeAluminumBox[part][seg][gap] != 0) {
//     return m_NodeAluminumBox[part][seg][gap];
//   }
//   else {
//     std::cout << "Node: " << "Part" << part << "Seg" << seg << "Gap" << gap << "AluminumBox" << " not found" << std::endl;
//     return 0;
//   } 
// }


TGeoNode*
MucROOTGeo::GetStripPlane( int part, int seg, int gap )
{
  if (m_NodeStripPlane[part][seg][gap] != 0) {
    return m_NodeStripPlane[part][seg][gap];
  }
  else {
    std::cout << "Node: " << "Part" << part << "Seg" << seg << "Gap" << gap << "StripPlane" << " not found" << std::endl;
    return 0;
  }
}
    
TGeoNode*
MucROOTGeo::GetStrip( int part, int seg, int gap, int strip )
{
  if (m_NodeStrip[part][seg][gap][strip] != 0) {
    return m_NodeStrip[part][seg][gap][strip];
  }
  else {
    std::cout << "Node: " << "Part" << part << "Seg" << "0" << "Gap" << gap << "Strip" << strip << " not found" << std::endl;
    return 0;
  } 
}

TGeoPhysicalNode*
MucROOTGeo::GetPhysicalGap( int part, int seg, int gap )
{
  if (m_PhysicalGap[part][seg][gap] != 0) {
    return m_PhysicalGap[part][seg][gap];
  }
  else {
    std::cout << "PhysicalNode: " << "Part" << part << "Seg" << seg << "Gap" << gap << " not found" << std::endl;
    return 0;
  } 
}

// TGeoPhysicalNode*
// MucROOTGeo::GetPhysicalAluminumBox( int part, int seg, int gap )
// {
//   if (m_PhysicalAluminumBox[part][seg][gap] != 0) {
//     return m_PhysicalAluminumBox[part][seg][gap];
//   }
//   else {
//     std::cout << "PhysicalNode: " << "Part" << part << "Seg" << seg << "Gap" << gap << "AluminumBox" << " not found" << std::endl;
//     return 0;
//   } 
// }

TGeoPhysicalNode*
MucROOTGeo::GetPhysicalStrip( int part, int seg, int gap, int strip )
{
  if (m_PhysicalStrip[part][seg][gap][strip] != 0) {
    return m_PhysicalStrip[part][seg][gap][strip];
  }
  else {
    std::cout << "PhysicalNode: " << "Part" << part << "Seg" << seg << "Gap" << gap << "Strip" << strip << " not found" << std::endl;
    return 0;
  } 
}
