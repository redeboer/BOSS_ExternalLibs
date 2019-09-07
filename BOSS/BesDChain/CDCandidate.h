#ifndef CLEODCHAIN_CDCANDIDATE_H
#define CLEODCHAIN_CDCANDIDATE_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      CDCandidate
// 
// Description: Basic particle candidate need to use DChain list package
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: CDCandidate.h,v 1.5 2011/10/27 06:15:12 zoujh Exp $
//
// Revision history
//
// $Log: CDCandidate.h,v $
// Revision 1.5  2011/10/27 06:15:12  zoujh
// add user tag to particle data
//
// Revision 1.4  2010/03/29 04:35:45  zoujh
// See ChangeLog
//
// Revision 1.3  2009/09/22 08:24:41  hujf
// see ChangeLog
//
// Revision 1.2  2009/06/22 14:55:48  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.6  2006/01/11 20:37:27  cdj
// work with renaming done in DChain package
//
// Revision 1.5  2004/03/05 22:01:49  chengp
// implemented Monte Carlo matching
//
// Revision 1.4  2003/05/15 19:58:15  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.3  2001/09/12 19:10:36  ajm36
// add lambda functions to CDCandidate
//
// Revision 1.2  2001/04/20 14:03:38  ajm36
// add finalChildren function to return tracks and showers
//
// Revision 1.1  2001/04/11 13:19:10  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.3  2001/03/23 23:05:34  urner
// added pi0 eta and CDKs decay lists
//
// Revision 1.2  2000/12/19 15:09:34  cdj
// removed forward decleration of DBCDPhotonFit
//
// Revision 1.1.1.1  2000/12/18 22:17:25  cdj
// imported CleoDChain
//
// Revision 1.23  1998/05/04 19:10:43  sjp
// Fixed access confusion for kinematicData
//
// Revision 1.22  1998/04/17 18:48:56  sjp
// Modifed to use latest CLEO types
//
// Revision 1.21  1997/09/03 14:58:39  sjp
// Use new report.h and KTKinematicData
//
// Revision 1.20  1997/08/29 17:01:16  sjp
// Modified to handle new Cairn Templated classes
//
// Revision 1.19  1997/08/22 16:18:49  sjp
// New name for access functions
//
// Revision 1.18  1997/08/19 23:01:37  sjp
// Restructured package to be independent of CleoDChain
//
// Revision 1.17  1997/08/19 20:41:29  sjp
// Updated to use <package>/<file>.h include structure.
//   (Note: This version of the code has not been compiled)
//
// Revision 1.16  1997/07/03 17:47:59  sjp
// Cleaned up include files
//
// Revision 1.15  1997/01/21 20:30:21  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.14  1996/12/20 20:58:34  sjp
// New pathname for FoorPrint
//
// Revision 1.13  1996/11/04 17:00:49  sjp
// Separated of CDDecay part of CDCandidate into new class
//

// system include files
#include <vector>

#include "CLHEP/Vector/LorentzVector.h"

// user include files
#include "DecayChain/Element/ReferenceCount.h"  // superclass
#include "BesDChain/DBCandidate.h"
#include "BesDChain/CDFootPrint.h" // CDFootPrint

using CLHEP::HepLorentzVector;
using CLHEP::Hep3Vector;

// forward declarations
class CDDecay ;
class EvtRecTrack;
class EvtRecPi0;
class EvtRecEtaToGG;
class EvtRecVeeVertex;

namespace DecayChain {
   class KinematicData;
}

using std::vector;

class CDCandidate : public dchain::ReferenceCount
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef CDDecay DecayEvidence ;
      typedef std::pair< vector<const EvtRecTrack*>, vector<const EvtRecTrack*> >
	 TracksAndShowers;

      // Constructors and destructor
      virtual ~CDCandidate() ;

      // assignment operator(s)
      const CDCandidate& operator=( const CDCandidate& aOtherCDCandidate ) ;

      // member functions
      void   setUserTag(int tag);
      CDCandidate& setP4( const HepLorentzVector& aMomentum ) ;

      // const member functions
      int    userTag() const;
      double mass() const ;
      int    charge() const ;
      double energy() const ;
      const  Hep3Vector& momentum() const;
      const  HepLorentzVector& p4() const;
      const  DecayChain::KinematicData* kinematicData() const;
#ifdef EXTEND
      //for recover 4p after kinematic fit
      void recover() const ;
#endif

      TracksAndShowers finalChildren() const;

      inline virtual DBCandidate::Hypo hypo() const;

      virtual bool  builtFromTrack() const ;
      virtual const EvtRecTrack* track() const ;
      virtual bool  builtFromCDPhoton() const ;
      virtual const EvtRecTrack* photon() const ;
      virtual bool  builtFromCDPi0() const ;
      virtual const EvtRecPi0* navPi0() const ;
      virtual bool  builtFromCDEta() const ;
      virtual const EvtRecEtaToGG* navEta() const ;
      virtual bool  builtFromCDKs() const ;
      virtual const EvtRecVeeVertex* navKshort() const ;
      virtual bool  builtFromCDLambda() const ;
      virtual const EvtRecVeeVertex* navLambda() const ;
      virtual bool  builtFromCDDecay() const ;
      virtual const DecayEvidence& decay() const ;

      //
      bool  overlap( const CDCandidate& aOtherCDCandidate ) const ;
      const CDFootPrint& footPrint() const ;

      // static member functions

   protected:
      // Constructors and destructor
      CDCandidate( const CDCandidate& aOtherCDCandidate ) ;
      CDCandidate( const CDFootPrint& aCDFootPrint ) ;

      // protected member functions
      void setKinematicData( const DecayChain::KinematicData& aKinematicData ) ;
      void setCDFootPrint( const CDFootPrint& aCDFootPrint ) ;

      virtual DecayChain::KinematicData* defaultKinematicData() const = 0 ;

      void recurseNode(TracksAndShowers& final, const CDCandidate& cand) const;

      // protected data members

   private:
      // Constructors and destructor
      CDCandidate() ; // stop default
      // private member functions
      DecayChain::KinematicData* modifiableKinematicData() ;

      // data members
      DecayChain::KinematicData*  m_kinematicDataPtr;

      CDFootPrint m_footPrint ;

} ;

typedef const CDCandidate& FixedCDCandidate ;

// inline function definitions
inline DBCandidate::Hypo CDCandidate::hypo() const
{
    return DBCandidate::kMaxHypo;
}

#endif /* CLEODCHAIN_CDCANDIDATE_H */
