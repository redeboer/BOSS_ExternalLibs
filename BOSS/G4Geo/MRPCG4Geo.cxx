//$id$

/*
 *    2011/08/15   Matthias Ullrich  Giessen University
 *                 MRPC Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

using namespace std;

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "Identifier/TofID.h"
#include "G4Geo/MRPCG4Geo.h"



MRPCG4Geo::MRPCG4Geo()
{ 
  string GdmlManagementPath = getenv("GDMLMANAGEMENTROOT");
  if (GdmlManagementPath == "") cout << "MRPCG4Geo::GdmlManagementPath not set" << endl;
 

      string GdmlFile = GdmlManagementPath + string("/dat/Tof_mrpc.gdml");
      cout << "Construct new Tof (including MRPC (double sided readout) as End Cap Tof) from GdmlFile " << GdmlFile << endl;


  InitFromGdml( GdmlFile.c_str(), "Tof" );
}

MRPCG4Geo::~MRPCG4Geo()
{ }

void
MRPCG4Geo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  m_TopVolume = GetLogicalVolume("logicalTof");
  if (!m_TopVolume) cout << "MRPCG4Geo::InitFromGdml, m_TopVolume not found" << endl;
  else cout << "MRPC TopVolume name " << m_TopVolume->GetName() << endl;
  SetDefaultVis();

  m_G4GeoInit = 1;  
}

void 
MRPCG4Geo::SetDefaultVis()
{
}
