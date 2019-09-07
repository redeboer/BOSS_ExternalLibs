//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkContextEv.cxx,v 1.2 2006/12/01 01:10:32 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

//#include "BaBar/BaBar.h"
#include "TrkFitter/TrkContextEv.h"
//#include "AbsEnv/AbsEnv.h"
//#include "AbsEvent/AbsEvent.h"
//#include "ProxyDict/Ifd.h"
#include "TrkFitter/TrkIdManDefault.h"
#include "TrkFitter/TrkIdManDumb.h"
#include "TrkBase/TrkId.h"
//#include "ErrLogger/ErrLog.h"
//#include "TrkEnv/TrkEnv.h"
//#include "AbsEnv/AbsEnv.h"
#include "BField/BField.h"

//------------------------------------------------------------------------
TrkContextEv::TrkContextEv(const BField *bf) : 
  TrkContext(bf)
//------------------------------------------------------------------------
{
  TrkIdManager* manTmp;
//  if (anEvent == 0) {
//    manTmp = TrkIdManDumb::instance();//yzhang temp
/*  }
  else {
    // Default Id manager
    TrkIdManager* manDef = 
      Ifd< TrkIdManager >::get( anEvent, IfdStrKey("Default") );
    if (0 == manDef) {
      manDef = new TrkIdManDefault(1);
      IfdDataProxy<TrkIdManager>* theProxy = 
    	new IfdDataProxy<TrkIdManager>(manDef);
      if (!(Ifd<TrkIdManager>::put(anEvent,theProxy,IfdStrKey("Default")))){
    	ErrMsg(error) << "TrkContextEv: Oops - couldn't put TrkIdManager"
		      << endmsg;
      }
    }
    manTmp = manDef;
  }*/
  //yzhang
  TrkIdManDefault* manDef = new TrkIdManDefault(0);
  manTmp = dynamic_cast<TrkIdManager*> (manDef);
  //zhangy
  _idman = manTmp;
}

//------------------------------------------------------------------------
TrkContextEv::~TrkContextEv() {
//------------------------------------------------------------------------
  delete _idman;
}


//------------------------------------------------------------------------
TrkId
TrkContextEv::getId() const {
//------------------------------------------------------------------------
  return TrkId(idManager());
}

