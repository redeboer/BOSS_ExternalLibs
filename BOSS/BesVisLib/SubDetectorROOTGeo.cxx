//$id$
/*
 *    2004/12/9    Zhengyun You      Peking University
 *                 ROOT Geometry for Bes sub-detector in event display and reconstruction.
 *
 */

using namespace std;

#include <string>
#include <iostream>

#include "TGeoVolume.h"
#include "TGeoNode.h"

#include "BesVisLib/SubDetectorROOTGeo.h"

SubDetectorROOTGeo::SubDetectorROOTGeo()
        : m_ROOTGeoInit(0),
        m_2DGeoInit(0)
{
    // Default constructor.
    m_DetectorsArray = new TObjArray();
    m_HitsArray      = new TObjArray();
    m_2DHitsArray    = new TObjArray();
}

SubDetectorROOTGeo::~SubDetectorROOTGeo()
{
    m_sxp.Finalize();
    //Long Peixun's update: Add delete arrays code
    delete m_DetectorsArray;
    delete m_HitsArray;
    delete m_2DHitsArray;
}

void
SubDetectorROOTGeo::ReadGdml( const char *gdmlFile, const char *setupName )
{
    m_config.SetURI( gdmlFile );
    m_config.SetSetupName( setupName );
    m_config.SetType ( "ROOT" );

    m_sxp.Configure( &m_config );
    m_sxp.Initialize();
    m_sxp.Run();

    m_TopVolume = (TGeoVolume*)TGDMLProcessor::GetInstance()->GetWorldVolume();
}

TGeoVolume* SubDetectorROOTGeo::GetLogicalVolume( const std::string& vn )
{
    TGeoVolume *lv = (TGeoVolume*)TGDMLProcessor::GetInstance()->GetLogicalVolume(vn);
    return lv;
}

TGeoVolumeAssembly* SubDetectorROOTGeo::GetAssemblyVolume( const std::string& an )
{
    TGeoVolumeAssembly *av = (TGeoVolumeAssembly*)TGDMLProcessor::GetInstance()->GetAssemblyVolume(an);
    return av;
}

TGeoNode* SubDetectorROOTGeo::GetNode( const std::string& nn )
{
    TGeoNode *node = (TGeoNode*)TGDMLProcessor::GetInstance()->GetPhysicalVolume(nn);
    return node;
}

TGeoPhysicalNode* SubDetectorROOTGeo::GetHit(int i)
{
    if (i < m_HitsArray->GetEntries())
        return (TGeoPhysicalNode*)m_HitsArray->At(i);
    else
        return 0;
}

void SubDetectorROOTGeo::SetDetectorOn()
{
    for (Int_t i = 0; i < m_DetectorsArray->GetEntries(); i++) {
        TGeoPhysicalNode *pNode = (TGeoPhysicalNode*)m_DetectorsArray->At(i);
        if (pNode) pNode->SetVisibility(1);
    }
}
