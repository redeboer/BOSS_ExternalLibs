//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Tof Geometry General for EventDisplay
 *
 *    2004/12/9    Zhengyun You      Peking University
 *                 named from TofGeo to MdcROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 */

#ifndef MDC_ROOT_GEO_H
#define MDC_ROOT_GEO_H

#include <map>

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "SubDetectorROOTGeo.h"
/**
 * Class MdcGeo contains all of the objects necessary to describe the
 * mdc geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MdcROOTGeo : public SubDetectorROOTGeo
{
 public:
  
  /// Constructor.
  MdcROOTGeo();

  /// Destructor.
  ~MdcROOTGeo();

  /// Initialize the instance of ROOTGeo.
  void InitFromGdml( const char *gdmlFile, const char *setupName );

  /// Set the pointers to theirs nodes; 
  void SetNode();
  
  /// Correct some axial layer id to copyNo;
  int CorrectReplica(int layer, int replica);

  /// Set default visual attributes;
  void SetDefaultVis();

  /// Set all visible;
  void SetAllVisible();

  /// Set quater visible;
  void SetQuarterVisible();

  /// Set the pointers to the physical nodes; 
  void SetPhysicalNode();
  
  /// Get number of segments;
  int GetSegmentNb();

  /// Get number of layers;
  int GetLayerNb();

  /// Get number of replicas on each layer;
  int GetReplicaNb(int layer) { return m_kReplica[layer]; }

  /// Get Mdc volume;
  TGeoVolume *GetVolumeMdc() { return m_Mdc; }
  
  /// Get segment volume;
  TGeoVolume *GetVolumeSegment( int segment );

  /// Get layer volume;  
  TGeoVolume *GetVolumeLayer( int layer );
  
  /// Get replica volume;
  TGeoVolume *GetVolumeReplica( int layer );

  /// Get segment node;
  TGeoNode *GetSegment( int segment, int no );
  
  /// Get layer node;  
  TGeoNode *GetLayer( int layer );
  
  /// Get replica node;
  TGeoNode *GetReplica( int layer, int replica );

  /// Get replica physical node;
  TGeoPhysicalNode *GetPhysicalReplica( int layer, int replica );

 private:

  static const int m_kSegment        = 66;
  static const int m_kStereoLayerIn  = 8;
  static const int m_kAxialLayerIn   = 12;
  static const int m_kStereoLayerOut = 16;
  static const int m_kAxialLayerOut  = 7;
  static const int m_kLayer          = 50;
  static const int m_kReplicaMax     = 288;
  static const int m_kReplica[m_kLayer];
  static const int m_kCorrectLayer   = 10;
  static const int m_kiCorrectLayer[m_kCorrectLayer];
  static const int m_kiCorrectReplica[m_kCorrectLayer];

  std::map<int ,int> m_CorrectMap;
  typedef std::map<int, int> intMap;

  TGeoVolume       *m_Mdc;
  TGeoNode         *m_NodeReplica[m_kLayer][m_kReplicaMax];
  TGeoPhysicalNode *m_PhysicalReplica[m_kLayer][m_kReplicaMax];
};

#endif   /* MDC_ROOT_GEO_H */
