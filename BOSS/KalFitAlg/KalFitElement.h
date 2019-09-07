//-----------------------------------------------------------------------
// File from KalFit module
//
// Filename : KalFitElement.h
//------------------------------------------------------------------------
// Description :
// Element is a base class which represents an element of detector
//
//------------------------------------------------------------------------
// Modif :
//------------------------------------------------------------------------
#ifndef _DEFINE_ELEMENT_H_
#define _DEFINE_ELEMENT_H_

#include "KalFitAlg/lpav/Lpav.h" // Use for first circle fit....

class KalFitMaterial;
class KalFitTrack;


/**
 * Element is a base class which represents an element of detector 
 */
class KalFitElement {
protected:

  /// Material of this element
  const KalFitMaterial* material_;

  /// Flags
  static int muls_;
  static int loss_;

public:
  /// Constructor
  KalFitElement(const KalFitMaterial* material)
    : material_(material)
  {}
  
  /// Destructor
  virtual ~KalFitElement(){}

  /// Find intersection with Helix
  virtual double intersect(const KalFitTrack& track,
			   HepPoint3D& x) const = 0;


  virtual double intersect(const KalFitTrack& track,
                           HepPoint3D& x, const HepPoint3D& point) const = 0;    

  /// Return Material of which Element is made
  const KalFitMaterial& material(void) const
  {
    return *material_;
  }

  /// Return distance in r-phi plane
  virtual double radius(void) const = 0;

  /// Update track according the material properties of the current element
  virtual void updateTrack(KalFitTrack& track, int index);
  virtual void updateTrack_rphi(KalFitTrack& track, int index);

  virtual void updateTrack_alreadyfound(KalFitTrack& track, 
					int index);
  /// 
  virtual void asso_rphi(Lpav& circ, KalFitTrack& track);
  virtual void asso_rphi(KalFitTrack& track);

  /// Operator
  bool operator < (const KalFitElement& a) const
  {
    return radius() < a.radius();
  }

  static int muls(void);
  static int loss(void);

  static void muls(int i);
  static void loss(int i);
};

#endif
