//$id$
/*
 *    2007/05/15   Miao He <hem@ihep.ac.cn>
 *                 Emc Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

using namespace std;

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "Identifier/EmcID.h"
#include "G4Geo/EmcG4Geo.h"

EmcG4Geo*
EmcG4Geo::m_pEmcG4Geo = 0L;

EmcG4Geo::EmcG4Geo()
{ 
  string GdmlManagementPath = getenv("GDMLMANAGEMENTROOT");
  if (GdmlManagementPath == "") cout << "EmcG4Geo::GdmlManagementPath not set!" << endl;
  string GdmlFile = GdmlManagementPath + string("/dat/Emc.gdml");
  cout << "Construct Emc from GdmlFile " << GdmlFile << endl;
  InitFromGdml( GdmlFile.c_str(), "Emc" );
}

EmcG4Geo::~EmcG4Geo()
{ }

void
EmcG4Geo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  m_TopVolume = GetLogicalVolume("logicalEMC");
  if (!m_TopVolume) cout << "EmcG4Geo::InitFromGdml, m_TopVolume not found" << endl;
  else cout << "Emc TopVolume name " << m_TopVolume->GetName() << endl;
  SetDefaultVis();

  m_G4GeoInit = 1;  
}

void 
EmcG4Geo::SetDefaultVis()
{
}

EmcG4Geo*
EmcG4Geo::Instance()
{
  //Get a pointer to the single instance of EmcG4Geo
  if(!m_pEmcG4Geo) {
    m_pEmcG4Geo = new EmcG4Geo;
    //cout<<"in EmcG4Geo:: no EmcG4Geo now."<<m_pEmcG4Geo<<endl;
  }
  //cout<<"in EmcG4Geo:: get EmcG4Geo successfully."<<endl;
  return m_pEmcG4Geo;
}

