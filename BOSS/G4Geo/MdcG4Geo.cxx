//$id$
/*
 *    2005/10/30   Zhengyun You      Peking University
 *    2007/05/23   Yuan Ye           IHEP
 *                 Mdc Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

using namespace std;

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "Identifier/MdcID.h"
#include "G4Geo/MdcG4Geo.h"

MdcG4Geo::MdcG4Geo()
{ 
  string GdmlManagementPath = getenv("GDMLMANAGEMENTROOT");
  if (GdmlManagementPath == "") cout << "MdcG4Geo::GdmlManagementPath not set" << endl;
  string GdmlFile = GdmlManagementPath + string("/dat/Mdc.gdml");
  cout << "Construct Mdc from GdmlFile " << GdmlFile << endl;
  InitFromGdml( GdmlFile.c_str(), "Mdc" );
}

MdcG4Geo::~MdcG4Geo()
{ }

void
MdcG4Geo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  m_TopVolume = GetLogicalVolume("logicalMdc");
  if (!m_TopVolume) cout << "MdcG4Geo::InitFromGdml, m_TopVolume not found" << endl;
  else cout << "Mdc TopVolume name " << m_TopVolume->GetName() << endl;
  SetDefaultVis();

  m_G4GeoInit = 1;  
}

void 
MdcG4Geo::SetDefaultVis()
{
}
