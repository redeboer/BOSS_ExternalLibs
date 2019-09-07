#ifndef CLEODCHAIN_CDLAMBDA_H
#define CLEODCHAIN_CDLAMBDA_H
// -*- C++ -*-
//
// Package:     <CleoDChain>
// Module:      CDLambda
// 
/**\class CDLambda CDLambda.h CleoDChain/CDLambda.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Author:      David Urner
// Created:     Thu Mar 15 15:41:34 EST 2001
// $Id: CDLambda.h,v 1.1 2009/06/22 14:55:48 zoujh Exp $
//
// Revision history
//
// $Log: CDLambda.h,v $
// Revision 1.1  2009/06/22 14:55:48  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.2  2001/11/06 15:20:52  ajm36
// override defaultKinematicData for pi0, eta, ks, lambda
//
// Revision 1.1  2001/09/11 15:08:30  ajm36
// add files for lambdas
//
// Revision 1.1  2001/04/11 13:19:17  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2001/03/24 03:08:00  cdj
// fixed CDPi0 CDLambda and CDEta classes so could properly assess info from CDCandidate class
//
// Revision 1.1  2001/03/23 23:05:36  urner
// added pi0 eta and CDLambda decay lists
//

// system include files

// user include files

#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "BesDChain/CDDecay.h"

// forward declarations
namespace DecayChain {
      class KinematicData;
}

class CDLambda:public CDDecay
{
      // ---------- friend classes and functions ---------------

   public:
      // ---------- constants, enums and typedefs --------------

      // ---------- Constructors and destructor ----------------
      CDLambda(const EvtRecVeeVertex* aVisible);

      virtual ~CDLambda();

      // ---------- member functions ---------------------------
      
      // ---------- const member functions ---------------------
      //const VXFitVeeLambda& lambda() const {return m_lambda.lambda();}

      virtual bool builtFromCDLambda() const ;
      virtual const EvtRecVeeVertex* navLambda() const ;

      bool isBar() const { return (m_lambda->vertexId() < 0); }

      // ---------- static member functions --------------------

   protected:
      // ---------- protected member functions -----------------

      // ---------- protected const member functions -----------
      virtual DecayChain::KinematicData* defaultKinematicData() const;

   private:
      // ---------- Constructors and destructor ----------------
      //      CDLambda( const CDLambda& ); // stop default

      // ---------- assignment operator(s) ---------------------
      const CDLambda& operator=( const CDLambda& ); // stop default

      // ---------- private member functions -------------------

      // ---------- private const member functions -------------

      // ---------- data members -------------------------------
      const EvtRecVeeVertex* m_lambda;

      // ---------- static data members ------------------------

};

// inline function definitions

// Uncomment the following lines, if your class is templated 
// and has an implementation file (in the Template directory)
//#if defined(INCLUDE_TEMPLATE_DEFINITIONS)
//# include "CleoDChain/Template/CDLambda.cc"
//#endif

#endif /* CLEODCHAIN_CDLAMBDA_H */
