//$id$
/*
 *    2005/10/31    Zhengyun You      Peking University
 *                  G4 Geometry for Bes sub-detector in simulation and track ext in reconstruction.
 *
 */

#ifndef SUBDETECTOR_G4_GEO_H
#define SUBDETECTOR_G4_GEO_H

#include <string>

#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include "G4VisAttributes.hh"
#include "G4Color.hh"

#include "Saxana/SAXProcessor.h"
#include "Saxana/ProcessingConfigurator.h"
#include "G4Processor/GDMLProcessor.h"

/**
 * Class SubDetectorG4Geo is a base class for the four subdetector
 * G4 geometry class. 
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class SubDetectorG4Geo
{
 public:

  /// Constructor.
  SubDetectorG4Geo();

  /// Destructor.
  virtual ~SubDetectorG4Geo();
  
  /// Initialize the instance of G4Geo.
  void ReadGdml( const char *gdmlFile, const char *setupName );
  
  /// If the G4 geometry of this subdetctor is initialized;
  int IsInitialized() { return m_G4GeoInit; }

  /// Get a logical volume by name;
  G4LogicalVolume* GetLogicalVolume( const std::string& vn );
  
  /// Get the top(world) volume;
  G4LogicalVolume* GetTopVolume() { return m_TopVolume; }
  
 private:

  SAXProcessor           m_sxp;
  ProcessingConfigurator m_config;
  
 protected:
  // Have we initialize the geometry.
  int m_G4GeoInit;          

  G4LogicalVolume *m_TopVolume;
};

#endif   /* SUBDETECTOR_G4_GEO_H */
