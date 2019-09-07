//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Tof Geometry General for EventDisplay
 *
 *    2004/12/9    Zhengyun You      Peking University
 *                 named from TofGeo to TofROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 */

#ifndef TOF_ROOT_GEO_H
#define TOF_ROOT_GEO_H

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "SubDetectorROOTGeo.h"
/**
 * Class TofGeo contains all of the objects necessary to describe the
 * tof geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class TofROOTGeo : public SubDetectorROOTGeo
{
 public:
  
  /// Constructor.
  TofROOTGeo();

  /// Destructor.
  ~TofROOTGeo();

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
  int GetPartNb();

  /// Get number of scintillators on each part;
  int GetScinNb(int part);

  /// Get Tof volume;
  TGeoVolume *GetVolumeTof() { return m_Tof; }
  
  /// Get assembly volume; part=0,2 for AssemblyEc, 1 for AssemblyBr;  
  TGeoVolumeAssembly *GetVolumeAssembly( int part );

  /// Get scintillator volume;
  TGeoVolume *GetVolumeScin( int part );

  /// Get bucket volume;
  TGeoVolume *GetVolumeBucket( int part );

  /// Get scintillator node;
  TGeoNode *GetScin( int part, int scin );

  /// Get bucket node, 0 for west bucket, 1 for the east one; 
  TGeoNode *GetBucket( int part, int scin, int bucket );

  /// Get scintillator physical node; 
  TGeoPhysicalNode *GetPhysicalScin( int part, int scin );

 private:
  static const int m_kPart       = 3;
  static const int m_kAssemblyEc = 48;
  static const int m_kAssemblyBr = 88;
  static const int m_kBucketEc   = 1;
  static const int m_kBucketBr   = 2;

  TGeoVolume       *m_Tof;
  TGeoNode         *m_NodeScin[m_kPart][m_kAssemblyBr];
  TGeoPhysicalNode *m_PhysicalScin[m_kPart][m_kAssemblyBr];
};

#endif   /* TOF_ROOT_GEO_H */
