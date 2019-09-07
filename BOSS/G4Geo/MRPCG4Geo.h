//$id$
/*
 *    2010/08/15   Matthias Ullrich  Gießen University
 *                 Tof Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

#ifndef MRPC_G4_GEO_H
#define MRPC_G4_GEO_H

#include "SubDetectorG4Geo.h"

/**
 * Class TofGeo contains all of the objects necessary to describe the
 * tof geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MRPCG4Geo : public SubDetectorG4Geo
{
 public:
  
  /// Constructor.
  MRPCG4Geo();

  /// Destructor.
  ~MRPCG4Geo();

  /// Initialize the instance of G4Geo.
  void InitFromGdml( const char *gdmlFile, const char *setupName );
  
  /// Set default visual attributes;
  void SetDefaultVis();

 private:

};

#endif   /* MRPC_G4_GEO_H */
