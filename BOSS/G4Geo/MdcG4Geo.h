//$id$
/*
 *    2005/10/31   Zhengyun You      Peking University
 *    2007/05/23   Yuan Ye           IHEP
 *                 Mdc Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

#ifndef MDC_G4_GEO_H
#define MDC_G4_GEO_H

#include "SubDetectorG4Geo.h"

/**
 * Class MdcGeo contains all of the objects necessary to describe the
 * Mdc geometry by GDML.
 */

class MdcG4Geo : public SubDetectorG4Geo
{
 public:
  
  /// Constructor.
  MdcG4Geo();

  /// Destructor.
  ~MdcG4Geo();

  /// Initialize the instance of G4Geo.
  void InitFromGdml( const char *gdmlFile, const char *setupName );
  
  /// Set default visual attributes;
  void SetDefaultVis();

 private:

};

#endif   /* MDC_G4_GEO_H */
