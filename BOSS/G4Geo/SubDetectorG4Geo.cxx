//$id$
/*
 *    2005/10/30    Zhengyun You      Peking University
 *                  G4 Geometry for Bes sub-detector
 *
 */

using namespace std;

#include <string>
#include <iostream>

#include "G4Geo/SubDetectorG4Geo.h"

SubDetectorG4Geo::SubDetectorG4Geo()
  : m_G4GeoInit(0)
{ 
  // Default constructor.
  //m_sxp.Initialize();
}

SubDetectorG4Geo::~SubDetectorG4Geo()
{ 
  m_sxp.Finalize();
}

void
SubDetectorG4Geo::ReadGdml( const char *gdmlFile, const char *setupName )
{
  m_config.SetURI( gdmlFile );
  m_config.SetSetupName( setupName );
  m_config.SetType ( "G4" );
  
  m_sxp.Configure( &m_config );
  m_sxp.Initialize();
  m_sxp.Run();
}

G4LogicalVolume* 
SubDetectorG4Geo::GetLogicalVolume( const std::string& vn )
{
  G4LogicalVolume *lv = (G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume(vn);
  //if (!lv) std::cout << "Logical Volume " << vn << " not found " << std::endl;
  return lv;
}


