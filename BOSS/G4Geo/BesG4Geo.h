//$id$
/*
 *    2007/05/28   Deng Ziyan 
 *                 Bes Geometry General for Simulation
 *                 inherit from class SubDetectorG4Geo
 */

#ifndef BES_G4_GEO_H
#define BES_G4_GEO_H

#include "SubDetectorG4Geo.h"

/**
 * Class BesGeo contains all of the objects necessary to describe the
 * tof geometry.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class BesG4Geo : public SubDetectorG4Geo
{
 public:
  
  /// Constructor.
  BesG4Geo();

  /// Destructor.
  ~BesG4Geo();

  /// Initialize the instance of G4Geo.
  void InitFromGdml( const char *gdmlFile, const char *setupName );
  
  /// Set default visual attributes;
  void SetDefaultVis();

 private:

};

#endif   /* BES_G4_GEO_H */
