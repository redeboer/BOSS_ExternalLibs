//$id$

/*
 *    2005/10/30   Zhengyun You      Peking University
 *                 Tof Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

using namespace std;

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "Identifier/TofID.h"
#include "G4Geo/TofG4Geo.h"
#include "ReadBoostRoot.hh"


TofG4Geo::TofG4Geo()
{ 
  string GdmlManagementPath = getenv("GDMLMANAGEMENTROOT");
  if (GdmlManagementPath == "") cout << "TofG4Geo::GdmlManagementPath not set" << endl;
 


  string GdmlFile = GdmlManagementPath + string("/dat/Tof.gdml");
      cout << "Construct old Tof from GdmlFile " << GdmlFile << endl;



  InitFromGdml( GdmlFile.c_str(), "Tof" );
}

TofG4Geo::~TofG4Geo()
{ }

void
TofG4Geo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  m_TopVolume = GetLogicalVolume("logicalTof");
  if (!m_TopVolume) cout << "TofG4Geo::InitFromGdml, m_TopVolume not found" << endl;
  else cout << "Tof TopVolume name " << m_TopVolume->GetName() << endl;
  SetDefaultVis();

  m_G4GeoInit = 1;  
}

void 
TofG4Geo::SetDefaultVis()
{
}
