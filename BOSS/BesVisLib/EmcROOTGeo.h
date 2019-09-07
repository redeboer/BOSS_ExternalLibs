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

#ifndef Emc_ROOT_GEO_H
#define Emc_ROOT_GEO_H

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "SubDetectorROOTGeo.h"
#include "Emc2DCrystal.h"

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

  /// Initialize ROOTGeo from GDML.
  void InitFromGDML( const char *gdmlFile, const char *setupName );

  /// Initialize ROOTGeo from TGeoVolume logicalEmc.
  void InitFromROOT( TGeoVolume *vol );

  /// Initialize 2D Geometry
  void Init2DGeometry();

  /// Set the pointers to theirs nodes; 
  void SetNode();

  /// Set default visual attributes;
  void SetVolumeAppendInVis();

  /// Set default visual attributes;
  void SetVolumeDefaultVis();

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
  
  /// Set default physical node attributes;
  void SetPhysicalDefaultVis();   //Long Peixun's update: Remove annotation

  /// Set Detecor (what is detector depends on you)
  void SetDetector();
  
  /// Set all physicalNodes corresponding to digiCol;
  void SetHits();
  
  /// Set Emc detector visibility;
  void SetVisEmcDetector();

  /// Set Emc hits visibility;
  void SetVisEmcHits();

  /// Get number of part;
  int GetPartNb();

  /// Get number of theta on part;
  int GetThetaNb(int part);

  /// Get number of phi on part;
  int GetPhiNb(int part, int theta);

  /// Set Emc volume, while initializing from ROOT;
  void SetVolumeEmc(TGeoVolume *vol) { m_Emc = vol; }
  
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
  
  /// Get theta2 node;
  TGeoNode *GetTheta2( int part, int phi, int theta );
  
  /// Get crystal one; 
  TGeoNode *GetCrystal( int part, int phi, int theta );

  /// Get crystal2 one; 
  TGeoNode *GetCrystal2( int part, int phi, int theta );

  /// Get crystal physical node; all part,phi,theta has a physicalNode
  TGeoPhysicalNode *GetPhysicalCrystal( int part, int phi, int theta);
  
  /// Get crystal physical node2; EC nb 5,6,14,15,16 in any sector has two physicalNode
  TGeoPhysicalNode *GetPhysicalCrystal2( int part, int phi, int theta);
  
  /// Whether this id is combined of two nodes
  bool HasTwoNodes( int part, int phi, int theta );

  /// Compute theta, phi from sector, nb
  void ComputeThetaPhi( int id, int sector, int nb, int &CryNumberTheta, int &CryNumberPhi);

  /// Compute copyNb, copyNb 5,6,14,15,16 corresponding to two volume
  int  ComputeEndCopyNb(int num);

  /// Get Emc2DCrystal;
  Emc2DCrystal *Get2DCrystal( Int_t part, Int_t phi, Int_t theta ); 

  /// Get part no of a crystal physcial node
  Int_t GetPart(TGeoPhysicalNode* phyNode);

  /// Draw function
  void Draw(Option_t *option);

  /// Draw 2D hits
  void DrawHits(Option_t *option);
 
  /// Align a physical node, change its position and shape
  void Align(TGeoPhysicalNode *phyNode, TGeoMatrix *newmat=0, TGeoShape *newshape=0, Bool_t check=kFALSE);

  /// Restore the physical node to original shape
  void RestorePhyNode(TGeoPhysicalNode *phyNode, TGeoNode *node);

 private:
  static const int m_kPart     = 3;
  static const int m_kPhiBr    = 120;
  static const int m_kThetaBr  = 44;

  int m_kPhiEc;
  int m_kThetaEc;
  int m_kSectorEc;
  int m_kNbEc;
  int m_kRealNbEc;
  
  int m_EmcColor;
  int m_partColor;
  int m_phiColor;
  int m_thetaColor;
  int m_brCrystalColor;
  int m_ecCrystalColor;

  static const int EMC_TIME_FACTOR   = 1;
  static const int EMC_CHARGE_FACTOR = 1000; 

  TGeoVolume       *m_Emc;
  TGeoNode         *m_NodePart[m_kPart]; 
  TGeoNode         *m_NodePhi[m_kPart][m_kPhiBr]; 
  TGeoNode         *m_NodeTheta[m_kPart][m_kPhiBr][m_kThetaBr]; // m_kPhiBr > m_kPhiEc, m_kThetaBr > m_kThetaEc
  TGeoNode         *m_NodeTheta2[m_kPart][m_kPhiBr][m_kThetaBr]; // contains theta phi has tow nodes 
  TGeoPhysicalNode *m_PhysicalCrystal[m_kPart][m_kPhiBr][m_kThetaBr];
  TGeoPhysicalNode *m_PhysicalCrystal2[m_kPart][m_kPhiBr][m_kThetaBr];

  TObjArray        *m_PhyNodeOrgArray;
  
  // Volume refer to logicalVolume,  only refer to the one exists.
  // Node   refer to physicalVolume, each real theta has a corresponding physical volume, althought they may be the same one.
  // PhysicalNode gives the path combined by physical volume, it's unique for each real theta.

  const TObjArray  *m_EmcDigiCol;

  // 2D
  Emc2DCrystal     *m_Emc2DCrystal[m_kPart][m_kPhiBr][m_kThetaBr];

};

#endif   /* Emc_ROOT_GEO_H */
