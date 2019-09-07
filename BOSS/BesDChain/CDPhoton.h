#ifndef CLEODCHAIN_CDPHOTON_H
#define CLEODCHAIN_CDPHOTON_H
// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDPhoton
// 
// Description: CDPhoton candidate built from shower or truth table
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: CDPhoton.h,v 1.2 2010/01/11 08:05:20 zoujh Exp $
//
// Revision history
//
// $Log: CDPhoton.h,v $
// Revision 1.2  2010/01/11 08:05:20  zoujh
// Add setIP for vertex correction
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.2  2004/03/05 22:01:50  chengp
// implemented Monte Carlo matching
//
// Revision 1.1  2001/04/11 13:19:20  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/23 23:05:37  urner
// added pi0 eta and CDKs decay lists
//
// Revision 1.1.1.1  2000/12/18 22:17:26  cdj
// imported CleoDChain
//
// Revision 1.13  1998/04/17 18:52:24  sjp
// Modifed to use latest CLEO types
//
// Revision 1.12  1997/12/23 21:54:49  sjp
// Modified package to be completely templated
//
// Revision 1.11  1997/09/03 14:58:45  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.10  1997/08/29 17:01:22  sjp
// Modified to handle new CDDecay Templated classes
//
// Revision 1.9  1997/08/19 20:41:39  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.8  1997/01/21 20:30:26  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.7  1996/12/20 21:02:01  sjp
// Extended pathname for Include.
//
// Revision 1.6  1996/11/04 17:01:39  sjp
// Updated to work with new CDCandidate
//

// system include files

// user include files
#include "BesDChain/CDCandidate.h"

// forward declarations
namespace DecayChain {
   class KinematicData;
}

class EvtRecTrack;

class CDPhoton : public CDCandidate
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef CDCandidate CandidateClass ;
      typedef EvtRecTrack VisibleEvidence ;

      // Constructors and destructor
      CDPhoton( const VisibleEvidence* aVisible ) ;
      CDPhoton( const CDPhoton& aOtherCDPhoton ) ;
      virtual ~CDPhoton() ;

      // member functions

      // const member functions
      virtual bool builtFromCDPhoton() const ;
      virtual const EvtRecTrack* photon() const ;
      inline virtual DBCandidate::Hypo hypo() const;

      // static member functions
      //set event IP point
      static bool setIP(double x, double y, double z) {
	 m_IPV.set(x, y, z);
      }

   protected:
      // Constructors and destructor

      // protected member functions

      // protected const member functions
      virtual DecayChain::KinematicData* defaultKinematicData() const;

   private:
      // Constructors and destructor
      CDPhoton() ; // stop default

      // assignment operator(s)
      const CDPhoton& operator=( const CDPhoton& aOtherCDPhoton ) ; // stop default

      // private member functions

      // private const member functions

      // data members
      const VisibleEvidence* m_visibleEvidence ;

      // static data members
      static Hep3Vector m_IPV;  //IP point

} ;

typedef const CDPhoton& FixedCDPhoton ;
typedef CDPhoton& PossibleCDPhoton ;

// inline function definitions
inline DBCandidate::Hypo CDPhoton::hypo() const
{
  return DBCandidate::kPhoton;
}

#endif /* CLEODCHAIN_CDPHOTON_H */
