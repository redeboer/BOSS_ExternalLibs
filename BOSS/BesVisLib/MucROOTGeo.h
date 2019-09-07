//$id$
/*
 *    2004/9/20    Zhengyun You      Peking University
 *                 Muc Geometry General for EventDisplay
 *
 *    2004/12/11   Zhengyun You      Peking University
 *                 named from MucGeo to MucROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 */

#ifndef Muc_ROOT_GEO_H
#define Muc_ROOT_GEO_H

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "SubDetectorROOTGeo.h"
#include "BesPolygon2D.h"
#include "Muc2DStrip.h"

/**
 * Class MucGeo contains all of the objects necessary to describe the
 * muc geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MucROOTGeo : public SubDetectorROOTGeo
{
 public:
  
  /// Constructor.
  MucROOTGeo();

  /// Destructor.
  ~MucROOTGeo();

  /// Initialize ROOTGeo from GDML.
  void InitFromGDML( const char *gdmlFile, const char *setupName );

  /// Initialize ROOTGeo from TGeoVolume logicalMuc.
  void InitFromROOT( TGeoVolume *vol );

  /// Initialize 2D Geometry
  void Init2DGeometry();

  /// Set the pointers to theirs nodes; 
  void SetNode();
  
  /// Set default visual attributes;
  void SetVolumeDefaultVis();

  //Long Peixun's update: Set detector 3D color
  void SetPhysicalDefaultVis();

  /// Set all visible;
  void SetAllVisible();

  /// Set quater visible;
  void SetQuarterVisible();

  /// Set half visible;
  void SetHalfVisible();

  /// Set noend visible;
  void SetNoEndVisible();

  /// Set the pointers to the physical nodes; 
  void SetPhysicalNode();
  
  /// Set Detecor (what is detector depends on you)
  void SetDetector();
  
  /// Set all physicalNodes corresponding to digiCol;
  void SetHits();
  
  /// Set Muc detector visibility;
  void SetVisMucDetector();

  /// Set Muc hits visibility;
  void SetVisMucHits();

  /// Get number of part;
  int GetPartNb();

  /// Get number of segment on part;
  int GetSegNb(int part);
  
  /// Get number of gap on part;
  int GetGapNb(int part);

  /// Get number of strip on gap;
  int GetStripNb(int part, int seg, int gap);

  /// Get number of absorber on part;
  int GetAbsorberNb(int part);
  
  /// Set Muc volume, while initializing from ROOT;
  void SetVolumeMuc(TGeoVolume *vol) { m_Muc = vol; }
  
  /// Get Muc volume;
  TGeoVolume *GetVolumeMuc() { return m_Muc; }
  
  /// Get absorber volume;   
  TGeoVolume *GetVolumeAbsorber( int part, int seg, int absorber );

  /// Get absorber small block;
  TGeoVolume *GetVolumeAbsorberSmallBlock( int gap, int sb );

  /// Get absorber panel volume;   
  TGeoVolume *GetVolumeAbsorberPanel( int part, int seg, int absorber, int panel );

  /// Get gap volume;   
  TGeoVolume *GetVolumeGap( int part, int seg, int gap );

  /// Get box volume;   
  TGeoVolume *GetVolumeBox( int part, int seg, int gap);

  /// Get box surface volume;   
  TGeoVolume *GetVolumeBoxSurface( int part, int seg, int gap , int up);

  /// Get strip plane volume;
  TGeoVolume *GetVolumeStripPlane( int part, int seg, int gap );

  /// Get strip volume;   
  TGeoVolume *GetVolumeStrip( int part, int seg, int gap, int strip );
  
  /// Get rpc gas chamber volume;   
  TGeoVolume *GetVolumeGasChamber( int part, int seg, int gap, int panel, int gasChamber );

  /// Get rpc gas border volume;   
  TGeoVolume *GetVolumeGasBorder( int part, int seg, int gap, int panel, int gasChamber );

  /// Get rpc bakelite volume;   
  TGeoVolume *GetVolumeBakelite( int part, int seg, int gap, int RpcUpDown, int panel, int bakelite );

  /// Get absorber node;   
  TGeoNode *GetAbsorber( int part, int seg, int absorber );

  /// Get absorber panel node;   
  TGeoNode *GetAbsorberPanel( int part, int seg, int absorber, int panel );

  /// Get gap node;   
  TGeoNode *GetGap( int part, int seg, int gap );

  /// Get strip plane node;
  TGeoNode *GetStripPlane( int part, int seg, int gap);

  /// Get strip node;   
  TGeoNode *GetStrip( int part, int seg, int gap, int strip );
  
  /// Get rpc gas chamber node;   
  //TGeoNode *GetGasChamber( int part, int seg, int gap, int gasChamber );

  /// Get rpc bakelite node;   
  //TGeoNode *GetBakelite( int part, int seg, int gap, int bakelite );

  /// Get absorber physical node;
  TGeoPhysicalNode *GetPhysicalAbsorber( int part, int seg, int gap, int panel );

  /// Get gap physical node;
  TGeoPhysicalNode *GetPhysicalGap( int part, int seg, int gap );

  /// Get strip physical node;
  TGeoPhysicalNode *GetPhysicalStrip( int part, int seg, int gap, int strip );

  /// Get Muc2DStrip;
  Muc2DStrip *Get2DStrip( int part, int seg, int gap, int strip ); 

  /// Get part no of a physcial node
  Int_t GetPart(TGeoPhysicalNode* phyNode);

  /// Is a segment visible in ZR view
  Bool_t IsZRVisible( int part, int seg );

  /// Get input value 0~360 
  Double_t Range360(Double_t input);

  /// Draw function
  void Draw(Option_t *option);

  /// Draw 2D hits
  void DrawHits(Option_t *option);
 
 private:

  static const int m_kPart        = 3;
  static const int m_kSegMax      = 8;
  static const int m_kAbsorberMax = 9;
  static const int m_kGapMax      = 9;
  static const int m_kPanelMax    = 4;
  static const int m_kStripMax    = 112;
  static const int m_kBakelite    = 2;
  static const int m_kGasChamber  = 1;
  static const int m_kSmallBlockMax = 2;
  static const int m_kUpDown      = 2;

  static const int m_kSeg[m_kPart];
  static const int m_kAbsorber[m_kPart];
  static const int m_kGap[m_kPart];
  static const int m_kPanel[m_kPart];
  static const int m_kBakelitePanel[m_kPart][m_kUpDown];

  int m_StripNum[m_kPart][m_kSegMax][m_kGapMax];

  int m_MucColor;
  int m_absorberColor;
  int m_gapColor;
  int m_gasChamberColor;
  int m_bakeliteColor;
  int m_stripColor;

  static const Int_t m_kStripZMuliple = 12;  // as dz is very thin, multiply so that it can be seen.

  TGeoVolume       *m_Muc;
  TGeoNode         *m_NodeAbsorber[m_kPart][m_kSegMax][m_kAbsorberMax];
  TGeoNode         *m_NodeAbsorberPanel[m_kPart][m_kSegMax][m_kAbsorberMax][m_kPanelMax];
  TGeoNode         *m_NodeGap[m_kPart][m_kSegMax][m_kGapMax];
  TGeoNode         *m_NodeStripPlane[m_kPart][m_kSegMax][m_kGapMax];
  TGeoNode         *m_NodeStrip[m_kPart][m_kSegMax][m_kGapMax][m_kStripMax];

  TGeoPhysicalNode *m_PhysicalAbsorber[m_kPart][m_kSegMax][m_kAbsorberMax][m_kPanelMax];
  TGeoPhysicalNode *m_PhysicalGap[m_kPart][m_kSegMax][m_kGapMax];
  TGeoPhysicalNode *m_PhysicalStrip[m_kPart][m_kSegMax][m_kGapMax][m_kStripMax];

  const TObjArray  *m_MucDigiCol;

  // 2D
  BesPolygon2D     *m_MucXY[m_kPart][m_kSegMax];
  BesPolygon2D     *m_MucZR[m_kPart][m_kSegMax];
  BesPolygon2D     *m_MucXYGap[m_kPart][m_kSegMax][m_kGapMax];
  BesPolygon2D     *m_MucZRGap[m_kPart][m_kSegMax][m_kGapMax];
  
  Muc2DStrip       *m_Muc2DStrip[m_kPart][m_kSegMax][m_kGapMax][m_kStripMax];
};

#endif   /* Muc_ROOT_GEO_H */
