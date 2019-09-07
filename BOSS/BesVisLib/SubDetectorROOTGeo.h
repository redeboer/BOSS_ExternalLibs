//$id$
/*
 *    2004/12/9    Zhengyun You      Peking University
 *                 ROOT Geometry for Bes sub-detector in event display and reconstruction.
 *
 */

#ifndef SUBDETECTOR_ROOT_GEO_H
#define SUBDETECTOR_ROOT_GEO_H

#include <string>

#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TGeoPhysicalNode.h>

#include "Saxana/SAXProcessor.h"
#include "Saxana/ProcessingConfigurator.h"
#include "Processor/TGDMLProcessor.h"

/**
 * Class SubDetectorROOTGeo is a base class for the four subdetector
 * ROOT geometry class. 
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class SubDetectorROOTGeo
{
 public:

  /// Constructor.
  SubDetectorROOTGeo();

  /// Destructor.
  virtual ~SubDetectorROOTGeo();
  
  /// Initialize the instance of ROOTGeo.
  virtual void ReadGdml( const char *gdmlFile, const char *setupName );
  
  /// If the ROOT geometry of this subdetctor is initialized;
  virtual int IsInitialized() { return m_ROOTGeoInit; }

  // Set child no of this subdetector in bes;
  virtual void SetChildNo(int childNo) { m_childNo = childNo; }
  
  /// Get a logical volume by name;
  virtual TGeoVolume *GetLogicalVolume( const std::string& vn );
  
  /// Get the top(world) volume;
  virtual TGeoVolume *GetTopVolume() { return m_TopVolume; }

  /// Get an assembly by name;
  virtual TGeoVolumeAssembly *GetAssemblyVolume( const std::string& an );

  /// Get a node(physical volume) by name;
  virtual TGeoNode* GetNode( const std::string& nn );

  /// Get number of hits in HitsArray;
  virtual int GetHitsNum() { return m_HitsArray->GetEntries(); }
  
  /// Get ith hit in HitsArray;
  virtual TGeoPhysicalNode* GetHit(int i);

  /// Set all physicalNodes in m_DeteorsArray visible;
  virtual void SetDetectorOn();


 private:

  SAXProcessor           m_sxp;
  ProcessingConfigurator m_config;
  
 protected:
  // Have we initialize the geometry.
  int m_ROOTGeoInit;          
  int m_childNo;    // child no of this subdetector in bes. 
  int m_2DGeoInit;

  TGeoVolume *m_TopVolume;

  TObjArray *m_DetectorsArray;
  TObjArray *m_HitsArray;
  TObjArray *m_2DHitsArray;
};

#endif   /* SUBDETECTOR_ROOT_GEO_H */
