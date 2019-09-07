//$id$
/*
 *    2005/10/30   Zhengyun You      Peking University
 *                 Muc Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

using namespace std;

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "Identifier/MucID.h"
#include "G4Geo/MucG4Geo.h"


MucG4Geo*
MucG4Geo::m_pMucG4Geo = 0L;


MucG4Geo::MucG4Geo()
{ 
  string GdmlManagementPath = getenv("GDMLMANAGEMENTROOT");
  if (GdmlManagementPath == "") cout << "MucG4Geo::GdmlManagementPath not set" << endl;
  string GdmlFile = GdmlManagementPath + string("/dat/Muc.gdml");
  cout << "Construct Muc from GdmlFile " << GdmlFile << endl;
  InitFromGdml( GdmlFile.c_str(), "Muc" );
}

MucG4Geo::~MucG4Geo()
{ }

void
MucG4Geo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  m_TopVolume = GetLogicalVolume("logicalMuc");
  if (!m_TopVolume) cout << "MucG4Geo::InitFromGdml, m_TopVolume not found" << endl;
  else cout << "Muc TopVolume name " << m_TopVolume->GetName() << endl;
  SetDefaultVis();

  m_G4GeoInit = 1;  
}

void 
MucG4Geo::SetDefaultVis()
{
  //std::cout << "begin of set defaultvis" << std::endl;
  
  //std::cout << "end of set defaultvis" << std::endl;
}

MucG4Geo*
MucG4Geo::Instance()
{
  //Get a pointer to the single instance of MucG4Geo
  if(!m_pMucG4Geo){
    m_pMucG4Geo = new MucG4Geo;
    //cout<<"in MucG4Geo:: no MucG4Geo now."<<m_pMucG4Geo<<endl;

  }
  //cout<<"in MucG4Geo:: get MucG4Geo successfully."<<endl;
  return m_pMucG4Geo;
}
