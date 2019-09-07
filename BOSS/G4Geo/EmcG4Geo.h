//$id$
/*
 *    2007/05/15   Miao He <hem@ihep.ac.cn>
 *                 Emc Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

#ifndef EMC_G4_GEO_H
#define EMC_G4_GEO_H

#include "SubDetectorG4Geo.h"

/**
 * Class EmcGeo contains all of the objects necessary to describe the
 * Emc geometry.
 */

class EmcG4Geo : public SubDetectorG4Geo
{
 public:
  
  /// Constructor.
  EmcG4Geo();

  /// Destructor.
  ~EmcG4Geo();

  /// Get a pointer to the single instance of EmcG4Geo
  static EmcG4Geo *Instance();

  /// Initialize the instance of G4Geo.
  void InitFromGdml( const char *gdmlFile, const char *setupName );
  
  /// Set default visual attributes;
  void SetDefaultVis();

 private:

  // Pointer to the instance of MucG4Geo.
  static EmcG4Geo *m_pEmcG4Geo;
    

};

#endif   /* EMC_G4_GEO_H */
