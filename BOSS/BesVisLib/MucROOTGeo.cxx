//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Muc Geometry General for EventDisplay
 *
 *    2004/12/11   Zhengyun You      Peking University
 *                 named from MucGeo to MucROOTGeo
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
#include <TGeoBoolNode.h>
#include <TGeoBBox.h>
#include <TGeoTube.h>
#include <TGeoArb8.h>
#include <TMath.h>

#include "BesVisLib/MucROOTGeo.h"
#include "Identifier/MucID.h"
#include "BesVisLib/BesEvent.h"
#include "BesVisLib/BesView.h"

const int MucROOTGeo::m_kSeg[m_kPart]  = {4, 8, 4};
const int MucROOTGeo::m_kAbsorber[m_kPart] = {9, 9, 9};
const int MucROOTGeo::m_kGap[m_kPart]      = {8, 9, 8};
const int MucROOTGeo::m_kPanel[m_kPart]    = {3, 4, 3};
const int MucROOTGeo::m_kBakelitePanel[m_kPart][m_kUpDown]= {3, 5, 4, 4, 3, 5};

MucROOTGeo::MucROOTGeo()
        : SubDetectorROOTGeo()
{
    // Default constructor.
    for (int part = 0; part < m_kPart; part++) {
        for (int seg = 0; seg < m_kSegMax; seg++) {
            m_MucXY[part][seg] = 0;
            m_MucZR[part][seg] = 0;

            for (int gap = 0; gap < m_kGapMax; gap++) {
                m_NodeGap[part][seg][gap] = 0;
                m_PhysicalGap[part][seg][gap] = 0;
                m_MucXYGap[part][seg][gap] = 0;
                m_MucZRGap[part][seg][gap] = 0;

                for (int strip = 0; strip < m_kStripMax; strip++) {
                    m_NodeStrip[part][seg][gap][strip] = 0;
                    m_PhysicalStrip[part][seg][gap][strip] = 0;
                    //m_Muc2DStrip[part][seg][gap][strip] = 0;
                }
            }
            for (int absorber = 0; absorber < m_kAbsorberMax; absorber++) {
                m_NodeAbsorber[part][seg][absorber] = 0;
                for (int panel = 0; panel < m_kPanelMax; panel++) {
                    m_NodeAbsorberPanel[part][seg][absorber][panel] = 0;
                    m_PhysicalAbsorber[part][seg][absorber][panel] = 0;
                }
            }
        }
    }

    m_MucColor        = 2;
    m_absorberColor   = 2; // 1006
    m_gapColor        = 16; // 1
    m_gasChamberColor = 8;
    m_bakeliteColor   = 8;
    m_stripColor      = 2;//kWhite;
}

MucROOTGeo::~MucROOTGeo()
{
    //Long Peixun's update: Add destructor
    cout << "delete old MucROOTGeo" << endl;
    for (int part = 0; part < GetPartNb(); part++)
    {
        for (int seg = 0; seg < GetSegNb(part); seg++)
        {
            for (int gap = 0; gap < GetGapNb(part); gap++)
            {
                for (int strip = 0; strip < GetStripNb(part, seg, gap); strip++)
                {
                    delete m_Muc2DStrip[part][seg][gap][strip];
                }
                delete m_MucXYGap[part][seg][gap];
                delete m_MucZRGap[part][seg][gap];
            }
            delete m_MucXY[part][seg];
            delete m_MucZR[part][seg];
        }
    }
}

void
MucROOTGeo::InitFromGDML( const char *gdmlFile, const char *setupName )
{
    m_ROOTGeoInit = 2;

    ReadGdml(gdmlFile, setupName);
    SetNode();
}

void
MucROOTGeo::InitFromROOT( TGeoVolume *vol )
{
    m_ROOTGeoInit = 1;

    SetVolumeMuc(vol);
    SetNode();
}

void
MucROOTGeo::Init2DGeometry()
{
    if (m_ROOTGeoInit != 1 && m_ROOTGeoInit != 2) {
        cout << "MucROOTGeo::Init2DGeometry, ROOT Geometry not Initialized yet!" << endl;
        return;
    }
    m_2DGeoInit = 1;

    Int_t mucColor     = 1002;
    Int_t mucLineColor = 15;   //15;
    Int_t mucXYStyle   = 1001;//3001;
    Int_t mucZRStyle   = 1001;//3007;

    Int_t mucGapColor = 10; // 1002
    Int_t mucGapStyle = 1001; //1001;
    Int_t mucGapXYECStyle = 4000; //1001;
    Int_t mucGapLineColor = 15;

    Double_t x = 0.0, y = 0.0, z = 0.0;
    Double_t r = 0.0, phi = 0.0;
    Double_t local[3] = {0.0, 0.0, 0.0};
    Double_t master[3] = {0.0, 0.0, 0.0};
    //Int_t    nPoints = 4;
    Double_t  P[300] = {0.0};
    //Double_t  center[3] = {0.0, 0.0, 0.0};
    TString  name;

    Int_t part = 0, seg = 0, gap = 0, strip = 0;

    //----------XY-----------
    // Muc
    TGeoCompositeShape *mucShape = (TGeoCompositeShape*)GetVolumeMuc()->GetShape();
    //if (mucShape) ;

    //Update by Huang Shuhui: Muc's geometry has changed.
    TGeoBoolNode *mucBoolNode = mucShape->GetBoolNode();
	TGeoCompositeShape *mucWithHoleShape = (TGeoCompositeShape*)mucBoolNode->GetLeftShape();
	TGeoBoolNode *mucWithHoleBoolNode = mucWithHoleShape->GetBoolNode();
    
    TGeoBBox *mucOuterShape = (TGeoBBox*)mucWithHoleBoolNode->GetLeftShape();
    TGeoTube *mucInnerShape = (TGeoTube*)mucWithHoleBoolNode->GetRightShape();
    
    //if (mucOuterShape) ;
    //if (mucInnerShape) cout << "mucInnerShape name : " << mucInnerShape->GetName() << endl;
    //else cout << "MucROOTGeo:Init2DGeoometry, mucInnerShape not found" << endl;

    Double_t mucOuterR = mucOuterShape->GetDX();
    Double_t mucInnerR = mucInnerShape->GetRmax() + 40.0; // 40.0 is the height of first gap

    // Barrel
    // barrel segment XY view

    part = 1;
    Int_t xySegPointSeq[4] = {0,1,1,0}; // point 0,1,2,3 -> 0,1,1,0
    for (seg = 0; seg < GetSegNb(part); seg++) {
        name = TString("MucBarrelSeg");
        name += seg;
        for (Int_t iPoint = 0; iPoint < 4; iPoint++) {
            if (iPoint < 2) r = mucInnerR/TMath::Cos(TMath::Pi()/8.0);
            else            r = mucOuterR/TMath::Cos(TMath::Pi()/8.0);
            phi = TMath::Pi()/8.0 * (2*seg-1 + 2*xySegPointSeq[iPoint]);
            x = r * TMath::Cos(phi);
            y = r * TMath::Sin(phi);
            z = 0.0;
            P[3*iPoint] = x;
            P[3*iPoint+1] = y;
            P[3*iPoint+2] = z;
        }
        m_MucXY[part][seg] = new BesPolygon2D(name, name, 4, &P[0]);
        m_MucXY[part][seg]->SetFillColor(mucColor);
        m_MucXY[part][seg]->SetFillStyle(mucXYStyle);
        m_MucXY[part][seg]->SetLineColor(mucLineColor);
    }

    // barrel segment ZR view

    part = 1;
    for (seg = 0; seg < GetSegNb(part); seg++) {
        for (Int_t iPoint = 0; iPoint < 4; iPoint++) {
            if (iPoint == 0 || iPoint == 3 ) r = mucInnerR;
            else r = mucOuterR;
            phi = TMath::Pi()/4.0 * seg;

            x = r * TMath::Cos(phi);
            y = r * TMath::Sin(phi);

            TGeoVolume *volAbsorber = 0;
            volAbsorber= GetAbsorber(part, seg, 0)->GetVolume();
            TGeoBBox *absorberShape = (TGeoBBox*)volAbsorber->GetShape();
            if (seg == 2)cout<<"in MucROOTGeo::Init2DGeometry() x,y,z = "<<absorberShape->GetDX()<<" "<<absorberShape->GetDY()<<" "<<absorberShape->GetDZ()<<endl;
            Double_t dy = absorberShape->GetDY();
            z = dy;
            if (iPoint < 2) z = -dy;

            P[3*iPoint] = x;
            P[3*iPoint+1] = y;
            P[3*iPoint+2] = z;
        }

        if (seg == 2 || seg == 6) {
            if (seg == 2) name = TString("MucBarrelUpSeg");
            if (seg == 6) name = TString("MucBarrelDownSeg");

            m_MucZR[part][seg] = new BesPolygon2D(name, name, 4, &P[0]);
            m_MucZR[part][seg]->SetFillColor(mucColor);
            m_MucZR[part][seg]->SetFillStyle(mucZRStyle);
            m_MucZR[part][seg]->SetLineColor(mucLineColor);
        }
    }

    // barrel gap

    part = 1;
    for (seg = 0; seg < GetSegNb(part); seg++) {
        for (gap = 0; gap < GetGapNb(part); gap++) {

            TGeoPhysicalNode *phyNode = 0;
            phyNode = GetPhysicalGap(part, seg, gap);

            TGeoBBox *gapShape = (TGeoBBox*)phyNode->GetShape();
            Double_t dx = gapShape->GetDX();
            Double_t dy = gapShape->GetDY();
            Double_t dz = gapShape->GetDZ();

            // barrel gap XY view
            for (Int_t i = 0; i < 4; i++) {
                local[1] = 0.0; // y
                switch (i) {
                case 0:
                    local[0] = -dx;
                    local[2] = -dz;
                    break;
                case 1:
                    local[0] =  dx;
                    local[2] = -dz;
                    break;
                case 2:
                    local[0] =  dx;
                    local[2] =  dz;
                    break;
                case 3:
                    local[0] = -dx;
                    local[2] =  dz;
                    break;
                }
                phyNode->GetMatrix(-1*phyNode->GetLevel())->LocalToMaster(local, &master[0]); // transform to top
                for (Int_t j = 0; j < 3; j++) P[3*i+j] = master[j];
            }

            name = TString("MucBarrelSeg");
            name += seg;
            name += TString("Gap");
            name += gap;

            m_MucXYGap[part][seg][gap] = new BesPolygon2D(name, name, 4, &P[0]);
            m_MucXYGap[part][seg][gap]->SetFillColor(mucGapColor);
            m_MucXYGap[part][seg][gap]->SetFillStyle(mucGapStyle);
            m_MucXYGap[part][seg][gap]->SetLineColor(mucGapLineColor);

            // barrel gap ZR view
            for (Int_t i = 0; i < 4; i++) {
                local[0] = 0.0; // x
                switch (i) {
                case 0:
                    local[1] = -dy;
                    local[2] = -dz;
                    break;
                case 1:
                    local[1] = -dy;
                    local[2] =  dz;
                    break;
                case 2:
                    local[1] =  dy;
                    local[2] =  dz;
                    break;
                case 3:
                    local[1] =  dy;
                    local[2] = -dz;
                    break;
                }
                phyNode->GetMatrix(-1*phyNode->GetLevel())->LocalToMaster(local, &master[0]); // transform to top
                for (Int_t j = 0; j < 3; j++) P[3*i+j] = master[j];
            }

            if (seg == 2 || seg == 6) {
                if (seg == 2)      {
                    name = TString("MucBarrelUpSegGap");
                    name += gap;
                }
                else if (seg == 6) {
                    name = TString("MucBarrelDownSegGap");
                    name += gap;
                }

                m_MucZRGap[part][seg][gap] = new BesPolygon2D(name, name, 4, &P[0]);
                m_MucZRGap[part][seg][gap]->SetFillColor(mucGapColor);
                m_MucZRGap[part][seg][gap]->SetFillStyle(mucGapStyle);
                m_MucZRGap[part][seg][gap]->SetLineColor(mucGapLineColor);
            }
        }
    }
    // End cap
    for (part = 0; part < GetPartNb(); part++) {
        if (part == 1) continue;
        for (seg = 0; seg < GetSegNb(part); seg++) {
            Double_t segP[100];
            Int_t iSegPoint = 0;

            for (gap = 0; gap < GetGapNb(part); gap++) {
                TGeoPhysicalNode *phyNode = 0;
                phyNode = GetPhysicalGap(part, seg, gap);

                Int_t ixyPoint = 0, izrPoint = 0;
                Int_t nPanel = 3;  //4
                //TGeoCompositeShape *addPanelShape[nPanel];
                //TGeoTrap *panelShape[nPanel];
                TGeoCompositeShape *addPanelShape[4];
                TGeoTrap *panelShape[4];


                for (Int_t panel = nPanel-1; panel >= 0; panel--) {
                    if (panel == nPanel-1)  addPanelShape[panel] = (TGeoCompositeShape*)GetGap(part, seg, gap)->GetVolume()->GetShape();
                    else addPanelShape[panel] = (TGeoCompositeShape*)addPanelShape[panel+1]->GetBoolNode()->GetLeftShape();

                    panelShape[panel] = (TGeoTrap*)addPanelShape[panel]->GetBoolNode()->GetRightShape();

                    Double_t *localArb8Point, masterArb8Point[3*8];
                    localArb8Point = panelShape[panel]->GetVertices();
                    for (Int_t i = 0; i < 8; i++) {
                        local[0] = localArb8Point[2*i];
                        local[1] = localArb8Point[2*i+1];
                        if (i < 4) local[2] = panelShape[panel]->GetDz() * (-1.0);
                        else local[2] = panelShape[panel]->GetDz();

                        addPanelShape[panel]->GetBoolNode()->GetRightMatrix()->LocalToMaster(local, &master[0]); // transform to gap coordinate

                        for (Int_t j = 0; j < 3; j++) local[j] = master[j];
                        phyNode->GetMatrix(-1*phyNode->GetLevel())->LocalToMaster(local, &master[0]); // transform to top
                        for (Int_t j = 0; j < 3; j++) {
                            masterArb8Point[3*i+j] = master[j];
                        }
                    }

                    // xy, add point on panel 3 and panel 0   //panel 3 -> panel 2(new geom)
                    if (panel == 2) {
                        Int_t xyPanelPointSeq[4] = {0, 3, 7, 4};
                        for (Int_t i = 0; i < 4; i++) {
                            for (Int_t j = 0; j < 3; j++) {
                                P[3*ixyPoint+j] = masterArb8Point[3*xyPanelPointSeq[i]+j];
                            }
                            ixyPoint++;
                        }
                    }
                    if (panel == 0) {
                        Int_t xyPanelPointSeq[4] = {7, 4, 0, 3};
                        for (Int_t i = 0; i < 4; i++) {
                            for (Int_t j = 0; j < 3; j++) {
                                P[3*ixyPoint+j] = masterArb8Point[3*xyPanelPointSeq[i]+j];
                            }
                            ixyPoint++;
                        }
                    }

                    // zr, add point on panel 3 and panel 0, use P from P[100]    //panel 3 -> panel 2(new geom)
                    if (panel == 2) {
                        Int_t zrPanelPointSeq[4] = {4, 7, 6, 5};
                        for (Int_t i = 0; i < 4; i++) {
                            for (Int_t j = 0; j < 3; j++) {
                                P[100+3*izrPoint+j] = masterArb8Point[3*zrPanelPointSeq[i]+j];
                            }
                            izrPoint++;
                        }
                    }
                    if (panel == 0) {
                        Int_t zrPanelPointSeq[4] = {6, 5, 4, 7};
                        for (Int_t i = 0; i < 4; i++) {
                            for (Int_t j = 0; j < 3; j++) {
                                P[100+3*izrPoint+j] = masterArb8Point[3*zrPanelPointSeq[i]+j];
                            }
                            izrPoint++;
                        }
                    }
                }

                if (part == 0)      name = TString("MucEastEc");
                else if (part == 2) name = TString("MucWestEc");
                name += TString("Seg");
                name += seg;
                name += TString("Gap");
                name += gap;

                //cout << name << endl;
                m_MucXYGap[part][seg][gap] = new BesPolygon2D(name, name, ixyPoint, &P[0]); // ixyPoint = 8
                m_MucXYGap[part][seg][gap]->SetFillColor(mucGapColor);
                m_MucXYGap[part][seg][gap]->SetFillStyle(mucGapXYECStyle);
                m_MucXYGap[part][seg][gap]->SetLineColor(mucGapLineColor);

                m_MucZRGap[part][seg][gap] = new BesPolygon2D(name, name, izrPoint, &P[100]); // izrPoint = 8
                m_MucZRGap[part][seg][gap]->SetFillColor(mucGapColor);
                m_MucZRGap[part][seg][gap]->SetFillStyle(mucGapStyle);
                m_MucZRGap[part][seg][gap]->SetLineColor(mucGapLineColor);

                // end cap segment ZR view

                if (gap == 0) {
                    Double_t zrFirstAbsorberHeight = ((TGeoBBox*)GetAbsorber(part, seg, 0)->GetVolume()->GetShape())->GetDZ()*2.0;
                    for (Int_t i = 0; i < 4; i++) {
                        for (Int_t j = 0; j < 3; j++) {
                            if ( (part == 0 && (seg == 0 || seg == 2)) || (part == 2 && (seg == 1 || seg == 3)) ) {
                                Int_t zrSegPointSeq[4] = {6, 7, 0, 1};
                                segP[3*iSegPoint+j] = P[100+3*zrSegPointSeq[i]+j];
                            }
                            else {
                                Int_t zrSegPointSeq[4] = {5, 4, 3, 2};
                                segP[3*iSegPoint+j] = P[100+3*zrSegPointSeq[i]+j];
                            }

                            if (j == 2) {
                                if (part == 0)      segP[3*iSegPoint+j] -= zrFirstAbsorberHeight;
                                else if (part == 2) segP[3*iSegPoint+j] += zrFirstAbsorberHeight;
                            }
                        }
                        iSegPoint++;
                    }
                }
                else if (gap == 7) {
                    Double_t zrLastAbsorberHeight = ((TGeoBBox*)GetAbsorber(part, seg, 8)->GetVolume()->GetShape())->GetDZ()*2.0;
                    for (Int_t i = 0; i < 4; i++) {
                        for (Int_t j = 0; j < 3; j++) {
                            if ( (part == 0 && (seg == 0 || seg == 2)) || (part == 2 && (seg == 1 || seg == 3)) ){
                                Int_t zrSegPointSeq[4] = {2, 3, 4, 5};
                                segP[3*iSegPoint+j] = P[100+3*zrSegPointSeq[i]+j];
                            }
                            else {
                                Int_t zrSegPointSeq[4] = {1, 0, 7, 6};
                                segP[3*iSegPoint+j] = P[100+3*zrSegPointSeq[i]+j];
                            }

                            if (j == 2) {
                                if (part == 0)      segP[3*iSegPoint+j] += zrLastAbsorberHeight;
                                else if (part == 2) segP[3*iSegPoint+j] -= zrLastAbsorberHeight;
                            }
                        }
                        iSegPoint++;
                    }
                }

            }
            if (part == 0)      name = TString("MucEastEc");
            else if (part == 2) name = TString("MucWestEc");
            name += TString("Seg");
            name += seg;
            m_MucZR[part][seg] = new BesPolygon2D(name, name, iSegPoint, &segP[0]); // izrPoint = 8
            m_MucZR[part][seg]->SetFillColor(mucColor);
            m_MucZR[part][seg]->SetFillStyle(mucZRStyle);
            m_MucZR[part][seg]->SetLineColor(mucLineColor);
        }
    }

    for (part = 0; part < GetPartNb(); part++) {
        for (seg = 0; seg < GetSegNb(part); seg++) {
            for (gap = 0; gap < GetGapNb(part); gap++) {
                for (strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
                    TGeoPhysicalNode *phyNode = 0;
                    phyNode = GetPhysicalStrip(part, seg, gap, strip);
                    TGeoBBox *stripShape = (TGeoBBox*)phyNode->GetShape();
                    Double_t dx = stripShape->GetDX();
                    Double_t dy = stripShape->GetDY();
                    Double_t dz = stripShape->GetDZ();
                    dz *= m_kStripZMuliple;

                    for (Int_t i = 0; i < 8; i++) {
                        switch (i) {
                        case 0:
                            local[0] = -dx;
                            local[1] = -dy;
                            local[2] = -dz;
                            break;
                        case 1:
                            local[0] =  dx;
                            local[1] = -dy;
                            local[2] = -dz;
                            break;
                        case 2:
                            local[0] =  dx;
                            local[1] =  dy;
                            local[2] = -dz;
                            break;
                        case 3:
                            local[0] = -dx;
                            local[1] =  dy;
                            local[2] = -dz;
                            break;
                        case 4:
                            local[0] = -dx;
                            local[1] = -dy;
                            local[2] =  dz;
                            break;
                        case 5:
                            local[0] =  dx;
                            local[1] = -dy;
                            local[2] =  dz;
                            break;
                        case 6:
                            local[0] =  dx;
                            local[1] =  dy;
                            local[2] =  dz;
                            break;
                        case 7:
                            local[0] = -dx;
                            local[1] =  dy;
                            local[2] =  dz;
                            break;
                        }

                        phyNode->GetMatrix(-1*phyNode->GetLevel())->LocalToMaster(local, &master[0]); // transform to top
                        for (Int_t j = 0; j < 3; j++) P[3*i+j] = master[j];
                    }

                    name  = TString("Muc Part");
                    name += part;
                    name += TString(" Seg");
                    name += seg;
                    name += TString(" Gap");
                    name += gap;
                    name += TString(" Strip");
                    name += strip;

                    m_Muc2DStrip[part][seg][gap][strip] = new Muc2DStrip(name, name, 8, &P[0], part, seg, gap, strip);
                }
            }
        }
    }
}

void
MucROOTGeo::SetNode()
{
    if (m_ROOTGeoInit == 2) { // from GDML

        m_Muc = GetTopVolume();
        if (!m_Muc) std::cout << "m_Muc = 0" << std::endl;

        int gapCount = 0;
        for (int part = 0; part < GetPartNb(); part++) {
            for (int seg = 0; seg < GetSegNb(part); seg++) {
                for (int gap = 0; gap < GetGapNb(part); gap++) {
                    m_StripNum[part][seg][gap] = GetVolumeStripPlane(part, seg, gap)->GetNdaughters();

                    std::stringstream osnameGap;
                    osnameGap << "pv_" << "l" << "Muc" << "P" << part << "S" << seg << "G" << gap << "_" << gapCount;
                    m_NodeGap[part][seg][gap] = GetNode( osnameGap.str() );
                    gapCount++;

                    int segment = 0;
                    if (part==1&&seg==2) segment = 2;
                    std::stringstream osnameStripPlane;
                    osnameStripPlane << "pv_" << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "SP" << "_" << "0";
                    m_NodeStripPlane[part][seg][gap] = GetNode( osnameStripPlane.str() );
                    for (int strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
                        std::stringstream osnameStrip;
                        if (strip < 10) {
                            osnameStrip << "pv_" << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << "00" << strip << "_" << strip;
                        }
                        else if (strip < 100) {
                            osnameStrip << "pv_" << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << "0" << strip << "_" << strip;
                        }
                        else {
                            osnameStrip << "pv_" << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << strip << "_" << strip;
                        }

                        m_NodeStrip[part][seg][gap][strip] = GetNode( osnameStrip.str() );
                    }
                }
            }
        }

        int absorberCount = gapCount;
        for (int part = 0; part < GetPartNb(); part++) {
            for (int seg = 0; seg < GetSegNb(part); seg++) {
                for (int absorber = 0; absorber < GetAbsorberNb(part); absorber++) {
                    std::stringstream osnameAbsorber;
                    osnameAbsorber << "pv_" << "l" << "Muc" << "P" << part << "S" << seg << "Ab" << absorber << "_" << absorberCount;
                    m_NodeAbsorber[part][seg][absorber] = GetNode( osnameAbsorber.str() );
                    absorberCount++;
// 	  if (part != 1) {
// 	    for (int panel = 0; panel < m_kPanelMax; panel++) { // end cap is the absorber panel, to be modified to an absorber
// 	      std::stringstream osnamePanel;
// 	      osnamePanel << "pv_" << "logical" << "Muc" << "Part" << part << "Seg" << seg << "Absorber" << absorber << "Panel" << panel << "_" << panel;
// 	      m_NodeAbsorberPanel[part][seg][absorber][panel] = GetNode( osnamePanel.str() );
// 	    }
// 	  }
                }
            }
        }
    }
    else if (m_ROOTGeoInit == 1) { // from ROOT object
        int gapCount = 0;
        for (int part = 0; part < GetPartNb(); part++) {
            for (int seg = 0; seg < GetSegNb(part); seg++) {
                for (int gap = 0; gap < GetGapNb(part); gap++) {
                    m_NodeGap[part][seg][gap] = m_Muc->GetNode(gapCount);
                    gapCount++;

                    m_NodeStripPlane[part][seg][gap] = GetGap(part, seg, gap)->GetDaughter(0)->GetDaughter(0);  //new geom: gap->box->stripplane
                    m_StripNum[part][seg][gap] = GetStripPlane(part, seg, gap)->GetNdaughters();

                    for (int strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
                        m_NodeStrip[part][seg][gap][strip] = GetStripPlane(part, seg, gap)->GetVolume()->GetNode(strip);
                    }
                }
            }
        }

        int absorberCount = gapCount;
        for (int part = 0; part < GetPartNb(); part++) {
            for (int seg = 0; seg < GetSegNb(part); seg++) {
                for (int absorber = 0; absorber < GetAbsorberNb(part); absorber++) {
                    m_NodeAbsorber[part][seg][absorber] = m_Muc->GetNode( absorberCount );
                    absorberCount++;

// 	  if (part != 1) {
// 	    for (int panel = 0; panel < m_kPanelMax; panel++) { // end cap is the absorber panel, to be modified to an absorber
// 	      m_NodeAbsorberPanel[part][seg][absorber][panel] = m_NodeAbsorber[part][seg][absorber]->GetVolume()->GetNode(panel);
// 	    }
// 	  }
                }
            }
        }
    }

}

void
MucROOTGeo::SetVolumeDefaultVis()
{
    m_Muc->SetLineColor(m_MucColor);
    m_Muc->SetVisibility(0);

    for (int part = 0; part < GetPartNb(); part++) {
        for (int seg = 0; seg < GetSegNb(part); seg++) {
            for (int absorber = 0; absorber < GetAbsorberNb(part); absorber++) {
                GetVolumeAbsorber(part, seg, absorber)->SetLineColor(m_absorberColor);
                GetVolumeAbsorber(part, seg, absorber)->SetVisibility(0);
// 	if (part != 1) {
// 	  for (int panel = 0; panel < m_kPanel[part]; panel++) {
// 	    GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetLineColor(m_absorberColor);
// 	    GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetVisibility(0);
// 	  }
// 	}
            }

            for (int gap = 0; gap < GetGapNb(part); gap++) {
                GetVolumeGap(part, seg, gap)->SetLineColor(m_gapColor);
                GetVolumeGap(part, seg, gap)->SetVisibility(0);

                GetVolumeBox(part, seg, gap)->SetLineColor(m_gapColor);
                GetVolumeBox(part, seg, gap)->SetVisibility(0);

                if (part == 1){
                    for (int sb = 0; sb < m_kSmallBlockMax; sb++){
                        GetVolumeAbsorberSmallBlock( gap, sb)->SetLineColor(m_absorberColor);
                        GetVolumeAbsorberSmallBlock( gap, sb)->SetVisibility(0);
                    }
                }

                GetVolumeStripPlane(part, seg, gap)->SetLineColor(m_gapColor);
                GetVolumeStripPlane(part, seg, gap)->SetVisibility(0);

                for (int strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
                    GetVolumeStrip(part, seg, gap, strip)->SetLineColor(m_stripColor);
                    GetVolumeStrip(part, seg, gap, strip)->SetVisibility(0);
                }

                for (int up = 0; up < 2; up ++){
                    GetVolumeBoxSurface(part, seg, gap, up)->SetLineColor(m_bakeliteColor);   //need change
                    GetVolumeBoxSurface(part, seg, gap, up)->SetVisibility(0);
                    int seg_shift = 0;
                    if (part == 1 && seg == 2)seg_shift = 1;
                    for (int panel = 0; panel < m_kPanel[part] + seg_shift; panel++) {
                        GetVolumeGasChamber(part, seg, gap, up, panel)->SetLineColor(m_gasChamberColor);
                        GetVolumeGasChamber(part, seg, gap, up, panel)->SetVisibility(0);

                        GetVolumeGasBorder(part, seg, gap, up, panel)->SetLineColor(m_bakeliteColor);
                        GetVolumeGasBorder(part, seg, gap, up, panel)->SetVisibility(0);

                    }
                    for (int panel = 0; panel < m_kBakelitePanel[part][up] + seg_shift; panel++) {
                        for (int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
                            GetVolumeBakelite(part, seg, gap, up, panel, bakelite)->SetLineColor(m_bakeliteColor);
                            GetVolumeBakelite(part, seg, gap, up, panel, bakelite)->SetVisibility(0);
                        }
                    }
                }

// 	for (int panel = 0; panel < m_kPanel[part]; panel++) {    //comment out 2007.1.2
// 	  for(int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
// 	    GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetLineColor(m_bakeliteColor);
// 	    GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetVisibility(0);
// 	  }
// 	  for (int gasChamber = 0; gasChamber < m_kGasChamber; gasChamber++) {
// 	    GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetLineColor(m_gasChamberColor);
// 	    GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetVisibility(0);
// 	  }
// 	}
            }
        }
    }

    for (int part = 0; part < GetPartNb(); part++) {
        for (int seg = 0; seg < GetSegNb(part); seg++) {
            for (int gap = 0; gap < GetGapNb(part); gap++) {
                GetGap(part, seg, gap)->SetVisibility(0);
                for (int strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
                    GetStrip(part, seg, gap, strip)->SetVisibility(0);
                }
            }
        }
    }

}

//Long Peixun's update: Set default 3D color
void MucROOTGeo::SetPhysicalDefaultVis()
{
    for (int part = 0; part < GetPartNb(); part++)
    {
        for (int seg = 0; seg < GetSegNb(part); seg++)
        {
            for (int gap = 0; gap < GetGapNb(part); gap++)
            {
                for (int strip = 0; strip < GetStripNb(part, seg, gap); strip++)
                    //Long Peixun's update: gray -> more deep gray
                    GetPhysicalStrip(part, seg, gap, strip)->SetLineColor(922);
            }
        }
    }
}

void
MucROOTGeo::SetAllVisible()
{
    m_Muc->SetLineColor(m_MucColor);
    m_Muc->SetVisibility(0);

    for (int part = 0; part < GetPartNb(); part++) {
        for (int seg = 0; seg < GetSegNb(part); seg++) {
            for (int absorber = 0; absorber < GetAbsorberNb(part); absorber++) {
                GetVolumeAbsorber(part, seg, absorber)->SetLineColor(m_absorberColor);
                GetVolumeAbsorber(part, seg, absorber)->SetVisibility(1);
// 	if (part != 1) {
// 	  for (int panel = 0; panel < m_kPanel[part]; panel++) {
// 	    GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetLineColor(m_absorberColor);
// 	    GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetVisibility(0);
// 	  }
// 	}
            }

            for (int gap = 0; gap < GetGapNb(part); gap++) {
                GetVolumeGap(part, seg, gap)->SetLineColor(m_gapColor);
                GetVolumeGap(part, seg, gap)->SetVisibility(1);

                GetVolumeBox(part, seg, gap)->SetLineColor(m_stripColor);
                GetVolumeBox(part, seg, gap)->SetVisibility(1);

                if (part == 1){
                    for (int sb = 0; sb < m_kSmallBlockMax; sb++){
                        GetVolumeAbsorberSmallBlock( gap, sb)->SetLineColor(m_absorberColor);
                        GetVolumeAbsorberSmallBlock( gap, sb)->SetVisibility(1);
                    }
                }

                GetVolumeStripPlane(part, seg, gap)->SetLineColor(m_gapColor);
                GetVolumeStripPlane(part, seg, gap)->SetVisibility(1);

                for (int strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
                    GetVolumeStrip(part, seg, gap, strip)->SetLineColor(m_stripColor);
                    GetVolumeStrip(part, seg, gap, strip)->SetVisibility(1);
                }

                for (int up = 0; up < 2; up ++){
                    GetVolumeBoxSurface(part, seg, gap, up)->SetLineColor(m_gapColor);   //need change
                    GetVolumeBoxSurface(part, seg, gap, up)->SetVisibility(1);
                    int seg_shift = 0;
                    if (part == 1 && seg == 2)seg_shift = 1;
                    for (int panel = 0; panel < m_kPanel[part] + seg_shift; panel++) {
                        GetVolumeGasChamber(part, seg, gap, up, panel)->SetLineColor(m_gasChamberColor);
                        GetVolumeGasChamber(part, seg, gap, up, panel)->SetVisibility(1);

                        GetVolumeGasBorder(part, seg, gap, up, panel)->SetLineColor(m_bakeliteColor);
                        GetVolumeGasBorder(part, seg, gap, up, panel)->SetVisibility(1);
                    }
                    for (int panel = 0; panel < m_kBakelitePanel[part][up] + seg_shift; panel++) {
                        for (int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
                            GetVolumeBakelite(part, seg, gap, up, panel, bakelite)->SetLineColor(m_bakeliteColor);
                            GetVolumeBakelite(part, seg, gap, up, panel, bakelite)->SetVisibility(1);
                        }
                    }


                }

// 	for (int panel = 0; panel < m_kPanel[part]; panel++) {    //comment out 2007.1.2
// 	  for(int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
// 	    GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetLineColor(m_bakeliteColor);
// 	    GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetVisibility(0);
// 	  }
// 	  for (int gasChamber = 0; gasChamber < m_kGasChamber; gasChamber++) {
// 	    GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetLineColor(m_gasChamberColor);
// 	    GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetVisibility(0);
// 	  }
// 	}
            }
        }
    }

    for (int part = 0; part < GetPartNb(); part++) {
        for (int seg = 0; seg < GetSegNb(part); seg++) {
            for (int gap = 0; gap < GetGapNb(part); gap++) {
                GetGap(part, seg, gap)->SetVisibility(0);
                for (int strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
                    GetStrip(part, seg, gap, strip)->SetVisibility(0);
                }
            }
        }
    }

}


void
MucROOTGeo::SetQuarterVisible()
{
    for (int part = 0; part < m_kPart; part++) {
        for (int seg = 0; seg < m_kSeg[part]; seg++) {
            for (int absorber = 0; absorber < m_kAbsorber[part]; absorber++) {
                if (part == 1) {
                    if (seg > 2) GetVolumeAbsorber(part, seg, absorber)->SetVisibility(1);
                }
                else {
                    GetVolumeAbsorber(part, seg, absorber)->SetVisibility(0);
                    for (int panel = 0; panel < m_kPanel[part]; panel++) {
                        if (seg > 0 && seg < 3) GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetVisibility(1);
                    }
                }
            }

            for (int gap = 0; gap < m_kGap[part]; gap++) {
                if (part == 1 && seg > 2) {
                    GetVolumeGap(part, seg, gap)->SetVisibility(1);
                    GetGap(part, seg, gap)->SetVisibility(1);
                }
                else {
                    GetVolumeGap(part, seg, gap)->SetVisibility(0);
                    GetGap(part, seg, gap)->SetVisibility(0);
                }
                for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
                    GetVolumeStrip(part, seg, gap, strip)->SetVisibility(0);
                }

// 	for (int panel = 0; panel < m_kPanel[part]; panel++) {
// 	  for (int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
// 	    if ((part != 1 && seg > 0 && seg < 3) || (part == 1 && seg > 2)) GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetVisibility(1);
// 	  }
// 	  for (int gasChamber = 0; gasChamber < m_kGasChamber; gasChamber++) {
// 	    if ((part != 1 && seg > 0 && seg < 3) || (part == 1 && seg > 2)) GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetVisibility(1);
// 	  }
// 	}
            }
        }
    }
}

void
MucROOTGeo::SetHalfVisible()
{
    for (int part = 0; part < m_kPart; part++) {
        for (int seg = 0; seg < m_kSeg[part]; seg++) {
            for (int absorber = 0; absorber < m_kAbsorber[part]; absorber++) {
                if (part == 1) {
                    if (seg < 3 || seg > 5) GetVolumeAbsorber(part, seg, absorber)->SetVisibility(1);
                }
                else {
                    GetVolumeAbsorber(part, seg, absorber)->SetVisibility(0);
                    for (int panel = 0; panel < m_kPanel[part]; panel++) {
                        if (seg == 0 || seg == 3) GetVolumeAbsorberPanel(part, seg, absorber, panel)->SetVisibility(1);
                    }
                }
            }

            for (int gap = 0; gap < m_kGap[part]; gap++) {
                if (part == 1 && (seg < 3 || seg > 5)) {
                    GetVolumeGap(part, seg, gap)->SetVisibility(1);
                    GetGap(part, seg, gap)->SetVisibility(1);
                }
                else {
                    GetVolumeGap(part, seg, gap)->SetVisibility(0);
                    GetGap(part, seg, gap)->SetVisibility(0);
                }
                for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
                    GetVolumeStrip(part, seg, gap, strip)->SetVisibility(0);
                }

// 	for (int panel = 0; panel < m_kPanel[part]; panel++) {
// 	  for (int bakelite = 0; bakelite < m_kBakelite; bakelite++) {
// 	    if (part != 1 && (seg == 0 || seg == 3)) GetVolumeBakelite(part, seg, gap, panel, bakelite)->SetVisibility(1);
// 	  }
// 	  for (int gasChamber = 0; gasChamber < m_kGasChamber; gasChamber++) {
// 	    if (part != 1 && (seg == 0 || seg == 3)) GetVolumeGasChamber(part, seg, gap, panel, gasChamber)->SetVisibility(1);
// 	  }
// 	}
            }
        }
    }
}

void
MucROOTGeo::SetNoEndVisible()
{
    int part = 1;
    for (int seg = 0; seg < m_kSeg[part]; seg++) {
        for (int absorber = 0; absorber < m_kAbsorber[part]; absorber++) {
            GetVolumeAbsorber(part, seg, absorber)->SetVisibility(1);
        }

        for (int gap = 0; gap < m_kGap[part]; gap++) {
            GetVolumeGap(part, seg, gap)->SetVisibility(1);
            GetGap(part, seg, gap)->SetVisibility(1);
        }
    }
}

void
MucROOTGeo::SetPhysicalNode()
{

    if (gGeoManager == 0) std::cout << "Create gGeoManager first" << std::endl;
    TGeoNode *bes = gGeoManager->GetTopNode();
    TGeoNode *muc = bes->GetDaughter(3);  //3->0

    for (int part = 0; part < GetPartNb(); part++) {
        for (int seg = 0; seg < GetSegNb(part); seg++) {
            for (int gap = 0; gap < GetGapNb(part); gap++) {
                TGeoNode *nodeGap = GetGap(part, seg, gap);
                m_PhysicalGap[part][seg][gap] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                                TString("/") + muc->GetName() +
                                                TString("/") + nodeGap->GetName() +
                                                TString("/") + nodeGap->GetDaughter(0)->GetName());

                m_PhysicalGap[part][seg][gap]->SetVisibility(0);
                m_PhysicalGap[part][seg][gap]->SetIsVolAtt(kFALSE);
                m_PhysicalGap[part][seg][gap]->SetLineColor(m_gapColor);

                TGeoNode *nodeStripPlane = GetStripPlane(part, seg, gap);

                for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
                    TGeoNode *nodeStrip = GetStrip(part, seg, gap, strip);
                    m_PhysicalStrip[part][seg][gap][strip] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                            TString("/") + muc->GetName() +
                            TString("/") + nodeGap->GetName() +
                            TString("/") + nodeGap->GetDaughter(0)->GetName() +
                            TString("/") + nodeStripPlane->GetName() +
                            TString("/") + nodeStrip->GetName() );
                    m_PhysicalStrip[part][seg][gap][strip]->SetVisibility(0);
                    m_PhysicalStrip[part][seg][gap][strip]->SetIsVolAtt(kFALSE);
                    m_PhysicalStrip[part][seg][gap][strip]->SetLineColor(m_stripColor);
                }
            }
        }
    }

    for (int part = 0; part < GetPartNb(); part++) {
        for (int seg = 0; seg < GetSegNb(part); seg++) {
            for (int absorber = 0; absorber < GetAbsorberNb(part); absorber++) {
                TGeoNode *nodeAbsorber = GetAbsorber(part, seg, absorber);
// 	if (part == 1) {
                m_PhysicalAbsorber[part][seg][absorber][0] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                        TString("/") + muc->GetName() +
                        TString("/") + nodeAbsorber->GetName() );
                m_PhysicalAbsorber[part][seg][absorber][0]->SetVisibility(0);
                m_PhysicalAbsorber[part][seg][absorber][0]->SetIsVolAtt(kFALSE);
                m_PhysicalAbsorber[part][seg][absorber][0]->SetLineColor(m_absorberColor);
// 	}
// 	else {
// 	  for (int panel = 0; panel < m_kPanelMax; panel++) {
// 	    TGeoNode *nodeAbsorberPanel = GetAbsorberPanel(part, seg, absorber, panel);
// 	    m_PhysicalAbsorber[part][seg][absorber][panel] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
// 											    TString("/") + muc->GetName() +
// 											    TString("/") + nodeAbsorber->GetName() +
// 											    TString("/") + nodeAbsorberPanel->GetName() );
// 	    m_PhysicalAbsorber[part][seg][absorber][panel]->SetVisibility(0);
// 	    m_PhysicalAbsorber[part][seg][absorber][panel]->SetIsVolAtt(kFALSE);
// 	    m_PhysicalAbsorber[part][seg][absorber][panel]->SetLineColor(m_absorberColor);
// 	  }
// 	}
            }
        }
    }

    SetDetector();
}

void
MucROOTGeo::SetDetector()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

    m_DetectorsArray->Clear();
    for (int part = 0; part < GetPartNb(); part++) {
        for (int seg = 0; seg < GetSegNb(part); seg++) {
            for (int gap = 0; gap < GetGapNb(part); gap++) {
                TGeoPhysicalNode *phyNode = 0;
                phyNode = GetPhysicalGap(part, seg, gap);
                if (phyNode) {
                    phyNode->SetVisibility(0); // set all invisible before set any visible
                    if ( (part == 1 && seg <= 8) ||
                            (part == 0 && seg <= 3) ||
                            (part == 2 && seg <= 3) ) {
                        m_DetectorsArray->Add( phyNode );
                    }
                    else if (view && view->GetVisFull3DMuc()) {
                        m_DetectorsArray->Add( phyNode );
                    }
                }
            }
            for (int absorber = 0; absorber < GetAbsorberNb(part); absorber++) {
                for (int panel = 0; panel < m_kPanelMax; panel++) {
                    TGeoPhysicalNode *phyNode = 0;
                    phyNode = GetPhysicalAbsorber(part, seg, absorber, panel);
                    if (phyNode) {
                        phyNode->SetVisibility(0);
                        if ( (part == 1 && seg <= 8) ||
                                (part == 0 && seg <= 3) ||
                                (part == 2 && seg <= 3) ) {
                            m_DetectorsArray->Add( phyNode );
                        }
                        else if (view && view->GetVisFull3DMuc()) {
                            m_DetectorsArray->Add( phyNode );
                        }
                    }
                }
            }
        }
    }

}

void
MucROOTGeo::SetHits()
{
    // set previous event hits to default vis
    for (int i = 0; i < m_HitsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_HitsArray->At(i);
        phyNode->SetLineColor(m_stripColor);
        phyNode->SetVisibility(0);
    }
    m_HitsArray->Clear("C");

    // set previous event 2D hits info to default
    for (int i = 0; i < m_2DHitsArray->GetEntries(); i++) {
        Muc2DStrip *aStrip = (Muc2DStrip*)m_2DHitsArray->At(i);
        aStrip->ClearInfo();
        aStrip->AddInfo(aStrip->GetTitle());
        aStrip->CloseInfo();
    }
    m_2DHitsArray->Clear("C");

    //Long Peixun's update: Consider gEvent == NULL
    int NDigiCol = 0;
    if (gEvent)
    {
        m_MucDigiCol = gEvent->GetMucDigiCol();
        //Long Peixun's update: Consider m_MucDigiCol == NULL
        if (m_MucDigiCol) NDigiCol = m_MucDigiCol->GetEntries();
        else NDigiCol = 0;
    }

    for (int i = 0; i < NDigiCol; i++) {
        Identifier aMucID( ((TMucDigi*)m_MucDigiCol->At(i))->getIntId() );
        int part  = MucID::part(  aMucID );
        int seg   = MucID::seg(   aMucID );
        int gap   = MucID::gap(   aMucID );
        int strip = MucID::strip( aMucID );

        TGeoPhysicalNode *phyNode = GetPhysicalStrip( part, seg, gap, strip );  //Long Peixun's update: code optimization
        if (phyNode) m_HitsArray->Add( phyNode );

        Muc2DStrip *aStrip = 0;
        aStrip = m_Muc2DStrip[part][seg][gap][strip];
        if (aStrip) {
            aStrip->ClearInfo();
            aStrip->AddInfo(aStrip->GetTitle());

            char data[100];
            sprintf(data, "Fired");
            aStrip->AddInfo( TString(data) );

            aStrip->CloseInfo();

            m_2DHitsArray->Add(aStrip);
        }
    }
}

void
MucROOTGeo::SetVisMucDetector()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

    for (int i = 0; i < m_DetectorsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_DetectorsArray->At(i);
        phyNode->SetVisibility(0);

        if (view && view->GetVisMucGlobal()) {
            int part = GetPart(phyNode);
            if (part == 0 && view->GetVisMucEast()   ||
                    part == 1 && view->GetVisMucBarrel() ||
                    part == 2 && view->GetVisMucWest()   )
                phyNode->SetVisibility(1);
        }
    }
}

void
MucROOTGeo::SetVisMucHits()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

    for (int i = 0; i < m_HitsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_HitsArray->At(i);
        if (view && view->GetVisMucHitsGlobal()) {
            int part = GetPart(phyNode);
            if (part == 0 && view->GetVisMucHitsEast()   ||
                    part == 1 && view->GetVisMucHitsBarrel() ||
                    part == 2 && view->GetVisMucHitsWest()   ) {
                phyNode->SetVisibility(1);
                continue;
            }
            phyNode->SetLineColor(922);  //Long Peixun's update: Set color despite of visibility
        }
        phyNode->SetVisibility(0);
    }
}

int
MucROOTGeo::GetPartNb()
{
    return m_kPart;  //Long Peixun's update: code optimization
}

int
MucROOTGeo::GetSegNb(int part)
{
    return m_kSeg[part];    //Long Peixun's update: code optimization
}

int
MucROOTGeo::GetGapNb(int part)
{
    return m_kGap[part];    //Long Peixun's update: code optimization
}

int
MucROOTGeo::GetStripNb(int part, int seg, int gap)
{
    return m_StripNum[part][seg][gap];  //Long Peixun's update: code optimization
}

int
MucROOTGeo::GetAbsorberNb(int part)
{
    return m_kAbsorber[part];   //Long Peixun's update: code optimization
}

TGeoVolume*
MucROOTGeo::GetVolumeAbsorber( int part, int seg, int absorber )
{
    std::stringstream osname;
    osname << "l" << "Muc" << "P" << part << "S" << seg << "Ab" << absorber;

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeAbsorberSmallBlock( int gap, int sb )
{
    std::stringstream osname;
    osname << "VolumeSmallBlock" << "Gap" << gap << "SB" << sb;

    return GetLogicalVolume( osname.str() );
}



TGeoVolume*
MucROOTGeo::GetVolumeAbsorberPanel( int part, int seg, int absorber, int panel )
{
    std::stringstream osname;  //useless now 2007.1.2
    osname << "logical" << "Muc" << "Part" << part << "Seg" << seg << "Absorber" << absorber << "Panel" << panel;

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeGap( int part, int seg, int gap )
{
    std::stringstream osname;
    osname << "l" << "Muc" << "P" << part << "S" << seg << "G" << gap;

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeBox( int part, int seg, int gap)
{
    std::stringstream osname;
    osname << "l" << "Muc" << "P" << part << "S" << seg << "G" << gap << "Al";

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeBoxSurface( int part, int seg, int gap ,int up)
{
    std::stringstream osname;
    int segment = 0;
    if (part ==1 && seg ==2) segment = 2;
    osname << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "AlSf" <<up;

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeStripPlane( int part, int seg, int gap )
{
    int segment = 0;
    if (part==1&&seg==2) segment = 2;
    std::stringstream osname;
    osname << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "SP";

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeStrip( int part, int seg, int gap, int strip )
{
    std::stringstream osname;
    int segment = 0;
    if (part == 1 && seg == 2) segment = 2;
    if (strip < 10) {
        osname << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << "00" << strip;
    }
    else if (strip<100){
        osname << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << "0" << strip;
    }
    else {
        osname << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "s" << strip;
    }

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeGasChamber( int part, int seg, int gap, int rpcUpDown, int panel )
{
    std::stringstream osname;
    int segment = 0;
    if (part ==1 && seg ==2) segment = 2;
    osname << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "R" << rpcUpDown << "Pn" << panel << "C";

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeGasBorder( int part, int seg, int gap, int rpcUpDown, int panel )
{
    std::stringstream osname;
    int segment = 0;
    if (part ==1 && seg ==2) segment = 2;
    osname << "l" << "Muc" << "P" << part%2 << "S" << segment << "G" << gap << "R" << rpcUpDown << "Pn" << panel << "GB";

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MucROOTGeo::GetVolumeBakelite( int part, int seg, int gap, int RpcUpDown, int panel, int bakelite )
{
    std::stringstream osname;
    int segment = 0;
    if (part ==1 && seg ==2) segment = 2;
    osname << "l" << "Muc" << "P" << part << "S" << segment << "G" << gap << "R" << RpcUpDown << "Pn" << panel<< "B" << bakelite;

    return GetLogicalVolume( osname.str() );
}

TGeoNode*
MucROOTGeo::GetAbsorber( int part, int seg, int absorber )
{
    if (m_NodeAbsorber[part][seg][absorber] != 0) {
        return m_NodeAbsorber[part][seg][absorber];
    }
    else {
        return 0;
    }
}

TGeoNode*
MucROOTGeo::GetAbsorberPanel( int part, int seg, int absorber, int panel )
{
    if (m_NodeAbsorberPanel[part][seg][absorber][panel] != 0) {
        return m_NodeAbsorberPanel[part][seg][absorber][panel];
    }
    else {
        return 0;
    }
}

TGeoNode*
MucROOTGeo::GetGap( int part, int seg, int gap )
{
    if (m_NodeGap[part][seg][gap] != 0) {
        return m_NodeGap[part][seg][gap];
    }
    else {
        return 0;
    }
}

TGeoNode*
MucROOTGeo::GetStripPlane( int part, int seg, int gap )
{
    if (m_NodeStripPlane[part][seg][gap] != 0) {
        return m_NodeStripPlane[part][seg][gap];
    }
    else {
        return 0;
    }
}

TGeoNode*
MucROOTGeo::GetStrip( int part, int seg, int gap, int strip )
{
    if (m_NodeStrip[part][seg][gap][strip] != 0) {
        return m_NodeStrip[part][seg][gap][strip];
    }
    else {
        return 0;
    }
}

TGeoPhysicalNode*
MucROOTGeo::GetPhysicalAbsorber( int part, int seg, int absorber, int panel )
{
    return m_PhysicalAbsorber[part][seg][absorber][panel];
}

TGeoPhysicalNode*
MucROOTGeo::GetPhysicalGap( int part, int seg, int gap )
{
    if (m_PhysicalGap[part][seg][gap] != 0) {
        return m_PhysicalGap[part][seg][gap];
    }
    else {
        return 0;
    }
}

TGeoPhysicalNode*
MucROOTGeo::GetPhysicalStrip( int part, int seg, int gap, int strip )
{
    if (m_PhysicalStrip[part][seg][gap][strip] != 0) {
        return m_PhysicalStrip[part][seg][gap][strip];
    }
    else {
        return 0;
    }
}

Muc2DStrip*
MucROOTGeo::Get2DStrip( int part, int seg, int gap, int strip )
{
    if (m_Muc2DStrip[part][seg][gap][strip]) return m_Muc2DStrip[part][seg][gap][strip];
    else return 0;
}

int
MucROOTGeo::GetPart(TGeoPhysicalNode* phyNode)
{
    for (int part = 0; part < GetPartNb(); part++) {
        for (int seg = 0; seg < GetSegNb(part); seg++) {
            for (int gap = 0; gap < GetGapNb(part); gap++) {
                if (phyNode == GetPhysicalGap(part, seg, gap)) {
                    return part;
                }
                for (int strip = 0; strip < m_StripNum[part][seg][gap]; strip++) {
                    if (phyNode == GetPhysicalStrip(part, seg, gap, strip)) {
                        return part;
                    }
                }
            }
            for (int absorber = 0; absorber < GetAbsorberNb(part); absorber++) {
                for (int panel = 0; panel < m_kPanelMax; panel++) {
                    if (phyNode == GetPhysicalAbsorber(part, seg, absorber, panel)) {
                        return part;
                    }
                }
            }
        }
    }

    return -1;
}

Bool_t
MucROOTGeo::IsZRVisible( int part, int seg )
{
    if (part != 1) return true;

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    Double_t viewPhi = view->GetLongitude();
    viewPhi = Range360(viewPhi);

    Int_t viewSeg = Int_t(viewPhi/45.0+0.5);
    Int_t upSeg   = viewSeg+2;
    Int_t downSeg = viewSeg-2;
    if (upSeg   < 0) upSeg   += 8;
    else if (upSeg   >= 8) upSeg   -= 8;
    if (downSeg < 0) downSeg += 8;
    else if (downSeg >= 8) downSeg -= 8;

    if (seg == upSeg || seg == downSeg) return true;
    else return false;
}

Double_t
MucROOTGeo::Range360(Double_t input)
{
    if (input >= 360.0) {
        do {
            input -= 360.0;
        }
        while (input >= 360.0);
    }
    else if (input < 0.0) {
        do {
            input += 360.0;
        }
        while (input < 0.0);
    }

    return input;
}

void
MucROOTGeo::Draw(Option_t *option)
{
    TString opt = option;
    opt.ToUpper();

    if (!m_2DGeoInit) cout << "MucROOTGeo::Draw2D(), 2D Geometry not initialized!" << endl;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) cout << "MucROOTGeo::Draw(), BesView not found" << endl;

    if (view->GetVisMucGlobal()) { // MucVisGlobal
        // seg and gap
        for (Int_t part = GetPartNb()-1; part >= 0; part--) { // draw west first
            if ( (part == 0 && view->GetVisMucEast())   ||
                    (part == 1 && view->GetVisMucBarrel()) ||
                    (part == 2 && view->GetVisMucWest()) ) {
                for (Int_t seg = 0; seg < GetSegNb(part); seg++) {
                    if (opt.Contains("XY")) {
                        if (m_MucXY[part][seg]) m_MucXY[part][seg]->Draw("");
                        for (Int_t gap = 0; gap < GetGapNb(part); gap++) {
                            if (m_MucXYGap[part][seg][gap]) m_MucXYGap[part][seg][gap]->Draw("");
                        }
                    }
                    if (opt.Contains("ZR")) {
                        if ( m_MucZR[part][seg] ) { // && IsZRVisible(part, seg) ) {
                            if (part == 1) m_MucZR[part][seg]->SetRotatable(true);
                            m_MucZR[part][seg]->Draw("");
                        }
                        for (Int_t gap = 0; gap < GetGapNb(part); gap++) {
                            if (m_MucZRGap[part][seg][gap]) {
                                if (part == 1) m_MucZRGap[part][seg][gap]->SetRotatable(true);
                                m_MucZRGap[part][seg][gap]->Draw("");
                            }
                        }
                    }
                }
            }
        }

        // strips, should be drawn after all segs and gaps have been drawn
        for (Int_t part = GetPartNb()-1; part >= 0; part--) { // draw west first
            for (Int_t seg = 0; seg < GetSegNb(part); seg++) {
                for (Int_t gap = 0; gap < GetGapNb(part); gap++) {
                    for (Int_t strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
                        if (m_Muc2DStrip[part][seg][gap][strip]) {
                            m_Muc2DStrip[part][seg][gap][strip]->SetFired(false);
                            if ( (part == 0 && view->GetVisMucEast())   ||
                                    (part == 1 && view->GetVisMucBarrel()) ||
                                    (part == 2 && view->GetVisMucWest()) ) {
                                m_Muc2DStrip[part][seg][gap][strip]->Draw("");
                            }
                        }
                    }
                }
            }
        }

    }
}

void
MucROOTGeo::DrawHits(Option_t *option)
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) cout << "MucROOTGeo::DrawHits(), BesView not found" << endl;


    if (view->GetVisMucHitsGlobal()) {

        /*
        for (Int_t part = GetPartNb()-1; part >= 0; part--) { // draw west first
          for (Int_t seg = 0; seg < GetSegNb(part); seg++) {
        for (Int_t gap = 0; gap < GetGapNb(part); gap++) {
          for (Int_t strip = 0; strip < GetStripNb(part, seg, gap); strip++) {
            if (m_Muc2DStrip[part][seg][gap][strip]) m_Muc2DStrip[part][seg][gap][strip]->ClearInfo();
          }
        }
          }
          }*/

        //Long Peixun's update: Use m_2DHitsArray which contains ready hits rather than rebuild hits
        for (int i = 0; i < m_2DHitsArray->GetEntries(); ++i)
        {
            Muc2DStrip* aStrip = (Muc2DStrip*)m_2DHitsArray->At(i);
            if (aStrip)
            {
                if ((aStrip->GetPart() == 0 && view->GetVisMucHitsEast())   ||
                    (aStrip->GetPart() == 1 && view->GetVisMucHitsBarrel()) ||
                    (aStrip->GetPart() == 2 && view->GetVisMucHitsWest()) ) 
                {
                    aStrip->SetFired(true);
                    aStrip->Draw();
                }
            }
        }

/*         if (m_MucDigiCol) {
            for (int i = 0; i < m_MucDigiCol->GetEntries(); i++) {
                TMucDigi *aMucDigi = (TMucDigi*)m_MucDigiCol->At(i);

                //cout << aMucDigi->getIntId() << endl;
                Identifier aMucID( aMucDigi->getIntId() );
                int part  = MucID::part(  aMucID );
                int seg   = MucID::seg(   aMucID );
                int gap   = MucID::gap(   aMucID );
                int strip = MucID::strip( aMucID );

                Muc2DStrip *aStrip = 0;
                aStrip = m_Muc2DStrip[part][seg][gap][strip];
                if (aStrip) {
                    if ( (part == 0 && view->GetVisMucHitsEast())   ||
                            (part == 1 && view->GetVisMucHitsBarrel()) ||
                            (part == 2 && view->GetVisMucHitsWest()) ) {
                        aStrip->SetFired(true);
                        aStrip->Draw();
                    }
                }
            }
        } */
    }
}

