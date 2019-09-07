//$id$
/*
 *    2005/10/31   Zhengyun You      Peking University
 *                 Muc Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

#ifndef MUC_G4_GEO_H
#define MUC_G4_GEO_H

#include "SubDetectorG4Geo.h"

/**
 * Class MucGeo contains all of the objects necessary to describe the
 * muc geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MucG4Geo : public SubDetectorG4Geo
{
 public:
  
  /// Constructor.
  MucG4Geo();

  /// Destructor.
  ~MucG4Geo();

  /// Get a pointer to the single instance of MucG4Geo
  static MucG4Geo *Instance();

  /// Initialize the instance of G4Geo.
  void InitFromGdml( const char *gdmlFile, const char *setupName );
  
  /// Set default visual attributes;
  void SetDefaultVis();

 private:

  // Pointer to the instance of MucG4Geo.
  static MucG4Geo *m_pMucG4Geo;
};

#endif   /* MUC_G4_GEO_H */
