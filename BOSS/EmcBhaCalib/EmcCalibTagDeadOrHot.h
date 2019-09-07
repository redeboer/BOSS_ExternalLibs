//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//	Copyright (C) 2005              IHEP
//
//------------------------------------------------------------------------

#ifndef EMCCALIBTAGDEADORHOT_H
#define EMCCALIBTAGDEADORHOT_H

//-------------
// C Headers --
//-------------
#include <ctime>

#include <cstring>


#include "GaudiKernel/Algorithm.h"

#include "EmcCalibConstSvc/IEmcCalibConstSvc.h"
#include "EmcCalibConstSvc/EmcCalibConstSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

//Ntuple
#include "GaudiKernel/NTuple.h"

using namespace std;


class EmcCalibTagDeadOrHot:public Algorithm{

  
 public:
  
  EmcCalibTagDeadOrHot(const std::string& name, ISvcLocator* pSvcLocator);


  //--------------
  // Destructor
  //--------------
  virtual ~EmcCalibTagDeadOrHot(); 
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:


  // read in calibration constants from file or initialize them to 1   
  void initCalibConst();

  void SetDeadOrHotChannel();

private:

  //cut on minimum number of direct hits needed per xtal  
  int m_deadIxtal;
  int m_hotIxtal;

  //file name extensions for different output files 
  std::string m_fileExt;

  //file directory 
  std::string m_fileDir;

  //the old absolute constants the new once are relative to 
  double* m_oldConstants;
    int* m_IxtalNumber;
   IEmcCalibConstSvc* m_emcCalibConstSvc;

};

#endif // EMCCALIBTAGDEADORHOT_H



















