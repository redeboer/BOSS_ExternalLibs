//-----------------------------------------------------------------------
// File from KalFit module
//
// Filename : KalFitMaterial.h
//------------------------------------------------------------------------
// Description : 
// Material is a class which describes the properties of a given 
// material, for instance atomic number, atomic weight and so on.
//
//------------------------------------------------------------------------
// Modif :
//------------------------------------------------------------------------
#ifndef _DEFINE_MATERIAL_H_
#define _DEFINE_MATERIAL_H_

/** Material is a class which describes the properties of a given 
 * material, for instance atomic number, atomic weight and so on.
 */
class KalFitMaterial {
  double rza_;  // density * (atomic number) / weight
  double isq_;  // mean excitation potential ** 2
  double x0_;   // radiation length
  double z_;   

public:
  /// Constructor
  KalFitMaterial() {}
  KalFitMaterial(double z, double a, double i,
	       double rho, double x0);
  KalFitMaterial( const KalFitMaterial& mat); 
  /// Calculate energy loss 
  double dE(double mass, double path, double p) const;

  /// Calculate Multiple Scattering angle
  double mcs_angle(double mass, double path, double p) const;

  /// Calculate the straggling of energy loss
  double del_E(double mass, double path, double p) const;

  /// Extractor
  double X0(void) const { return x0_; }

};

#endif







