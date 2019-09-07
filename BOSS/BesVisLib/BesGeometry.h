//
// BesGeometry.h
//

#ifndef BES_GEOMETRY
#define BES_GEOMETRY

#include <TGeometry.h>

#include "BesView.h"
#include "MdcROOTGeo.h"
#include "TofROOTGeo.h"
#include "EmcROOTGeo.h"
#include "MucROOTGeo.h"
#include "BesCircle2D.h"
#include "BesPolygon2D.h"

class BesGeometry : public TObject {

 public:
  BesGeometry();
  virtual ~BesGeometry();
  
  //Long Peixun's update: Add parameters for MRPC and CGEM
  virtual void        InitFromGDML(const TString fPath, bool mrpc = false, bool cgem = false);
  virtual void        InitFromROOT(TGeoVolume* volBes);
  virtual void        InitGeometry();

  const   char*       GetFilename(const char* RunPeriod);
  virtual void        SetDefaultVis();
  virtual void        SetPhysicalDefaultVis();    //Long Peixun's update: Reset 3D color
  virtual void        Draw(Option_t *option = "3D");
  virtual void        Draw3D();
  virtual void        Draw2DXY();
  virtual void        Draw2DZR();

  Float_t             GetBesR() { return m_BesR; }
  Float_t             GetBesZ() { return m_BesZ; }

  TGeoVolume         *GetVolBes()     { return m_Bes; }
  TGeoVolume         *GetVolumeBeamPipe() { return m_BeamPipe; } 

  MdcROOTGeo         *GetMdcROOTGeo() { return m_MdcROOTGeo; }
  TofROOTGeo         *GetTofROOTGeo() { return m_TofROOTGeo; }
  EmcROOTGeo         *GetEmcROOTGeo() { return m_EmcROOTGeo; }
  MucROOTGeo         *GetMucROOTGeo() { return m_MucROOTGeo; }

  TGeoVolume         *GetVolumeMdc()  { return m_MdcROOTGeo->GetVolumeMdc(); }
  TGeoVolume         *GetVolumeMdcSegment(int segment) { return m_MdcROOTGeo->GetVolumeSegment(segment); }
  TGeoVolume         *GetVolumeMdcLayer(int layer)     { return m_MdcROOTGeo->GetVolumeLayer(layer); }
  TGeoVolume         *GetVolumeMdcReplica(int layer)   { return m_MdcROOTGeo->GetVolumeReplica(layer); }
  TGeoPhysicalNode   *GetPhysicalMdcReplica(int layer, int replica) { return m_MdcROOTGeo->GetPhysicalReplica(layer, replica); }
  
  TGeoVolume         *GetVolumeTof() { return m_TofROOTGeo->GetVolumeTof(); }
  //TGeoVolumeAssembly *GetVolumeTofAssembly(int part) { return m_TofROOTGeo->GetVolumeAssembly(part); }
  //TGeoVolume         *GetVolumeTofScin(int part)     { return m_TofROOTGeo->GetVolumeScin(part); }
  //TGeoVolume         *GetVolumeTofBucket(int part)   { return m_TofROOTGeo->GetVolumeBucket(part); }
  TGeoPhysicalNode   *GetPhysicalTofScin(int part, int layer, int scin) { return m_TofROOTGeo->GetPhysicalScin(part, layer, scin); }

  TGeoVolume         *GetVolumeEmc() { return m_EmcROOTGeo->GetVolumeEmc(); }
  TGeoVolume         *GetVolumeEmcPart(int part) { return m_EmcROOTGeo->GetVolumePart(part); }
  TGeoVolume         *GetVolumeEmcPhi(int part, int phi) { return m_EmcROOTGeo->GetVolumePhi(part, phi); }
  TGeoVolume         *GetVolumeEmcTheta(int part, int phi, int theta)     { return m_EmcROOTGeo->GetVolumeTheta(part, phi, theta); }
  TGeoVolume         *GetVolumeEmcCrystal(int part, int phi, int theta)   { return m_EmcROOTGeo->GetVolumeCrystal(part, phi, theta); }
  TGeoPhysicalNode   *GetPhysicalEmcCrystal(int part, int phi, int theta) { return m_EmcROOTGeo->GetPhysicalCrystal(part, phi, theta); }

  TGeoVolume         *GetVolumeMuc()  { return m_MucROOTGeo->GetVolumeMuc(); }
  TGeoVolume         *GetVolumeMucAbsorber(int part, int seg, int absorber) {  return m_MucROOTGeo->GetVolumeAbsorber(part, seg, absorber); }
  TGeoVolume         *GetVolumeMucAbsorberPanel(int part, int seg, int absorber, int panel) { return m_MucROOTGeo->GetVolumeAbsorberPanel(part, seg, absorber, panel); }
  TGeoVolume         *GetVolumeMucGap(int part, int seg, int gap) { return m_MucROOTGeo->GetVolumeGap(part, seg, gap); }
  TGeoVolume         *GetVolumeMucStripPlane(int part, int seg, int gap) { return m_MucROOTGeo->GetVolumeStripPlane(part, seg, gap); }
  TGeoVolume         *GetVolumeMucStrip(int part, int seg, int gap, int strip) { return m_MucROOTGeo->GetVolumeStripPlane(part, seg, gap); }
  TGeoVolume         *GetVolumeMucGasChamber(int part, int seg, int gap, int panel, int gasChamber)  { return m_MucROOTGeo->GetVolumeGasChamber(part, seg, gap, panel, gasChamber); }
  TGeoVolume         *GetVolumeMucBakelite(int part, int seg, int gap, int RpcUpDown, int panel, int bakelite) { return m_MucROOTGeo->GetVolumeBakelite(part, seg, gap, RpcUpDown, panel, bakelite); }
  TGeoPhysicalNode   *GetPhysicalMucGap(int part, int seg, int gap) { return m_MucROOTGeo->GetPhysicalGap(part, seg, gap); }
  TGeoPhysicalNode   *GetPhysicalMucStrip(int part, int seg, int gap, int strip) { return m_MucROOTGeo->GetPhysicalStrip(part, seg, gap, strip); }

 private:
  Double_t          m_BesR;
  Double_t          m_BesZ;

  Double_t          m_BeamPipeRMin;
  Double_t          m_BeamPipeRMax;
  Double_t          m_BeamPipeZ;

  TGeoVolume       *m_Bes;                  //Bes world volume
  TGeoVolume       *m_BeamPipe;             //Beam Pipe volume
  TGeoVolume       *m_Mdc;
  TGeoVolume       *m_Tof;
  TGeoVolume       *m_Emc;
  TGeoVolume       *m_Muc;
  TGeoPhysicalNode *m_phyBeamPipe;          //Long Peixun's update: for BeamPipe full 3D view

  MdcROOTGeo       *m_MdcROOTGeo;           //MdcROOTGeo
  TofROOTGeo       *m_TofROOTGeo;           //TofROOTGeo
  EmcROOTGeo       *m_EmcROOTGeo;           //EmcROOTGeo
  MucROOTGeo       *m_MucROOTGeo;           //MucROOTGeo

  BesCircle2D      *m_BeamPipeXY;
  BesPolygon2D     *m_BeamPipeZR;
  BesPolygon2D     *m_ZRPlaneOnXY;

  Int_t            m_BeamPipeColor;       //Long Peixun's update: color of beampipe

  ClassDef(BesGeometry, 1) // Bes Geometry
};

R__EXTERN BesGeometry *gBesGeometry;

#endif 
