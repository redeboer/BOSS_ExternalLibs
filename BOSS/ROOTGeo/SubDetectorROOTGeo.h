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
  void ReadGdml( const char *gdmlFile, const char *setupName );
  
  /// If the ROOT geometry of this subdetctor is initialized;
  int IsInitialized() { return m_ROOTGeoInit; }

  // Set child no of this subdetector in bes;
  void SetChildNo(int childNo) { m_childNo = childNo; }
  
  /// Get a logical volume by name;
  TGeoVolume *GetLogicalVolume( const std::string& vn );
  
  /// Get the top(world) volume;
  TGeoVolume *GetTopVolume() { return m_TopVolume; }

  /// Get an assembly by name;
  TGeoVolumeAssembly *GetAssemblyVolume( const std::string& an );

  /// Get a node(physical volume) by name;
  TGeoNode* GetNode( const std::string& nn );
  
 private:

  SAXProcessor           m_sxp;
  ProcessingConfigurator m_config;
  
 protected:
  // Have we initialize the geometry.
  int m_ROOTGeoInit;          
  int m_childNo;    // child no of this subdetector in bes. 

  TGeoVolume *m_TopVolume;
};

#endif   /* SUBDETECTOR_ROOT_GEO_H */
