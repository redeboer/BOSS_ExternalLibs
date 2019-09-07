//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//
//
// Copyright Information:
//
//------------------------------------------------------------------------

#ifndef BESGENMODULE_H
#define BESGENMODULE_H
#include "GeneratorModule/GenModule.h"
#include <string>

using std::string;
typedef std::vector<std::string> CommandVector;

class IBesRndmGenSvc;

class BesGenModule:public GenModule {
public:
  BesGenModule(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~BesGenModule();
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(GenEvent* evt);
//  StatusCode writeHepevt();
 
  // Pointer On BesRndmGenSvc
  IBesRndmGenSvc* 	p_BesRndmGenSvc;
  static std::string	besMcGen_stream;

protected:
  // Setable Properties:-

  //BesGenModule output file
  int m_fortout;

  // Event seeds
  std::vector<long int> m_seeds;

  const char* cardname;
  std::string runname;
  int Nr;
  int mvec[10];
  int nvec[10];

  std::vector<int> m_par;

  // event counter
  int m_events ;
  int n;
  int n1;

  int m_GenIndex;
  
  int m_SuperIndex;

  int m_SubIndex;

  // besmcgen parameters
  // Ffphot m_ffphot;

  // io (LUN-s) for besmcgen
  // Ffpholun m_ffpholun;
  
  // Commands to setup besmcgen
//yugw  CommandVector m_BesMcGenCommandVector;
  
  // I/O to HEPEVT
  // StatusCode restore_Atlas_HEPEVT (void);

  // static Atlas_HEPEVT* atlas_HEPEVT;

};

#endif

