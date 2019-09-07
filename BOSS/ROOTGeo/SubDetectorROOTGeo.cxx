//$id$
/*
 *    2004/12/9    Zhengyun You      Peking University
 *                 ROOT Geometry for Bes sub-detector in event display and reconstruction.
 *
 */

using namespace std;

#include <string>
#include <iostream>

#include "TGeoVolume.h"
#include "TGeoNode.h"

#include "ROOTGeo/SubDetectorROOTGeo.h"

SubDetectorROOTGeo::SubDetectorROOTGeo()
  : m_ROOTGeoInit(0)
{ 
  // Default constructor.
  //m_sxp.Initialize();
}

SubDetectorROOTGeo::~SubDetectorROOTGeo()
{ 
  m_sxp.Finalize();
}

void
SubDetectorROOTGeo::ReadGdml( const char *gdmlFile, const char *setupName )
{
  m_config.SetURI( gdmlFile );
  m_config.SetSetupName( setupName );
  m_config.SetType ( "ROOT" );
  
  m_sxp.Configure( &m_config );
  m_sxp.Initialize();
  m_sxp.Run();
  
  m_TopVolume = (TGeoVolume*)TGDMLProcessor::GetInstance()->GetWorldVolume();
  if (!m_TopVolume) std::cout << "Top Volume not found " << std::endl;
}

TGeoVolume* SubDetectorROOTGeo::GetLogicalVolume( const std::string& vn )
{
  TGeoVolume *lv = (TGeoVolume*)TGDMLProcessor::GetInstance()->GetLogicalVolume(vn);
  if (!lv) std::cout << "Logical Volume " << vn << " not found " << std::endl;
  return lv;
}

TGeoVolumeAssembly* SubDetectorROOTGeo::GetAssemblyVolume( const std::string& an )
{
  TGeoVolumeAssembly *av = (TGeoVolumeAssembly*)TGDMLProcessor::GetInstance()->GetAssemblyVolume(an);
  if (!av) std::cout << "Assembly Volume " << an << " not found " << std::endl;
  return av;
}

TGeoNode* SubDetectorROOTGeo::GetNode( const std::string& nn )
{
  TGeoNode *node = (TGeoNode*)TGDMLProcessor::GetInstance()->GetPhysicalVolume(nn);
  if (!node) std::cout << "Physical Volume " << nn << " not found " << std::endl;
  return node;
}

