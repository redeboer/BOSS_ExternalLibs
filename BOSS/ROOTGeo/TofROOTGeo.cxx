//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Tof Geometry General for EventDisplay
 *
 *    2004/12/9    Zhengyun You      Peking University
 *                 named from TofGeo to TofROOTGeo
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

#include "ROOTGeo/TofROOTGeo.h"

TofROOTGeo::TofROOTGeo()
{ 
  // Default constructor.
  for (int part = 0; part < m_kPart; part++) {
    for (int scin = 0; scin < m_kAssemblyBr; scin++) {
      m_NodeScin[part][scin] = 0;
      m_PhysicalScin[part][scin] = 0;
    }
  }
}

TofROOTGeo::~TofROOTGeo()
{ }

void
TofROOTGeo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  SetNode();
  SetDefaultVis();

  m_ROOTGeoInit = 1;  
}

void 
TofROOTGeo::SetNode()
{
  m_Tof = GetTopVolume();
  if(!m_Tof) std::cout << "m_Tof = 0" << std::endl;
  
  for (int part = 0; part < m_kPart; part++) {
    int nScin = 0;
    if (part == 1) nScin = m_kAssemblyBr;
    else nScin = m_kAssemblyEc; 
    for (int scin = 0; scin < nScin; scin++) {
      std::stringstream osname;
      if (part == 0) {
	osname << "pv_" << "logical" << "Tof" << "ScinEc" << "_" << m_kAssemblyBr + m_kAssemblyEc + scin << "_0"; 
    }
      else if (part == 1) {
	osname << "pv_" << "logical" << "Tof" << "ScinBr" << "_" << scin << "_0"; 
      }
      else if (part == 2) {
	osname << "pv_" << "logical" << "Tof" << "ScinEc" << "_" << m_kAssemblyBr + scin << "_0"; 
      }
      
      m_NodeScin[part][scin] = GetNode( osname.str() );
    }
  }
}

void 
TofROOTGeo::SetDefaultVis()
{
  //std::cout << "begin of set defaultvis" << std::endl;
  int tofColor    = 2;
  int scinColor   = 6;
  int bucketColor = 4;

  m_Tof->SetLineColor(tofColor);
  m_Tof->SetVisibility(0);
  
  for (int part = 0; part < m_kPart; part++) {
    GetVolumeScin(part)->SetLineColor(scinColor);
    GetVolumeScin(part)->SetVisibility(1);
    GetVolumeBucket(part)->SetLineColor(bucketColor);
    GetVolumeBucket(part)->SetVisibility(0);

    int nScin = 0;
    if (part == 1) nScin = m_kAssemblyBr;
    else nScin = m_kAssemblyEc; 
    for (int scin = 0; scin < nScin; scin++) {
      GetScin(part, scin)->SetVisibility(0);
      
      int nBucket = 0;
      if (part == 1) nBucket = m_kBucketBr;
      else nBucket = m_kBucketEc; 
      for (int bucket = 0; bucket < nBucket; bucket++) {
	GetBucket(part, scin, bucket)->SetVisibility(0);
      }
    }
  }
  //std::cout << "end of set defaultvis" << std::endl;
}

void
TofROOTGeo::SetAllVisible()
{
  for (int part = 0; part < m_kPart; part++) {
    GetVolumeScin(part)->SetVisibility(1);
    GetVolumeBucket(part)->SetVisibility(1);
    
    int nScin = 0;
    if (part == 1) nScin = m_kAssemblyBr;
    else nScin = m_kAssemblyEc; 
    for (int scin = 0; scin < nScin; scin++) {
      GetScin(part, scin)->SetVisibility(1);
      
      int nBucket = 0;
      if (part == 1) nBucket = m_kBucketBr;
      else nBucket = m_kBucketEc; 
      for (int bucket = 0; bucket < nBucket; bucket++) {
	GetBucket(part, scin, bucket)->SetVisibility(1);
      }
    }
  }
}

void
TofROOTGeo::SetQuarterVisible()
{
  for (int part = 0; part < m_kPart; part++) {
    GetVolumeScin(part)->SetVisibility(1);
    GetVolumeBucket(part)->SetVisibility(1);
    
    int nScin = 0;
    if (part == 1) nScin = m_kAssemblyBr;
    else nScin = m_kAssemblyEc; 
    for (int scin = 0; scin < nScin; scin++) {
      if (scin < nScin/4) GetScin(part, scin)->SetVisibility(0);
      else                GetScin(part, scin)->SetVisibility(1);
      
      int nBucket = 0;
      if (part == 1) nBucket = m_kBucketBr;
      else nBucket = m_kBucketEc; 
      for (int bucket = 0; bucket < nBucket; bucket++) {
	if (scin < nScin/4) GetBucket(part, scin, bucket)->SetVisibility(0);
	else                GetBucket(part, scin, bucket)->SetVisibility(1);
      }
    }
  }
}

void 
TofROOTGeo::SetPhysicalNode()
{
  int scinColor = 6;

  if (gGeoManager == 0) std::cout << "Create gGeoManager first" << std::endl; 
  TGeoNode *bes = gGeoManager->GetTopNode();
  //std::cout << "m_childNo " << m_childNo << std::endl;
  TGeoNode *tof = bes->GetDaughter(m_childNo);
  
  for (int part = 0; part < m_kPart; part++) {
    int nScin = 0;
    if (part == 1) nScin = m_kAssemblyBr;
    else nScin = m_kAssemblyEc; 
    for (int scin = 0; scin < nScin; scin++) {
      TGeoNode *nodeScin = GetScin(part, scin);
      m_PhysicalScin[part][scin] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() + 
								  TString("/") + tof->GetName() +
								  TString("/") + nodeScin->GetName() );
      m_PhysicalScin[part][scin]->SetVisibility(0);
      m_PhysicalScin[part][scin]->SetIsVolAtt(kFALSE);
      m_PhysicalScin[part][scin]->SetLineColor(scinColor);
    }
  }
}

int
TofROOTGeo::GetPartNb()
{
  int nPart = m_kPart;
  return nPart;
}

int
TofROOTGeo::GetScinNb(int part)
{
  int nScin = m_kAssemblyBr;
  if (part != 1) nScin = m_kAssemblyEc;
  return nScin;
}

TGeoVolumeAssembly*
TofROOTGeo::GetVolumeAssembly( int part )
{
  std::stringstream osname;
  if (part == 0 || part == 2) {
    osname << "logical" << "Tof" << "AssemblyEc";
  }
  else {
    if (part == 1) {
      osname << "logical" << "Tof" << "AssemblyBr";
    }
  }

  return GetAssemblyVolume( osname.str() );
}
  
TGeoVolume* 
TofROOTGeo::GetVolumeScin( int part )
{
  std::stringstream osname;
  if (part == 0 || part == 2) {
    osname << "logical" << "Tof" << "ScinEc";
  }
  else {
    if (part == 1) {
      osname << "logical" << "Tof" << "ScinBr";
    }
  }

  return GetLogicalVolume( osname.str() );
}

TGeoVolume*
TofROOTGeo::GetVolumeBucket( int part )
{
  std::stringstream osname;
  if (part == 0 || part == 2) {
    osname << "logical" << "Tof" << "BucketEc";
  }
  else {
    if (part == 1) {
      osname << "logical" << "Tof" << "BucketBr";
    }
  }
  
  return GetLogicalVolume( osname.str() );
}

TGeoNode*
TofROOTGeo::GetScin( int part, int scin )
{
  if (m_NodeScin[part][scin] != 0) {
    return m_NodeScin[part][scin];
  }
  else {
    std::cout << "Node: " << "Part" << part << "Scin" << scin << " not found" << std::endl;
    return 0;
  } 

  return m_NodeScin[part][scin];
}

TGeoNode *
TofROOTGeo::GetBucket( int part, int scin, int bucket )
{
  std::stringstream osname;
  if (part == 0) {
    osname << "pv_" << "logical" << "Tof" << "BucketEc" << "_" << m_kAssemblyBr + m_kAssemblyEc + scin << "_1"; 
  }
  else if (part == 1) {
    osname << "pv_" << "logical" << "Tof" << "BucketBr" << "_" << scin << "_" << bucket+1; 
  }
  else if (part == 2) {
    osname << "pv_" << "logical" << "Tof" << "BucketEc" << "_" << m_kAssemblyBr + scin << "_1"; 
  }
  
  return GetNode( osname.str() );
}

TGeoPhysicalNode*
TofROOTGeo::GetPhysicalScin( int part, int scin )
{
  if (m_PhysicalScin[part][scin] != 0) {
    return m_PhysicalScin[part][scin];
  }
  else {
    std::cout << "PhysicalNode: " << "Part" << part << "Scin" << scin << " not found" << std::endl;
    return 0;
  } 
}

