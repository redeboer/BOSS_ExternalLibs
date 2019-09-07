//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Tof Geometry General for EventDisplay
 *
 *    2004/12/10   Zhengyun You      Peking University
 *                 named from MdcGeo to MdcROOTGeo
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

#include "ROOTGeo/MdcROOTGeo.h"

const int 
MdcROOTGeo::m_kReplica[m_kLayer] = { 40,  44,  48,  56,  64,  72,  80,  80,
				     76,  76,  88,  88,  100, 100, 112, 112, 128, 128, 140, 140,
				     160, 160, 160, 160, 192, 192, 192, 192, 208, 208, 208, 208, 240, 240, 240, 240,
				     256, 256, 256, 256, 288, 288, 288,
				     256, 256, 256, 256, 288, 288, 288 };
const int 
MdcROOTGeo::m_kiCorrectLayer[m_kCorrectLayer] =   {9, 11, 13, 15, 17, 19,  36,  38,  40,  42};
const int 
//MdcROOTGeo::m_kiCorrectReplica[m_kCorrectLayer] = {38, 44, 50, 56, 65, 71, 127, 127, 144, 143};
MdcROOTGeo::m_kiCorrectReplica[m_kCorrectLayer] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

MdcROOTGeo::MdcROOTGeo()
{ 
  // Default constructor.
  for (int layer = 0; layer < m_kLayer; layer++) {
    for (int replica = 0; replica < m_kReplicaMax; replica++) {
      m_NodeReplica[layer][replica] = 0;
      m_PhysicalReplica[layer][replica] = 0;
    }
  }

  for (int i = 0; i < m_kCorrectLayer; i++) {
    m_CorrectMap[m_kiCorrectLayer[i]] = m_kiCorrectReplica[i];
  }
}

MdcROOTGeo::~MdcROOTGeo()
{ }

void
MdcROOTGeo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  SetNode();
  SetDefaultVis();

  m_ROOTGeoInit = 1;  
}

void 
MdcROOTGeo::SetNode()
{
  m_Mdc = GetTopVolume();
  if(!m_Mdc) std::cout << "m_Mdc = 0" << std::endl;
  
  for (int layer = 0; layer < m_kLayer; layer++) {
    //std::cout << "Layer " << layer << std::endl;
    for (int replica = 0; replica < m_kReplica[layer]; replica++) {
      std::stringstream osname;
      int rep = replica;
      if (layer >= 0 && layer < m_kStereoLayerIn) {
	osname << "pv_" << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "TwistedTubs" << "_" << rep; 
      }
      else if (layer >= m_kStereoLayerIn && layer < m_kStereoLayerIn + m_kAxialLayerIn) {
	rep = CorrectReplica(layer, rep);
	osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << layer << "Replica" << "_" << rep; 
      }
      else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
	       layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) {
	osname << "pv_" << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "TwistedTubs" << "_" << rep;
      }
      else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut &&
	       layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut) {
	rep = CorrectReplica(layer, rep);
	osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << layer << "_0" << "Replica" << "_" << rep;
      }
      else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut &&
	       layer <  m_kLayer) {
	rep = CorrectReplica(layer, rep);
	osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << layer - m_kAxialLayerOut << "_1" << "Replica" << "_" << rep;
      }
      
      m_NodeReplica[layer][replica] = GetNode( osname.str() );
    }
  }
}

int 
MdcROOTGeo::CorrectReplica(int layer, int replica)
{
  int rep = replica;
  for (intMap::iterator iter = m_CorrectMap.begin(); iter != m_CorrectMap.end(); iter++) {
    if (layer == (*iter).first ||
	(layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut &&
	 layer - m_kAxialLayerOut == (*iter).first)) {
      rep -= (*iter).second;
      if (rep < 0) rep += m_kReplica[layer];
    }
  }

  //std::cout << "layer" << layer << " " << replica << "->" << rep << std::endl; 
  return rep;
}

void 
MdcROOTGeo::SetDefaultVis()
{
  //std::cout << "begin of set defaultvis" << std::endl;
  int mdcColor         = 3;
  int segmentColor     = 0;
  int hypeColor        = 3;
  int tubeColor        = 4;
  int twistedTubsColor = 3;
  int replicaColor     = 4;

  m_Mdc->SetLineColor(mdcColor);
  m_Mdc->SetVisibility(0);
  
  for (int segment = 1; segment <= m_kSegment; segment++) {
    GetVolumeSegment(segment)->SetLineColor(segmentColor);
    GetVolumeSegment(segment)->SetVisibility(0);
  }

  for (int layer =0; layer < m_kLayer; layer++) {
    if ( (layer >= 0 && layer < m_kStereoLayerIn) ||
	 (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
	  layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) ) {
      GetVolumeLayer(layer)->SetLineColor(hypeColor);
      GetVolumeReplica(layer)->SetLineColor(twistedTubsColor);
    }
    else {
      GetVolumeLayer(layer)->SetLineColor(tubeColor);
      GetVolumeReplica(layer)->SetLineColor(replicaColor);
    }
    GetVolumeLayer(layer)->SetVisibility(0);
    GetVolumeReplica(layer)->SetVisibility(1);
  }

  for (int segment = 1; segment <= m_kSegment; segment++) {
    for (int no = 0; no < 2; no++) {
      GetSegment(segment, no)->SetVisibility(0);
    }
  }

  for (int layer = 0; layer < m_kLayer; layer++) {
    GetLayer(layer)->SetVisibility(0);
    for (int replica = 0; replica < m_kReplica[layer]; replica++) {
      GetReplica(layer, replica)->SetVisibility(0);
    }
  }

  //std::cout << "end of set defaultvis" << std::endl;
}

void
MdcROOTGeo::SetAllVisible()
{
  for (int segment = 1; segment <= m_kSegment; segment++) {
    GetVolumeSegment(segment)->SetVisibility(0);
  }
  
  for (int layer = 0; layer < m_kLayer; layer++) {
    GetVolumeLayer(layer)->SetVisibility(1);
    GetVolumeReplica(layer)->SetVisibility(0);
  }

  for (int segment = 1; segment <= m_kSegment; segment++) {
    for (int no = 0; no < 2; no++) {
      GetSegment(segment, no)->SetVisibility(1);
    }
  }

  for (int layer = 0; layer < m_kLayer; layer++) {
    GetLayer(layer)->SetVisibility(1);
    for (int replica = 0; replica < m_kReplica[layer]; replica++) {
      GetReplica(layer, replica)->SetVisibility(0);
    }
  }
}

void
MdcROOTGeo::SetQuarterVisible()
{
  for (int segment = 1; segment <= m_kSegment; segment++) {
    GetVolumeSegment(segment)->SetVisibility(0);
  }
  
  for (int layer = 0; layer < m_kLayer; layer++) {
    GetVolumeLayer(layer)->SetVisibility(0);
    GetVolumeReplica(layer)->SetVisibility(1);
  }

  for (int segment = 1; segment <= m_kSegment; segment++) {
    for (int no = 0; no < 2; no++) {
      GetSegment(segment, no)->SetVisibility(0);
    }
  }

  for (int layer = 0; layer < m_kLayer; layer++) {
    GetLayer(layer)->SetVisibility(0);
    for (int replica = 0; replica < m_kReplica[layer]; replica++) {
      if (replica < m_kReplica[layer]/4) GetReplica(layer, replica)->SetVisibility(0);
      else                               GetReplica(layer, replica)->SetVisibility(1);
    }
  }
}

void 
MdcROOTGeo::SetPhysicalNode()
{
  int twistedTubsColor = 3;
  int replicaColor     = 4;

  if (gGeoManager == 0) std::cout << "Create gGeoManager first" << std::endl; 
  TGeoNode *bes = gGeoManager->GetTopNode();
  //std::cout << "m_childNo " << m_childNo << std::endl;
  TGeoNode *mdc = bes->GetDaughter(m_childNo);

  //if(!m_Mdc) std::cout << "m_Mdc = 0" << std::endl;
  
  for (int layer = 0; layer < m_kLayer; layer++) {
    TGeoNode *nodeLayer = GetLayer(layer);
    //std::cout << "Layer " << layer << std::endl;
    for (int replica = 0; replica < m_kReplica[layer]; replica++) {
      TGeoNode *nodeReplica = GetReplica(layer, replica);
      m_PhysicalReplica[layer][replica] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() + 
									 TString("/") + mdc->GetName() +
									 TString("/") + nodeLayer->GetName() +
									 TString("/") + nodeReplica->GetName());
      m_PhysicalReplica[layer][replica]->SetVisibility(0); 
      m_PhysicalReplica[layer][replica]->SetIsVolAtt(kFALSE);
      if ( (layer >= 0 && layer < m_kStereoLayerIn) ||
	   (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
	    layer < m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) ) {
	m_PhysicalReplica[layer][replica]->SetLineColor(twistedTubsColor);
      }
      else {
	m_PhysicalReplica[layer][replica]->SetLineColor(replicaColor);
      }	
      //if (m_PhysicalReplica[layer][replica]->IsVolAttributes()) std::cout << "yes " << std::endl; 
    }
  }
}

TGeoVolume*
MdcROOTGeo::GetVolumeSegment( int segment )
{
  std::stringstream osname;
  osname << "logical" << "Mdc" << "Segment" << segment;
  return GetLogicalVolume( osname.str() );
}
  
TGeoVolume* 
MdcROOTGeo::GetVolumeLayer( int layer )
{
  std::stringstream osname;
  if (layer >= 0 && layer < m_kStereoLayerIn) {
    osname << "logical" << "Mdc" << "Stereo" << "Layer" << layer; 
  }
  else if (layer >= m_kStereoLayerIn && layer < m_kStereoLayerIn + m_kAxialLayerIn) {
    osname << "logical" << "Mdc" << "Axial" << "Layer" << layer; 
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
	   layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) {
    osname << "logical" << "Mdc" << "Stereo" << "Layer" << layer;
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut &&
	   layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut) {
    osname << "logical" << "Mdc" << "Axial" << "Layer" << layer << "_0";
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut &&
	   layer <  m_kLayer) {
    osname << "logical" << "Mdc" << "Axial" << "Layer" << layer - m_kAxialLayerOut << "_1";
  }
  
  return GetLogicalVolume( osname.str() );
}

TGeoVolume* 
MdcROOTGeo::GetVolumeReplica( int layer )
{
  std::stringstream osname;
  if (layer >= 0 && layer < m_kStereoLayerIn) {
    osname << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "TwistedTubs"; 
  }
  else if (layer >= m_kStereoLayerIn && layer < m_kStereoLayerIn + m_kAxialLayerIn) {
    osname << "logical" << "Mdc" << "Axial" << "Layer" << layer << "Replica"; 
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
	   layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) {
    osname << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "TwistedTubs";
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut &&
	   layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut) {
    osname << "logical" << "Mdc" << "Axial" << "Layer" << layer << "_0" << "Replica";
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut &&
	   layer <  m_kLayer) {
    osname << "logical" << "Mdc" << "Axial" << "Layer" << layer - m_kAxialLayerOut << "_1" << "Replica";
  }
  
  return GetLogicalVolume( osname.str() );
}

TGeoNode*
MdcROOTGeo::GetSegment( int segment, int no )
{
  std::stringstream osname;
  if (segment == 1 || segment == 2) {
    osname << "pv_" << "logical" << "Mdc" << "Segment" << segment << "_" << m_kLayer + segment - 1; 
  }
  else {
    osname << "pv_" << "logical" << "Mdc" << "Segment" << segment << "_" << m_kLayer + 2 + (segment - 3)*2 + no; 
  }    

  return GetNode( osname.str() );
}

TGeoNode*
MdcROOTGeo::GetLayer( int layer )
{
  std::stringstream osname;
  if (layer >= 0 && layer < m_kStereoLayerIn) {
    osname << "pv_" << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "_" << layer; 
  }
  else if (layer >= m_kStereoLayerIn && layer < m_kStereoLayerIn + m_kAxialLayerIn) {
    osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << layer << "_" << layer; 
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
	   layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) {
    osname << "pv_" << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "_" << layer;
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut &&
	   layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut) {
    osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << layer << "_0" << "_" 
	   << 2*layer - m_kStereoLayerIn - m_kAxialLayerIn - m_kStereoLayerOut; 
  }
  else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut &&
	   layer <  m_kLayer) {
    osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << layer - m_kAxialLayerOut << "_1" << "_" 
	   << 2*(layer - m_kAxialLayerOut) - m_kStereoLayerIn - m_kAxialLayerIn - m_kStereoLayerOut + 1;
  }
  
  //std::cout << osname.str() << std::endl;
  return GetNode( osname.str() );
}

TGeoNode*
MdcROOTGeo::GetReplica( int layer, int replica )
{
  if (m_NodeReplica[layer][replica] != 0) {
    //std::cout << " replica " << layer <<  "  " << replica << " found " << std::endl;
    return m_NodeReplica[layer][replica];
  }
  else {
    std::cout << "Node: " << "Layer" << layer << "Replica" << replica << " not found" << std::endl;
    return 0;
  } 
}

TGeoPhysicalNode*
MdcROOTGeo::GetPhysicalReplica( int layer, int replica )
{
  if (m_PhysicalReplica[layer][replica] != 0) {
    return m_PhysicalReplica[layer][replica];
  }
  else {
    std::cout << "PhysicalNode: " << "Layer" << layer << "Replica" << replica << " not found" << std::endl;
    return 0;
  } 
}
