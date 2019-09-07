//$id$
/*
 *    2005/10/31   Zhengyun You      Peking University
 *                 Tof Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

#ifndef TOF_G4_GEO_H
#define TOF_G4_GEO_H

#include "SubDetectorG4Geo.h"

/**
 * Class TofGeo contains all of the objects necessary to describe the
 * tof geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class TofG4Geo : public SubDetectorG4Geo
{
 public:
  
  /// Constructor.
  TofG4Geo();

  /// Destructor.
  ~TofG4Geo();

  /// Initialize the instance of G4Geo.
  void InitFromGdml( const char *gdmlFile, const char *setupName );
  
  /// Set default visual attributes;
  void SetDefaultVis();

 private:

};

#endif   /* TOF_G4_GEO_H */
