//$id$
/*
 *    2004/11/29   Zhengyun You      Peking University
 *                 Tof Geometry General for EventDisplay
 *
 *    2004/12/9    Zhengyun You      Peking University
 *                 named from TofGeo to MdcROOTGeo
 *                 inherit from class SubDetectorROOTGeo
 */

#ifndef Mdc_ROOT_GEO_H
#define Mdc_ROOT_GEO_H

#include <map>

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "SubDetectorROOTGeo.h"
#include "Mdc2DWire.h"

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
  //Long Peixun's update: Add CGEM flag
  MdcROOTGeo(Bool_t cgem_flag = 0);

  /// Destructor.
  ~MdcROOTGeo();

  /// Initialize ROOTGeo from GDML.
  void InitFromGDML( const char *gdmlFile, const char *setupName );

  /// Initialize ROOTGeo from TGeoVolume logicalMdc.
  void InitFromROOT( TGeoVolume *vol );

  /// Initialize 2D Geometry
  void Init2DGeometry();

  /// Set the pointers to theirs nodes; 
  void SetNode();
  
  /// Correct some axial layer id to copyNo;
  int CorrectReplica(int layer, int replica);

  /// Set default visual attributes;
  void SetVolumeDefaultVis();

  /// Set all visible;
  void SetAllVisible();

  /// Set quater visible;
  void SetQuarterVisible();

  /// Set half visible;
  void SetHalfVisible();

  /// Set the pointers to the physical nodes; 
  void SetPhysicalNode();
  
  /// Set default physical node attributes;
  void SetPhysicalDefaultVis();   //Long Peixun' update: Remove annotation

  /// Draw Detecor (what is detector depends on you)
  void SetDetector();

  /// Set all physicalNodes corresponding to digiCol;
  void SetHits();
  void SetTFire(Bool_t input);
  void SetQFire(Bool_t input);
  //void SetTOverflow(Bool_t input);
  void SetQNotOverflow(Bool_t input);
  void SetColorfulWire(Bool_t input);
  void SetMdcTimeSubEvTime(Bool_t input);
  
  /// Set Mdc default detector visibility;
  void SetVisMdcDetector();

  /// Set Mdc hits visibility;
  void SetVisMdcHits();

  /// Get number of segments;
  int GetSegmentNb() { return m_kSegment; }

  /// Get number of layers;
  int GetLayerNb() { return m_kLayer; }

  /// Get number of replicas on each layer;
  int GetReplicaNb(int layer) { return m_kReplica[layer]; }

  /// Set Mdc volume, while initializing from ROOT;
  void SetVolumeMdc(TGeoVolume *vol) { m_Mdc = vol; }
  
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

  /// Get segment physical node;
  TGeoPhysicalNode *GetPhysicalSegment( int segment );

  /// Get replica physical node;
  TGeoPhysicalNode *GetPhysicalReplica( int layer, int replica );

  /// Get Mdc2DWire;
  Mdc2DWire *Get2DWire( Int_t layer, Int_t replica ); 

  /// Trans trueLayer to simuLayer (0~42)->(0~49)
  Int_t GetSimuLayer(Int_t trueLayer);
  
  /// Judge whether the digiCol contains (layer, wire) 
  Bool_t IsHit(Int_t layer, Int_t wire);

  /// Draw function
  void Draw(Option_t *option);

  /// Draw 2D hits
  void DrawHits(Option_t *option);

 private:

  static const int m_kSegment        = 66;
  static const int m_kStereoLayerIn  = 8;
  static const int m_kAxialLayerIn   = 12;
  static const int m_kStereoLayerOut = 16;
  static const int m_kAxialLayerOut  = 7;
  static const int m_kLayer          = 50;
  static const int m_kTrueLayer      = 43; // m_kLayer-m_kAxialLayerOut
  static const int m_kReplicaMax     = 288;
  static const int m_kReplica[m_kLayer];
  static const int m_kStereoDir[m_kTrueLayer];
  static const int m_kCorrectLayer   = 10;
  static const int m_kiCorrectLayer[m_kCorrectLayer];
  static const int m_kiCorrectReplica[m_kCorrectLayer];

	//huangsh
  static const int m_kSegmentNodeOffset  = 891;
  static const int m_kSegmentBr          = 2;//Segment 1 or 2 is special

  Bool_t k_TFire;
  Bool_t k_QFire;
  //Bool_t k_TOverflow;
  Bool_t k_QNotOverflow;
  Bool_t k_ColorfulWire;
  Bool_t k_MdcTimeSubEvTime;

  //Long Peixun's update: Add CGEM flags
  int m_StartLayer;   //0 - No CGEM;  8 - Contains CGEM

  static const int MDC_TIME_FACTOR   = 100000;
  static const int MDC_CHARGE_FACTOR = 1000000;

  int m_MdcColor;
  int m_segmentColor;
  int m_hypeColor;
  int m_tubeColor;
  int m_twistedTubsColor;
  int m_replicaColor;

  std::map<int ,int> m_CorrectMap;
  typedef std::map<int, int> intMap;

  TGeoVolume       *m_Mdc;
  TGeoNode         *m_NodeLayer[m_kLayer];
  TGeoNode         *m_NodeReplica[m_kLayer][m_kReplicaMax];
  TGeoPhysicalNode *m_PhysicalSegment[m_kSegment*2];
  TGeoPhysicalNode *m_PhysicalReplica[m_kLayer][m_kReplicaMax];

  const TObjArray  *m_MdcDigiCol;

  // 2D XY
  BesCircle2D        *m_MdcXY;
  BesCircle2D        *m_MdcXYSuper[4];
  Mdc2DWire          *m_Mdc2DWire[m_kTrueLayer][m_kReplicaMax];

  // 2D ZR
  BesPolygon2D       *m_MdcZR[2];

};

#endif   /* Mdc_ROOT_GEO_H */
