//$id$

/*
 *    2007/05/28   Deng Ziyan 
 *                 Bes Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

using namespace std;

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>


#include "ReadBoostRoot.hh"
#include "G4Geo/BesG4Geo.h"

BesG4Geo::BesG4Geo()
{ 
  string GdmlManagementPath = getenv("GDMLMANAGEMENTROOT");
  if (GdmlManagementPath == "") cout << "BesG4Geo::GdmlManagementPath not set" << endl;
  string GdmlFile = GdmlManagementPath + string("/dat/Bes.gdml");
  cout << "Construct Bes from GdmlFile " << GdmlFile << endl;
  InitFromGdml( GdmlFile.c_str(), "Default" );
}

BesG4Geo::~BesG4Geo()
{ }

void
BesG4Geo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  m_TopVolume = GetLogicalVolume("logicalWorld");
  if (!m_TopVolume) cout << "BesG4Geo::InitFromGdml, m_TopVolume not found" << endl;
  else cout << "Bes TopVolume name " << m_TopVolume->GetName() << endl;
  SetDefaultVis();

  m_G4GeoInit = 1;  
}

void 
BesG4Geo::SetDefaultVis()
{
}
