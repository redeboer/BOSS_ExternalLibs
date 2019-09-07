//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Tof Geometry General for EventDisplay
 *
 *    2004/12/9    Zhengyun You      Peking University
 *                 named from TofGeo to TofROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 */

using namespace std;

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

#include <TGeoManager.h>
#include <TGeoTube.h>
#include <TGeoArb8.h>
#include <TGeoTrd2.h>     //Huang Shuhui's update: For Mrpc
#include <TGeoNode.h>     //...
#include <TMath.h>

#include "BesVisLib/TofROOTGeo.h"
#include "Identifier/TofID.h"
#include "BesVisLib/BesEvent.h"
#include "BesVisLib/BesView.h"
#include "RawEvent/RawDataUtil.h"


TofROOTGeo::TofROOTGeo()
        : SubDetectorROOTGeo()
{
    // Default constructor.
    k_TMatch = kTRUE;
    k_QMatch = kTRUE;
    for (int part = 0; part < m_kPart; part++)
    {
        //Long Peixun's update: m_kLayerBr -> m_kModuleEc
        for (int layer = 0; layer < m_kModuleEc; layer++) 
        {
            m_NodeAl[part][layer] = 0;
            m_NodeScin[part][layer] = 0;
            for (int scin = 0; scin < m_kScinBr; scin++)
            {
                m_NodePVF[part][layer][scin] = 0;
                m_PhysicalScin[part][layer][scin] = 0;
                m_Tof2DScin[part][layer][scin] = 0;
            }
        }
    }

    //Huang Shuhui's update: For Mrpc
    fTofMrpc = 0;

    //Long Peixun's update: Merge m_PhysicalMrpc into m_PhysicalScin
	//for (int part = 0; part < m_kPart; part++)
    //{
	//    for (int module = 0; module < m_kModuleEc; module++)
    //    {
    // 	      m_NodeModule[part][module] = 0;
    //        for (int strip = 0; strip < m_kStripEc; strip++)
    //        {
    // 		       m_PhysicalMrpc[part][module][strip] = 0;
    // 		  }
    // 	  }
    //}

    m_TofColor    = 6;
    m_BucketColor = 4;
    m_ScinColor   = kYellow;
}

TofROOTGeo::~TofROOTGeo()
{
    //Long Peixun's update: Add destructor
    cout << "delete old TofROOTGeo" << endl;
    for (int part = 0; part < m_kPart; part++)
    {
        for (int layer = 0; layer < m_kModuleEc; layer++) 
        {
            for (int scin = 0; scin < m_kScinBr; scin++)
            {
                delete m_Tof2DScin[part][layer][scin];
            }
        }
    }
}

void
TofROOTGeo::InitFromGDML( const char *gdmlFile, const char *setupName )
{
    m_ROOTGeoInit = 2;

    //Huang Shuhui's update: Load in Tof_mrpc Gdml Geometry, set fTofMrpc flag
    fTofMrpc = 0;
    if (TString(gdmlFile).Contains("Tof_mrpc.gdml")) fTofMrpc = 1;
    else cout << "Cannot read Tof_mrpc!" << endl;
    if (fTofMrpc) cout << "fTofMrpc: " << "Using new Geometry of Tof!" << endl;

    ReadGdml(gdmlFile, setupName);
    SetNode();
}

void
TofROOTGeo::InitFromROOT( TGeoVolume *vol )
{
    m_ROOTGeoInit = 1;

    //Huang Shuhui's update: To judge MRPC
	fTofMrpc = 0; 
    TGeoNode *contianer = vol->GetNode(0)->GetVolume()->GetNode(0);
    if (TString(contianer->GetName()).Contains("container")) fTofMrpc = 1;
	if (fTofMrpc) cout << "fTofMrpc: " << "Using new Geometry of Tof!" << endl;

    SetVolumeTof(vol);
    SetNode();
}

void
TofROOTGeo::Init2DGeometry()
{
    if (m_ROOTGeoInit != 1 && m_ROOTGeoInit != 2)
    {
        cout << "TofROOTGeo::Init2DGeometry, ROOT Geometry not Initialized yet!" << endl;
        return;
    }
    m_2DGeoInit = 1;

    Double_t local[3] = {0.0, 0.0, 0.0};
    Double_t master[3] = {0.0, 0.0, 0.0};
    //Int_t    nPoints = 8;
    //Double_t  P[300] = {0.0};
    //Double_t  center[3] = {0.0, 0.0, 0.0};
    TString  name;

    //Huang Shuhui's update: Tof Mrpc
    if (fTofMrpc)
    {
        for (Int_t part = 0; part < m_kPart; part++)
        {
			if (part == 1)
            {
                for (Int_t layer = 0; layer < GetLayerNb(part); layer++)
                {
                    for (int scin = 0; scin < GetScinNb(part); scin++)
                    {
                        TGeoPhysicalNode *scinPhyNode = GetPhysicalScin(part, layer, scin);
                        if (part == 1)
                        {
                            name = TString("Tof Part 1");
                            name += " Layer ";
                            name += layer;
                            name += " Phi ";
                            name += scin;
                        }
                        TGeoArb8 *scinShape = (TGeoArb8*)scinPhyNode->GetShape();
                        Double_t *localArb8Point, masterArb8Point[24];
                        localArb8Point = scinShape->GetVertices();

                        for (Int_t i = 0; i < 8; i++)
                        {
                            local[0] = localArb8Point[2*i];
                            local[1] = localArb8Point[2*i+1];
                            if (i < 4) local[2] = scinShape->GetDz() * (-1.0);
                            else local[2] = scinShape->GetDz();

                            //cout << i << " local x: " << local[0] << " y: " << local[1] << " z: " << local[2] << endl;
                            scinPhyNode->GetMatrix(-1*scinPhyNode->GetLevel())->LocalToMaster(local, &master[0]); // transform to top
                            //cout << " master x: " << master[0] << " y: " << master[1] << " z: " << master[2] << endl;

                            for (Int_t j = 0; j < 3; j++) {
                                masterArb8Point[3*i+j] = master[j];
                            }
                        }

                        m_Tof2DScin[part][layer][scin] = new Tof2DScin(name, name, 8, &masterArb8Point[0], part);
                    }//scin
                }//layer
            }//part = 1 
			else
            {
				for (int module = 0; module < GetModuleNb(part); module++)
                {
					for (int strip = 0; strip < GetStripNb(part); strip++)
                    {
                        TGeoPhysicalNode *mrpcPhyNode = GetPhysicalScin(part, module, strip);
                        if (part == 0)
                        {
                            name = TString("EastEc");
                            name = TString("Tof Part");
                            name += part;
                            name += " Module ";
                            name += module;
                            name += " Strip ";
                            name += strip;
                        }
                        else if (part == 2)
                        {
                            name = TString("WestEc");
                            name = TString("Tof Part");
                            name += part;
                            name += " Module ";
                            name += module;
                            name += " Strip ";
                            name += strip;
                        }

                        TGeoTrd2 *mrpcShape = (TGeoTrd2*)mrpcPhyNode->GetShape();

                        Double_t x1, x2, y1, y2, z;
                        x1 = mrpcShape->GetDx1();
                        x2 = mrpcShape->GetDx2();
                        y1 = mrpcShape->GetDy1() * 500;// *500, make mrpc thick in ZR view
                        y2 = mrpcShape->GetDy2() * 500;// *500, make mrpc thick in ZR view
                        z  = mrpcShape->GetDz();
                        Double_t masterArb8Point[24];

                        for (Int_t i =0; i < 8; i++)
                        {
                            if (i == 0) { local[0] = -x1; local[1] = -y1; local[2] = -z; }
                            else if (i == 4) { local[0] = -x1; local[1] =  y1; local[2] = -z; }
                            else if (i == 7) { local[0] =  x1; local[1] =  y1; local[2] = -z; }
                            else if (i == 3) { local[0] =  x1; local[1] = -y1; local[2] = -z; }
                            else if (i == 1) { local[0] = -x2; local[1] = -y2; local[2] =  z; }
                            else if (i == 5) { local[0] = -x2; local[1] =  y2; local[2] =  z; }
                            else if (i == 6) { local[0] =  x2; local[1] =  y2; local[2] =  z; }
                            else if (i == 2) { local[0] =  x2; local[1] = -y2; local[2] =  z; }

							mrpcPhyNode->GetMatrix(-1*mrpcPhyNode->GetLevel())->LocalToMaster(local, &master[0]);// transform to top
                            for (Int_t j = 0; j < 3; j++) {
                                masterArb8Point[3*i+j] = master[j];
                            }
                        }

					    m_Tof2DScin[part][module][strip] = new Tof2DScin(name, name, 8, &masterArb8Point[0], part);
					}//strip
				}//module
			}//part = 0/2
	    }//part
    }
    else    //Old Tof Geometry
    {
        // Tof Scintillators
        for (Int_t part = 0; part < m_kPart; part++)
        {
            for (Int_t layer = 0; layer < GetLayerNb(part); layer++)
            {
                for (int scin = 0; scin < GetScinNb(part); scin++)
                {
                    TGeoPhysicalNode *scinPhyNode = GetPhysicalScin(part, layer, scin);
                    if (part == 1)
                    {
                        name = TString("Tof Part 1");
                        name += " Layer ";
                        name += layer;
                        name += " Phi ";
                        name += scin;
                    }
                    else
                    {
                        if (part == 0) name = TString("EastEc");
                        else if (part == 2) name = TString("WestEc");
                        name = TString("Tof Part ");
                        name += part;
                        name += " Phi ";
                        name += scin;
                    }
                    TGeoArb8 *scinShape = (TGeoArb8*)scinPhyNode->GetShape();
                    Double_t *localArb8Point, masterArb8Point[24];
                    localArb8Point = scinShape->GetVertices();

                    for (Int_t i = 0; i < 8; i++)
                    {
                        local[0] = localArb8Point[2*i];
                        local[1] = localArb8Point[2*i+1];
                        if (i < 4) local[2] = scinShape->GetDz() * (-1.0);
                        else local[2] = scinShape->GetDz();

                        //cout << i << " local x: " << local[0] << " y: " << local[1] << " z: " << local[2] << endl;
                        scinPhyNode->GetMatrix(-1*scinPhyNode->GetLevel())->LocalToMaster(local, &master[0]); // transform to top
                        //cout << " master x: " << master[0] << " y: " << master[1] << " z: " << master[2] << endl;

                        for (Int_t j = 0; j < 3; j++) {
                            masterArb8Point[3*i+j] = master[j];
                        }
                    }

                    m_Tof2DScin[part][layer][scin] = new Tof2DScin(name, name, 8, &masterArb8Point[0], part);
                }
            } //scin
        } //layer
    } //part
}

void
TofROOTGeo::SetNode()
{
    if (m_ROOTGeoInit == 2)
    { // from GDML
        m_Tof = GetTopVolume();
        if (!m_Tof) std::cout << "m_Tof = 0" << std::endl;

        if (fTofMrpc)
        {   //Huang Shuhui's update: for Tof with MRPC
            for (int part = 0; part < m_kPart; part++)
            {
                if ( part != 1 ) continue;  //For TofMrpc Barrel
                for (int layer = 0; layer < GetLayerNb(part); layer++)
                {
                    std::stringstream osnameAl;
                    if (part == 1)      osnameAl << "pv_" << "logical" << "AlBr" << layer+1 << "_0";
                    m_NodeAl[part][layer] = GetNode( osnameAl.str() );
                    if (!m_NodeAl[part][layer]) cout << "empty pointer: m_NodeAl" << endl;

                    std::stringstream osnameScin;
                    if (part == 1)      osnameScin << "pv_" << "logical" << "ScinBr" << layer+1 << "_0";
                    m_NodeScin[part][layer] = GetNode( osnameScin.str() );
                    if (!m_NodeScin[part][layer]) cout << "empty pointer: m_NodeScin" << endl;

                    for (int scin = 0; scin < GetScinNb(part); scin++)
                    {
                        std::stringstream osnamePVF;
                        if (part == 1) {
                            osnamePVF << "pv_" << "logicalPVFBr"<< layer+1 << "_" << (2*m_kScinBr*3 - 1) - (layer*m_kScinBr + scin)*3;
                        }
                        m_NodePVF[part][layer][scin] = GetNode( osnamePVF.str() );
                        if (!m_NodePVF[part][layer][scin]) cout << "empty pointer: m_NodePVF" << endl;
                    }//scin
                }//layer
            }//part

            for (int part = 0; part < m_kPart; part++)
            {
                if (part == 1) continue;// For TofMrpc Endcap
                for (int module = 0; module < GetModuleNb(part); module++)
                {
                    std::stringstream osnameModule;
                    std::stringstream osnamegasContainer;
                    std::stringstream osnamebareChamber;
                    std::stringstream osnamepcbBoard1;

                    osnamebareChamber << "pv_" << "logical_" << "bareChamber" << "_" << GetChamberNodeNb(module);
                    m_NodebareChamber[part][module] = GetNode( osnamebareChamber.str() );
                    if (!m_NodebareChamber) cout << "empty pointer: m_NodebareChamber" << endl;

                    osnamepcbBoard1 << "pv_" << "logical_" << "pcbBoard1" << "_" << GetBoard1NodeNb(module);
                    m_NodepcbBoard1[part][module] = GetNode ( osnamepcbBoard1.str() );
                    if (!m_NodepcbBoard1) cout << "empty pointer: m_NodepcbBoard1" << endl;

                    if (part == 0)
                    {
                        if (!TMath::Odd(module))
                        {
                        osnameModule  << "pv_"<< "logical_" << "container_m0" << "_" << module;
                                osnamegasContainer << "pv_" << "logical_" << "gasContainer_m0" << "_" << GetContainerNodeNb(module);
                        } 
                        else {
                            osnameModule << "pv_" << "logical_" << "container_m3" << "_" << module;
                            osnamegasContainer << "pv_" << "logical_" << "gasContainer_m3" << "_" << GetContainerNodeNb(module);
                        }
                    }

                    if (part == 2)
                    {
                        if (!TMath::Odd(module))
                        {
                            osnameModule << "pv_" << "logical_" << "container_m1" << "_" << module;
                            osnamegasContainer << "pv_" << "logical_" << "gasContainer_m1" << "_" << GetContainerNodeNb(module);
                        }
                        else
                        {
                            osnameModule << "pv_" << "logical_" << "container_m2" << "_" << module;
                            osnamegasContainer << "pv_" << "logical_" << "gasContainer_m2" << "_" << GetContainerNodeNb(module);
                        }
                    }
										
                    m_NodeModule[part][module] = GetNode( osnameModule.str() );
                    if (!m_NodeModule[part][module]) cout<<"empty pointer: m_NodeModule "<<endl;
                    m_NodegasContainer[part][module] = GetNode( osnamegasContainer.str() );
                    if (!m_NodegasContainer[part][module]) cout<<"empty pointer: m_NodegasContainer "<<endl;

                    for (int strip = 0; strip < GetStripNb(module); strip++)
                    {
                        std::stringstream osnameStrip;
                        osnameStrip << "pv_" << "logical_" << "strip_" << (GetStripNb(module) - 1) - strip << "_" << strip;
                        m_NodeStrip[part][module][strip] = GetNode( osnameStrip.str() );
                        if (!m_NodeStrip[part][module][strip]) cout<<"empty pointer: m_NodeStrip "<<endl;
                    }//strip
				}//module
			}//part
        } //judge mrpc
        else
        {
            for (int part = 0; part < m_kPart; part++)
            {
                for (int layer = 0; layer < GetLayerNb(part); layer++)
                {
                    std::stringstream osnameAl;
                    if (part == 1)      osnameAl << "pv_" << "logical" << "AlBr" << layer+1 << "_0";
                    else if (part == 0)  osnameAl << "pv_" << "logical" << "AlEcEast" << "_0";
                    else                osnameAl << "pv_" << "logical" << "AlEcWest" << "_0";
                    m_NodeAl[part][layer] = GetNode( osnameAl.str() );

                    std::stringstream osnameScin;
                    if (part == 1)      osnameScin << "pv_" << "logical" << "ScinBr" << layer+1 << "_0";
                    else if (part == 0)  osnameScin << "pv_" << "logical" << "ScinEcEast" << "_0";
                    else                osnameScin << "pv_" << "logical" << "ScinEcWest" << "_0";
                    m_NodeScin[part][layer] = GetNode( osnameScin.str() );

                    for (int scin = 0; scin < GetScinNb(part); scin++)
                    {
                        std::stringstream osnamePVF;
                        if (part == 1) {
                            osnamePVF << "pv_" << "logicalPVFBr"<< layer+1 << "_" << (2*m_kScinBr*3 - 1) - (layer*m_kScinBr + scin)*3;
                        }
                        else if (part == 2) {
                            osnamePVF << "pv_" << "logicalPVFEcWest" << "_" << (2*m_kScinEc - 1) - scin*2;
                        }
                        else if (part == 0) {
                            osnamePVF << "pv_" << "logicalPVFEcEast" << "_" << (2*m_kScinEc - 1) - scin*2;
                        }
                        m_NodePVF[part][layer][scin] = GetNode( osnamePVF.str() );
                    } //scin
                } //layer
            } //part
        } //judge mrpc
    }
    else if (m_ROOTGeoInit == 1)
    { // from ROOT object
        //Huang Shuhui's update: for Tof with MRPC
        if (fTofMrpc)
        {
            for (int part = 0; part < m_kPart; part++)
            {
                int ipart = 0;
                if      (part == 1) ipart = 2;
                else if (part == 2) ipart = 1;
                for (int layer = 0; layer < GetLayerNb(part); layer++)
                {
                    for (int scin = 0; scin < GetScinNb(part); scin++)
                    {
                        int iNode = 0;
                        if (part == 1)
                        {
                            iNode = (2*m_kScinBr*3 - 1) - (layer*m_kScinBr + scin)*3;
                            m_NodePVF[part][layer][scin] = m_Tof->GetNode( ipart )->GetVolume()->GetNode(iNode);
                            m_NodeAl[part][layer] = m_NodePVF[part][layer][0]->GetVolume()->GetNode(0);
                            m_NodeScin[part][layer] = m_NodeAl[part][layer]->GetVolume()->GetNode(0);
                        }
                        if (part !=1)
                        {
                            m_NodeModule[part][layer] = m_Tof->GetNode(ipart)->GetVolume()->GetNode(layer);
                            m_NodegasContainer[part][layer] = m_NodeModule[part][layer]->GetVolume()->GetNode(0);
                            m_NodebareChamber[part][layer] = m_NodegasContainer[part][layer]->GetVolume()->GetNode(6);
                            m_NodepcbBoard1[part][layer] = m_NodebareChamber[part][layer]->GetVolume()->GetNode(30);
                            m_NodeStrip[part][layer][scin] = m_NodepcbBoard1[part][layer]->GetVolume()->GetNode(scin);
                        }
                    } //scin
                } //layer
            } //part
        } //judge mrpc
        else
        {
            for (int part = 0; part < m_kPart; part++)
            {
                int ipart = 0;
                if (part == 1) ipart = 2;
                if (part == 2) ipart = 1;
                for (int layer = 0; layer < GetLayerNb(part); layer++) {
                    for (int scin = 0; scin < GetScinNb(part); scin++) {
                        int iNode = 0;
                        //if (part == 1) {
                        // 	  iNode = layer*m_kScinBr + scin;
                        //}
                        //else if (part == 2) { // West placed first in Tof.gdml
                        // 	  iNode = 2*m_kScinBr + scin;
                        //}
                        //else if (part == 0) {
                        // 	  iNode = 2*m_kScinBr + m_kScinEc + scin;
                        //}
                        if (part != 1){
                            iNode = (2*m_kScinEc - 1) - scin*2;
                        }
                        if (part == 1) {
                            iNode = (2*m_kScinBr*3 - 1) - (layer*m_kScinBr + scin)*3;
                        }
                        m_NodePVF[part][layer][scin] = m_Tof->GetNode( ipart )->GetVolume()->GetNode(iNode);//fix me, change structure now
                    }
                    m_NodeAl[part][layer] = m_NodePVF[part][layer][0]->GetVolume()->GetNode(0);
                    //cout << m_NodeAl[part][layer]->GetName() << endl;
                    m_NodeScin[part][layer] = m_NodeAl[part][layer]->GetVolume()->GetNode(0);
                    //cout << m_NodeScin[part][layer]->GetName() << endl;
                } //layer
            } //part
        } //judge mrpc
    }

    //std::cout << "TofROOTGeo::SetNode, end of set node" << std::endl;
}

void
TofROOTGeo::SetVolumeDefaultVis()
{
    //std::cout << "begin of set defaultvis" << std::endl;

    m_Tof->SetLineColor(m_TofColor);
    m_Tof->SetVisibility(0);

    for (int part = 0; part < m_kPart; part++) {
        GetVolumePart(part)->SetLineColor(m_ScinColor);
        GetVolumePart(part)->SetVisibility(0);
        for (int layer = 0; layer < GetLayerNb(part); layer++) {
            GetVolumePVF(part, layer)->SetLineColor(m_ScinColor);
            GetVolumePVF(part, layer)->SetVisibility(0);

            GetVolumeAl(part, layer)->SetLineColor(m_ScinColor);
            GetVolumeAl(part, layer)->SetVisibility(0);

            GetVolumeScin(part, layer)->SetLineColor(m_ScinColor);
            GetVolumeScin(part, layer)->SetVisibility(0);

            GetVolumeBucket(part, layer)->SetLineColor(m_BucketColor);
            GetVolumeBucket(part, layer)->SetVisibility(0);
        }
    }
    //std::cout << "end of set defaultvis" << std::endl;
}

//Long Peixun's update: Set default 3D color
void TofROOTGeo::SetPhysicalDefaultVis()
{
    if (fTofMrpc)
    {   //Tof with MRPC
        for (int part = 0; part < m_kPart; part++)
        {
            if (part == 1)
            {
                for (int layer = 0; layer < GetLayerNb(part); layer++)
                {
                    for (int scin = 0; scin < GetScinNb(part); scin++)
                        GetPhysicalScin(part, layer, scin)->SetLineColor(m_ScinColor);
                }
            }
            else
            {
                for (int module = 0; module < GetModuleNb(part); module++)
                {
                    for (int strip = 0; strip < GetStripNb(module); strip++)
                        GetPhysicalScin(part, module, strip);
                }
            }
        }
    }
    else
    {   //Old Tof
        for (int part = 0; part < m_kPart; part++)
        {
            for (int layer = 0; layer < GetLayerNb(part); layer++)
            {
                for (int scin = 0; scin < GetScinNb(part); scin++)
                    GetPhysicalScin(part, layer, scin)->SetLineColor(m_ScinColor);
            }
        }
    }
}

void
TofROOTGeo::SetTMatch(Bool_t input = false){
    k_TMatch = input;
}

void
TofROOTGeo::SetQMatch(Bool_t input = false){
    k_QMatch = input;
}

void
TofROOTGeo::SetAllVisible()
{
    m_Tof->SetLineColor(m_TofColor);
    m_Tof->SetVisibility(0);

    for (int part = 0; part < m_kPart; part++) {
        for (int layer = 0; layer < GetLayerNb(part); layer++) {
            GetVolumePVF(part, layer)->SetLineColor(m_ScinColor);
            GetVolumePVF(part, layer)->SetVisibility(1);

            GetVolumeAl(part, layer)->SetLineColor(m_ScinColor);
            GetVolumeAl(part, layer)->SetVisibility(1);

            GetVolumeScin(part, layer)->SetLineColor(m_ScinColor);
            GetVolumeScin(part, layer)->SetVisibility(1);

            GetVolumeBucket(part, layer)->SetLineColor(m_BucketColor);
            GetVolumeBucket(part, layer)->SetVisibility(1);
        }
    }
}

void
TofROOTGeo::SetQuarterVisible()
{
    /*
    for (int part = 0; part < m_kPart; part++) {
      GetVolumeScin(part)->SetVisibility(1);
      GetVolumeBucket(part)->SetVisibility(1);
      
      int nScin = 0;
      if (part == 1) nScin = m_kAssemblyBr;
      else nScin = m_kAssemblyEc;
      for (int scin = 0; scin < nScin; scin++) {
        if (scin < nScin/4) GetScin(part, scin)->SetVisibility(0);
        else                GetScin(part, scin)->SetVisibility(1);
      
        int nBucket = 0;
        if (part == 1) nBucket = m_kBucketBr;
        else nBucket = m_kBucketEc;
        for (int bucket = 0; bucket < nBucket; bucket++) {
      if (scin < nScin/4) GetBucket(part, scin, bucket)->SetVisibility(0);
      else                GetBucket(part, scin, bucket)->SetVisibility(1);
        }
      }
    }
    */
}

void
TofROOTGeo::SetHalfVisible()
{
    /*
    for (int part = 0; part < m_kPart; part++) {
      GetVolumeScin(part)->SetVisibility(1);
      GetVolumeBucket(part)->SetVisibility(1);
      
      int nScin = 0;
      if (part == 1) nScin = m_kAssemblyBr;
      else nScin = m_kAssemblyEc;
      for (int scin = 0; scin < nScin; scin++) {
        if (scin < nScin/4+1 ||
        scin > nScin*3/4 ) GetScin(part, scin)->SetVisibility(1);
        else                   GetScin(part, scin)->SetVisibility(0);
      
        int nBucket = 0;
        if (part == 1) nBucket = m_kBucketBr;
        else nBucket = m_kBucketEc;
        for (int bucket = 0; bucket < nBucket; bucket++) {
      if (scin < nScin/4+1 ||
          scin > nScin*3/4 ) GetBucket(part, scin, bucket)->SetVisibility(1);
      else                   GetBucket(part, scin, bucket)->SetVisibility(0);
        }
      }
    }
    */
}

void
TofROOTGeo::SetNoEndVisible()
{
    /*
    int part = 1;
    GetVolumeScin(0)->SetVisibility(1);
    GetVolumeBucket(0)->SetVisibility(1);
      
    int nScin = 0;
    nScin = m_kAssemblyBr;
    for (int scin = 0; scin < nScin; scin++) {
      GetScin(part, scin)->SetVisibility(1);

      int nBucket = 0;
      nBucket = m_kBucketBr;
      for (int bucket = 0; bucket < nBucket; bucket++) {
        GetBucket(part, scin, bucket)->SetVisibility(1);
      }
    }
    */
}

void
TofROOTGeo::SetPhysicalNode()
{
    if (gGeoManager == 0) std::cout << "Create gGeoManager first" << std::endl;
    TGeoNode *bes = gGeoManager->GetTopNode();
    //std::cout << "Tof m_childNo " << m_childNo << std::endl;
    //TGeoNode *nodeTof = bes->GetVolume()->GetNode(0);
    TGeoNode *nodeTof = bes->GetDaughter(1);
    for (int part = 0; part < m_kPart; part++)
    {

        //std::stringstream osnamePart;
        //if (part == 1)      osnamePart << "pv_" << "logicalBrTof" << "_2";
        //else if(part == 0)  osnamePart << "pv_" << "logicalEcTofEast" << "_0";
        //else                osnamePart << "pv_" << "logicalEcTofWest" << "_1";
        //TGeoNode *nodePart = GetNode( osnamePart.str() );

        int ipart = 0;
        if (part == 1) ipart = 2;
        if (part == 2) ipart = 1;
        TGeoNode *nodePart;
        nodePart = nodeTof->GetVolume()->GetNode(ipart);

        if (fTofMrpc)
        {   //Huang Shuhui's update: for Tof with MRPC
            if (part == 1)
            {
                for (int layer = 0; layer < GetLayerNb(part); layer++)
                {
                    TGeoNode *nodeAl   = GetAl(part, layer);
                    TGeoNode *nodeScin = GetScin(part, layer);
                    for (int scin = 0; scin < GetScinNb(part); scin++)
                    {
                        TGeoNode *nodePVF = GetPVF(part, layer, scin);
                        m_PhysicalScin[part][layer][scin] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                                                                            TString("/") + nodeTof->GetName() +
                                                                                            TString("/") + nodePart->GetName() +
                                                                                            TString("/") + nodePVF->GetName() +
                                                                                            TString("/") + nodeAl->GetName() +
                                                                                            TString("/") + nodeScin->GetName() );
                        if(!m_PhysicalScin[part][layer][scin]) cout << "empty pointer: m_PhysicalScin" << endl;
                        m_PhysicalScin[part][layer][scin]->SetVisibility(0);
                        m_PhysicalScin[part][layer][scin]->SetIsVolAtt(kFALSE);
                        m_PhysicalScin[part][layer][scin]->SetLineColor(m_ScinColor);
                    }//scin
                }//layer
            }
            else
            {
                for (int module = 0; module < GetModuleNb(part); module++)
                {
                    TGeoNode *nodeModule       = GetModule(part,module);
                    TGeoNode *nodegasContainer = GetgasContainer(part,module);
                    TGeoNode *nodebareChamber  = GetbareChamber(part,module);
                    TGeoNode *nodepcbBoard1    = GetpcbBoard1(part,module);
                    for (int strip = 0; strip < GetStripNb(module); strip++)
                    {
                        TGeoNode *nodeStrip = GetStrip(part,module,strip);
                        m_PhysicalScin[part][module][strip] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                                                                            TString("/") + nodeTof->GetName() +
                                                                                            TString("/") + nodePart->GetName() +
                                                                                            TString("/") + nodeModule->GetName() +
                                                                                            TString("/") + nodegasContainer->GetName() +
                                                                                            TString("/") + nodebareChamber->GetName() +
                                                                                            TString("/") + nodepcbBoard1->GetName() +
                                                                                            TString("/") + nodeStrip->GetName() );
                        if(!m_PhysicalScin[part][module][strip]) cout << "empty pointer: m_PhysicalScin" << endl;
                        m_PhysicalScin[part][module][strip]= m_PhysicalScin[part][module][strip];

                        m_PhysicalScin[part][module][strip]->SetVisibility(0);
                        m_PhysicalScin[part][module][strip]->SetIsVolAtt(kFALSE);
                        m_PhysicalScin[part][module][strip]->SetLineColor(m_ScinColor);
                    }//strip
                }//module
            }
        } //judge mrpc
        else
        {
            for (int layer = 0; layer < GetLayerNb(part); layer++)
            {
                TGeoNode *nodeAl   = GetAl(part, layer);
                TGeoNode *nodeScin = GetScin(part, layer);
                for (int scin = 0; scin < GetScinNb(part); scin++)
                {
                    TGeoNode *nodePVF = GetPVF(part, layer, scin);
                    m_PhysicalScin[part][layer][scin] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                                                    TString("/") + nodeTof->GetName() +
                                                                    TString("/") + nodePart->GetName() +
                                                                    TString("/") + nodePVF->GetName() +
                                                                    TString("/") + nodeAl->GetName() +
                                                                    TString("/") + nodeScin->GetName() );
                    //cout<<"in SetPhysicalNode() "<<part<<" "<<layer<<" "<<scin<<" "<<endl;
                    //cout<<"                     "<<bes->GetName()<<" "<<nodeTof->GetName()<<" "<<nodePVF->GetName()<<" "<<nodeAl->GetName()<<" "<<nodeScin->GetName()<<endl;
                    m_PhysicalScin[part][layer][scin]->SetVisibility(0);
                    m_PhysicalScin[part][layer][scin]->SetIsVolAtt(kFALSE);
                    m_PhysicalScin[part][layer][scin]->SetLineColor(m_ScinColor);
                } //scin
            } //layer
        } //judge mrpc
    } //part

    SetDetector();
}

void
TofROOTGeo::SetDetector()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());
    //if (view) cout << "viewVisFull3DTof " << view->GetVisFull3DTof() << endl;
    m_DetectorsArray->Clear();

    //Long Peixun's update: for Tof Mrpc
    if (fTofMrpc)
    {
        for (int part = 0; part < m_kPart; part++)
        {
            if (part == 1)
            {
                for (int layer = 0; layer < GetLayerNb(part); layer++)
                {
                    for (int scin = 0; scin < GetScinNb(part); scin++)
                    {
                        TGeoPhysicalNode *phyNode = 0;
                        phyNode = GetPhysicalScin(part, layer, scin);
                        if (phyNode)
                        {
                            phyNode->SetVisibility(0); // set all invisible before set any visible
                            m_DetectorsArray->Add( phyNode );
                        }
                    }
                }
            }
            else
            {
                for (int module = 0; module < GetModuleNb(part); module++) 
                {
                    for (int strip = 0; strip < GetStripNb(module); strip++)
                    {
                        TGeoPhysicalNode *phyNode = 0;
                        phyNode = GetPhysicalScin(part, module, strip);
                        if (phyNode)
                        {
                            phyNode->SetVisibility(0); // set all invisible before set any visible
                            m_DetectorsArray->Add( phyNode );
                        }
                    }
                }
            }
        }
    }
    else
    {
        for (int part = 0; part < m_kPart; part++)
        {
            for (int layer = 0; layer < GetLayerNb(part); layer++)
            {
                for (int scin = 0; scin < GetScinNb(part); scin++)
                {
                    TGeoPhysicalNode *phyNode = 0;
                    phyNode = GetPhysicalScin(part, layer, scin);
                    if (phyNode)
                    {
                        phyNode->SetVisibility(0); // set all invisible before set any visible
                        //Long Peixun's update: Remove useless judgement
                        // if (part == 2 ||
                        //         (scin >= GetScinNb(part)*0/4 && scin < GetScinNb(part)*8/8)) {
                            m_DetectorsArray->Add( phyNode );
                        // }
                        // else if (view && view->GetVisFull3DTof()) {
                        //     m_DetectorsArray->Add( phyNode );
                        // }
                    }
                }
            }
        }
    }
}

void
TofROOTGeo::SetHits()
{
    // set previous event hits to default vis
    for (int i = 0; i < m_HitsArray->GetEntries(); i++)
    {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_HitsArray->At(i);
        if (phyNode)
        {
            //phyNode->SetLineColor(m_ScinColor);
            phyNode->SetVisibility(0);
        }
    }
    m_HitsArray->Clear("C");

    // set previous event 2D hits info to default
    for (int i = 0; i < m_2DHitsArray->GetEntries(); i++)
    {
        Tof2DScin *aScin = (Tof2DScin*)m_2DHitsArray->At(i);
        aScin->ClearInfo();
        aScin->AddInfo(aScin->GetTitle());
        aScin->CloseInfo();
        aScin->ResetTimeCharge();
    }
    m_2DHitsArray->Clear("C");

    //Long Peixun's update: Consider gEvent == NULL
    int NDigiCol = 0;
    if (gEvent)
    {
        m_TofDigiCol = gEvent->GetTofDigiCol();
        //Long Peixun's update: Consider m_TofDigiCol == NULL
        if (m_TofDigiCol) NDigiCol = m_TofDigiCol->GetEntries();
        else NDigiCol = 0;
    }

    for (int i = 0; i < NDigiCol; i++)
    {
        TTofDigi *aTofDigi = (TTofDigi*)m_TofDigiCol->At(i);
        Identifier atofID(aTofDigi->getIntId());
        int part, layer, scin;

        if (fTofMrpc)
        {   //Huang Shuhui's update: for Tof_mrpc hits
            part = TofID::barrel_ec(atofID);
            if (part == 1)
            {
                layer = TofID::layer(atofID);
                scin  = TofID::phi_module(atofID);
            }
            else
            {
                //In BOSS 7.0.3
                int part_Ec  = TofID::endcap(atofID);
                if (part_Ec == 0) part = 0;
                else              part = 2;
                layer = TofID::module(atofID);
                scin  = TofID::strip(atofID);

                //In CgemBOSS 6.6.5.b
                // int part_Ec  = TofID::end(atofID);
                // if (part_Ec == 0) part = 0;
                // else              part = 2;
                // layer = TofID::layer(atofID);
                // scin  = TofID::phi_module(atofID);
            }
        }
        else
        {
            part  = TofID::barrel_ec(atofID);
            layer = TofID::layer(atofID);
            scin  = TofID::phi_module(atofID);
        }

        //Long Peixun has a question: Do we need to divide by TOF_*_FACTOR?
        Double_t charge = Double_t(aTofDigi->getChargeChannel()) / TOF_CHARGE_FACTOR;
        Double_t time   = Double_t(aTofDigi->getTimeChannel())   / TOF_TIME_FACTOR;
        //Double_t charge = RawDataUtil::TofCharge(aTofDigi->getChargeChannel());
        //Double_t time   = RawDataUtil::TofTime(aTofDigi->getTimeChannel());

        TGeoPhysicalNode *phyNode = 0;
        phyNode = GetPhysicalScin( part, layer, scin );
        if (phyNode) {
            m_HitsArray->Add( phyNode );
        }

        Tof2DScin *aScin = 0;
        aScin = m_Tof2DScin[part][layer][scin];
        if (aScin)
        {
            char data[100];

            if (fTofMrpc)
            {   //Huang Shuhui's update: for Tof_mrpc hits
                if (part != 1)
                {
                    if (aScin->GetCharge() == 0.0)
                    {
                        aScin->ClearInfo();
                        aScin->AddInfo(aScin->GetTitle());
                        //Long Peixun's update: Subscript index
                        sprintf(data, "t_{leading1} = %-.3f ns, t_{trailing1} = %-.3f ns", time, charge);
                        aScin->AddInfo( TString(data) );
                    }
                    else
                    {
                        //Long Peixun's update: Subscript index
                        sprintf(data, "t_{leading2} = %-.3f ns, t_{trailing2} = %-.3f ns", time, charge);
                        aScin->AddInfo( TString(data) );
                        sprintf(data, "Fired");
                        aScin->AddInfo( TString(data) );
                    }
                }
                else
                {
                    if (aScin->GetCharge() == 0.0)
                    {
                        aScin->ClearInfo();
                        aScin->AddInfo(aScin->GetTitle());
                        sprintf(data, "time1 = %-.3f ns, charge1 = %-.3f MeV", time, charge);
                        aScin->AddInfo( TString(data) );
                    }
                    else
                    {
                        sprintf(data, "time2 = %-.3f ns, charge2 = %-.3f MeV", time, charge);
                        aScin->AddInfo( TString(data) );
                        sprintf(data, "Fired");
                        aScin->AddInfo( TString(data) );
                    }
                }
            }
            else
            {
                if (aScin->GetCharge() == 0.0)
                {
                    aScin->ClearInfo();
                    aScin->AddInfo(aScin->GetTitle());
                    sprintf(data, "time1 = %-.3f ns, charge1 = %-.3f MeV", time, charge);
                    aScin->AddInfo( TString(data) );
                }
                else
                {
                    sprintf(data, "time2 = %-.3f ns, charge2 = %-.3f MeV", time, charge);
                    aScin->AddInfo( TString(data) );
                    sprintf(data, "Fired");
                    aScin->AddInfo( TString(data) );
                }
            }

            aScin->CloseInfo();
            aScin->SetCharge(aScin->GetCharge() + charge);
            //Long Peixun's update: Set raw channel data of aScin
            aScin->SetTimeChannel(aTofDigi->getTimeChannel());
            aScin->SetChargeChannel(aTofDigi->getChargeChannel());
            m_2DHitsArray->Add(aScin);
        }
    }
}

  void
TofROOTGeo::SetVisTofDetector()
{
  /*
     for (int part = 0; part < m_kPart; part++) {
     for (int layer = 0; layer < GetLayerNb(part); layer++) {
     for (int scin = 0; scin < GetScinNb(part); scin++) {
     TGeoPhysicalNode *phyNode = GetPhysicalScin(part, layer, scin);
     if (phyNode) phyNode->SetLineColor(m_ScinColor);
     }
     }
     }
     */

  BesView *view = 0;
  if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

  //cout << "VisTofEast " << view->GetVisTofEast() << endl;
    for (int i = 0; i < m_DetectorsArray->GetEntries(); i++)
    {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_DetectorsArray->At(i);
        phyNode->SetVisibility(0);
        if (view && view->GetVisTofGlobal())
        {
            int part = GetPart(phyNode);
            if (part == 0 && view->GetVisTofEast()   ||
                part == 1 && view->GetVisTofBarrel() ||
                part == 2 && view->GetVisTofWest()   )
              phyNode->SetVisibility(1);
        }
    }
}

  void
TofROOTGeo::SetVisTofHits()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

    for (int i = 0; i < m_HitsArray->GetEntries(); i++) 
    {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_HitsArray->At(i);
        if (phyNode)
        {
            int part = GetPart(phyNode);
            if (view && view->GetVisTofHitsGlobal()) 
            {
                if (part == 0 && view->GetVisTofHitsEast()   ||
                    part == 1 && view->GetVisTofHitsBarrel() ||
                    part == 2 && view->GetVisTofHitsWest()   )
                {
                    phyNode->SetVisibility(1);
                    phyNode->SetLineColor(801);  //Long Peixun's update: Green -> Orange
                    continue;
                }
            }
            //Long Peixun's update: Unfired visibility should be determined by Detector option rather than His option
            //phyNode->SetVisibility(0);
            if (part > -1) phyNode->SetLineColor(m_ScinColor);
        }
    }
}

  int
TofROOTGeo::GetPartNb()
{
    return m_kPart;  //Long Peixun's update: code optimization
}

  int
TofROOTGeo::GetScinNb(int part)
{
    //Long Peixun's update: code optimization
    //Huang Shuhui's update: for Tof with MRPC
    if (part == 1) return m_kScinBr;
    else
    {
        if (fTofMrpc) return m_kStripEc;
        return m_kScinEc;
    }
}

  int
TofROOTGeo::GetLayerNb(int part)
{
    //Long Peixun's update: code optimization
    //Huang Shuhui's update: for Tof with MRPC
    if (part == 1) return m_kLayerBr;
    else
    {
        if (fTofMrpc) return m_kModuleEc;
        return m_kLayerEc;
    }
}

//Huang Shuhui's update: for Tof with MRPC
//Long Peixun's update: code optimization
int TofROOTGeo::GetModuleNb(int part)
{
	return m_kModuleEc;
}
int TofROOTGeo::GetChamberNodeNb(int module)
{
	return m_kChamberNodeNb;
}
int TofROOTGeo::GetContainerNodeNb(int module)
{
	return m_kContainerNodeNb;
}
int TofROOTGeo::GetBoard1NodeNb(int module)
{
  	return m_kBoard1NodeNb;
}
int TofROOTGeo::GetStripNb(int module)
{
  	return m_kStripEc;
}

/*
   TGeoVolumeAssembly*
   TofROOTGeo::GetVolumeAssembly( int part )
   {
   std::stringstream osname;
   if (part == 0 || part == 2) {
   osname << "logical" << "Tof" << "AssemblyEc";
   }
   else {
   if (part == 1) {
   osname << "logical" << "Tof" << "AssemblyBr";
   }
   }

   return GetAssemblyVolume( osname.str() );
   }
   */

  TGeoVolume*
TofROOTGeo::GetVolumePart(int part)
{
    std::stringstream osname;
    if (part == 1) {
        osname << "logicalBrTof";
    }
    else if (part == 0){
        osname << "logicalEcTofWest";
    }
    else {
        osname << "logicalEcTofEast";
    }
    return GetLogicalVolume( osname.str() );
}

  TGeoVolume*
TofROOTGeo::GetVolumePVF( int part, int layer )
{
    std::stringstream osname;
    if (part == 0) {
        osname << "logical" << "PVFEcEast";
    }
    else if (part == 2){
        osname << "logical" << "PVFEcWest";
    }
    else {
        if (part == 1) {
            osname << "logical" << "PVFBr" << layer+1;
        }
    }
    return GetLogicalVolume( osname.str() );
}

  TGeoVolume*
TofROOTGeo::GetVolumeAl( int part, int layer )
{
    std::stringstream osname;
    if (part == 0) {
        osname << "logical" << "AlEcEast";
    }
    else if (part == 2){
        osname << "logical" << "AlEcWest";
    }
    else {
        if (part == 1) {
            osname << "logical" << "AlBr" << layer+1;
        }
    }
    return GetLogicalVolume( osname.str() );
}

  TGeoVolume*
TofROOTGeo::GetVolumeScin( int part, int layer )
{
    std::stringstream osname;
    if (part == 0) {
        osname << "logical" << "ScinEcEast";
    }
    else if (part == 2){
        osname << "logical" << "ScinEcWest";
    }
    else {
        if (part == 1) {
            osname << "logical" << "ScinBr" << layer+1;
        }
    }
    return GetLogicalVolume( osname.str() );
}

  TGeoVolume*
TofROOTGeo::GetVolumeBucket( int part, int layer )
{
    std::stringstream osname;
    if (part == 0 || part == 2) {
        osname << "logical" << "BucketEc";
    }
    else {
        if (part == 1) {
            osname << "logical" << "BucketBr" << layer+1;
        }
    }
    return GetLogicalVolume( osname.str() );
}

  TGeoNode*
TofROOTGeo::GetPVF( int part, int layer, int scin )
{
    if (m_NodePVF[part][layer][scin] != 0) {
        return m_NodePVF[part][layer][scin];
    }
    else {
        std::cout << "Node: " << "Part" << part << "Layer" << layer << "PVF" << scin << " not found" << std::endl;
        return 0;
    }
    return m_NodePVF[part][layer][scin];
}

  TGeoNode*
TofROOTGeo::GetAl( int part, int layer )
{
    if (m_NodeAl[part][layer] != 0) {
        return m_NodeAl[part][layer];
    }
    else {
        std::cout << "Node: " << "Part" << part << "Layer" << layer << "Al" << " not found" << std::endl;
        return 0;
    }
    return m_NodeAl[part][layer];
}

  TGeoNode*
TofROOTGeo::GetScin( int part, int layer )
{
    if (m_NodeScin[part][layer] != 0) {
        return m_NodeScin[part][layer];
    }
    else {
        std::cout << "Node: " << "Part" << part << "Layer" << layer << "Scin" << " not found" << std::endl;
        return 0;
    }
    return m_NodeScin[part][layer];
}


//Huang Shuhui's update: for Tof with MRPC
TGeoNode* TofROOTGeo::GetModule( int part, int module )
{
    if (m_NodeModule[part][module] != 0) {
  	    return m_NodeModule[part][module];
  	}
  	else {
  	    std::cout << "Node: " << "Part" << part << "Module" << module << " not found" << std::endl;
  		return 0;
    }
    return m_NodeModule[part][module];
}

TGeoNode* TofROOTGeo::GetgasContainer( int part, int module )
{
    if (m_NodegasContainer[part][module] != 0) {
  	    return m_NodegasContainer[part][module];
  	}
  	else {
  	    std::cout << "Node: " << "Part" << part << "Module" << module << "gasContainer" << " not found" << std::endl;
  		return 0;
  	}
  
  	return m_NodegasContainer[part][module];
}

TGeoNode* TofROOTGeo::GetbareChamber( int part, int module )
{
    if (m_NodebareChamber[part][module] != 0) {
        return m_NodebareChamber[part][module];
    }
  	else {
  	    std::cout << "Node: " << "Part" << part << "Module" << module << "bareChamber" << " not found" << std::endl;
  		return 0;
  	}
  
    return m_NodebareChamber[part][module];
}

TGeoNode* TofROOTGeo::GetpcbBoard1( int part, int module )
{
    if (m_NodepcbBoard1[part][module] != 0) {
  	    return m_NodepcbBoard1[part][module];
  	}
  	else {
  	    std::cout << "Node: " << "Part" << part << "Module" << module << "pcbBoard1" << " not found" << std::endl;
  		return 0;
  	}
  
  	return m_NodepcbBoard1[part][module];
}

TGeoNode* TofROOTGeo::GetStrip( int part, int module, int strip )
{
    if (m_NodeStrip[part][module][strip] != 0) {
        return m_NodeStrip[part][module][strip];
    }
  	else {
  		  std::cout << "Node: " << "Part" << part << "Module" << module << "Strip" << strip << " not found" << std::endl;
  		return 0;
  	}
    
  	return m_NodeStrip[part][module][strip];
}


/*
   TGeoNode *
   TofROOTGeo::GetBucket( int part, int scin, int bucket )
   {
   std::stringstream osname;
   if (part == 0) {
   osname << "pv_" << "logical" << "Tof" << "BucketEc" << "_" << m_kAssemblyBr + m_kAssemblyEc + scin << "_1";
   }
   else if (part == 1) {
   osname << "pv_" << "logical" << "Tof" << "BucketBr" << "_" << scin << "_" << bucket+1;
   }
   else if (part == 2) {
   osname << "pv_" << "logical" << "Tof" << "BucketEc" << "_" << m_kAssemblyBr + scin << "_1";
   }

   return GetNode( osname.str() );
   }
   */

  TGeoPhysicalNode*
TofROOTGeo::GetPhysicalScin( int part, int layer, int scin )
{
    if (m_PhysicalScin[part][layer][scin] != 0) {
        return m_PhysicalScin[part][layer][scin];
    }
    else {
        std::cout << "PhysicalNode: " << "Part"
            << part << "Layer" << layer << "Scin"
            << scin << " not found" << std::endl;
        return 0;
    }
}

  Tof2DScin*
TofROOTGeo::Get2DScin( Int_t part, Int_t layer, Int_t scin)
{
    if (m_Tof2DScin[part][layer][scin]) return m_Tof2DScin[part][layer][scin];
    else return 0;
}

  Int_t
TofROOTGeo::GetPart(TGeoPhysicalNode* phyNode)
{
    for (Int_t part = 0; part < m_kPart; part++)
    {
        for (Int_t layer = 0; layer < GetLayerNb(part); layer++)
        {
            for (int scin = 0; scin < GetScinNb(part); scin++)
            {
                if (phyNode == GetPhysicalScin(part, layer, scin)) {
                    //cout << "GetPart()" << part << endl;
                    return part;
                }
            }
        }
    }
    cout << "TofROOTGeo::GetPart, this scintillator physical node does not exist!" << endl;
    return -1;
}

  void
TofROOTGeo::Draw(Option_t *option)
{
    TString opt = option;
    opt.ToUpper();

    if (!m_2DGeoInit) cout << "TofROOTGeo::Draw2D(), 2D Geometry not initialized!" << endl;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) cout << "TofROOTGeo::Draw(), BesView not found" << endl;

    if (view->GetVisTofGlobal())
    { // TofVisGlobal
        TString scinOpt;

        //if (opt.Contains("XY")) {
        // west part drawn first usually
        for (Int_t part = m_kPart-1; part >= 0; part--)
        {
            for (Int_t layer = 0; layer < GetLayerNb(part); layer++)
            {
                for (int scin = 0; scin < GetScinNb(part); scin++)
                {
                    if (m_Tof2DScin[part][layer][scin])
                    {
                        //cout << "part " << part << " layer " << layer << " scin " << scin << endl;
                        //m_Tof2DScin[part][layer][scin]->SetHighlighted(false);
                        m_Tof2DScin[part][layer][scin]->SetFired(false);
                        if ( (part == 0 && view->GetVisTofEast()) ||
                        (part == 1 && view->GetVisTofBarrel()) ||
                        (part == 2 && view->GetVisTofWest()) )
                            m_Tof2DScin[part][layer][scin]->Draw(scinOpt);
                    }
                }
            }
        }
    }
}

void
TofROOTGeo::DrawHits(Option_t *option)
{
    // cout << "Draw Hits" << endl;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) cout << "TofROOTGeo::DrawHits(), BesView not found" << endl;

    //cout << "VisTofHitsGlobal " << view->GetVisTofHitsGlobal() << endl;
    //cout << "VisTofHitsEast   " << view->GetVisTofHitsEast()   << endl;
    //cout << "VisTofHitsBarrel " << view->GetVisTofHitsBarrel() << endl;
    //cout << "VisTofHitsWest   " << view->GetVisTofHitsWest()   << endl;

    if (view->GetVisTofHitsGlobal())
    {
        //Long Peixun's update: Use m_2DHitsArray which contains ready hits rather than rebuild hits
        for (int i = 0; i < m_2DHitsArray->GetEntries(); ++i)
        {
            Tof2DScin* aScin = (Tof2DScin*)m_2DHitsArray->At(i);
            if (aScin)
            {
                if ((k_TMatch) && (aScin->GetTimeChannel() == 0x7FFFFFFF))
                    continue;
                if ((k_QMatch) && (aScin->GetChargeChannel() == 0x7FFFFFFF))
                    continue;

                if ((aScin->GetPart() == 0 && view->GetVisTofHitsEast())   ||
                    (aScin->GetPart() == 1 && view->GetVisTofHitsBarrel()) ||
                    (aScin->GetPart() == 2 && view->GetVisTofHitsWest()) ) 
                {
                    aScin->SetFired(true);
                    aScin->Draw();
                }
            }
        }

/* 	// reset time and charge to zero
	for (Int_t part = m_kPart-1; part >= 0; part--) {
	  for (Int_t layer = 0; layer < GetLayerNb(part); layer++) {
	    for (int scin = 0; scin < GetScinNb(part); scin++) {
	      if (m_Tof2DScin[part][layer][scin]) {
		m_Tof2DScin[part][layer][scin]->ResetTimeCharge();
	      }
	    }
	  }
	}

	if (m_TofDigiCol) {
	  for (int i = 0; i < m_TofDigiCol->GetEntries(); i++) {
	    TTofDigi *aTofDigi = (TTofDigi*)m_TofDigiCol->At(i);

	    Identifier aTofID( aTofDigi->getIntId() );
	    int part   = TofID::barrel_ec( aTofID );
	    int layer  = TofID::layer( aTofID );
	    int scin   = TofID::phi_module( aTofID );

	    Double_t charge = Double_t(aTofDigi->getChargeChannel()) / TOF_CHARGE_FACTOR;
	    //cout << "charge " << charge << endl;

	    Tof2DScin *aScin = 0;
	    aScin = m_Tof2DScin[part][layer][scin];
	    // Mdc TQMatch for example
	    //
	    //if (aWire &&
	    //        ((overflow == 0)||(overflow == 12))){
	    //    if ((k_TMatch) &&
	    //            (aMdcDigi->getTimeChannel() == 0x7FFFFFFF)){
	    //        continue;
	    //    }
	    //    if ((k_QMatch) &&
	    //            (aMdcDigi->getChargeChannel() == 0x7FFFFFFF)){
	    //        continue;
	    //    }
	    //}

	    if (aScin) {
	      if ((k_TMatch)&&
		  (aTofDigi->getTimeChannel() == 0x7FFFFFFF)){
		continue;
	      }
	      if ((k_QMatch)&&
		  (aTofDigi->getChargeChannel() == 0x7FFFFFFF)){
		continue;
	      }

	      aScin->SetCharge(aScin->GetCharge() + charge);

	      if ( (part == 0 && view->GetVisTofHitsEast())   ||
		  (part == 1 && view->GetVisTofHitsBarrel()) ||
		  (part == 2 && view->GetVisTofHitsWest()) ) {
		aScin->SetFired(true);
		aScin->Draw();
	      }
	    }
	  }
	} */
    }
}
