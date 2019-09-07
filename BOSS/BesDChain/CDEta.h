#ifndef CLEODCHAIN_CDETA_H
#define CLEODCHAIN_CDETA_H
// -*- C++ -*-
//
// Package:     <CleoDChain>
// Module:      CDEta
// 
/**\class CDEta CDEta.h CleoDChain/CDEta.h

 Description: <one line class summary>

 Usage:
    <usage>

//petez (2009.2.25): This is a modified copy of BesDChain CDPi0.h
*/

// system include files

// user include files

#include "BesDChain/CDDecay.h"

// forward declarations
class EvtRecEtaToGG;
namespace DecayChain {
   class KinematicData;
}

class CDEta:public CDDecay
{
      // ---------- friend classes and functions ---------------

   public:
      // ---------- constants, enums and typedefs --------------

      // ---------- Constructors and destructor ----------------
      CDEta(const EvtRecEtaToGG* aVisible);

      virtual ~CDEta();

      // ---------- member functions ---------------------------
      
      // ---------- const member functions ---------------------
      virtual bool builtFromCDEta() const ;
      virtual const EvtRecEtaToGG* navEta() const ;

      // ---------- static member functions --------------------

   protected:
      // ---------- protected member functions -----------------

      // ---------- protected const member functions -----------
      virtual DecayChain::KinematicData* defaultKinematicData() const;

   private:
      // ---------- Constructors and destructor ----------------
      //      CDEta( const CDEta& ); // stop default

      // ---------- assignment operator(s) ---------------------
      const CDEta& operator=( const CDEta& ); // stop default

      // ---------- private member functions -------------------

      // ---------- private const member functions -------------

      // ---------- data members -------------------------------
      const EvtRecEtaToGG* m_eta;

      // ---------- static data members ------------------------

};

// inline function definitions

// Uncomment the following lines, if your class is templated 
// and has an implementation file (in the Template directory)
//#if defined(INCLUDE_TEMPLATE_DEFINITIONS)
//# include "CleoDChain/Template/CDEta.cc"
//#endif

#endif /* CLEODCHAIN_CDETA_H */
