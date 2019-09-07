//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Tof Geometry General for EventDisplay
 *
 *    2004/12/9    Zhengyun You      Peking University
 *                 named from TofGeo to TofROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 */

#ifndef Tof_ROOT_GEO_H
#define Tof_ROOT_GEO_H

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "SubDetectorROOTGeo.h"
#include "Tof2DScin.h"

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

  //Long Peixun's update: return wether Tof contains Mrpc
  bool GetMrpc() const { return fTofMrpc; }

  /// Initialize ROOTGeo from GDML.
  void InitFromGDML( const char *gdmlFile, const char *setupName );

  /// Initialize ROOTGeo from TGeoVolume logicalTof.
  void InitFromROOT( TGeoVolume *vol );

  /// Initialize 2D Geometry
  void Init2DGeometry();

  /// Set the pointers to theirs nodes; 
  void SetNode();
  
  /// Set default visual attributes;
  void SetVolumeDefaultVis();
  //Long Peixun's update: Set default 3D color
  void SetPhysicalDefaultVis();

  /// Set all visible;
  void SetAllVisible();
  void SetTMatch(Bool_t input);
  void SetQMatch(Bool_t input);

  /// Set quater visible;
  void SetQuarterVisible();

  /// Set half visible;
  void SetHalfVisible();

  /// Set noend visible;
  void SetNoEndVisible();

  /// Set the pointers to the physical nodes; 
  void SetPhysicalNode();
  
  /// Draw Detecor (what is detector depends on you)
  void SetDetector();

  /// Set all physicalNodes corresponding to digiCol;
  void SetHits();
  
  /// Set Tof detector visibility;
  void SetVisTofDetector();

  /// Set Tof hits visibility;
  void SetVisTofHits();

  /// Get number of part;
  int GetPartNb();

  /// Get number of scintillators on each part;
  int GetScinNb(int part);

  // Get number of layer on part;
  int GetLayerNb(int part);

  //Huang ShuHui's update: for Tof with MRPC
  // Get number of Module/Container on each end-cap;
	int GetModuleNb(int part);
  // Get number of Strip on each module;
	int GetStripNb(int module);
	// Get bareChamber copy number;
	int GetChamberNodeNb(int module);
	// Get gasContainer copy number;
	int GetContainerNodeNb(int module);
	// Get pcbBoard1 copy number;
	int GetBoard1NodeNb(int module);

  /// Set Tof volume, while initializing from ROOT;
  void SetVolumeTof(TGeoVolume *vol) { m_Tof = vol; }
  
  /// Get Tof volume;
  TGeoVolume *GetVolumeTof() { return m_Tof; }
  
  /// Get assembly volume; part=0,2 for AssemblyEc, 1 for AssemblyBr;  
  //TGeoVolumeAssembly *GetVolumeAssembly( int part );
  
  /// Get part volume;
  TGeoVolume *GetVolumePart( int part);

  /// Get PVF volume;
  TGeoVolume *GetVolumePVF( int part, int layer );

  /// Get Al volume;
  TGeoVolume *GetVolumeAl( int part, int layer );

  /// Get scintillator volume;
  TGeoVolume *GetVolumeScin( int part, int layer );

  /// Get bucket volume;
  TGeoVolume *GetVolumeBucket( int part, int layer );

  /// Get PVF node;
  TGeoNode *GetPVF( int part, int layer, int scin );

  /// Get Al node;
  TGeoNode *GetAl( int part, int layer );

  /// Get scintillator node;
  TGeoNode *GetScin( int part, int layer );

  //Huang Shuhui's update: for Tof with MRPC
  /// Get module node;
  TGeoNode *GetModule( int part, int module );
	/// Get strip node;
	TGeoNode *GetStrip( int part, int module, int strip );
  /// Get gasContainer node;
  TGeoNode *GetgasContainer( int part, int module );
  /// Get bareChamber node;
  TGeoNode *GetbareChamber( int part, int module );
  /// Get pcbBoard1 node;
  TGeoNode *GetpcbBoard1( int part, int module );

  /// Get bucket node, 0 for west bucket, 1 for the east one; 
  //TGeoNode *GetBucket( int part, int layer, int scin, int bucket );

  /// Get scintillator physical node; 
  TGeoPhysicalNode *GetPhysicalScin( int part, int layer, int scin );

  /// Get Tof2DScin;
  Tof2DScin *Get2DScin( Int_t part, Int_t layer, Int_t scin ); 

  /// Get part no of a scintillator physcial node
  Int_t GetPart(TGeoPhysicalNode* phyNode);

  /// Draw function
  void Draw(Option_t *option);

  /// Draw 2D hits
  void DrawHits(Option_t *option);

 private:
  static const int m_kPart     = 3;
  static const int m_kLayerBr  = 2;
  static const int m_kLayerEc  = 1;
  static const int m_kScinBr   = 88;
  static const int m_kScinEc   = 48;
  static const int m_kBucketEc = 1;
  static const int m_kBucketBr = 2;

	static const int m_kModuleEc = 36;        //Huang Shuhui's update: For Mrpc
	static const int m_kStripEc  = 12;        //Huang Shuhui's update: For Mrpc
	static const int m_kChamberNodeNb   = 6;  //Huang Shuhui's update: For Mrpc Geometry Initialization
	static const int m_kContainerNodeNb = 0;  //Huang Shuhui's update: For Mrpc Geometry Initialization
	static const int m_kBoard1NodeNb    = 30; //Huang Shuhui's update: For Mrpc Geometry Initialization

  static const int TOF_TIME_FACTOR   = 1000000;
  static const int TOF_CHARGE_FACTOR = 1000000; 
   
  Bool_t k_TMatch;
  Bool_t k_QMatch;

  int m_TofColor;
  int m_BucketColor;
  int m_ScinColor;

  TGeoVolume       *m_Tof;
  TGeoNode         *m_NodePVF[m_kPart][m_kLayerBr][m_kScinBr];
  TGeoNode         *m_NodeAl[m_kPart][m_kLayerBr];
  TGeoNode         *m_NodeScin[m_kPart][m_kLayerBr];
  TGeoPhysicalNode *m_PhysicalScin[m_kPart][m_kModuleEc][m_kScinBr];  //Huang Shuhui's update: m_kLayerBr -> m_kModuleEc
  
	//Huang Shuhui's update: For Mrpc
	TGeoNode         *m_NodeModule[m_kPart][m_kModuleEc];
	TGeoNode         *m_NodegasContainer[m_kPart][m_kModuleEc];
  TGeoNode         *m_NodebareChamber[m_kPart][m_kModuleEc];
  TGeoNode         *m_NodepcbBoard1[m_kPart][m_kModuleEc];
	TGeoNode         *m_NodeStrip[m_kPart][m_kModuleEc][m_kStripEc];
  //Long Peixun's update: Merge m_PhysicalMrpc into m_PhysicalScin
  //TGeoPhysicalNode *m_PhysicalMrpc[m_kPart][m_kModuleEc][m_kStripEc];

  const TObjArray  *m_TofDigiCol;

	//Huang Shuhui's update: Geometry flag for Tof_mrpc
  //Long Peixun's update: Change fTofMrpc to be a private variable
	Bool_t fTofMrpc;

  // 2D
  Tof2DScin        *m_Tof2DScin[m_kPart][m_kModuleEc][m_kScinBr];  //Huang Shuhui's update: m_kLayerBr -> m_kModuleEc

};

#endif   /* Tof_ROOT_GEO_H */
