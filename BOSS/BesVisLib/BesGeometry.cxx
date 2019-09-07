
#include <iostream>
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>
#include <TGeoTube.h>
#include <TGeoArb8.h>
#include <TGeoMatrix.h>
#include <TCanvas.h>

#include "BesVisLib/BesGeometry.h"
#include "BesVisLib/BesCircle2D.h"
#include "BesVisLib/BesPolygon2D.h"
#include "BesVisLib/Mdc2DWire.h"

BesGeometry *gBesGeometry = 0;

using namespace std;

#ifndef __CINT__
ClassImp(BesGeometry)
#endif

//_____________________________________________________________________________

BesGeometry::BesGeometry() : TObject(),
        m_Bes(0),
        m_BeamPipe(0),
        m_Mdc(0),
        m_Tof(0),
        m_Emc(0),
        m_Muc(0),
        m_MdcROOTGeo(0),
        m_TofROOTGeo(0),
        m_EmcROOTGeo(0),
        m_MucROOTGeo(0)
{
    // Create Bes geometry
    if (!gGeoManager) new TGeoManager("BesGeo", "BesGeometry");
    else gGeoManager-> ClearPhysicalNodes();

    m_BesR = 2840.0;
    m_BesZ = 5680.0;

    m_BeamPipeRMin = 31.5;
    m_BeamPipeRMax = 35.8;
    m_BeamPipeZ    = 150.0;
    m_BeamPipeColor = 922;      //Long Peixun's update: Set Beampipe's color
}


BesGeometry::~BesGeometry()
{
    delete m_Bes;
    delete m_BeamPipe;

    delete m_MdcROOTGeo;
    delete m_TofROOTGeo;
    delete m_EmcROOTGeo;
    delete m_MucROOTGeo;
}

void BesGeometry::InitFromGDML(const TString fPath, bool mrpc, bool cgem)
{
    gGeoManager->SetVisOption(0);  // to show all visible volumes.
    gGeoManager->SetVisLevel(5);

    TGeoIdentity *identity = new TGeoIdentity();

    TGeoMaterial *mat = new TGeoMaterial("VOID",0,0,0);
    mat->SetTransparency(20);
    TGeoMedium   *med = new TGeoMedium("MED",1,mat);
    m_Bes = gGeoManager->MakeBox("volBes", med, m_BesR, m_BesR, 0.5*m_BesZ);

    TGeoVolume *topVolume = new TGeoVolume(); // temp top volume for load geometry more than once
    gGeoManager->SetTopVolume(topVolume);
    gGeoManager->SetTopVolume(m_Bes);

    Int_t iChildNo = 0;
    m_MdcROOTGeo = new MdcROOTGeo(false);
    m_MdcROOTGeo->InitFromGDML( (fPath + TString("Mdc.gdml")).Data(), "Default" );  //Long Peixun's update: "Mdc"->"Default"
    cout<<"in BesGeometry construct mdc1"<<endl;
    m_Mdc = m_MdcROOTGeo->GetVolumeMdc();
    if (m_Mdc)  cout << "Construct Mdc" << endl;
    else       cout << "Volume Mdc not found " << endl;
    m_Bes->AddNode(m_Mdc, iChildNo, identity);
    m_MdcROOTGeo->SetChildNo(iChildNo);//Connect bes with mdc, form physcialnode path.  m_Bes->GetNdaughters()-1

    iChildNo++;
    m_TofROOTGeo = new TofROOTGeo();

    //Long Peixun's update: Judge MRPC flag and load relevant Tof GDML
    if (mrpc)
        m_TofROOTGeo->InitFromGDML( (fPath + TString("Tof_mrpc.gdml")).Data(), "Default" );  //Huang Shuhui's update: for Tof_mrpc
    else
        m_TofROOTGeo->InitFromGDML( (fPath + TString("Tof.gdml")).Data(), "Default" );  //Long Peixun's update: "Tof"->"Default"

    cout<<"construct tof"<<endl;
    m_Tof = m_TofROOTGeo->GetVolumeTof();
    if (m_Tof)  cout << "Construct Tof" << endl;
    else       cout << "Volume Tof not found " << endl;
    m_Bes->AddNode(m_Tof, iChildNo, identity);
    m_TofROOTGeo->SetChildNo(iChildNo);

    iChildNo++;
    m_EmcROOTGeo = new EmcROOTGeo();
    m_EmcROOTGeo->InitFromGDML( (fPath + TString("Emc.gdml")).Data(), "Default" );  //Long Peixun's update: "Emc"->"Default"
    m_Emc = m_EmcROOTGeo->GetVolumeEmc();
    if (m_Emc)  cout << "Construct Emc" << endl;
    else       cout << "Volume Emc not found " << endl;
    m_Bes->AddNode(m_Emc, iChildNo, identity);
    m_EmcROOTGeo->SetChildNo(iChildNo);

    iChildNo++;
    m_MucROOTGeo = new MucROOTGeo();
    m_MucROOTGeo->InitFromGDML( (fPath + TString("Muc.gdml")).Data(), "Default" );  //Long Peixun's update: "Muc"->"Default"
    m_Muc = m_MucROOTGeo->GetVolumeMuc();
    if (m_Muc)  cout << "Construct Muc" << endl;
    else       cout << "Volume Muc not found " << endl;
    m_Bes->AddNode(m_Muc, iChildNo, identity);
    m_MucROOTGeo->SetChildNo(iChildNo);

    iChildNo++;
    Int_t beamPipeColor = m_BeamPipeColor;  //Long Peixun's update: Blue -> m_BeamPipeColor
    m_BeamPipe = gGeoManager->MakeTube("volBeamPipe", med, m_BeamPipeRMin, m_BeamPipeRMax, m_BeamPipeZ);
    m_BeamPipe->SetLineColor(beamPipeColor);
    m_Bes->AddNode(m_BeamPipe, iChildNo, identity);

    Int_t nDaughters = m_Bes->GetNodes()->GetEntries();
    cout << "volBes contains " << nDaughters  << " nodes : " << endl;
    for (Int_t i = 0; i < nDaughters; i++) {
        cout << i << " : " << m_Bes->GetNode(i)->GetName() << endl;
    }

    InitGeometry();
}

void BesGeometry::InitFromROOT(TGeoVolume *volBes)
{
    gGeoManager->SetVisOption(0);  // to show all visible volumes.
    gGeoManager->SetVisLevel(5);

    m_Bes = volBes;
    m_BesR = ((TGeoBBox*)m_Bes->GetShape())->GetDX();
    m_BesZ = ((TGeoBBox*)m_Bes->GetShape())->GetDZ()*2.0;

    TGeoVolume *topVolume = new TGeoVolume();
    gGeoManager->SetTopVolume(topVolume);
    gGeoManager->SetTopVolume(m_Bes);

    Int_t nDaughters = m_Bes->GetNodes()->GetEntries();

    //cout << "volBes contains " << nDaughters  << " nodes : " << endl;
    //for (Int_t i = 0; i < nDaughters; i++) {
    //    cout << i << " : " << m_Bes->GetNode(i)->GetName() << endl;
    //}

    Int_t iChildNo = 0;
    m_MdcROOTGeo = new MdcROOTGeo(false);
    TGeoNode *nodeMdc = m_Bes->GetNode(iChildNo);
    if (nodeMdc) m_Mdc = nodeMdc->GetVolume();
    //else cout << "Node logicalMdc not found" << endl;
    //if (m_Mdc)  cout << "Set Mdc" << endl;
    //else       cout << "Volume Mdc not found " << endl;
    m_MdcROOTGeo->InitFromROOT( m_Mdc );
    m_MdcROOTGeo->SetChildNo(iChildNo);//Connect bes with mdc, form physcialnode path.  m_Bes->GetNdaughters()-1

    iChildNo++;
    m_TofROOTGeo = new TofROOTGeo();
    TGeoNode *nodeTof = m_Bes->GetNode(iChildNo);
    if (nodeTof) m_Tof = nodeTof->GetVolume();
    //else cout << "Node logicalTof not found" << endl;
    //if (m_Tof)  cout << "Set Tof" << endl;
    //else       cout << "Volume Tof not found " << endl;
    m_TofROOTGeo->InitFromROOT( m_Tof );
    m_TofROOTGeo->SetChildNo(iChildNo);

    iChildNo++;
    m_EmcROOTGeo = new EmcROOTGeo();
    TGeoNode *nodeEmc = m_Bes->GetNode(iChildNo);
    if (nodeEmc) m_Emc = nodeEmc->GetVolume();
    //else cout << "Node logicalEmc not found" << endl;
    //if (m_Emc)  cout << "Set Emc" << endl;
    //else       cout << "Volume Emc not found " << endl;
    m_EmcROOTGeo->InitFromROOT( m_Emc );
    m_EmcROOTGeo->SetChildNo(iChildNo);

    iChildNo++;
    m_MucROOTGeo = new MucROOTGeo();
    TGeoNode *nodeMuc = m_Bes->GetNode(iChildNo);
    if (nodeMuc) m_Muc = nodeMuc->GetVolume();
    //else cout << "Node logicalMuc not found" << endl;
    //if (m_Muc)  cout << "Set Muc" << endl;
    //else       cout << "Volume Muc not found " << endl;
    m_MucROOTGeo->InitFromROOT( m_Muc );
    m_MucROOTGeo->SetChildNo(iChildNo);

    iChildNo++;
    TGeoNode *nodeBeamPipe = m_Bes->GetNode(iChildNo);
    if (nodeBeamPipe) {
        m_BeamPipe = nodeBeamPipe->GetVolume();
        m_BeamPipe->SetLineColor(m_BeamPipeColor);  //Long Peixun's update: Blue -> m_BeamPipeColor
    }
    //else cout << "Node volBeamPipe not found " << endl;
    //cout << "Set beanmpi complete" << endl;

    InitGeometry();
    //cout << "init root ok" << endl;
}

void BesGeometry::InitGeometry()
{
    gGeoManager->SetDrawExtraPaths();  // draw physical node or not;
    gGeoManager->CloseGeometry();
    gGeoManager->SetNsegments(20);  // devide a circle to 20 slides

    // PhysicalNode necessary for fast setting node visible or not in an event
    // Should be set after gGeoManager closed
    m_MdcROOTGeo->SetPhysicalNode();    //here liang
    m_TofROOTGeo->SetPhysicalNode();    //here liang
    m_EmcROOTGeo->SetPhysicalNode();    //here liang
    m_MucROOTGeo->SetPhysicalNode();

    //Long Peixun's update: Build BeamPipe's PhysicalNode to display 3D view
    m_phyBeamPipe = gGeoManager->MakePhysicalNode(TString("/") + m_Bes->GetName() + TString("_1/") + m_BeamPipe->GetName() + TString("_4"));
    m_phyBeamPipe->SetVisibility(0);
    m_phyBeamPipe->SetIsVolAtt(kFALSE);
    m_phyBeamPipe->SetLineColor(m_BeamPipeColor);  //Long Peixun's update: Blue -> m_BeamPipeColor

    // 2D Geometry initialization should be after SetPhysicalNode();
    //Long Peixun's update: Change initialization order of subdetectors can avoid a strange bug in Mdc2DWire::DistancetoPrimitive()
    //Mdc, Tof, Emu, Muc -> Tof, Mdc, Emc, Muc
    m_TofROOTGeo->Init2DGeometry();    //here liang
    m_MdcROOTGeo->Init2DGeometry();    //here liang
    m_EmcROOTGeo->Init2DGeometry();    //here liang
    m_MucROOTGeo->Init2DGeometry();

    Double_t  P[3*4] = {0.0};

    // BeamPipe
    Double_t beamPipeCenter[3] = {0.0, 0.0, 0.0};
    m_BeamPipeXY =
      new BesCircle2D("BeamPipe", "BeamPipe", m_BeamPipeRMin, m_BeamPipeRMax, &beamPipeCenter[0]);

    for (Int_t i = 0; i < 4; i++) {
        P[3*i]   = 0.0;
        if (i == 0 || i == 3) P[3*i+1] = -1.0*m_BeamPipeRMax;
        else P[3*i+1] = m_BeamPipeRMax;
        if (i == 0 || i == 1) P[3*i+2] = -1.0*m_BeamPipeZ;
        else P[3*i+2] = m_BeamPipeZ;
    }
    m_BeamPipeZR = new BesPolygon2D("BeamPipe", "BeamPipe", 4, &P[0]);
    m_BeamPipeZR->SetRotatable(true);   //Long Peixun's update: BeamPipe rectangle should not rotate when phi is changed

    // Auxiliary line
    if (!m_Bes) cout << "BesGeometry:InitGeometry, top volume m_Bes not found" << endl;
    TGeoBBox *besShape = (TGeoBBox*)m_Bes->GetShape();
    m_BesR = besShape->GetDX();
    m_BesZ = besShape->GetDZ()*2.0;
    for (Int_t i = 0; i < 4; i++) {
        P[3*i]   = 0.0;
        if (i == 0 || i == 3) P[3*i+1] = -1.0*m_BesR;
        else P[3*i+1] = m_BesR;
        if (i == 0 || i == 1) P[3*i+2] = -1.0*m_BesZ;
        else P[3*i+2] = m_BesZ;
    }

    m_ZRPlaneOnXY = new BesPolygon2D("ZRPlaneOnXY", "ZRPlaneOnXY", 4, &P[0]);
    m_ZRPlaneOnXY->SetRotatable(true);
    m_ZRPlaneOnXY->SetLineWidth(1);
    m_ZRPlaneOnXY->SetLineStyle(3);
    m_ZRPlaneOnXY->SetFillStyle(4000);
}

const char* BesGeometry::GetFilename(const char* RunPeriod)
{
    return RunPeriod;
}

void BesGeometry::SetDefaultVis()
{
    m_Bes->SetLineColor(2);
    m_Bes->SetVisibility(0);

    m_BeamPipe->SetLineColor(m_BeamPipeColor);  //Long Peixun's update: Blue -> m_BeamPipeColor
    m_BeamPipe->SetVisibility(0);

    //Long Peixun's update: Annotate useless code
    // m_MdcROOTGeo->SetAllVisible();
    // m_TofROOTGeo->SetAllVisible();
    // m_EmcROOTGeo->SetAllVisible();
    // m_MucROOTGeo->SetAllVisible();
}

//Long Peixun's update: Reset color
void BesGeometry::SetPhysicalDefaultVis()
{
    m_MdcROOTGeo->SetPhysicalDefaultVis();
    m_TofROOTGeo->SetPhysicalDefaultVis();
    m_EmcROOTGeo->SetPhysicalDefaultVis();
    m_MucROOTGeo->SetPhysicalDefaultVis();
}

void BesGeometry::Draw(Option_t *option)
{
    //
    // BesGeometry draw function
    TString opt = option;
    opt.ToUpper();

    if ( opt.Contains("3D") ) {
        Draw3D();
    }
    else if ( opt.Contains("XY") ) {
        Draw2DXY();
    }
    else if ( opt.Contains("ZR") ) {
        Draw2DZR();
    }
}

void BesGeometry::Draw3D()
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    if (view && view->GetViewType() == k3DView)
    {
        //Long Peixun's update: Draw Detector 3D
        m_BeamPipe->AppendPad();

        //Long Peixun's update: Set BeamPipe 3D visibility
        if (view->GetVisBeamPipe()) m_phyBeamPipe->SetVisibility(1);
        else m_phyBeamPipe->SetVisibility(0);

        if (m_MdcROOTGeo) {
            m_MdcROOTGeo->SetVisMdcDetector();
            m_MdcROOTGeo->SetPhysicalDefaultVis();  //Long Peixun's update: Reset Color
            m_MdcROOTGeo->SetVisMdcHits();
        }

        if (m_TofROOTGeo) {
            m_TofROOTGeo->SetVisTofDetector();
            m_TofROOTGeo->SetPhysicalDefaultVis();  //Long Peixun's update: Reset Color
            m_TofROOTGeo->SetVisTofHits();
        }

        if (m_EmcROOTGeo) {
            m_EmcROOTGeo->SetVisEmcDetector();
            m_EmcROOTGeo->SetPhysicalDefaultVis();  //Long Peixun's update: Reset Color
            m_EmcROOTGeo->SetVisEmcHits();
        }

        if (m_MucROOTGeo) {
            m_MucROOTGeo->SetVisMucDetector();
            m_MucROOTGeo->SetPhysicalDefaultVis();  //Long Peixun's update: Reset Color
            m_MucROOTGeo->SetVisMucHits();
        }
    }

    //if (gGeoManager) {
    //    cout << "gGeoManager exit" << endl;
    //}
    //else {
    //    cout << "gGeoManager do not exit" << endl;
    //}

    //if (m_Bes){
    //    cout << "m_Bes address:" << m_Bes << endl;
    //}


    //if (m_Bes) m_Bes->Draw();
    //if (view && view->GetVisAxis()) view->ShowAxis();
}

void BesGeometry::Draw2DXY()
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    //Long Peixun's update: Judge view before draw
    if (view)
    {
        if (view->GetVisBeamPipe())    m_BeamPipeXY->Draw();
        if (m_MdcROOTGeo) m_MdcROOTGeo->Draw("XY");
        if (m_TofROOTGeo) m_TofROOTGeo->Draw("XY");
        if (m_EmcROOTGeo) m_EmcROOTGeo->Draw("XY");
        if (m_MucROOTGeo) m_MucROOTGeo->Draw("XY");
        if (view->GetVisZRPlaneOnXY()) m_ZRPlaneOnXY->Draw();
        if (view->GetVisAxis())        view->ShowAxis();
    }
}

void BesGeometry::Draw2DZR()
{
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());
    //Long Peixun's update: Judge view before draw
    if (view)
    {
        if (view->GetVisBeamPipe()) m_BeamPipeZR->Draw();
        if (m_MdcROOTGeo) m_MdcROOTGeo->Draw("ZR");
        if (m_TofROOTGeo) m_TofROOTGeo->Draw("ZR");
        if (m_EmcROOTGeo) m_EmcROOTGeo->Draw("ZR");
        if (m_MucROOTGeo) m_MucROOTGeo->Draw("ZR");
        if (view->GetVisAxis())     view->ShowAxis();
    }
}
