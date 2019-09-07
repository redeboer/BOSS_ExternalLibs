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

#include <TGeoManager.h>
#include <TGeoArb8.h>
#include <TMath.h>

#include "BesVisLib/EmcROOTGeo.h"
#include "Identifier/EmcID.h"
#include "BesVisLib/BesEvent.h"
#include "BesVisLib/BesView.h"
#include "RawEvent/RawDataUtil.h"


EmcROOTGeo::EmcROOTGeo()
        : SubDetectorROOTGeo()
{
    // Default constructor.
    m_kPhiEc    = 96;
    m_kThetaEc  = 6;
    m_kSectorEc = 16;
    m_kNbEc     = 35;
    m_kRealNbEc = 30;

    for (int part = 0; part < m_kPart; part++) {
        for (int phi = 0; phi < m_kPhiBr; phi++) {
            m_NodePhi[part][phi] = 0;
            for (int theta = 0; theta < m_kThetaBr; theta++) {
                m_NodeTheta[part][phi][theta] = 0;
                m_NodeTheta2[part][phi][theta] = 0;
                m_PhysicalCrystal[part][phi][theta] = 0;
                m_PhysicalCrystal2[part][phi][theta] = 0;
                m_Emc2DCrystal[part][phi][theta] = 0;
            }
        }
    }
    m_PhyNodeOrgArray = new TObjArray();

    m_EmcColor     = 4;
    m_partColor    = 4;
    m_phiColor     = 4;
    m_thetaColor   = 4;
    m_brCrystalColor = 861;   //Long Peixun's update: deep blue -> azure
    m_ecCrystalColor = 866;   //Long Peixun's update: cyan -> light blue
}

EmcROOTGeo::~EmcROOTGeo()
{
    //Long Peixun's update: Add destructor
    cout << "delete old EmcROOTGeo" << endl;
    for (int part = 0; part < GetPartNb(); part++)
    {
        for (int theta = 0; theta < GetThetaNb(part); theta++)
        {
            for (int phi = 0; phi < GetPhiNb(part, theta); phi++)
            {
                delete m_Emc2DCrystal[part][phi][theta];
            }
        }
    }
    delete m_PhyNodeOrgArray;
}

void
EmcROOTGeo::InitFromGDML( const char *gdmlFile, const char *setupName )
{
    m_ROOTGeoInit = 2;

    ReadGdml(gdmlFile, setupName);
    SetNode();
}

void
EmcROOTGeo::InitFromROOT( TGeoVolume *vol )
{
    m_ROOTGeoInit = 1;

    SetVolumeEmc(vol);
    SetNode();
}

void
EmcROOTGeo::Init2DGeometry()
{
    if (m_ROOTGeoInit != 1 && m_ROOTGeoInit != 2) {
        cout << "EmcROOTGeo::Init2DGeometry, ROOT Geometry not Initialized yet!" << endl;
        return;
    }
    m_2DGeoInit = 1;

    Double_t local[3] = {0.0, 0.0, 0.0};
    Double_t master[3] = {0.0, 0.0, 0.0};
    //Int_t    nPoints = 8;
    //Double_t  P[300] = {0.0};
    //Double_t  center[3] = {0.0, 0.0, 0.0};
    TString  name;
    TString  title;

    // Emc crystals
    for (int part = 0; part < GetPartNb(); part++) {
        for (int theta = 0; theta < GetThetaNb(part); theta++) {
            for (int phi = 0; phi < GetPhiNb(part, theta); phi++) {
                TGeoPhysicalNode *phyNode = 0;
                phyNode = GetPhysicalCrystal(part, phi, theta);
                if (part == 1) name = TString("Barrel");
                else {
                    if (part == 0)      name = TString("EastEc");
                    else if (part == 2) name = TString("WestEc");
                }
                name = TString("Emc Part");
                name += part;
                name += " Theta";
                if (part == 1) name += m_kThetaBr - 1 - theta;
                else name += theta;
                name += " Phi";
                name += phi;

                char data[100];
                sprintf(data, "Emc part %i, #theta %i, #phi %i", part, theta, phi);
                title = TString( data );

                TGeoArb8 *crystalShape = (TGeoArb8*)phyNode->GetShape();
                Double_t *localArb8Point, masterArb8Point[24*2];
                localArb8Point = crystalShape->GetVertices();
                for (Int_t i = 0; i < 8; i++) {
                    local[0] = localArb8Point[2*i];
                    local[1] = localArb8Point[2*i+1];
                    if (i < 4) local[2] = crystalShape->GetDz() * (-1.0);
                    else local[2] = crystalShape->GetDz();

                    phyNode->GetMatrix(-1*phyNode->GetLevel())->LocalToMaster(local, &master[0]);
		    // transform to top
                    for (Int_t j = 0; j < 3; j++) {
                        masterArb8Point[3*i+j] = master[j];
                    }
                }

                TGeoPhysicalNode *phyNode2 = 0;
                phyNode2 = GetPhysicalCrystal2(part, phi, theta);
                if (phyNode2 != 0) {
                    crystalShape = (TGeoArb8*)phyNode2->GetShape();
                    localArb8Point = crystalShape->GetVertices();
                    for (Int_t i = 0; i < 8; i++) {
                        local[0] = localArb8Point[2*i];
                        local[1] = localArb8Point[2*i+1];
                        if (i < 4) local[2] = crystalShape->GetDz() * (-1.0);
                        else local[2] = crystalShape->GetDz();
                        phyNode2->GetMatrix(-1*phyNode->GetLevel())
                        ->LocalToMaster(local, &master[0]); // transform to top
                        for (Int_t j = 0; j < 3; j++) {
                            masterArb8Point[24+3*i+j] = master[j];
                        }
                    }
                }

                if (phyNode2 == 0)
                    m_Emc2DCrystal[part][phi][theta] =
                        new Emc2DCrystal(name, title, 8, &masterArb8Point[0], part, theta);
                else
                    m_Emc2DCrystal[part][phi][theta] =
                        new Emc2DCrystal(name, title, 16, &masterArb8Point[0], part, theta);
            }
        }
    }
}

void
EmcROOTGeo::SetNode()
{
    // Set NodePhi
    if (m_ROOTGeoInit == 2) { // from GDML

        m_Emc = GetTopVolume();
        if (!m_Emc) std::cout << "m_Emc = 0" << std::endl;

    }
    else if (m_ROOTGeoInit == 1) { // from ROOT object

        for (int part = 0; part < m_kPart; part++) {
            int ipart = 0;
            if (part == 1) ipart = 2;
            if (part == 0) ipart = 1;

            m_NodePart[part] = m_Emc->GetNode(ipart);

            if (part == 1) {
                for (int phi = 0; phi < m_kPhiBr; phi++) {
                    int iphi = 0;
                    iphi = 308 - phi;

                    m_NodePhi[part][phi] = GetPart(part)->GetVolume()->GetNode(iphi);
                }
            }
            else {
                int endphilist[16]= {15,7,14,6,13,12,11,10,9,8,5,4,3,2,1,0};
                int endphipos[16] = {15,14,13,12,11,10,3,1,9,8,7,6,5,4,2,0};
                //pos of phi in endworld
                for (int sector = 0; sector < m_kSectorEc; sector++) {
                    m_NodePhi[part][sector] =
                        GetPart(part)->GetVolume()->GetNode(endphipos[sector]);
                }
            }
        }
    }

    // Set NodeTheta
    for (int phi = 0; phi < m_kPhiBr; phi++) {
        int theta=0;
        for (int idaughter = 0;  idaughter< GetPhi(1, phi)->GetNdaughters(); idaughter++) {
            TString name = GetPhi(1, phi)->GetDaughter(idaughter)->GetName();
            if (name.Contains("logicalBSCCasing")){

                m_NodeTheta[1][phi][theta] = GetPhi(1, phi)->GetDaughter(idaughter);
                theta++;
            }
        }
    }

    for (int part = 0; part < m_kPart; part++) {
        if (part == 1) continue;
        for (int sector = 0; sector < m_kSectorEc; sector++) {
            for (int nb = 0; nb < m_kNbEc; nb++) {
                int theta, phi;
                if (nb < m_kRealNbEc) {
                    ComputeThetaPhi(part, sector, nb, theta, phi);
                    m_NodeTheta[part][phi][theta] = GetPhi(part, sector)->GetDaughter(nb);
// 	  std::cout << part << " sector " << sector << " nb " << nb
// 	     << " phi " << phi << " theta " << theta
// 	     << " name " << m_NodeTheta[part][phi][theta]->GetName() << std::endl;
                }
                else {
                    ComputeThetaPhi(part, sector, ComputeEndCopyNb(nb), theta, phi);
                    m_NodeTheta2[part][phi][theta] = GetPhi(part, sector)->GetDaughter(nb);
// 	  std::cout << "Additional " << part << " sector " << sector << " nb " << nb
// 	     << " phi " << phi << " theta " << theta
// 	     << " name " << m_NodeTheta2[part][phi][theta]->GetName() << std::endl;
                }
            }
        }
    }

    //std::cout << "EmcROOTGeo::SetNode, end of set node" << std::endl;
}

void
EmcROOTGeo::SetVolumeAppendInVis()
{

    //-------------------------------------------------------------
    //Barrel
    //GetLogicalVolume
    TGeoVolume *logicRear,*logicOrgGlass,*logicRearCasing,*logicAlPlate,*logicPD,*logicPreAmpBox,*logicAirInPABox,*logicHangingPlate,*logicWaterPipe;
    TGeoVolume *logicCable,*logicOCGirder,*logicSupportBar,*logicSupportBar1,*logicEndRing,*logicGear,*logicTaperRing1,*logicTaperRing2,*logicTaperRing3;


    logicRear = GetLogicalVolume("logicalRearBox");
    logicOrgGlass = GetLogicalVolume("logicalOrganicGlass");
    logicRearCasing = GetLogicalVolume("logicalRearCasing");
    logicAlPlate = GetLogicalVolume("logicalAlPlate");
    logicPD = GetLogicalVolume("logicalPD");
    logicPreAmpBox = GetLogicalVolume("logicalPreAmpBox");
    logicAirInPABox = GetLogicalVolume("logicalAirInPABox");
    logicHangingPlate = GetLogicalVolume("logicalHangingPlate");
    logicWaterPipe = GetLogicalVolume("logicalWaterPipe");

    for (int i = 0; i < 44; i++){

        std::ostringstream osnameBSCCable1;
        osnameBSCCable1 << "logicalBSCCable_1_"<<i;
        logicCable = GetLogicalVolume( osnameBSCCable1.str() );
        if (logicCable)
            logicCable->SetVisibility(0);

        std::ostringstream osnameBSCCable2;
        osnameBSCCable2 << "logicalBSCCable_2_"<<i;
        logicCable = GetLogicalVolume( osnameBSCCable2.str() );
        if (logicCable)
            logicCable->SetVisibility(0);

        std::ostringstream osnameOCGirder1;
        osnameOCGirder1 <<"logicalOpenningCutGirder_1_"<<i;
        logicOCGirder = GetLogicalVolume( osnameOCGirder1.str() );
        if (logicOCGirder)
            logicOCGirder->SetVisibility(0);

        std::ostringstream osnameOCGirder2;
        osnameOCGirder2 <<"logicalOpenningCutGirder_2_"<<i;
        logicOCGirder = GetLogicalVolume( osnameOCGirder2.str() );
        if (logicOCGirder)
            logicOCGirder->SetVisibility(0);

        std::ostringstream osnameOCGirder3;
        osnameOCGirder3 <<"logicalOpenningCutGirder_3_"<<i;
        logicOCGirder = GetLogicalVolume( osnameOCGirder3.str() );
        if (logicOCGirder)
            logicOCGirder->SetVisibility(0);

        std::ostringstream osnameOCGirder4;
        osnameOCGirder4 <<"logicalOpenningCutGirder_4_"<<i;
        logicOCGirder = GetLogicalVolume( osnameOCGirder4.str() );
        if (logicOCGirder)
            logicOCGirder->SetVisibility(0);
    }

    //-------------------------------------------------------------
    //Support system
    logicSupportBar = GetLogicalVolume("logicalSupportBar0");
    logicSupportBar1 = GetLogicalVolume("logicalSupportBar1");
    logicEndRing = GetLogicalVolume("logicalEndRing");
    logicGear = GetLogicalVolume("logicalGear");
    logicTaperRing1 = GetLogicalVolume("logicalTaperRing1");
    logicTaperRing2 = GetLogicalVolume("logicalTaperRing2");
    logicTaperRing3 = GetLogicalVolume("logicalTaperRing3");

    if (logicRear)
        logicRear->SetVisibility(0);
    if (logicOrgGlass)
        logicOrgGlass->SetVisibility(0);
    if (logicRearCasing)
        logicRearCasing->SetVisibility(0);
    if (logicAlPlate)
        logicAlPlate->SetVisibility(0);
    if (logicPD)
        logicPD->SetVisibility(0);
    if (logicPreAmpBox)
        logicPreAmpBox->SetVisibility(0);
    if (logicAirInPABox)
        logicAirInPABox->SetVisibility(0);
    if (logicHangingPlate)
        logicHangingPlate->SetVisibility(0);
    if (logicWaterPipe)
        logicWaterPipe->SetVisibility(0);
    if (logicGear)
        logicGear->SetVisibility(0);
    if (logicTaperRing1)
        logicTaperRing1->SetVisibility(0);
    if (logicTaperRing2)
        logicTaperRing2->SetVisibility(0);
    if (logicTaperRing3)
        logicTaperRing3->SetVisibility(0);
    if (logicSupportBar)
        logicSupportBar->SetVisibility(0);
    if (logicSupportBar1)
        logicSupportBar1->SetVisibility(0);
    if (logicEndRing)
        logicEndRing->SetVisibility(0);
}

void
EmcROOTGeo::SetVolumeDefaultVis()
{

    SetVolumeAppendInVis();


    //std::cout << "begin of set defaultvis" << std::endl;
    m_Emc->SetLineColor(m_EmcColor);
    m_Emc->SetVisibility(0);

    for (int part = 0; part < m_kPart-1; part++) {
        GetVolumePart(part)->SetLineColor(m_partColor);
        int nPhi = (part == 1 ? m_kPhiBr : m_kSectorEc);
        for (int phi = 0; phi < nPhi; phi++) {
            GetVolumePhi(part, phi)->SetLineColor(m_phiColor);
            GetVolumePhi(part, phi)->SetVisibility(0);
        }
        if (part == 1) {
            for (int theta = 0; theta < m_kThetaBr; theta++) {
                GetVolumeTheta(part, 0, theta)->SetLineColor(m_thetaColor);
                GetVolumeTheta(part, 0, theta)->SetVisibility(0);
                GetVolumeCrystal(part, 0, theta)->SetLineColor(m_brCrystalColor);
                GetVolumeCrystal(part, 0, theta)->SetVisibility(0);
            }
        }
        else if (part == 0) {
            int iPhi[3] = {0,1,2};
            for (int i = 0; i < 3; i++) {
                int phi = iPhi[i];
                int nTheta = (part == 1 ? m_kThetaBr : m_kNbEc);
                for (int theta = 0; theta < nTheta; theta++) {
                    GetVolumeTheta(part, phi, theta)->SetLineColor(m_thetaColor);
                    GetVolumeTheta(part, phi, theta)->SetVisibility(0);
                    GetVolumeCrystal(part, phi, theta)->SetLineColor(m_ecCrystalColor);
                    GetVolumeCrystal(part, phi, theta)->SetVisibility(0);
                }
            }
        }
    }

    // phi and sector
    for (int part = 0; part < m_kPart; part++) {
        GetPart(part)->SetVisibility(0);
        int nPhi = (part == 1 ? m_kPhiBr : m_kSectorEc);
        for (int phi = 0; phi < nPhi; phi++) {
            GetPhi(part, phi)->SetVisibility(0);
        }
    }

    // theta with real phi
    for (int part = 0; part < m_kPart; part++) {
        for (int theta = 0; theta < GetThetaNb(part); theta++) {
            for (int phi = 0; phi < GetPhiNb(part, theta); phi++) {
                //cout << "part " << part << " theta " << theta << " phi " << phi << endl;
                TGeoNode *nodeTheta = GetTheta(part, phi, theta);
                if (nodeTheta)   nodeTheta->SetVisibility(0);
                TGeoNode *nodeCrystal = GetCrystal(part, phi, theta);
                if (nodeCrystal) nodeCrystal->SetVisibility(0);

                TGeoNode *nodeTheta2 = GetTheta2(part, phi, theta);
                if (nodeTheta2)   nodeTheta2->SetVisibility(0);
                TGeoNode *nodeCrystal2 = GetCrystal2(part, phi, theta);
                if (nodeCrystal2) nodeCrystal2->SetVisibility(0);
            }
        }
    }
    //std::cout << "end of set defaultvis" << std::endl;

}

void
EmcROOTGeo::SetAllVisible()
{

    SetVolumeAppendInVis();


    //std::cout << "begin of set defaultvis" << std::endl;
    m_Emc->SetLineColor(m_EmcColor);
    m_Emc->SetVisibility(0);

    for (int part = 0; part < m_kPart-1; part++) {
        GetVolumePart(part)->SetLineColor(m_partColor);
        int nPhi = (part == 1 ? m_kPhiBr : m_kSectorEc);
        for (int phi = 0; phi < nPhi; phi++) {
            GetVolumePhi(part, phi)->SetLineColor(m_phiColor);
            GetVolumePhi(part, phi)->SetVisibility(1);
        }
        if (part == 1) {
            for (int theta = 0; theta < m_kThetaBr; theta++) {
                GetVolumeTheta(part, 0, theta)->SetLineColor(m_thetaColor);
                GetVolumeTheta(part, 0, theta)->SetVisibility(1);
                GetVolumeCrystal(part, 0, theta)->SetLineColor(m_brCrystalColor);
                GetVolumeCrystal(part, 0, theta)->SetVisibility(1);
            }
        }
        else if (part == 0) {
            int iPhi[3] = {0,1,2};
            for (int i = 0; i < 3; i++) {
                int phi = iPhi[i];
                int nTheta = (part == 1 ? m_kThetaBr : m_kNbEc);
                for (int theta = 0; theta < nTheta; theta++) {
                    GetVolumeTheta(part, phi, theta)->SetLineColor(m_thetaColor);
                    GetVolumeTheta(part, phi, theta)->SetVisibility(1);
                    GetVolumeCrystal(part, phi, theta)->SetLineColor(m_ecCrystalColor);
                    GetVolumeCrystal(part, phi, theta)->SetVisibility(1);
                }
            }
        }
    }

    // phi and sector
    for (int part = 0; part < m_kPart; part++) {
        GetPart(part)->SetVisibility(1);
        int nPhi = (part == 1 ? m_kPhiBr : m_kSectorEc);
        for (int phi = 0; phi < nPhi; phi++) {
            GetPhi(part, phi)->SetVisibility(1);
        }
    }

    // theta with real phi
    for (int part = 0; part < m_kPart; part++) {
        for (int theta = 0; theta < GetThetaNb(part); theta++) {
            for (int phi = 0; phi < GetPhiNb(part, theta); phi++) {
                //cout << "part " << part << " theta " << theta << " phi " << phi << endl;
                TGeoNode *nodeTheta = GetTheta(part, phi, theta);
                if (nodeTheta)   nodeTheta->SetVisibility(1);
                TGeoNode *nodeCrystal = GetCrystal(part, phi, theta);
                if (nodeCrystal) nodeCrystal->SetVisibility(1);

                TGeoNode *nodeTheta2 = GetTheta2(part, phi, theta);
                if (nodeTheta2)   nodeTheta2->SetVisibility(1);
                TGeoNode *nodeCrystal2 = GetCrystal2(part, phi, theta);
                if (nodeCrystal2) nodeCrystal2->SetVisibility(1);
            }
        }
    }
    //std::cout << "end of set defaultvis" << std::endl;

}

void
EmcROOTGeo::SetQuarterVisible()
{
    /*
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


    for (int phi = GetPhiNb(1)/4; phi < GetPhiNb(1); phi++) {
      for (int theta = 0; theta < GetThetaNb(1); theta++) {
        GetPhysicalCrystal(1, phi, theta)->SetVisibility(1);
      }
    }
     
    for (int theta = 0; theta < GetThetaNb(0); theta++) {
      for (int i = 0; i < 3; i++) {
        GetPhysicalCrystal(0, i, theta)->SetVisibility(1);
      }
      for (int i = 7; i < 16; i++) {
        GetPhysicalCrystal(0, i, theta)->SetVisibility(1);
      }
      for (int i = 0; i < 7; i++) {
        GetPhysicalCrystal(2, i, theta)->SetVisibility(1);
      }
      for (int i = 11; i < 16; i++) {
        GetPhysicalCrystal(2, i, theta)->SetVisibility(1);
      }
    }
    */
}

void
EmcROOTGeo::SetHalfVisible()
{
    /*
    for (int theta = 0; theta < GetThetaNb(1); theta++) {
      for (int phi = 0; phi < GetPhiNb(1); phi++) {
        if (phi >= GetPhiNb(1)/4 && phi < GetPhiNb(1)*3/4) {
      GetPhysicalCrystal(1, phi, theta)->SetVisibility(0);
        }
        else {
      GetPhysicalCrystal(1, phi, theta)->SetVisibility(1);
        }
      }
    }
     
    for (int theta = 0; theta < GetThetaNb(0); theta++) {
      for (int i = 0; i < 16; i++) {
        if (i >= 7 && i <= 14) {
      GetPhysicalCrystal(0, i, theta)->SetVisibility(0);
        }
        else {
      GetPhysicalCrystal(0, i, theta)->SetVisibility(1);
        }
      }
    }

    for (int theta = 0; theta < GetThetaNb(2); theta++) {
      for (int i = 0; i < 16; i++) {
        if ((i >= 0 && i < 7) || i == 15) {
      GetPhysicalCrystal(2, i, theta)->SetVisibility(0);
        }
        else {
      GetPhysicalCrystal(2, i, theta)->SetVisibility(1);
        }
      }
    }
    */
}

void
EmcROOTGeo::SetNoEndVisible()
{
    /*
    for (int theta = 0; theta < GetThetaNb(1); theta++) {
      for (int phi = 0; phi < GetPhiNb(1); phi++) {
        GetPhysicalCrystal(1, phi, theta)->SetVisibility(1);
      }
    }
    */
}

void
EmcROOTGeo::SetPhysicalNode()
{
    if (gGeoManager == 0) std::cout << "Create gGeoManager first" << std::endl;
    TGeoNode *bes = gGeoManager->GetTopNode();
    std::cout << "Emc m_childNo " << m_childNo << std::endl;
    TGeoNode *emc = bes->GetDaughter(2); //2->0

    // Barrel
    int part = 1;
    TGeoNode *nodePart = GetPart(part);
    for (int phi = 0; phi < GetPhiNb(part, 0); phi++) {
        TGeoNode *nodePhi = GetPhi(part, phi);
        for (int theta = 0; theta < GetThetaNb(part); theta++) {
            TGeoNode *nodeTheta = GetTheta(part, phi, theta);
            TGeoNode *nodeCrystal = GetCrystal(part, phi, theta);
            m_PhysicalCrystal[part][phi][theta] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                                  TString("/") + emc->GetName() +
                                                  TString("/") + nodePart->GetName() +
                                                  TString("/") + nodePhi->GetName() +
                                                  TString("/") + nodeTheta->GetName() +
                                                  TString("/") + nodeCrystal->GetName() );
            m_PhysicalCrystal[part][phi][theta]->SetVisibility(0);
            m_PhysicalCrystal[part][phi][theta]->SetIsVolAtt(kFALSE);
            m_PhysicalCrystal[part][phi][theta]->SetLineColor(m_brCrystalColor);

        }
    }

    // EC
    for (part = 0; part < m_kPart; part++) {
        if (part == 1) continue;
        nodePart = GetPart(part);
        for (int sector = 0; sector < m_kSectorEc; sector++) {
            TGeoNode *nodeSector = GetPhi(part, sector);
            for (int nb = 0; nb < m_kNbEc; nb++) {
                TGeoNode *nodeNb = nodeSector->GetDaughter(nb);
                TGeoNode *nodeCrystal = nodeNb->GetDaughter(0);

                int theta, phi;
                if (nb < m_kRealNbEc) {
                    ComputeThetaPhi(part, sector, nb, theta, phi);
                    m_PhysicalCrystal[part][phi][theta] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                                          TString("/") + emc->GetName() +
                                                          TString("/") + nodePart->GetName() +
                                                          TString("/") + nodeSector->GetName() +
                                                          TString("/") + nodeNb->GetName() +
                                                          TString("/") + nodeCrystal->GetName() );
                    m_PhysicalCrystal[part][phi][theta]->SetVisibility(0);
                    m_PhysicalCrystal[part][phi][theta]->SetIsVolAtt(kFALSE);
                    m_PhysicalCrystal[part][phi][theta]->SetLineColor(m_ecCrystalColor);
                    //std::cout << part << " " << phi << " " << theta << " " << m_PhysicalCrystal[part][phi][theta]->GetName() << std::endl;
                }
                else {
                    ComputeThetaPhi(part, sector, ComputeEndCopyNb(nb), theta, phi);
                    m_PhysicalCrystal2[part][phi][theta] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                                           TString("/") + emc->GetName() +
                                                           TString("/") + nodePart->GetName() +
                                                           TString("/") + nodeSector->GetName() +
                                                           TString("/") + nodeNb->GetName() +
                                                           TString("/") + nodeCrystal->GetName() );
                    m_PhysicalCrystal2[part][phi][theta]->SetVisibility(0);
                    m_PhysicalCrystal2[part][phi][theta]->SetIsVolAtt(kFALSE);
                    m_PhysicalCrystal2[part][phi][theta]->SetLineColor(m_ecCrystalColor);
                    //std::cout << "Additional " << part << " " << phi << " " << theta << " " << m_PhysicalCrystal2[part][phi][theta]->GetName() << std::endl;
                }
            }
        }
    }

    SetDetector();
}

//Long Peixun's update: Remove annotation
void
EmcROOTGeo::SetPhysicalDefaultVis()
{
    for (int part = 0; part < m_kPart; part++)
    {
        for (int theta = 0; theta < GetThetaNb(part); theta++)
        {
            for (int phi = 0; phi < GetPhiNb(part, theta); phi++)
            {
                TGeoPhysicalNode *phyCrystal  = GetPhysicalCrystal(part, phi, theta);
                TGeoPhysicalNode *phyCrystal2 = GetPhysicalCrystal2(part, phi, theta);
                if (part == 1)
                {
                    if (phyCrystal)
                    {
                        phyCrystal->SetIsVolAtt(kFALSE);
                        phyCrystal->SetLineColor(m_brCrystalColor);
                    }
                }
                else
                {
                    if (phyCrystal)
                    {
                        phyCrystal->SetIsVolAtt(kFALSE);
                        phyCrystal->SetLineColor(m_ecCrystalColor);
                    }
                    if (phyCrystal2)
                    {
                        phyCrystal2->SetIsVolAtt(kFALSE);
                        phyCrystal2->SetLineColor(m_ecCrystalColor);
                    }
                }
            }
        }
    }
    //std::cout << "end of set defaultvis" << std::endl;
}


void
EmcROOTGeo::SetDetector()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());
    //if (view) cout << "viewVisFull3DEmc " << view->GetVisFull3DEmc() << endl;

    m_DetectorsArray->Clear();

    // Barrel
    int part = 1;
    for (int theta = 0; theta < GetThetaNb(part); theta++) {
        for (int phi = 0; phi < GetPhiNb(part, theta); phi++) {
            TGeoPhysicalNode *phyNode = GetPhysicalCrystal(part, phi, theta);
            if (phyNode) {
                phyNode->SetVisibility(0); // set all invisible before set any visible
                if ( phi >= 0 && phi < m_kPhiBr*4/4
                        //|| (theta == 0 || theta == 43) && phi >= m_kPhiBr/4 || phi == 0
                   ) {
                    m_DetectorsArray->Add( phyNode );
                }
                else if (view && view->GetVisFull3DEmc()) {
                    m_DetectorsArray->Add( phyNode );
                }
            }
        }
    }

    // End cap
    for (int part = 0; part < m_kPart; part++) {
        if (part == 1) continue;
        for (int theta = 0; theta < GetThetaNb(part); theta++) {
            for (int phi = 0; phi < GetPhiNb(part, theta); phi++) {
                TGeoPhysicalNode *phyNode = GetPhysicalCrystal(part, phi, theta);
                if (phyNode) {
                    phyNode->SetVisibility(0); // set all invisible before set any visible
                    if (part == 2 && phi <= GetPhiNb(part, theta)||
                            part == 0 && phi <= GetPhiNb(part, theta) ) {
                        m_DetectorsArray->Add( phyNode );
                    }
                    else if (view && view->GetVisFull3DEmc()) {
                        m_DetectorsArray->Add( phyNode );
                    }
                }

                phyNode = 0;
                phyNode = GetPhysicalCrystal2(part, phi, theta);
                if (phyNode) {
                    phyNode->SetVisibility(0); // set all invisible before set any visible
                    if (part == 2 && phi <= GetPhiNb(part, theta)||
                            part == 0 && phi <= GetPhiNb(part, theta) ) {
                        m_DetectorsArray->Add( phyNode );
                    }
                    else if (view && view->GetVisFull3DEmc()) {
                        m_DetectorsArray->Add( phyNode );
                    }
                }
            }
        }
    }

    //std::cout << "EmcROOTGeo::SetDetector(), end of set detector " << std::endl;
}

void
EmcROOTGeo::SetHits()
{
    // set previous event hits to default vis
    //cout << "m_HitsArray->GetEntries(): " << m_HitsArray->GetEntries() << endl;
    //cout << "m_PhyNodeOrgArray->GetEntries(): " << m_PhyNodeOrgArray->GetEntries() << endl;
    //cout << "m_2DHitsArray->GetEntries(): " << m_2DHitsArray->GetEntries() << endl;
    for (int i = 0; i < m_HitsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_HitsArray->At(i);
        //cout << phyNode->GetName() << endl;
        int color = m_brCrystalColor;
        int part = GetPart(phyNode);
        if (part == 0 || part == 2)
            color = m_ecCrystalColor;

        phyNode->SetLineColor(color);
        phyNode->SetVisibility(0);

        RestorePhyNode(phyNode, (TGeoNode*)m_PhyNodeOrgArray->At(i));
    }
    m_PhyNodeOrgArray->Clear("C");
    m_HitsArray->Clear("C");

    // set previous event 2D hits info to default
    for (int i = 0; i < m_2DHitsArray->GetEntries(); i++) {
        Emc2DCrystal *aCrystal = (Emc2DCrystal*)m_2DHitsArray->At(i);
        aCrystal->ClearInfo();
        aCrystal->AddInfo(aCrystal->GetTitle());
        aCrystal->CloseInfo();
    }
    m_2DHitsArray->Clear("C");

    // set new hits
    //Long Peixun's update: Consider gEvent == NULL
    int NDigiCol = 0;
    if (gEvent)
    {
        m_EmcDigiCol = gEvent->GetEmcDigiCol();
        //Long Peixun's update: Consider m_EmcDigiCol == NULL
        if (m_EmcDigiCol) NDigiCol = m_EmcDigiCol->GetEntries();
        else NDigiCol = 0;
    }

    for (int i = 0; i < NDigiCol; i++) {
        TEmcDigi *aEmcDigi = (TEmcDigi*)m_EmcDigiCol->At(i);
        Identifier aEmcID( aEmcDigi->getIntId() );
        int part  = EmcID::barrel_ec( aEmcID );
        int theta = EmcID::theta_module( aEmcID );
        int phi   = EmcID::phi_module( aEmcID );

        Double_t charge = RawDataUtil::EmcCharge(aEmcDigi->getMeasure(),
                          aEmcDigi->getChargeChannel());
        Double_t time = RawDataUtil::EmcTime(aEmcDigi->getTimeChannel());

        TGeoPhysicalNode *phyNode = 0;
        phyNode = GetPhysicalCrystal( part, phi, theta );
        if (phyNode) {
            m_PhyNodeOrgArray->Add ( phyNode->GetNode() );

            //Double_t *P; // = new Double_t[16];
            //TGeoArb8 *oldArb8 = (TGeoArb8*)phyNode->GetShape();
            //P = oldArb8->GetVertices();
            //TGeoArb8 *newArb8 = new TGeoArb8(charge, &P[0]);

            //TGeoTranslation *newmat =
            //    new TGeoTranslation(0.0, 0.0, oldArb8->GetDz()+newArb8->GetDz());
            //newmat->RegisterYourself();

            //Align(phyNode, newmat, newArb8);

            m_HitsArray->Add(phyNode);  //Long Peiuxn's update: Remove annotation
	    ////delete newArb8;
	    ////delete newmat;
        }

        phyNode = GetPhysicalCrystal2( part, phi, theta );
        if (phyNode) {
            m_PhyNodeOrgArray->Add ( phyNode->GetNode() );

            //Double_t *P; // = new Double_t[16];
            //TGeoArb8 *oldArb8 = (TGeoArb8*)phyNode->GetShape();
            //P = oldArb8->GetVertices();
            //TGeoArb8 *newArb8 = new TGeoArb8(charge, &P[0]);

            //TGeoTranslation *newmat =
            //    new TGeoTranslation(0.0, 0.0, oldArb8->GetDz()+newArb8->GetDz());
            //newmat->RegisterYourself();

            //Align(phyNode, newmat, newArb8);

            //m_HitsArray->Add( phyNode );
	    ////delete newArb8;
	    ////delete newmat;
        }

        Emc2DCrystal *aCrystal = 0;
        aCrystal = m_Emc2DCrystal[part][phi][theta];
        if (aCrystal) {
            aCrystal->SetTime(time);        //Long Peixun's update: Set Emc crystals' time and charge
            aCrystal->SetCharge(charge);

            aCrystal->ClearInfo();
            aCrystal->AddInfo(aCrystal->GetTitle());

            char data[100];
            sprintf(data, "time = %-.3f ns, charge = %-.3f MeV", time, charge);
            aCrystal->AddInfo( TString(data) );
            sprintf(data, "Fired");
            aCrystal->AddInfo( TString(data) );

            aCrystal->CloseInfo();

            m_2DHitsArray->Add(aCrystal);
        }
    }
}

void
EmcROOTGeo::SetVisEmcDetector()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

    for (int i = 0; i < m_DetectorsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_DetectorsArray->At(i);
        phyNode->SetVisibility(0);
        if (view && view->GetVisEmcGlobal()) {
            int part = GetPart(phyNode);
            if (part == 0 && view->GetVisEmcEast()   ||
                    part == 1 && view->GetVisEmcBarrel() ||
                    part == 2 && view->GetVisEmcWest()   )
                phyNode->SetVisibility(1);
        }
    }
}

void
EmcROOTGeo::SetVisEmcHits()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

    for (int i = 0; i < m_HitsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_HitsArray->At(i);
        int part = GetPart(phyNode);
        if (view && view->GetVisEmcHitsGlobal()) {
            if (part == 0 && view->GetVisEmcHitsEast()   ||
                    part == 1 && view->GetVisEmcHitsBarrel() ||
                    part == 2 && view->GetVisEmcHitsWest()   ) {
                phyNode->SetVisibility(1);
                phyNode->SetLineColor(881);    //Long Peixun's update: Magenta -> Violet
                continue;
            }
        }
        //Long Peixun's update: Unfired visibility should be determined by Detector option rather than His option
        //phyNode->SetVisibility(0);
        if (part == 1) phyNode->SetLineColor(m_brCrystalColor);
        else phyNode->SetLineColor(m_ecCrystalColor);
    }
}

int
EmcROOTGeo::GetPartNb()
{
    return m_kPart;     //Long Peixun's update: code optimization
}

int
EmcROOTGeo::GetThetaNb(int part)   // real theta number
{
    //Long Peixun's update: code optimization
    if (part == 1) return m_kThetaBr;
    else return m_kThetaEc;
}

int
EmcROOTGeo::GetPhiNb(int part, int theta) // real phi number
{
    int phiNb = m_kPhiBr;
    if (part != 1) {
        if (theta < 2) phiNb = 4*m_kSectorEc;
        else if (theta >= 2 && theta < 4) phiNb = 5*m_kSectorEc;
        else if (theta >= 4 && theta < m_kThetaEc) phiNb = 6*m_kSectorEc;
    }

    return phiNb;
}

TGeoVolume*
EmcROOTGeo::GetVolumePart( int part )
{
    // only part0 and part1 logical volume.
    std::stringstream osname;
    if (part != 1) osname <<"logicalEndWorld";
    else osname <<"logicalBSCWorld";

    //osname << "logical" << "Emc" << "Part" << part;
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
    //osname << "logical" << "Emc" << "Part" << part << "Phi" << phi;
    if (part == 1) osname << "logicalBSCPhi" ;
    else{
        if (phi == 15 || phi == 7)  osname << "logicalEndPhi2" ;
        else if (phi == 14 || phi == 6) osname << "logicalEndPhi1" ;
        else  osname << "logicalEndPhi0" ;
    }


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
    // part0phi0theta0-34, part0phi6theta0-34, part0phi7theta0-34
    // part1theta0-43
    std::stringstream osname;
    if (part == 1 && theta >= 0 && theta < 44) {
        //osname << "logical" << "Emc" << "Part" << part << "Theta" << theta;
        osname << "logicalBSCCasing" << theta;
        return GetLogicalVolume( osname.str() );
    }
    else if ( part != 1 && theta >= 0 && theta < 35 &&
              (phi < 3) ) {
        //osname << "logical" << "Emc" << "Part" << part << "Phi" << phi << "Theta" << theta;
        osname << "logicalEndCasing_" << phi << "_" << theta;
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
        //osname << "logical" << "Emc" << "Part" << part << "Theta" << theta << "Crystal";
        osname << "logicalCrystal";
        return GetLogicalVolume( osname.str() );
    }
    else if ( part == 0 && theta >= 0 && theta < 35 &&
              (phi < 3) ) {
        //osname << "logical" << "Emc" << "Part" << part << "Phi" << phi << "Theta" << theta << "Crystal";
        osname << "logicalEndCrystal_" << phi << "_" << theta;
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
    if (m_ROOTGeoInit == 2) { // from GDML
        std::stringstream osname;
        //osname << "pv_" << "logical" << "Emc" << "Part" << (part == 2 ? (part -2) : part) << "_" << part;
        if (part == 1) osname <<"pv_logicalBSCWorld_2";
        if (part == 0) osname <<"pv_logicalEndWorld_1";
        if (part == 2) osname <<"pv_logicalEndWorld_0";

        return GetNode( osname.str() );
    }
    else if (m_ROOTGeoInit == 1) { // from ROOT object
        if (m_NodePart[part] != 0) {
            return m_NodePart[part];
        }
        else {
            std::cout << "Node: " << "Part" << part << " not found" << std::endl;
            return 0;
        }
    }

    return 0;
}

TGeoNode*
EmcROOTGeo::GetPhi( int part, int phi )   // in EC, sector in fact
{
    if (m_ROOTGeoInit == 2) { // from GDML
        int endphilist[16]= {15,7,14,6,13,12,11,10,9,8,5,4,3,2,1,0};
        int endphipos[16] = {15,14,13,12,11,10,3,1,9,8,7,6,5,4,2,0}; //pos of phi in endworld

        int realphi=0;
        if (phi==15||phi==7) realphi = 2;
        if (phi==14||phi==6) realphi = 1;

        std::stringstream osname;
        //osname << "pv_" << "logical" << "Emc" << "Part" << (part == 2 ? (part -2) : part) << "Phi" << phi << "_" << phi;
        if (part == 1) osname << "pv_logicalBSCPhi" << "_" << 308-phi;
        else  osname <<"pv_logicalEndPhi"<<realphi<<"_" <<endphipos[phi];
        return GetNode( osname.str() );
    }
    else if (m_ROOTGeoInit == 1) { // from ROOT object
        if (m_NodePhi[part][phi] != 0) {
            return m_NodePhi[part][phi];
        }
        else {
            std::cout << "Node: " << "Part" << part << "Phi" << phi << " not found" << std::endl;
            return 0;
        }
    }

    return 0;
}

TGeoNode*
EmcROOTGeo::GetTheta( int part, int phi, int theta ) // real phi, theta
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
EmcROOTGeo::GetTheta2( int part, int phi, int theta ) // real phi, theta
{
    if (m_NodeTheta2[part][phi][theta] != 0) {
        return m_NodeTheta2[part][phi][theta];
    }
    else {
        return 0;
    }
}

TGeoNode*
EmcROOTGeo::GetCrystal( int part, int phi, int theta ) // real phi, theta
{
    // in fact, br only 44 crystal nodes;
    return GetTheta(part, phi, theta)->GetDaughter(0);
}

TGeoNode*
EmcROOTGeo::GetCrystal2( int part, int phi, int theta ) // real phi, theta
{
    // in fact, br only 44 crystal nodes;
    if (GetTheta2(part, phi, theta) == 0) return 0;
    else return GetTheta2(part, phi, theta)->GetDaughter(0);
}

TGeoPhysicalNode*
EmcROOTGeo::GetPhysicalCrystal( int part, int phi, int theta ) // real phi, theta
{
    if (m_PhysicalCrystal[part][phi][theta] != 0) {
        return m_PhysicalCrystal[part][phi][theta];
    }
    else {
        std::cout << "PhysicalNode: " << "Part" << part << "Phi" << phi << "Theta" << theta << " not found" << std::endl;
        return 0;
    }
}

TGeoPhysicalNode*
EmcROOTGeo::GetPhysicalCrystal2( int part, int phi, int theta ) // real phi, theta
{
    return m_PhysicalCrystal2[part][phi][theta];
}

bool
EmcROOTGeo::HasTwoNodes( int part, int phi, int theta )
{
    if (part == 1) return false;
    else return true;
}


void
EmcROOTGeo::ComputeThetaPhi( int id, int sector, int nb, int &CryNumberTheta, int &CryNumberPhi)
{
    if ((sector>=0)&&(sector<3))
        sector+=16;
    if ((sector!=7)&&(sector!=15))
    {
        if ((nb>=0)&&(nb<4))
        {
            CryNumberTheta = 0;
            CryNumberPhi = (sector-3)*4+nb;
        }
        else if ((nb>=4)&&(nb<8))
        {
            CryNumberTheta = 1;
            CryNumberPhi = (sector-3)*4+nb-4;
        }
        else if ((nb>=8)&&(nb<13))
        {
            CryNumberTheta = 2;
            CryNumberPhi = (sector-3)*5+nb-8;
        }
        else if ((nb>=13)&&(nb<18))
        {
            CryNumberTheta = 3;
            CryNumberPhi = (sector-3)*5+nb-13;
        }
        else if ((nb>=18)&&(nb<24))
        {
            CryNumberTheta = 4;
            CryNumberPhi = (sector-3)*6+nb-18;
        }
        else if ((nb>=24)&&(nb<30))
        {
            CryNumberTheta = 5;
            CryNumberPhi = (sector-3)*6+nb-24;
        }
    }
    else// if((sector=7)||(sector==15))
    {
        if ((nb>=0)&&(nb<4))
        {
            CryNumberTheta = 0;
            CryNumberPhi = (sector-3)*4+3-nb;
        }
        else if ((nb>=4)&&(nb<8))
        {
            CryNumberTheta = 1;
            CryNumberPhi = (sector-3)*4+7-nb;
        }
        else if ((nb>=8)&&(nb<13))
        {
            CryNumberTheta = 2;
            CryNumberPhi = (sector-3)*5+12-nb;
        }
        else if ((nb>=13)&&(nb<18))
        {
            CryNumberTheta = 3;
            CryNumberPhi = (sector-3)*5+17-nb;
        }
        else if ((nb>=18)&&(nb<24))
        {
            CryNumberTheta = 4;
            CryNumberPhi = (sector-3)*6+23-nb;
        }
        else if ((nb>=24)&&(nb<30))
        {
            CryNumberTheta = 5;
            CryNumberPhi = (sector-3)*6+29-nb;
        }
    }

    if (id==2)
    {
        switch (CryNumberTheta)
        {
        case 0:
            if (CryNumberPhi<32)
                CryNumberPhi = 31-CryNumberPhi;
            else
                CryNumberPhi = 95-CryNumberPhi;
            break;
        case 1:
            if (CryNumberPhi<32)
                CryNumberPhi = 31-CryNumberPhi;
            else
                CryNumberPhi = 95-CryNumberPhi;
            break;
        case 2:
            if (CryNumberPhi<40)
                CryNumberPhi = 39-CryNumberPhi;
            else
                CryNumberPhi = 119-CryNumberPhi;
            break;
        case 3:
            if (CryNumberPhi<40)
                CryNumberPhi = 39-CryNumberPhi;
            else
                CryNumberPhi = 119-CryNumberPhi;
            break;
        case 4:
            if (CryNumberPhi<48)
                CryNumberPhi = 47-CryNumberPhi;
            else
                CryNumberPhi = 143-CryNumberPhi;
            break;
        case 5:
            if (CryNumberPhi<48)
                CryNumberPhi = 47-CryNumberPhi;
            else
                CryNumberPhi = 143-CryNumberPhi;
            break;
        }
    }
}

int
EmcROOTGeo::ComputeEndCopyNb(int num)
{
    int copyNb;
    switch (num){
    case 30:
        copyNb = 5;
        break;
    case 31:
        copyNb = 6;
        break;
    case 32:
        copyNb = 14;
        break;
    case 33:
        copyNb = 15;
        break;
    case 34:
        copyNb = 16;
        break;
    default:
        copyNb = num;
        break;
    }
    return copyNb;
}

Emc2DCrystal*
EmcROOTGeo::Get2DCrystal( Int_t part, Int_t phi, Int_t theta )
{
    if (m_Emc2DCrystal[part][phi][theta]) return m_Emc2DCrystal[part][phi][theta];
    else return 0;
}

int
EmcROOTGeo::GetPart(TGeoPhysicalNode* phyNode)
{
    for (int part = 0; part < GetPartNb(); part++) {
        for (int theta = 0; theta < GetThetaNb(part); theta++) {
            for (int phi = 0; phi < GetPhiNb(part, theta); phi++) {
                if (phyNode == GetPhysicalCrystal(part, phi, theta)) {
                    //cout << "EmcROOTGeo::GetPart()" << part << endl;
                    return part;
                }
                if (phyNode == GetPhysicalCrystal2(part, phi, theta)) {
                    //cout << "EmcROOTGeo::GetPart()" << part << endl;
                    return part;
                }
            }
        }
    }

    cout << "EmcROOTGeo::GetPart, this crystal physical node does not exist!" << endl;
    return -1;
}

void
EmcROOTGeo::Draw(Option_t *option)
{
    TString opt = option;
    opt.ToUpper();

    if (!m_2DGeoInit) cout << "EmcROOTGeo::Draw2D(), 2D Geometry not initialized!" << endl;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) cout << "EmcROOTGeo::Draw(), BesView not found" << endl;

    if (view->GetVisEmcGlobal()) { // EmcVisGlobal
        TString crystalOpt;

        //if (opt.Contains("XY")) {
        // west part drawn first usually
        for (Int_t part = GetPartNb()-1; part >= 0; part--) {
            for (int theta = 0; theta < GetThetaNb(part); theta++) {
                for (int phi = 0; phi < GetPhiNb(part, theta); phi++) {

                    if (m_Emc2DCrystal[part][phi][theta]) {
                        //cout << "part " << part << " layer " << layer << " crystal " << crystal << endl;
                        m_Emc2DCrystal[part][phi][theta]->SetFired(false);
                        if ( (part == 0 && view->GetVisEmcEast())   ||
                                (part == 1 && view->GetVisEmcBarrel()) ||
                                (part == 2 && view->GetVisEmcWest()) )
                            m_Emc2DCrystal[part][phi][theta]->Draw(crystalOpt);
                    }
                }
            }
        }
    }
}

void
EmcROOTGeo::DrawHits(Option_t *option)
{
    // cout << "EmcROOTGeo::DrawHits" << endl;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) cout << "EmcROOTGeo::DrawHits(), BesView not found" << endl;

    //cout << "VisEmcHitsGlobal " << view->GetVisEmcHitsGlobal() << endl;
    //cout << "VisEmcHitsEast   " << view->GetVisEmcHitsEast()   << endl;
    //cout << "VisEmcHitsBarrel " << view->GetVisEmcHitsBarrel() << endl;
    //cout << "VisEmcHitsWest   " << view->GetVisEmcHitsWest()   << endl;

    if (view->GetVisEmcHitsGlobal())
    {
        //Long Peixun's update: Use m_2DHitsArray which contains ready hits rather than rebuild hits
        for (int i = 0; i < m_2DHitsArray->GetEntries(); ++i)
        {
            Emc2DCrystal* aCrystal = (Emc2DCrystal*)m_2DHitsArray->At(i);
            if (aCrystal && aCrystal->GetCharge() > 5)
            {
                if ( (aCrystal->GetPart() == 0 && view->GetVisEmcHitsEast())  ||
                    (aCrystal->GetPart() == 1 && view->GetVisEmcHitsBarrel()) ||
                    (aCrystal->GetPart() == 2 && view->GetVisEmcHitsWest()) )
                {
                    aCrystal->SetFired(true);
                    aCrystal->Draw();
                }
            }
        }

/*         // reset time and charge to zero
        for (Int_t part = GetPartNb()-1; part >= 0; part--) {
            for (int theta = 0; theta < GetThetaNb(part); theta++) {
                for (int phi = 0; phi < GetPhiNb(part, theta); phi++) {
                    if (m_Emc2DCrystal[part][phi][theta]) {
                        m_Emc2DCrystal[part][phi][theta]->ResetTimeCharge();
                        //m_Emc2DCrystal[part][phi][theta]->ClearInfo();
                    }
                }
            }
        }

        if (m_EmcDigiCol) {
            for (int i = 0; i < m_EmcDigiCol->GetEntries(); i++) {
                TEmcDigi *aEmcDigi = (TEmcDigi*)m_EmcDigiCol->At(i);

                //cout << aEmcDigi->getIntId() << endl;
                Identifier aEmcID( aEmcDigi->getIntId() );
                int part  = EmcID::barrel_ec( aEmcID );
                int theta = EmcID::theta_module( aEmcID );
                int phi   = EmcID::phi_module( aEmcID );

                Emc2DCrystal *aCrystal = 0;
                aCrystal = m_Emc2DCrystal[part][phi][theta];

                //Double_t charge = Double_t(aEmcDigi->getChargeChannel())  / EMC_CHARGE_FACTOR;
                Double_t charge = RawDataUtil::EmcCharge(aEmcDigi->getMeasure(),
                                  aEmcDigi->getChargeChannel());
                aCrystal->SetCharge(charge);
                //cout << "charge " << aCrystal->GetCharge() << endl;

                if (aCrystal&&charge > 5) {
                    if ( (part == 0 && view->GetVisEmcHitsEast())   ||
                            (part == 1 && view->GetVisEmcHitsBarrel()) ||
                            (part == 2 && view->GetVisEmcHitsWest()) ) {
                        aCrystal->SetFired(true);
                        aCrystal->Draw();
                    }
                }
            }
        } */
    }
}

void EmcROOTGeo::Align(TGeoPhysicalNode *phyNode, TGeoMatrix *newmat, TGeoShape *newshape, Bool_t check)
{
    //phyNode->Align(newmat, newshape, check);

    if (!newmat && !newshape) return;
    TGeoNode *node = phyNode->GetNode();

    Int_t fLevel=phyNode->GetLevel();
    TGeoNode *nnode = 0;
    TGeoVolume *vm = phyNode->GetVolume(fLevel-2);   
    // fLevel-2 is good for Emc geometry, use fLevel-1 if I want to use it in TofROOTGeo
    TGeoVolume *vd = 0;

    TGeoNode *nodeArray[30];
    TString orgName = phyNode->GetName();
    Int_t i,id;
    if (!phyNode->IsAligned()) {
        for (i=0; i<=fLevel-2; i++) {
            nodeArray[i] = phyNode->GetNode(i);
        }

        // fLevel-2 is good for Emc geometry, 
	// in ROOT::TGeoPhysicalNode::Align, 
	// it starts from 0, so some nodes were lost.
        for (i=fLevel-2; i<fLevel; i++) {
            // Get daughter node and its id inside vm
            node = phyNode->GetNode(i+1);
            id = vm->GetIndex(node);

            if (id < 0) {
                //printf("cannot align node %s",phyNode->GetNode(i+1)->GetName());
                return;
            }
            // Clone daughter volume and node
            vd = node->GetVolume()->CloneVolume();
            nnode = node->MakeCopyNode();
            // Correct pointers to mother and volume
            nnode->SetName(node->GetName());
            nnode->SetVolume(vd);
            nnode->SetMotherVolume(vm);
            // Decouple old node from mother volume and connect new one
            vm->GetNodes()->RemoveAt(id);
            vm->GetNodes()->AddAt(nnode,id);
            nodeArray[i+1] = nnode;
            vm = vd;
        }
    } else {
        nnode = phyNode->GetNode();
    }

    TString name;
    for (Int_t j = 0; j < phyNode->GetLevel()+1; j++) {
        name += "/";
        name += nodeArray[j]->GetName();
    }
    //phyNode->SetPath(name);
    phyNode->SetName(name);
    phyNode->Refresh();

    // Now nnode is a cloned node of the one that need to be aligned
    TGeoNodeMatrix *aligned = (TGeoNodeMatrix*)nnode;
    vm = nnode->GetMotherVolume();
    vd = nnode->GetVolume();
    if (newmat) {
        // Register matrix and make it the active one
        if (!newmat->IsRegistered()) newmat->RegisterYourself();
        aligned->SetMatrix(newmat);
        // Update the global matrix for the aligned node
        TGeoHMatrix *global = phyNode->GetMatrix();
        TGeoHMatrix *up = phyNode->GetMatrix(fLevel-1);
        *global = up;
        global->Multiply(newmat);
    }
    // Change the shape for the aligned node
    if (newshape) vd->SetShape(newshape);
    // Now we have to re-voxelize the mother volume
    vm->SetVoxelFinder(0);
    vm->Voxelize("ALL");
    vm->FindOverlaps();
    // Eventually check for overlaps
    if (check) vm->CheckOverlaps();
    //phyNode->SetAligned(kTRUE);
}

void EmcROOTGeo::RestorePhyNode(TGeoPhysicalNode *phyNode, TGeoNode *node)
{
    Align(phyNode, node->GetMatrix(), node->GetVolume()->GetShape());
}
