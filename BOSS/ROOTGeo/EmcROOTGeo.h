//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Emc Geometry General for EventDisplay
 *
 *    2004/12/11   Zhengyun You      Peking University
 *                 named from EmcGeo to EmcROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 *
 *    2005/04/04   Zhengyun You      Peking University
 *                 Emc End geometry added
 */

#ifndef EMC_ROOT_GEO_H
#define EMC_ROOT_GEO_H

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "SubDetectorROOTGeo.h"
/**
 * Class EmcGeo contains all of the objects necessary to describe the
 * emc geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class EmcROOTGeo : public SubDetectorROOTGeo
{
 public:
  
  /// Constructor.
  EmcROOTGeo();

  /// Destructor.
  ~EmcROOTGeo();

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

  /// Get number of theta on part;
  int GetThetaNb(int part);

  /// Get number of phi on part;
  int GetPhiNb(int part);

  /// Get Emc volume;
  TGeoVolume *GetVolumeEmc() { return m_Emc; }
  
  /// Get part volume;
  TGeoVolume *GetVolumePart( int part );

  /// Get phi volume;  
  TGeoVolume *GetVolumePhi( int part, int phi );
  
  /// Get theta volume;
  TGeoVolume *GetVolumeTheta( int part, int phi, int theta );

  /// Get crystal volume;
  TGeoVolume *GetVolumeCrystal( int part, int phi, int theta );

  /// Get part node;
  TGeoNode *GetPart( int part );

  /// Get phi node;
  TGeoNode *GetPhi( int part, int phi );

  /// Get theta node;
  TGeoNode *GetTheta( int part, int phi, int theta );
  
  /// Get crystal one; 
  TGeoNode *GetCrystal( int part, int phi, int theta );

  /// Get crystal physical node; 
  TGeoPhysicalNode *GetPhysicalCrystal( int part, int phi, int theta );

 private:
  static const int m_kPart    = 3;
  static const int m_kPhiBr   = 120;
  static const int m_kThetaBr = 44;
  //static const int m_kPhiEc   = 16;
  //static const int m_kThetaEc = 35;
  int m_kPhiEc;
  int m_kThetaEc;

  TGeoVolume       *m_Emc;
  TGeoNode         *m_NodeTheta[m_kPart][m_kPhiBr][m_kThetaBr]; // m_kPhiBr > m_kPhiEc, m_kThetaBr > m_kThetaEc
  TGeoPhysicalNode *m_PhysicalCrystal[m_kPart][m_kPhiBr][m_kThetaBr];

  // Volume refer to logicalVolume,  only refer to the one exists.
  // Node   refer to physicalVolume, each real theta has a corresponding physical volume, althought they may be the same one.
  // PhysicalNode gives the path combined by physical volume, it's unique for each real theta.
};

#endif   /* EMC_ROOT_GEO_H */
