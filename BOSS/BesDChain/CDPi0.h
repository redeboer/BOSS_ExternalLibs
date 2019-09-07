#ifndef CLEODCHAIN_CDPI0_H
#define CLEODCHAIN_CDPI0_H
// -*- C++ -*-
//
// Package:     <CleoDChain>
// Module:      CDPi0
// 
/**\class CDPi0 CDPi0.h CleoDChain/CDPi0.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Author:      David Urner
// Created:     Thu Mar 15 15:41:34 EST 2001
// $Id: CDPi0.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDPi0.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.3  2004/03/05 22:01:51  chengp
// implemented Monte Carlo matching
//
// Revision 1.2  2001/11/06 15:20:52  ajm36
// override defaultKinematicData for pi0, eta, ks, lambda
//
// Revision 1.1  2001/04/11 13:19:21  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/24 03:08:00  cdj
// fixed CDPi0 CDKs and CDEta classes so could properly assess info from CDCandidate class
//
// Revision 1.1  2001/03/23 23:05:37  urner
// added pi0 eta and CDKs decay lists
//

// system include files

// user include files

#include "BesDChain/CDDecay.h"

// forward declarations
class EvtRecPi0;
namespace DecayChain {
   class KinematicData;
}

class CDPi0:public CDDecay
{
      // ---------- friend classes and functions ---------------

   public:
      // ---------- constants, enums and typedefs --------------

      // ---------- Constructors and destructor ----------------
      CDPi0(const EvtRecPi0* aVisible);
      virtual ~CDPi0();

      // ---------- member functions ---------------------------
      
      // ---------- const member functions ---------------------
      virtual bool builtFromCDPi0() const ;
      virtual const EvtRecPi0* navPi0() const ;

      // ---------- static member functions --------------------

   protected:
      // ---------- protected member functions -----------------

      // ---------- protected const member functions -----------
      virtual DecayChain::KinematicData* defaultKinematicData() const;

   private:
      // ---------- Constructors and destructor ----------------
      //      CDPi0( const CDPi0& ); // stop default

      // ---------- assignment operator(s) ---------------------
      const CDPi0& operator=( const CDPi0& ); // stop default

      // ---------- private member functions -------------------

      // ---------- private const member functions -------------

      // ---------- data members -------------------------------
      const EvtRecPi0* m_pi0;

      // ---------- static data members ------------------------

};

// inline function definitions

// Uncomment the following lines, if your class is templated 
// and has an implementation file (in the Template directory)
//#if defined(INCLUDE_TEMPLATE_DEFINITIONS)
//# include "CleoDChain/Template/CDPi0.cc"
//#endif

#endif /* CLEODCHAIN_CDPI0_H */
