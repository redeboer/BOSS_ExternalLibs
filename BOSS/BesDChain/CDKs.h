#ifndef CLEODCHAIN_CDKS_H
#define CLEODCHAIN_CDKS_H
// -*- C++ -*-
//
// Package:     <CleoDChain>
// Module:      CDKs
// 
/**\class CDKs CDKs.h CleoDChain/CDKs.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Author:      David Urner
// Created:     Thu Mar 15 15:41:34 EST 2001
// $Id: CDKs.h,v 1.2 2009/06/22 14:55:48 zoujh Exp $
//
// Revision history
//
// $Log: CDKs.h,v $
// Revision 1.2  2009/06/22 14:55:48  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.3  2004/03/05 22:01:50  chengp
// implemented Monte Carlo matching
//
// Revision 1.2  2001/11/06 15:20:52  ajm36
// override defaultKinematicData for pi0, eta, ks, lambda
//
// Revision 1.1  2001/04/11 13:19:17  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/24 03:08:00  cdj
// fixed CDPi0 CDKs and CDEta classes so could properly assess info from CDCandidate class
//
// Revision 1.1  2001/03/23 23:05:36  urner
// added pi0 eta and CDKs decay lists
//

// system include files

// user include files

#include "BesDChain/CDDecay.h"

// forward declarations
class EvtRecVeeVertex;
namespace DecayChain {
   class KinematicData;
}


class CDKs : public CDDecay
{
      // ---------- friend classes and functions ---------------

   public:
      // ---------- constants, enums and typedefs --------------

      // ---------- Constructors and destructor ----------------
      CDKs(const EvtRecVeeVertex* aVisible);

      virtual ~CDKs();

      // ---------- member functions ---------------------------
      
      // ---------- const member functions ---------------------
      //const VXFitVeeKShort& ks() const {return m_ks.kShort();}

      virtual bool builtFromCDKs() const ;
      virtual const EvtRecVeeVertex* navKshort() const ;

      // ---------- static member functions --------------------

   protected:
      // ---------- protected member functions -----------------

      // ---------- protected const member functions -----------
      virtual DecayChain::KinematicData* defaultKinematicData() const;

   private:
      // ---------- Constructors and destructor ----------------
      //      CDKs( const CDKs& ); // stop default

      // ---------- assignment operator(s) ---------------------
      const CDKs& operator=( const CDKs& ); // stop default

      // ---------- private member functions -------------------

      // ---------- private const member functions -------------

      // ---------- data members -------------------------------
      const EvtRecVeeVertex* m_ks;

      // ---------- static data members ------------------------

};

// inline function definitions

// Uncomment the following lines, if your class is templated 
// and has an implementation file (in the Template directory)
//#if defined(INCLUDE_TEMPLATE_DEFINITIONS)
//# include "CleoDChain/Template/CDKs.cc"
//#endif

#endif /* CLEODCHAIN_CDKS_H */
