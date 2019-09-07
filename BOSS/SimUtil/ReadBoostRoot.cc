#include "ReadBoostRoot.hh"
#include "globals.hh"
#include <fstream>
using namespace std;

G4String ReadBoostRoot::BoostRoot = "";
G4int ReadBoostRoot::m_mdc = 1;
G4int ReadBoostRoot::m_tof = 1;
G4int ReadBoostRoot::m_emc = 1;
G4int ReadBoostRoot::m_muc = 1;
G4int ReadBoostRoot::m_field = 1;
G4int ReadBoostRoot::m_formatAR=0;
G4int ReadBoostRoot::m_tuning = 0;
G4int ReadBoostRoot::m_hitOut = 0;

ReadBoostRoot::ReadBoostRoot(G4int mdc, G4int tof, G4int emc, G4int muc, G4int field,G4int formatAR,G4int tuning, G4int hitOut)
{
  m_mdc = mdc;
  m_tof = tof;
  m_emc = emc;
  m_muc = muc;
  m_field = field;
  m_formatAR = formatAR;
  m_tuning = tuning;  
  m_hitOut = hitOut;
	/*G4String iniFileName = getenv("G4WORKDIR");
	if(!iniFileName)
	{
		G4Exception("G4WORKDIR environment not set!");
	}
	G4String g4system = getenv("G4SYSTEM");
	if(!g4system)
	{
		G4Exception("G4SYSTEM environment not set!");
	}
	iniFileName += "/tmp/";
	iniFileName += g4system;
	iniFileName += "/boost.ini";
	ifstream fin(iniFileName);
	
	G4String GeometryPath;
	fin>>GeometryPath;
	if(!GeometryPath)
	{
		G4Exception("Fail in reading boost.ini!");
	}*/
	BoostRoot = getenv("SIMUTILROOT");
}

ReadBoostRoot::~ReadBoostRoot()
{}
