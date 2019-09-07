//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TrkTypeUtil.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//	Class TrkTypeUtil
//
// Environment:
//	Software developed for BaBar expirment @ SLAC B-Factory
//
// Author List:
//	Eric A Charles
//
// Copyright Information:
//	Copyright (C) 1998	Univ. Wisconsin-Madison
//
//------------------------------------------------------------------------

//----------------
// BaBar header
//----------------
//#include "BaBar/BaBar.hh"

//-----------------------
// This Class's Header --
//-----------------------
#include "TrkBase/TrkTypeUtil.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkRep.h"
#include "TrkBase/TrkFitTypeKey.h"
#include "TrkBase/TrkHitList.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------



//		-----------------------------------------------
// 		-- Static Data & Function Member Definitions --
//		-----------------------------------------------

const TrkRep* 
TrkTypeUtil::getRep( const TrkRecoTrk& aTrk, 
		     const PdtPid::PidType& pid){
    return pid != PdtPid::null ? aTrk.testRep(pid) 
      : aTrk.testRep(aTrk.defaultType());
  }

const TrkHitList* 
TrkTypeUtil::getHits( const TrkRecoTrk& aTrk, 
		      const PdtPid::PidType& pid){
  return pid != PdtPid::null ? aTrk.hits(pid) 
    : aTrk.hits(aTrk.defaultType());
}

const TrkRep* 
TrkTypeUtil::getRep( const TrkRecoTrk& aTrk,
		     const TrkFitTypeKey& key ){
  return getRep( aTrk, key.pidType() );
}

const TrkHitList* 
TrkTypeUtil::getHits( const TrkRecoTrk& aTrk,
		      const TrkFitTypeKey& key ){
  return getHits( aTrk, key.pidType() );
}

PdtPid::PidType 
TrkTypeUtil::pidType( const TrkRecoTrk& aTrk, 
		      const TrkFitTypeKey& key ){
  return key.pidType() != PdtPid::null ? key.pidType()
    : aTrk.defaultType();
}

