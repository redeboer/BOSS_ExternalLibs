//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Tof Geometry General for EventDisplay
 *
 *    2004/12/10   Zhengyun You      Peking University
 *                 named from MdcGeo to MdcROOTGeo
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
#include <TMath.h>

#include "BesVisLib/MdcROOTGeo.h"
#include "Identifier/MdcID.h"
#include "BesVisLib/BesEvent.h"
#include "BesVisLib/BesView.h"
#include "RawEvent/RawDataUtil.h"

const int
MdcROOTGeo::m_kReplica[m_kLayer] = { 40,  44,  48,  56,  64,  72,  80,  80,
                                     76,  76,  88,  88,  100, 100, 112, 112, 128, 128, 140, 140,
                                     160, 160, 160, 160, 176, 176, 176, 176, 208, 208, 208, 208, 240, 240, 240, 240,
                                     256, 256, 256, 256, 256, 256, 256, 256,
                                     288, 288, 288, 288, 288, 288
                                   };

const int
MdcROOTGeo::m_kStereoDir[m_kTrueLayer] = { 1, 1, 1, 1, -1, -1, -1, -1,
        0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
        1, 1, 1, 1, -1, -1, -1, -1, 1, 1, 1, 1, -1, -1, -1, -1,
        0, 0, 0, 0,  0,  0,  0
                                         };
const int
MdcROOTGeo::m_kiCorrectLayer[m_kCorrectLayer] =   {9, 11, 13, 15, 17, 19,  36,  38,  40,  42};
const int
//MdcROOTGeo::m_kiCorrectReplica[m_kCorrectLayer] = {38, 44, 50, 56, 65, 71, 127, 127, 144, 143};
MdcROOTGeo::m_kiCorrectReplica[m_kCorrectLayer] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

MdcROOTGeo::MdcROOTGeo(Bool_t cgem_flag)    //Long Peixun's update: Add CGEM flag
        : SubDetectorROOTGeo()
{
    m_MdcColor         = 3;
    m_segmentColor     = 0;
    m_hypeColor        = 3;
    m_tubeColor        = 4;
    //m_twistedTubsColor = kBlue;
    m_twistedTubsColor = 9;  //Long Peixun's update: Blue -> Green, as same as XY View
    //m_replicaColor     = kCyan;
    m_replicaColor     = 8;  //Long Peixun's update: Grey -> Blue, as same as XY View

    //Long Peixun's update: According to CGEM flag, set start wire layer
    if (cgem_flag) m_StartLayer = 8;
    else m_StartLayer = 0;

    k_TFire = kTRUE;
    k_QFire = kTRUE;
    k_QNotOverflow = kFALSE;

    // Default constructor.
    for (int layer = 0; layer < m_kLayer; layer++) {
        m_NodeLayer[layer] = 0;
        for (int replica = 0; replica < m_kReplicaMax; replica++) {
            m_NodeReplica[layer][replica] = 0;
            m_PhysicalReplica[layer][replica] = 0;
        }
    }

    for (int i = 0; i < m_kCorrectLayer; i++) {
        m_CorrectMap[m_kiCorrectLayer[i]] = m_kiCorrectReplica[i];
    }

    for (int layer = 0; layer < m_kTrueLayer; layer++) {
        for (int replica = 0; replica < m_kReplicaMax; replica++) {
            m_Mdc2DWire[layer][replica] = 0;
        }
    }
}

MdcROOTGeo::~MdcROOTGeo()
{
    //Long Peixun's update: Add destructor
    cout << "delete old MdcROOTGeo" << endl;
    for (Int_t layer = 0; layer < m_kTrueLayer; ++layer)
    {
        Int_t simuLayer = GetSimuLayer(layer);
        for (Int_t replica = 0; replica < m_kReplica[simuLayer]; ++replica)
        {
            delete m_Mdc2DWire[layer][replica];
        }
    }
    delete m_MdcXY;
    delete m_MdcZR[0];
    delete m_MdcZR[1];
}

void
MdcROOTGeo::InitFromGDML(const char *gdmlFile, const char *setupName)
{
    m_ROOTGeoInit = 2;
    ReadGdml(gdmlFile, setupName);
    SetNode();
}

void
MdcROOTGeo::InitFromROOT( TGeoVolume *vol )
{
    m_ROOTGeoInit = 1;
    SetVolumeMdc(vol);
    SetNode();
}

void
MdcROOTGeo::Init2DGeometry()
{
    if (m_ROOTGeoInit != 1 && m_ROOTGeoInit != 2) {
        cout << "MdcROOTGeo::Init2DGeometry, ROOT Geometry not Initialized yet!" << endl;
        return;
    }

    m_2DGeoInit = 1;
    Int_t mdcColor = 11; //1002;  // 41, 29
    Int_t mdcLineColor = 15;
    Int_t mdcXYStyle = 1001;//3001;
    Int_t mdcZRStyle = 1001;//3007;
    Int_t mdcStereoSuperColor = 38;
    Int_t mdcAxialSuperColor  = 4;

    Double_t local[3] = {0.0, 0.0, 0.0};
    Double_t master[3] = {0.0, 0.0, 0.0};
    //Int_t    nPoints = 4;
    Double_t  P[306] = {0.0};   //Long Peixun's update: 300 -> 306, at most 102 points
    Double_t  center[3] = {0.0, 0.0, 0.0};
    TString  name;

    //----------XY-----------
    // Mdc
    TGeoTube *mdcShape = (TGeoTube*)GetVolumeMdc()->GetShape();
    //Long Peixun's update: Consider CGEM detector
    Double_t rmin = mdcShape->GetRmin();
    if (m_StartLayer > 0)   //if contains CGEM
    {
        TGeoTube *mdcShapeOuter = (TGeoTube*)(GetLayer(m_StartLayer)->GetVolume())->GetShape();
        rmin = mdcShapeOuter->GetRmin();
    }

    m_MdcXY = new BesCircle2D("Mdc", "Mdc", rmin, mdcShape->GetRmax(), &center[0]);
    m_MdcXY->SetNSegment(360);
    m_MdcXY->SetFillColor(mdcColor);
    m_MdcXY->SetFillStyle(mdcXYStyle);
    m_MdcXY->SetLineColor(mdcLineColor);

    //Long Peixun's update: Annotate
/*     // Mdc Supers
    Int_t layerIn[4]  = {0,
                         m_kStereoLayerIn,
                         m_kStereoLayerIn+m_kAxialLayerIn,
                         m_kStereoLayerIn+m_kAxialLayerIn+m_kStereoLayerOut
                        };
    Int_t layerOut[4] = {m_kStereoLayerIn-1,
                         m_kStereoLayerIn+m_kAxialLayerIn-1,
                         m_kStereoLayerIn+m_kAxialLayerIn+m_kStereoLayerOut-1,
                         m_kLayer-1
                        };
    for (Int_t i = 0; i < 4; i++) {
        TGeoTube *mdcSuperShapeIn  = (TGeoTube*)(GetLayer(layerIn[i])->GetVolume())->GetShape();
        TGeoTube *mdcSuperShapeOut = (TGeoTube*)(GetLayer(layerOut[i])->GetVolume())->GetShape();
        name = TString("MdcSuper" + i);
        m_MdcXYSuper[i] = new BesCircle2D(name, name, mdcSuperShapeIn->GetRmin(), mdcSuperShapeOut->GetRmax(), &center[0]);
        if (i%2 == 0) m_MdcXYSuper[i]->SetFillColor(mdcStereoSuperColor);
        else m_MdcXYSuper[i]->SetFillColor(mdcAxialSuperColor);
    } */

    // Mdc Wires
    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (Int_t layer = m_StartLayer; layer < m_kTrueLayer; layer++) {
        Int_t simuLayer = GetSimuLayer(layer);
        for (int replica = 0; replica < m_kReplica[simuLayer]; replica++) {
            //cout << "layer " << layer << " replica " << replica << endl;
            TGeoPhysicalNode *wirePhyNode = GetPhysicalReplica(simuLayer, replica);
            name = TString("Mdc Layer ");
            name += layer;
            name += " Wire ";
            name += replica;

            if (m_kStereoDir[layer] != 0) { // stereo Layer
                TGeoArb8 *wireShape = (TGeoArb8*)wirePhyNode->GetShape();
                Double_t *localArb8Point, masterArb8Point[24];
                localArb8Point = wireShape->GetVertices();
                for (Int_t i = 0; i < 8; i++) {
                    local[0] = localArb8Point[2*i];
                    local[1] = localArb8Point[2*i+1];
                    if (i < 4) local[2] = wireShape->GetDz() * (-1.0);
                    else local[2] = wireShape->GetDz();

                    wirePhyNode->GetMatrix(-1*wirePhyNode->GetLevel())
                    ->LocalToMaster(local, &master[0]); // transform to top
                    for (Int_t j = 0; j < 3; j++) {
                        masterArb8Point[3*i+j] = master[j];
                    }
                }

                m_Mdc2DWire[layer][replica] = new Mdc2DWire(name, name, 8, &masterArb8Point[0]);
		
            }
            else { // axial Layer

                TGeoTubeSeg *wireShape = (TGeoTubeSeg*)wirePhyNode->GetShape();

                Double_t centerR = 0.5*(wireShape->GetRmin() + wireShape->GetRmax());
                if (layer >= m_kStereoLayerIn+m_kAxialLayerIn+m_kStereoLayerOut)
                    centerR = wireShape->GetRmax();
                Double_t centerPhi =
                    0.5*(wireShape->GetPhi1() + wireShape->GetPhi2()) * TMath::DegToRad();
                local[0] = centerR * cos(centerPhi);
                local[1] = centerR * sin(centerPhi);
                local[2] = 0.0;
                wirePhyNode->GetMatrix(-1*wirePhyNode->GetLevel())
                ->LocalToMaster(local, &master[0]); // transform to top

                Double_t rmin = wireShape->GetRmin();
                Double_t rmax = wireShape->GetRmax();
                if (layer >= m_kStereoLayerIn+m_kAxialLayerIn+m_kStereoLayerOut)
                    rmax = rmin + 2*(rmax-rmin);
                m_Mdc2DWire[layer][replica] =
                    new Mdc2DWire( name, name, rmin, rmax, wireShape->GetDz(),centerPhi, &master[0] );
            }
        }
    }

    //----------ZR-----------
    // Mdc
    Int_t iPoint = 0;
    TGeoTube *aMdcLayer = 0;

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (Int_t layer = m_StartLayer; layer < m_kLayer; layer++) {
        aMdcLayer = (TGeoTube*)GetLayer(layer)->GetVolume()->GetShape();
        P[3*iPoint] = 0.0;
        P[3*iPoint+1] = aMdcLayer->GetRmin();
        P[3*iPoint+2] = aMdcLayer->GetDz();
        iPoint++;
    }

    P[3*iPoint] = 0.0;
    P[3*iPoint+1] = aMdcLayer->GetRmax();
    P[3*iPoint+2] = aMdcLayer->GetDz();
    iPoint++;
    P[3*iPoint] = 0.0;
    P[3*iPoint+1] = aMdcLayer->GetRmax();
    P[3*iPoint+2] = aMdcLayer->GetDz() * (-1.0);
    iPoint++;

    for (Int_t layer = m_kLayer-1; layer >= m_StartLayer; layer--) {
        aMdcLayer = (TGeoTube*)GetLayer(layer)->GetVolume()->GetShape();
        P[3*iPoint] = 0.0;
        P[3*iPoint+1] = aMdcLayer->GetRmin();
        P[3*iPoint+2] = aMdcLayer->GetDz() * (-1.0);
        iPoint++;
    }


    m_MdcZR[0] = new BesPolygon2D("MdcZRUp", "MdcZRUp", iPoint, &P[0]);

    for (Int_t i = 0; i < iPoint; i++)
        P[3*i+1] *= -1.0;

    m_MdcZR[1] = new BesPolygon2D("MdcZRDown", "MdcZRDown", iPoint, &P[0]);

    for (Int_t i = 0; i < 2; i++) {
        m_MdcZR[i]->SetFillColor(mdcColor);
        m_MdcZR[i]->SetFillStyle(mdcZRStyle);
        m_MdcZR[i]->SetLineColor(mdcLineColor);
        m_MdcZR[i]->SetRotatable(true);     //Long Peixun's update
    }
}

void
MdcROOTGeo::SetNode()
{
    if (gDebug) {
        Int_t nDaughters = m_TopVolume->GetNodes()->GetEntries();
        cout << "logicalMdc contains " << nDaughters  << " nodes : " << endl;
        for (Int_t i = 0; i < nDaughters; i++) {
            cout << i << " : " << m_TopVolume->GetNode(i)->GetName() << endl;
        }
    }

    if (m_ROOTGeoInit == 2) { // from GDML
        m_Mdc = GetTopVolume();
        if (!m_Mdc) std::cout << "m_Mdc = 0" << std::endl;

        //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
        for (int layer = m_StartLayer; layer < m_kLayer; layer++) { // is simuLayer
            // m_NodeReplica[layer][replica] = m_Mdc->GetNode(layer); // you could take it by id;
            for (int replica = 0; replica < m_kReplica[layer]; replica++) {
                std::stringstream osname;
                int rep = replica;
                if (layer >= 0 && layer < m_kStereoLayerIn) {
                    //osname << "pv_" << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "TwistedTubs" << "_" << rep;
                    osname << "pv_" << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "Cell" << "_" << rep;
                }
                else if (layer >= m_kStereoLayerIn && layer < m_kStereoLayerIn + m_kAxialLayerIn) {
                    rep = CorrectReplica(layer, rep);
                    //osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << layer << "Replica" << "_" << rep;
                    osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << layer << "Cell" << "_" << rep;
                }
                else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
                         layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) {
                    //osname << "pv_" << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "TwistedTubs" << "_" << rep;
                    osname << "pv_" << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "Cell" << "_" << rep;
                }
                else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut && layer <  m_kLayer) {
                    rep = CorrectReplica(layer, rep);
                    Int_t nBeforeAxialLayerOut = m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut;
                    Int_t iTrueLayer = nBeforeAxialLayerOut + (layer - nBeforeAxialLayerOut)/2;
                    if (layer%2 == 0) {
                        //osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << iTrueLayer << "_0" << "Replica" << "_" << rep;
                        osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << iTrueLayer << "_0" << "Cell" << "_" << rep;
                    }
                    else {
                        //osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << iTrueLayer << "_1" << "Replica" << "_" << rep;
                        osname << "pv_" << "logical" << "Mdc" << "Axial" << "Layer" << iTrueLayer << "_1" << "Cell" << "_" << rep;
                    }
                }

                m_NodeReplica[layer][replica] = GetNode( osname.str() );
                //cout << osname.str() << endl;
            }
        }
    }
    else if (m_ROOTGeoInit == 1) { // from ROOT object
        //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
        for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
            // 0  : logicalMdcStereoLayer0_0      35 : logicalMdcStereoLayer35_35
            // 36 : logicalMdcAxialLayer36_0_36   37 : logicalMdcAxialLayer36_1_37
            // 49 : logicalMdcAxialLayer42_1_49   50 : logicalMdcSegment1_50
            TGeoNode *nodeLayer = m_Mdc->GetNode(layer);
            m_NodeLayer[layer] = nodeLayer;
            //cout << layer << " " << m_NodeLayer[layer]->GetName() << endl;

            Int_t nDaughters = nodeLayer->GetVolume()->GetNodes()->GetEntries();
            // if (0) {
            //     cout << m_NodeLayer[layer] << " contains " << nDaughters  << " nodes : " << endl;
            //     for (Int_t i = 0; i < nDaughters; i++) {
            //         cout << i << " : " << nodeLayer->GetVolume()->GetNode(i)->GetName() << endl;
            //     }
            // }    //Long Peixun's update: Annotate

            for (int replica = 0; replica < m_kReplica[layer]; replica++) {
                m_NodeReplica[layer][replica] = nodeLayer->GetVolume()->GetNode(replica);
                //cout << layer << " " << replica << " " << m_NodeReplica[layer][replica]->GetName() << endl;
            }
        }
    }

    //std::cout << "MdcROOTGeo::SetNode, end of set node" << std::endl;
}

int
MdcROOTGeo::CorrectReplica(int layer, int replica)
{
    int rep = replica;
    for (intMap::iterator iter = m_CorrectMap.begin(); iter != m_CorrectMap.end(); iter++) {
        if (layer == (*iter).first ||
                (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut &&
                 layer - m_kAxialLayerOut == (*iter).first)) {
            rep -= (*iter).second;
            if (rep < 0) rep += m_kReplica[layer];
        }
    }

    //std::cout << "layer" << layer << " " << replica << "->" << rep << std::endl;
    return rep;
}

void
MdcROOTGeo::SetVolumeDefaultVis()
{
    //std::cout << "begin of set defaultvis" << std::endl;
    m_Mdc->SetLineColor(m_MdcColor);
    m_Mdc->SetVisibility(0);

    for (int segment = 1; segment <= m_kSegment; segment++) {
        GetVolumeSegment(segment)->SetLineColor(m_segmentColor);
        //if (segment > 3) GetVolumeSegment(segment)->SetVisibility(0);
        GetVolumeSegment(segment)->SetVisibility(1);
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        if ( (layer >= 0 && layer < m_kStereoLayerIn) ||
                (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
                 layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) ) {
            GetVolumeLayer(layer)->SetLineColor(m_hypeColor);
            GetVolumeReplica(layer)->SetLineColor(m_twistedTubsColor);
        }
        else {
            GetVolumeLayer(layer)->SetLineColor(m_tubeColor);
            GetVolumeReplica(layer)->SetLineColor(m_replicaColor);
        }
        GetVolumeLayer(layer)->SetVisibility(0);
        GetVolumeReplica(layer)->SetVisibility(1);
    }

    for (int segment = 1; segment <= m_kSegment; segment++) {
        for (int no = 0; no < 2; no++) {
            GetSegment(segment, no)->SetVisibility(0);
        }
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        GetLayer(layer)->SetVisibility(0);
        for (int replica = 0; replica < m_kReplica[layer]; replica++) {
            GetReplica(layer, replica)->SetVisibility(0);
        }
    }

    //std::cout << "end of set defaultvis" << std::endl;
}

void
MdcROOTGeo::SetAllVisible()
{
    //std::cout << "begin of set defaultvis" << std::endl;
    m_Mdc->SetLineColor(m_MdcColor);
    m_Mdc->SetVisibility(0);

    for (int segment = 1; segment <= m_kSegment; segment++) {
        GetVolumeSegment(segment)->SetLineColor(m_segmentColor);
        //if (segment > 3) GetVolumeSegment(segment)->SetVisibility(0);
        GetVolumeSegment(segment)->SetVisibility(1);
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        if ( (layer >= 0 && layer < m_kStereoLayerIn) ||
                (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
                 layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) ) {
            GetVolumeLayer(layer)->SetLineColor(m_hypeColor);
            GetVolumeReplica(layer)->SetLineColor(m_twistedTubsColor);
        }
        else {
            GetVolumeLayer(layer)->SetLineColor(m_tubeColor);
            GetVolumeReplica(layer)->SetLineColor(m_replicaColor);
        }
        GetVolumeLayer(layer)->SetVisibility(1);
        GetVolumeReplica(layer)->SetVisibility(1);
    }

    for (int segment = 1; segment <= m_kSegment; segment++) {
        for (int no = 0; no < 2; no++) {
            GetSegment(segment, no)->SetVisibility(1);
        }
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        GetLayer(layer)->SetVisibility(1);
        for (int replica = 0; replica < m_kReplica[layer]; replica++) {
            GetReplica(layer, replica)->SetVisibility(1);
        }
    }



}

void
MdcROOTGeo::SetQuarterVisible()
{
    for (int segment = 1; segment <= m_kSegment; segment++) {
        GetVolumeSegment(segment)->SetVisibility(0);
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        GetVolumeLayer(layer)->SetVisibility(0);
        GetVolumeReplica(layer)->SetVisibility(1);
    }

    for (int segment = 1; segment <= m_kSegment; segment++) {
        for (int no = 0; no < 2; no++) {
            GetSegment(segment, no)->SetVisibility(0);
        }
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        GetLayer(layer)->SetVisibility(0);
        for (int replica = 0; replica < m_kReplica[layer]; replica++) {
            if (replica < m_kReplica[layer]/4) GetReplica(layer, replica)->SetVisibility(0);
            else                               GetReplica(layer, replica)->SetVisibility(1);
        }
    }
}

void
MdcROOTGeo::SetHalfVisible()
{
    for (int segment = 1; segment <= m_kSegment; segment++) {
        GetVolumeSegment(segment)->SetVisibility(0);
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        GetVolumeLayer(layer)->SetVisibility(0);
        GetVolumeReplica(layer)->SetVisibility(1);
    }

    for (int segment = 1; segment <= m_kSegment; segment++) {
        for (int no = 0; no < 2; no++) {
            GetSegment(segment, no)->SetVisibility(0);
        }
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        GetLayer(layer)->SetVisibility(0);
        for (int replica = 0; replica < m_kReplica[layer]; replica++) {
            if (replica < m_kReplica[layer]/4 ||
                    replica > m_kReplica[layer]*3/4 ) GetReplica(layer, replica)->SetVisibility(1);
            else                                  GetReplica(layer, replica)->SetVisibility(0);
        }
    }
}

void
MdcROOTGeo::SetPhysicalNode()
{
    if (gGeoManager == 0) std::cout << "Create gGeoManager first" << std::endl;
    TGeoNode *bes = gGeoManager->GetTopNode();
    TGeoNode *nodeMdc = bes->GetVolume()->GetNode(m_childNo);
    //std::cout << "Mdc m_childNo " << m_childNo << std::endl;

    if (!m_Mdc) std::cout << "m_Mdc = 0" << std::endl;

    for (int segment = 0; segment < 2*m_kSegment-2; segment++) {
        m_PhysicalSegment[segment] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                     TString("/") + nodeMdc->GetName() +
                                     TString("/") + m_Mdc->GetNode((m_kLayer + 2*m_kSegment - 2 - 1) - segment)->GetName() );
        m_PhysicalSegment[segment]->SetVisibility(0);
        m_PhysicalSegment[segment]->SetIsVolAtt(kFALSE);
        m_PhysicalSegment[segment]->SetLineColor(m_segmentColor);
        //cout << m_PhysicalSegment[segment]->GetName() << endl;
    }

    //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
    for (int layer = m_StartLayer; layer < m_kLayer; layer++) {
        TGeoNode *nodeLayer = GetLayer(layer);
        //std::cout << "Layer " << layer << std::endl;
        for (int replica = 0; replica < m_kReplica[layer]; replica++) {
            TGeoNode *nodeReplica = GetReplica(layer, replica);

            m_PhysicalReplica[layer][replica] = gGeoManager->MakePhysicalNode( TString("/") + bes->GetName() +
                                                TString("/") + nodeMdc->GetName() +
                                                TString("/") + nodeLayer->GetName() +
                                                TString("/") + nodeReplica->GetName() );
            m_PhysicalReplica[layer][replica]->SetVisibility(0);
            m_PhysicalReplica[layer][replica]->SetIsVolAtt(kFALSE);
            if ( (layer >= 0 && layer < m_kStereoLayerIn) ||
                    (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
                     layer < m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) ) {
                m_PhysicalReplica[layer][replica]->SetLineColor(m_twistedTubsColor);
            }
            else {
                m_PhysicalReplica[layer][replica]->SetLineColor(m_replicaColor);
            }
            //if (m_PhysicalReplica[layer][replica]->IsVolAttributes()) std::cout << "yes " << std::endl;
        }
    }

    SetDetector();
}

//Long Peixun's update: Remove annotation; Reset color of replicas 
void
MdcROOTGeo::SetPhysicalDefaultVis()
{
    // for (int part = 0; part < m_kPart; part++)
    // {
    //     int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
    //     for (int phi = 0; phi < nPhi; phi++) 
    //     {
    //         int nTheta = (part == 1 ? m_kThetaBr : m_kThetaEc);
    //         for (int theta = 0; theta < nTheta; theta++)
    //         {
    //             m_PhysicalCrystal[part][phi][theta]->SetIsVolAtt(kFALSE);
    //             if (part == 1) m_PhysicalCrystal[part][phi][theta]->SetLineColor(m_brCrystalColor);
    //             else m_PhysicalCrystal[part][phi][theta]->SetLineColor(m_ecCrystalColor);
    //         }
    //     }
    // }

    for (int layer = m_StartLayer; layer < m_kLayer; layer++)
    {
        if ( (layer >= 0 && layer < m_kStereoLayerIn) ||
            (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
            layer < m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) )
        {
            for (int replica = 0; replica < m_kReplica[layer]; replica++)
                GetPhysicalReplica(layer, replica)->SetLineColor(m_twistedTubsColor);
        }
        else
        {
            for (int replica = 0; replica < m_kReplica[layer]; replica++)
                GetPhysicalReplica(layer, replica)->SetLineColor(m_replicaColor);
        }
    }
  //std::cout << "end of set defaultvis" << std::endl;
}


void
MdcROOTGeo::SetDetector()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());
    //if (view) cout << "viewVisFull3DMdc " << view->GetVisFull3DMdc() << endl;

    m_DetectorsArray->Clear();
    for (int segment = 0; segment < 2*m_kSegment-2; segment++) {
        TGeoPhysicalNode *phyNode = 0;
        phyNode = m_PhysicalSegment[segment];
        //cout << m_PhysicalSegment[segment]->GetName() << endl;
        if (phyNode) {
            phyNode->SetVisibility(0); // set all invisible before set any visible
            if ( (segment >= 2 && segment <= 3) || segment > 59) {
                m_DetectorsArray->Add( phyNode );
            }
            else if (view && view->GetVisFull3DMdc()) {
                m_DetectorsArray->Add( phyNode );
            }
        }
    }
    /*
    for (int part = 0; part < m_kPart; part++) {
      int nPhi = (part == 1 ? m_kPhiBr : m_kPhiEc);
      for (int phi = 0; phi < nPhi; phi++) {
        int nTheta = (part == 1 ? m_kThetaBr : m_kThetaEc);
        for (int theta = 0; theta < nTheta; theta++) {
      // This is you detector, set it on
      if ( (part == 1 && theta != 0 && theta != 43 && ((phi != 0 && phi != 60) || theta%2 == 0)) ||
           (part != 1 && theta != 0) ) {
        m_PhysicalCrystal[part][phi][theta]->SetVisibility(0);
      }
      
      // This is you hit
      // when you are to set fired hits to another color, say red,
      // SetIsVolAtt(kFALSE) and to then SetLineColor(kRed);
      if (phi == 12 && theta == 5) {
        m_PhysicalCrystal[part][phi][theta]->SetVisibility(1);
        m_PhysicalCrystal[part][phi][theta]->SetLineColor(2);
      }
        }
      }
    }
    std::cout << "end of set detector on" << std::endl;
    */
}

void
MdcROOTGeo::SetHits()
{
    // set previous event hits to default vis
    //Long Peixun's update: Annotate cout
	// cout << "m_HitsArray size(): "   << m_HitsArray->GetEntries()   << endl;
	// cout << "m_2DHitsArray size(): " << m_2DHitsArray->GetEntries() << endl;
    for (int i = 0; i < m_HitsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_HitsArray->At(i);
        phyNode->SetVisibility(0);
    }
    m_HitsArray->Clear("C");

    // set previous event 2D hits info to default
    for (int i = 0; i < m_2DHitsArray->GetEntries(); i++) {
        Mdc2DWire *aWire = (Mdc2DWire*)m_2DHitsArray->At(i);
        aWire->ClearInfo();
        aWire->AddInfo(aWire->GetTitle());
        aWire->CloseInfo();
    }
    m_2DHitsArray->Clear("C");

    // set new hits
    //Long Peixun's update: Consider gEvent == NULL
    int NDigiCol = 0;
    if (gEvent) 
    {
        m_MdcDigiCol = gEvent->GetMdcDigiCol();
        //Long Peixun's update: Consider m_MdcDigiCol == NULL
        if (m_MdcDigiCol) NDigiCol = m_MdcDigiCol->GetEntries();
        else NDigiCol = 0;
    }

    for (int i = 0; i < NDigiCol; i++) {
        TMdcDigi *aMdcDigi = (TMdcDigi*)m_MdcDigiCol->At(i);
        Identifier aMdcID( aMdcDigi->getIntId() );
        int layer = MdcID::layer( aMdcID );
        int wire  = MdcID::wire( aMdcID );

        Double_t time   = RawDataUtil::MdcTime(aMdcDigi->getTimeChannel());
        Int_t charge = RawDataUtil::MdcCharge(aMdcDigi->getChargeChannel());

        TGeoPhysicalNode *phyNode = 0;
        if (layer < 36) { // number of layers in the three inner
            phyNode = GetPhysicalReplica(layer, wire);
            if (phyNode) m_HitsArray->Add( phyNode );
        }
        else {
            phyNode = GetPhysicalReplica( GetSimuLayer(layer), wire );
            if (phyNode) m_HitsArray->Add( phyNode );
            phyNode = GetPhysicalReplica( GetSimuLayer(layer)+1, wire);
            if (phyNode) m_HitsArray->Add( phyNode );
        }

        Mdc2DWire *aWire = 0;
        aWire = m_Mdc2DWire[layer][wire];
        if (aWire) {
            aWire->ClearInfo();
            aWire->AddInfo(aWire->GetTitle());
	    double evTime = gEvent->GetHeader().GetEvTime();

            char data[200];     //Long Peixun's update: 100 -> 200
	    if(time>1000000) {
		sprintf(data, "NO time");
	    }else{
		sprintf(data, "time=%-.1f ns", time);
		if(fabs(evTime)>0.0001){ 
		    sprintf(data, "%s, Tdrift(Raw)=%-.1f ns", data, time - evTime +230); 
		}
	    }
	    if(charge>1000000) {
		sprintf(data, "%s, NO charge", data);
	    }else{
		sprintf(data, "%s, charge=%d", data,charge);
	    }
            aWire->AddInfo( TString(data) );
            sprintf(data, "Fired");
            aWire->AddInfo( TString(data) );

	    aWire->SetTime(time);//yzhang 
	    aWire->SetCharge(charge);//yzhang
	    unsigned int overflow = aMdcDigi->getOverflow();
	    aWire->SetQOverflow(overflow&2);//yzhang
	    aWire->SetEvTime(evTime);

            //Long Peixun's update: Set raw channel data of aWire
            aWire->SetTimeChannel(aMdcDigi->getTimeChannel());
            aWire->SetChargeChannel(aMdcDigi->getChargeChannel());

            aWire->CloseInfo();

            m_2DHitsArray->Add(aWire);
        }

    }
}

void
MdcROOTGeo::SetVisMdcDetector()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

    for (int i = 0; i < m_DetectorsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_DetectorsArray->At(i);
        if (view && view->GetVisMdcGlobal()) {
            phyNode->SetVisibility(1);
            continue;
        }
        phyNode->SetVisibility(0);
    }
}

void
MdcROOTGeo::SetVisMdcHits()
{
    BesView *view = 0;
    if (gPad) view = dynamic_cast<BesView*>(gPad->GetView());

    for (int i = 0; i < m_HitsArray->GetEntries(); i++) {
        TGeoPhysicalNode *phyNode = (TGeoPhysicalNode*)m_HitsArray->At(i);
        if (view && view->GetVisMdcHitsGlobal()) {
            phyNode->SetVisibility(1);
        }
        else {
            phyNode->SetVisibility(0);
        }
    }
}

void
MdcROOTGeo::SetTFire(Bool_t input){
    k_TFire= input;
    //cout << "Mdc ROOT Geometry TDC Match: " << k_TMatch << endl;
}

//void
//MdcROOTGeo::SetTOverflow(Bool_t input){
//    k_TOverflow = input;
//    //cout << "Mdc ROOT Geometry TDC Match: " << k_TMatch << endl;
//}

void
MdcROOTGeo::SetQNotOverflow(Bool_t input){
    k_QNotOverflow = input;
    //cout << "Mdc ROOT Geometry TDC Match: " << k_TMatch << endl;
}

void
MdcROOTGeo::SetQFire(Bool_t input){
    k_QFire = input;
    //cout << "Mdc ROOT Geometry ADC Match: " << k_QMatch << endl;
}

void
MdcROOTGeo::SetColorfulWire(Bool_t input){
    k_ColorfulWire = input;
}

void
MdcROOTGeo::SetMdcTimeSubEvTime(Bool_t input){
    k_MdcTimeSubEvTime = input;
}

TGeoVolume*
MdcROOTGeo::GetVolumeSegment( int segment )
{
    std::stringstream osname;
    osname << "logical" << "Mdc" << "Segment" << segment;
    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MdcROOTGeo::GetVolumeLayer( int layer )
{
    std::stringstream osname;
    if (layer >= 0 && layer < m_kStereoLayerIn) {
        osname << "logical" << "Mdc" << "Stereo" << "Layer" << layer;
    }
    else if (layer >= m_kStereoLayerIn && layer < m_kStereoLayerIn + m_kAxialLayerIn) {
        osname << "logical" << "Mdc" << "Axial" << "Layer" << layer;
    }
    else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
             layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) {
        osname << "logical" << "Mdc" << "Stereo" << "Layer" << layer;
    }
    else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut &&
             layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut) {
        osname << "logical" << "Mdc" << "Axial" << "Layer" << layer << "_0";
    }
    else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut &&
             layer <  m_kLayer) {
        osname << "logical" << "Mdc" << "Axial" << "Layer" << layer - m_kAxialLayerOut << "_1";
    }

    return GetLogicalVolume( osname.str() );
}

TGeoVolume*
MdcROOTGeo::GetVolumeReplica( int layer )
{
    std::stringstream osname;
    if (layer >= 0 && layer < m_kStereoLayerIn) {
        osname << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "Cell";
    }
    else if (layer >= m_kStereoLayerIn 
	&& layer < m_kStereoLayerIn + m_kAxialLayerIn) {
        osname << "logical" << "Mdc" << "Axial" << "Layer" << layer << "Cell";
    }
    else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
             layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) {
        osname << "logical" << "Mdc" << "Stereo" << "Layer" << layer << "Cell";
    }
    else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut &&
             layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut) {
        osname << "logical" << "Mdc" 
	  << "Axial" << "Layer" << layer << "_0" << "Cell";
    }
    else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut + m_kAxialLayerOut 
	&& layer <  m_kLayer) {
        osname << "logical" << "Mdc" 
	  << "Axial" << "Layer" << layer - m_kAxialLayerOut 
	  << "_1" << "Cell";
    }

    return GetLogicalVolume( osname.str() );
}

TGeoNode*
MdcROOTGeo::GetSegment( int segment, int no )
{
    std::stringstream osname;
    if (segment == 1 || segment == 2) {
        osname << "pv_" << "logical" << "Mdc" 
	  << "Segment" << segment << "_" 
      << m_kSegmentNodeOffset - (segment - 1);  //huangsh
	  //<< (m_kLayer + (2*m_kSegment -2) -1) - (segment - 1);
    }
    else {
        osname << "pv_" << "logical" << "Mdc" 
	  << "Segment" << segment << "_" 
      << m_kSegmentNodeOffset - m_kSegmentBr - 2*(segment - 1 - m_kSegmentBr) - no; //huangsh
	  //<< (m_kLayer + (2*m_kSegment -2) -1) - 2 - (segment - 3)*2 - no;
    }

    return GetNode( osname.str() );
}

TGeoNode*
MdcROOTGeo::GetLayer( int layer )
{
    // if m_NodeLayer[] set in SetNode(), could also return m_NodeLayer[layer]
    if (m_ROOTGeoInit == 2) { // from GDML
        std::stringstream osname;
        if (layer >= 0 && layer < m_kStereoLayerIn) {
            osname << "pv_" << "logical" << "Mdc" 
	      << "Stereo" << "Layer" << layer << "_" << layer;
        }
        else if (layer >= m_kStereoLayerIn 
	    && layer < m_kStereoLayerIn + m_kAxialLayerIn) {
            osname << "pv_" << "logical" << "Mdc" 
	      << "Axial" << "Layer" << layer << "_" << layer;
        }
        else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn &&
                 layer <  m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut) {
            osname << "pv_" << "logical" << "Mdc" 
	      << "Stereo" << "Layer" << layer << "_" << layer;
        }
        else if (layer >= m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut 
	    && layer <  m_kLayer) {
            Int_t nBeforeAxialLayerOut = 
	      m_kStereoLayerIn + m_kAxialLayerIn + m_kStereoLayerOut;
            Int_t iTrueLayer = 
	      nBeforeAxialLayerOut + (layer - nBeforeAxialLayerOut)/2;
            if (layer%2 == 0) {
                osname << "pv_" << "logical" << "Mdc" 
		  << "Axial" << "Layer" << iTrueLayer << "_0" << "_" << layer;
            }
            else {
                osname << "pv_" << "logical" << "Mdc" 
		  << "Axial" << "Layer" << iTrueLayer << "_1" << "_" << layer;
            }
        }

        //std::cout << osname.str() << std::endl;
        return GetNode( osname.str() );
    }
    else if (m_ROOTGeoInit == 1) { // from ROOT object
        if (m_NodeLayer[layer] != 0) {
            return m_NodeLayer[layer];
        }
        else {
            //std::cout << "Node: " << "Layer" << layer << " not found" << std::endl;
            return 0;
        }
    }

    return 0;
}

TGeoNode*
MdcROOTGeo::GetReplica( int layer, int replica )
{
    if (m_NodeReplica[layer][replica] != 0) {
        //std::cout << " replica " << layer <<  "  " << replica << " found " << std::endl;
        return m_NodeReplica[layer][replica];
    }
    else {
        //std::cout << "Node: " << "Layer" << layer << "Replica" << replica << " not found" << std::endl;
        return 0;
    }
}

TGeoPhysicalNode*
MdcROOTGeo::GetPhysicalSegment( int segment )
{
    if (m_PhysicalSegment[segment] != 0) {
        return m_PhysicalSegment[segment];
    }
    else {
        //std::cout << "PhysicalNode: " << "Segment" << segment << " not found" << std::endl;
        return 0;
    }
}

TGeoPhysicalNode*
MdcROOTGeo::GetPhysicalReplica( Int_t layer, Int_t replica )
{
    if (m_PhysicalReplica[layer][replica] != 0) {
        return m_PhysicalReplica[layer][replica];
    }
    else {
        //std::cout << "PhysicalNode: " << "Layer" << layer << "Replica" << replica << " not found" << std::endl;
        return 0;
    }
}

Mdc2DWire*
MdcROOTGeo::Get2DWire( Int_t layer, Int_t wire)
{
    if (m_Mdc2DWire[layer][wire]) return m_Mdc2DWire[layer][wire];
    else return 0;
}

Int_t
MdcROOTGeo::GetSimuLayer(Int_t trueLayer)
{
    if (trueLayer < 36 || trueLayer >= m_kTrueLayer) return trueLayer;
    if (trueLayer >= 36) return (36 + 2*(trueLayer-36));
    return trueLayer;
}

Bool_t
MdcROOTGeo::IsHit(Int_t layer, Int_t wire)
{
    if (m_MdcDigiCol) {
        for (int i = 0; i < m_MdcDigiCol->GetEntries(); i++) {
            Identifier mdcID( ((TMdcDigi*)m_MdcDigiCol->At(i))->getIntId() );
            if (layer == MdcID::layer( mdcID ) && wire == MdcID::wire( mdcID )) {
                return kTRUE;
            }
        }
    }

    return kFALSE;
}

void
MdcROOTGeo::Draw(Option_t *option)
{
    TString opt = option;
    opt.ToUpper();

    if (!m_2DGeoInit) cout << "MdcROOTGeo::Draw2D(), 2D Geometry not initialized!" << endl;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) cout << "MdcROOTGeo::Draw(), BesView not found" << endl;

    if (view->GetVisMdcGlobal()) { // MdcVisGlobal
        if (opt.Contains("XY")) {
            m_MdcXY->Draw("");
            //Long Peixun's update: Annotate
            // for (Int_t i = 0; i < 4; i++) {
            //     m_MdcXYSuper[i]->Draw("");
            // }
        }

        if (opt.Contains("ZR")) {
            for (Int_t i = 0; i < 2; i++) {
                //m_MdcZR[i]->SetRotatable(true);   //Long Peixun's update: Move to initializer
                m_MdcZR[i]->Draw("");
            }
        }

        TString wireOpt;
        if (view->GetVisMdcTubes()) wireOpt += "TUBE";
        if (view->GetVisMdcWires()) wireOpt += ",WIRE";

        Int_t replicaDraw = 0; // change draw sequence in a circle, (-1: 0->Max; 1: Max->0)
        //Long Peixun's update: Ignore layer 0~7 if detector contains CGEM
        for (Int_t layer = m_StartLayer; layer < m_kTrueLayer; layer++) {
            Int_t simuLayer = GetSimuLayer(layer);
            for (int replica = 0; replica < m_kReplica[simuLayer]; replica++) {
                if (m_kStereoDir[layer] == -1) replicaDraw = replica;
                else if (m_kStereoDir[layer] == 1) replicaDraw = m_kReplica[simuLayer]-1 - replica;
                else replicaDraw = replica;

                if (m_Mdc2DWire[layer][replicaDraw]) {
                    //cout << layer << " " << replicaDraw << endl;
                    //m_Mdc2DWire[layer][replicaDraw]->SetHighlighted(false);
                    m_Mdc2DWire[layer][replicaDraw]->SetFired(false);
                    m_Mdc2DWire[layer][replicaDraw]->Draw(wireOpt);
                }
            }
        }
    }
}

void
MdcROOTGeo::DrawHits(Option_t *option)
{
    // cout << "Draw Hits" << endl;
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    if (!view) cout << "MdcROOTGeo::DrawHits(), BesView not found" << endl;

    //cout << "VisMdcHitsGlobal " << view->GetVisMdcHitsGlobal() << endl;
    //cout << "VisMdcHits       " << view->GetVisMdcHits() << endl;


    if (view->GetVisMdcHitsGlobal()) {
        TString wireOpt("");
        if (view->GetVisMdcTubes()) wireOpt += ",TUBE";
        if (view->GetVisMdcHits())  wireOpt += ",WIRE";

        //Long Peixun's update: Use m_2DHitsArray which contains ready hits rather than rebuild hits
        for (int i = 0; i < m_2DHitsArray->GetEntries(); ++i)
        {
            Mdc2DWire* aWire = (Mdc2DWire*)m_2DHitsArray->At(i);
            if (aWire)
            {
                if ((k_TFire) && (aWire->GetTimeChannel() == 0x7FFFFFFF))
                    continue;
                if ((k_QFire) && (aWire->GetChargeChannel() == 0x7FFFFFFF))
                    continue;
                if ((k_QNotOverflow) && aWire->GetQOverflow() )
                    continue;
                aWire->SetFired(true);
                aWire->SetColorfulWire(k_ColorfulWire, k_MdcTimeSubEvTime);
                aWire->Draw(wireOpt);
            }
        }

/*         if (m_MdcDigiCol) {
            for (int i = 0; i < m_MdcDigiCol->GetEntries(); i++) {
                TMdcDigi *aMdcDigi = (TMdcDigi*)m_MdcDigiCol->At(i);

                Identifier aMdcID( aMdcDigi->getIntId() );
                int layer  = MdcID::layer( aMdcID );
                int wire   = MdcID::wire( aMdcID );
                unsigned int overflow = aMdcDigi->getOverflow();

                Mdc2DWire *aWire = 0;
                aWire = m_Mdc2DWire[layer][wire];
                if (aWire){
                    if ((k_TFire) && (aMdcDigi->getTimeChannel() == 0x7FFFFFFF)){
                        continue;
                    }
                    if ((k_QFire) && (aMdcDigi->getChargeChannel() == 0x7FFFFFFF)){
                        continue;
                    }
                    //if ((!k_TOverflow) && ((overflow&1)>0) ){
                    //    continue;
		            //}
                    if ((k_QNotOverflow) && ((overflow&2)>0) ){
                        continue;
		            }
                    aWire->SetFired(true);
		            aWire->SetColorfulWire(k_ColorfulWire,k_MdcTimeSubEvTime);
                    aWire->Draw(wireOpt);
                }
            }
        } */
    }
}
