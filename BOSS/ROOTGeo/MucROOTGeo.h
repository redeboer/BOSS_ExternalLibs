//$id$
/*
 *    2004/9/20    Zhengyun You      Peking University
 *                 Muc Geometry General for EventDisplay
 *
 *    2004/12/11   Zhengyun You      Peking University
 *                 named from MucGeo to MucROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 */

#ifndef MUC_ROOT_GEO_H
#define MUC_ROOT_GEO_H

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "SubDetectorROOTGeo.h"
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

  /// Initialize the instance of ROOTGeo.
  void InitFromGdml( const char *gdmlFile, const char *setupName );

  /// Set the pointers to theirs nodes; 
  void SetNode();
  
  /// Set default visual attributes;
  void SetDefaultVis();

  /// Set all visible;
  void SetAllVisible();

  /// Set quater visible;
  void SetQuarterVisible();

  /// Set the pointers to the physical nodes; 
  void SetPhysicalNode();
  
  /// Get number of part;
  int GetPartNum();

  /// Get number of segment on part;
  int GetSegNum(int part);
  
  /// Get number of gap on part;
  int GetGapNum(int part);

  /// Get number of strip on gap;
  int GetStripNum(int part, int seg, int gap);

  /// Get Muc volume;
  TGeoVolume *GetVolumeMuc() { return m_Muc; }
  
  /// Get absorber volume;   
  TGeoVolume *GetVolumeAbsorber( int part, int seg, int absorber );

  /// Get thickness of an absorber;
  float GetAbsorberThickness( int part, int seg, int absorber );

  /// Get absorber panel volume;   
  TGeoVolume *GetVolumeAbsorberPanel( int part, int seg, int absorber, int panel );

  /// Get gap volume;   
  TGeoVolume *GetVolumeGap( int part, int seg, int gap );

  /// Get box volume;   
  TGeoVolume *GetVolumeAluminumBox( int part, int seg, int gap );

  /// Get strip plane volume;
  TGeoVolume *GetVolumeStripPlane( int part, int seg, int gap );

  /// Get strip volume;   
  TGeoVolume *GetVolumeStrip( int part, int seg, int gap, int strip );
  
  /// Get rpc gas chamber volume;   
  TGeoVolume *GetVolumeGasChamber( int part, int seg, int gap, int panel, int gasChamber );

  /// Get rpc bakelite volume;   
  TGeoVolume *GetVolumeBakelite( int part, int seg, int gap, int panel, int bakelite );

  /// Get absorber node;   
  //TGeoNode *GetAbsorber( int part, int seg, int absorber );

  /// Get absorber panel node;   
  //TGeoNode *GetAbsorberPanel( int part, int seg, int absorber, int panel );

  /// Get gap node;   
  TGeoNode *GetGap( int part, int seg, int gap );

  /// Get box node;
  //TGeoNode *GetAluminumBox( int part, int seg, int gap );

  /// Get strip plane node;
  TGeoNode *GetStripPlane( int part, int seg, int gap);

  /// Get strip node;   
  TGeoNode *GetStrip( int part, int seg, int gap, int strip );
  
  /// Get rpc gas chamber node;   
  //TGeoNode *GetGasChamber( int part, int seg, int gap, int gasChamber );

  /// Get rpc bakelite node;   
  //TGeoNode *GetBakelite( int part, int seg, int gap, int bakelite );

  /// Get gap physical node;
  TGeoPhysicalNode *GetPhysicalGap( int part, int seg, int gap );

  /// Get box physical node;
  TGeoPhysicalNode *GetPhysicalAluminumBox( int part, int seg, int gap );


  /// Get strip physical node;
  TGeoPhysicalNode *GetPhysicalStrip( int part, int seg, int gap, int strip );

 private:

  static const int m_kPart        = 3;
  static const int m_kSegMax      = 8;
  static const int m_kAbsorberMax = 9;
  static const int m_kGapMax      = 9;
  static const int m_kPanelMax    = 4;
  static const int m_kStripMax    = 112;
  static const int m_kBakelite    = 4;
  static const int m_kGasChamber  = 2;

  static const int m_kSeg[m_kPart];
  static const int m_kAbsorber[m_kPart];
  static const int m_kGap[m_kPart];
  static const int m_kPanel[m_kPart];
  
  int m_StripNum[m_kPart][m_kSegMax][m_kGapMax];

  TGeoVolume       *m_Muc;
  TGeoNode         *m_NodeGap[m_kPart][m_kSegMax][m_kGapMax];
  //TGeoNode         *m_NodeAluminumBox[m_kPart][m_kSegMax][m_kGapMax];
  TGeoNode         *m_NodeStripPlane[m_kPart][m_kSegMax][m_kGapMax];
  TGeoNode         *m_NodeStrip[m_kPart][m_kSegMax][m_kGapMax][m_kStripMax];

  TGeoPhysicalNode *m_PhysicalGap[m_kPart][m_kSegMax][m_kGapMax];
  //  TGeoPhysicalNode *m_PhysicalBox[m_kPart][m_kSegMax][m_kGapMax];
  TGeoPhysicalNode *m_PhysicalStrip[m_kPart][m_kSegMax][m_kGapMax][m_kStripMax];
};

#endif   /* MUC_ROOT_GEO_H */
