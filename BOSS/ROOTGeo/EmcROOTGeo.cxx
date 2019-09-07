//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Emc Geometry General for EventDisplay
 *
 *    2004/12/11   Zhengyun You      Peking University
 *                 named from EmcGeo to EmcROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 *
 *    2005/04/04   Zhengyun You      Peking University
 *                 Emc End geometry added
 */

using namespace std;

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <TGeoManager.h>

#include "ROOTGeo/EmcROOTGeo.h"
//#include <TGeoArb8.h>

EmcROOTGeo::EmcROOTGeo()
{ 
  // Default constructor.
  m_kPhiEc   = 16;
  m_kThetaEc = 35;
  
  for (int part = 0; part < m_kPart; part++) {
    for (int phi = 0; phi < m_kPhiBr; phi++) {
      for (int theta = 0; theta < m_kThetaBr; theta++) {
        m_NodeTheta[part][phi][theta] = 0;
        m_PhysicalCrystal[part][phi][theta] = 0;
      }
    }
  }

  string GdmlManagementPath = getenv("GDMLMANAGEMENTROOT");
  if (GdmlManagementPath == "") cout << "EmcROOTGeo::GdmlManagementPath not set!" << endl;
  string GdmlFile = GdmlManagementPath + string("/dat/Emc.gdml");
  cout << "Construct Emc from GdmlFile " << GdmlFile << endl;
  InitFromGdml( GdmlFile.c_str(), "Emc" );
}

EmcROOTGeo::~EmcROOTGeo()
{ }

void
EmcROOTGeo::InitFromGdml( const char *gdmlFile, const char *setupName )
{
  ReadGdml(gdmlFile, setupName);
  SetNode();
  //SetDefaultVis();

  m_ROOTGeoInit = 1;  
}

void 
EmcROOTGeo::SetNode()
{
  m_Emc = GetTopVolume();
  if(!m_Emc) {
    m_Emc = GetLogicalVolume("logicalEMC");
  }
  if(!m_Emc) std::cout << "m_Emc = 0" << std::endl;
  else cout<<"Find logicalEMC!"<<endl;
  
  for (int part = 0; part < m_kPart; part++) {
    //TGeoNode *nodePart = GetPart(part);
    //if(nodePart) cout<<nodePart->GetName()<<endl;
    int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
    
    for (int phi = 0; phi < nPhi; phi++) {
      //total number of daughters in BSCPhi is 215
      int nTheta = (part == 1 ? 219 : m_kThetaEc);
      TGeoNode *nodePhi = GetPhi(part,phi);
      //if(nodePhi) cout<<nodePhi->GetName()<<endl;
      
      for (int theta = 0; theta < nTheta; theta++) {
        string name = nodePhi->GetDaughter(theta)->GetName();
        int nthe=-1;
        string strthe;
        if(name.find("BSCCasing",0)==7) {
          strthe = name.substr(16,2);
        } else if(name.find("EndCasing",0)==7) {
          strthe = name.substr(19,2);
        }
        
        istringstream thetaBuf(strthe);
        thetaBuf >> nthe;
        
        if(nthe>=0) {
          m_NodeTheta[part][phi][nthe] = nodePhi->GetDaughter(theta);
        }
      }
    }
  }
  //std::cout << "end of set node" << std::endl;
}

void 
EmcROOTGeo::SetDefaultVis()
{
  //std::cout << "begin of set defaultvis" << std::endl;
  int emcColor     = 2;
  int partColor    = 2;
  int phiColor     = 4;
  int thetaColor   = 4;
  int brCrystalColor = 4;
  int ecCrystalColor = 7;

  m_Emc->SetLineColor(emcColor);
  m_Emc->SetVisibility(0);
  
  for (int part = 0; part < m_kPart-1; part++) {
    GetVolumePart(part)->SetLineColor(partColor);
    int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
    for (int phi = 0; phi < nPhi; phi++) {
      GetVolumePhi(part, phi)->SetLineColor(phiColor);
      GetVolumePhi(part, phi)->SetVisibility(0);
    }
    if (part == 1) {
      for (int theta = 0; theta < m_kThetaBr; theta++) {
	GetVolumeTheta(part, 0, theta)->SetLineColor(thetaColor);
	GetVolumeTheta(part, 0, theta)->SetVisibility(0);
	GetVolumeCrystal(part, 0, theta)->SetLineColor(brCrystalColor);
	GetVolumeCrystal(part, 0, theta)->SetVisibility(1);
      }
    }
    else if (part == 0) {
      int iPhi[4] = {0,6,8,7};
      for (int i = 0; i < 4; i++) {
	int phi = iPhi[i];
	int nTheta = (part == 1 ? m_kThetaBr : m_kThetaEc);
	for (int theta = 0; theta < nTheta; theta++) {
	  GetVolumeTheta(part, phi, theta)->SetLineColor(thetaColor);
	  GetVolumeTheta(part, phi, theta)->SetVisibility(0);
	  GetVolumeCrystal(part, phi, theta)->SetLineColor(ecCrystalColor);
	  GetVolumeCrystal(part, phi, theta)->SetVisibility(1);
	}
      }
    }
  }

  for (int part = 0; part < m_kPart; part++) {
    GetPart(part)->SetVisibility(0);
    int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
    for (int phi = 0; phi < nPhi; phi++) {
      GetPhi(part, phi)->SetVisibility(0);
      int nTheta = (part == 1 ? m_kThetaBr : m_kThetaEc);
      for (int theta = 0; theta < nTheta; theta++) {
	GetTheta(part, phi, theta)->SetVisibility(0);
	GetCrystal(part, phi, theta)->SetVisibility(0);
      }
    }
  }
  //std::cout << "end of set defaultvis" << std::endl;
}

void
EmcROOTGeo::SetAllVisible()
{
  /*  for (int part = 0; part < m_kPart-1; part++) {
    int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
    for (int phi = 0; phi < nPhi; phi++) {
      GetVolumePhi(part, phi)->SetVisibility(1);
    }
    if (part == 1) {
      for (int theta = 0; theta < m_kThetaBr; theta++) {
	GetVolumeTheta(part, 0, theta)->SetVisibility(1);
	GetVolumeCrystal(part, 0, theta)->SetVisibility(0);
      }
    }
    else if (part == 0) {
      int iPhi[4] = {0,6,8,7};
      for (int i = 0; i < 4; i++) {
	int phi = iPhi[i];
	int nTheta = (part == 1 ? m_kThetaBr : m_kThetaEc);
	for (int theta = 0; theta < nTheta; theta++) {
	  GetVolumeTheta(part, phi, theta)->SetVisibility(1);
	  GetVolumeCrystal(part, phi, theta)->SetVisibility(0);
	}
      }
    }
  }
  */

  for (int part = 0; part < m_kPart; part++) {
    GetPart(part)->SetVisibility(0);
    int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
    for (int phi = 0; phi < nPhi; phi++) {
      GetPhi(part, phi)->SetVisibility(0);
      int nTheta = (part == 1 ? m_kThetaBr : m_kThetaEc);
      for (int theta = 0; theta < nTheta; theta++) {
	GetTheta(part, phi, theta)->SetVisibility(0);
	GetCrystal(part, phi, theta)->SetVisibility(1);
      }
    }
  }
}

void
EmcROOTGeo::SetQuarterVisible()
{
  /*  for (int part = 1; part < m_kPart-1; part++) {
    for (int phi = 1; phi <= m_kPhiBr; phi++) {
      GetVolumePhi(part, phi)->SetVisibility(0);
    }

     for (int theta = 1; theta <= m_kThetaBr; theta++) {
       GetVolumeTheta(part, theta)->SetVisibility(1);
       GetVolumeCrystal(part, theta)->SetVisibility(0);
    }
  }

  for (int part = 1; part < m_kPart-1; part++) {
    for (int phi = 1; phi <= m_kPhiBr; phi++) {
      GetPhi(part, phi)->SetVisibility(0);
      for (int theta = 1; theta <= m_kThetaBr; theta++) {
	if (phi < m_kPhiBr/4) GetTheta(part, phi, theta)->SetVisibility(0);
	else                  GetTheta(part, phi, theta)->SetVisibility(1);
	GetCrystal(part, phi, theta)->SetVisibility(0);
      }
    }
  }
  */

  for (int part = 0; part < m_kPart; part++) {
    GetPart(part)->SetVisibility(0);
    int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
    for (int phi = 0; phi < nPhi; phi++) {
      GetPhi(part, phi)->SetVisibility(0);
      int nTheta = (part == 1 ? m_kThetaBr : m_kThetaEc);
      for (int theta = 0; theta < nTheta; theta++) {
	GetTheta(part, phi, theta)->SetVisibility(0);
	if ( (part == 1 && (phi >= 0 && phi < nPhi/4)) ||
	     (part != 1) ) {
	  GetCrystal(part, phi, theta)->SetVisibility(1);
	}
	else {
	  GetCrystal(part, phi, theta)->SetVisibility(0);
	}
      }
    }
  }

}

void 
EmcROOTGeo::SetPhysicalNode()
{
  //int brCrystalColor = 4;
  //int ecCrystalColor = 7;

  if (gGeoManager == 0) std::cout << "Create gGeoManager first" << std::endl; 
  else cout<<"gGeoManager success!"<<endl;
  
  TGeoVolume *volEmc = GetLogicalVolume("logicalEMC");
  if(!volEmc) std::cout << "logicalEMC not found !" << std::endl;
      
  //construct bes world
  float m_BesR = 5200;
  float m_BesZ = 5680;    
  TGeoIdentity *identity = new TGeoIdentity();

  TGeoMaterial *mat = new TGeoMaterial("VOID",0,0,0);
  TGeoMedium   *med = new TGeoMedium("MED",1,mat);
  TGeoVolume   *m_Bes = gGeoManager->MakeBox("volBes", med, 
      0.5*m_BesR, 0.5*m_BesR, 0.5*m_BesZ);
  gGeoManager->SetTopVolume(m_Bes);
  m_Bes->AddNode(volEmc, 0, identity); 
  //cout<<"Ndaughters="<<m_Bes->GetNdaughters()<<endl;
  SetChildNo(m_Bes->GetNdaughters()-1);
        
  gGeoManager->SetDrawExtraPaths();  // draw physical node or not;
  gGeoManager->CloseGeometry();
  gGeoManager->SetNsegments(20);
      

  TGeoNode *bes = gGeoManager->GetTopNode();
  TGeoNode *emc = bes->GetDaughter(0);
  
  for (int part = 0; part < m_kPart; part++) {
    TGeoNode *nodePart = GetPart(part);
    int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
    for (int phi = 0; phi < nPhi; phi++) {
      TGeoNode *nodePhi = GetPhi(part, phi);
      int nTheta = (part == 1 ? m_kThetaBr : m_kThetaEc);
      for (int theta = 0; theta < nTheta; theta++) {
        //cout<<part<<"\t"<<phi<<"\t"<<theta<<endl;
        TGeoNode *nodeTheta = GetTheta(part, phi, theta);
        TGeoNode *nodeCrystal = GetCrystal(part, phi, theta);
        TString strPath =    TString("/") + bes->GetName() +
                             TString("/") + emc->GetName() +
                             TString("/") + nodePart->GetName() +
                             TString("/") + nodePhi->GetName() +
                             TString("/") + nodeTheta->GetName() +
                             TString("/") + nodeCrystal->GetName();
        m_PhysicalCrystal[part][phi][theta] = gGeoManager->MakePhysicalNode(strPath);
        
        //m_PhysicalCrystal[part][phi][theta]->SetVisibility(0);
        //m_PhysicalCrystal[part][phi][theta]->SetIsVolAtt(kFALSE);
        //if (part == 1) m_PhysicalCrystal[part][phi][theta]->SetLineColor(brCrystalColor);
        //else           m_PhysicalCrystal[part][phi][theta]->SetLineColor(ecCrystalColor);
      }
    }
  }
}

int
EmcROOTGeo::GetPartNb()
{
  int part = m_kPart;

  return part;
}

int
EmcROOTGeo::GetThetaNb(int part)
{
  int thetaNb = m_kThetaBr;
  if (part != 1) thetaNb = m_kThetaEc;
  
  return thetaNb;;
}

int
EmcROOTGeo::GetPhiNb(int part) 
{
  int phiNb = m_kPhiBr;
  if (part != 1) phiNb = m_kPhiEc;

  return phiNb;
}
 
TGeoVolume* 
EmcROOTGeo::GetVolumePart( int part )
{
  // only part0 and part1 logical volume.
  std::stringstream osname;
  osname << "logical" << "Emc" << "Part" << part;  
  if (part == 0 || part == 1) {
    return GetLogicalVolume( osname.str() );
  }
  else {
    std::cout << "No volume " << osname.str() << std::endl;
    return 0;
  }
}

TGeoVolume* 
EmcROOTGeo::GetVolumePhi( int part, int phi )
{
  // only part0phi0-15, part1phi0-119
  std::stringstream osname;
  osname << "logical" << "Emc" << "Part" << part << "Phi" << phi;
  if ( (part == 0 && phi >= 0 && phi < 16) ||
       (part == 1 && phi >= 0 && phi < 120) ) {
    return GetLogicalVolume( osname.str() );
  }
  else {
    std::cout << "No volume " << osname.str() << std::endl;
    return 0;
  }
}   

TGeoVolume*
EmcROOTGeo::GetVolumeTheta( int part, int phi, int theta )
{
  // part0phi0theta0-34, part0phi6theta0-34, part0phi8theta0-34, part0phi7theta0-34
  // part1theta0-43
  std::stringstream osname;
  if (part == 1 && theta >= 0 && theta < 44) {
    osname << "logical" << "Emc" << "Part" << part << "Theta" << theta;
    return GetLogicalVolume( osname.str() );
  }
  else if ( part == 0 && theta >= 0 && theta < 35 &&
	    (phi == 0 || phi == 6 || phi == 8 || phi == 7) ) {
    osname << "logical" << "Emc" << "Part" << part << "Phi" << phi << "Theta" << theta;
    return GetLogicalVolume( osname.str() );
  }
  else {
    std::cout << "No volume " << osname.str() << std::endl;
    return 0;
  }
}

TGeoVolume*
EmcROOTGeo::GetVolumeCrystal( int part, int phi, int theta )
{
  // part0phi0theta0-34, part0phi6theta0-34, part0phi8theta0-34, part0phi7theta0-34
  // part1theta0-43
  std::stringstream osname;
  if (part == 1 && theta >= 0 && theta < 44) {
    osname << "logical" << "Emc" << "Part" << part << "Theta" << theta << "Crystal";
    return GetLogicalVolume( osname.str() );
  }
  else if ( part == 0 && theta >= 0 && theta < 35 &&
	    (phi == 0 || phi == 6 || phi == 8 || phi == 7) ) {
    osname << "logical" << "Emc" << "Part" << part << "Phi" << phi << "Theta" << theta << "Crystal";
    return GetLogicalVolume( osname.str() );
  }
  else {
    std::cout << "No volume " << osname.str() << std::endl;
    return 0;
  }
}

TGeoNode*
EmcROOTGeo::GetPart( int part )
{
  std::stringstream osname;
  if(part==1) {
    osname << "pv_logicalBSCWorld_2";
  } else {
    osname << "pv_logicalEndWorld_" << 1-part/2;
  }
  //osname << "pv_" << "logical" << "Emc" << "Part" << (part == 2 ? (part -2) : part) << "_" << part; 
  
  return GetNode( osname.str() );
}

TGeoNode*
EmcROOTGeo::GetPhi( int part, int phi )
{
  int phiNb = -1;
  std::stringstream osname;
  if(part==1) { //barrel
    phiNb = 308-phi;
    osname << "pv_logicalBSCPhi_" << phiNb;
  } else {  //endcap
    if(phi>=0&&phi<6) {
      osname << "pv_logicalEndPhi0_" << 15-phi;
    } else if(phi>=8&&phi<14) {
      osname << "pv_logicalEndPhi0_" << 17-phi;
    } else if(phi==6||phi==14) {
      osname << "pv_logicalEndPhi1_" << (30-phi)/8;
    } else if(phi==7||phi==15) {
      osname << "pv_logicalEndPhi2_" << (15-phi)/8;
    }
  }
  //osname << "pv_" << "logical" << "Emc" << "Part" << (part == 2 ? (part -2) : part) << "Phi" << phi << "_" << phi; 
  
  return GetNode( osname.str() );
}

TGeoNode*
EmcROOTGeo::GetTheta( int part, int phi, int theta )
{
  if (m_NodeTheta[part][phi][theta] != 0) {
    return m_NodeTheta[part][phi][theta];
  }
  else {
    std::cout << "Node: " << "Part" << part << "Phi" << phi << "Theta" << theta << " not found" << std::endl;
    return 0;
  } 
}

TGeoNode*
EmcROOTGeo::GetCrystal( int part, int phi, int theta )
{
  // in fact, br only 44 crystal nodes;
  return GetTheta(part, phi, theta)->GetDaughter(0);
}

TGeoPhysicalNode*
EmcROOTGeo::GetPhysicalCrystal( int part, int phi, int theta )
{
  if (m_PhysicalCrystal[part][phi][theta] != 0) {
    return m_PhysicalCrystal[part][phi][theta];
  }
  else {
    std::cout << "PhysicalNode: " << "Part" << part << "Phi" << phi << "Theta" << theta << " not found" << std::endl;
    return 0;
  } 
}

