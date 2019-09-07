// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/CalibModel.h,v 1.16 2011/12/20 00:10:29 zhangy Exp $

#ifndef CalibData_CalibModel_h
#define CalibData_CalibModel_h

/**
       @file CalibModel.h
 Definition of strings holding paths to calibration data objects
 in the TDDS (transient detector data store)
   @author : adapted from similar structures for TDS
   @author   J. Bogart
 */ 

#include <string>
#include <vector>
#include "GaudiKernel/ClassID.h"


// Start class ids at 6000 to stay well away from Gaudi classes and
// our TDS event classes.  Layout could be
//     6000 - 6099        internals
//     6100 - 6199        tracker
//     6200 - 6299        calorimeter
//     6300 - 6399        ACD
//     6400 - 6499        this space intentionally left blank in case
//                        we have calibrations spanning subsystems
//     6500 - 6599        test 
//     6600 - 6699        NAS: anything not associated with a single subsystem
//     6400 - 6499        MDC              

const CLID CLID_Calib_CalibCLIDNode     = 6000;

const CLID CLID_Calib_CalibBase         = 6001;

const CLID CLID_Calib_CalibTest1       = 6500;

const CLID CLID_Calib_NAS_TowerCfg     = 6600;


//MDC CLID
const CLID CLID_Calib_MdcCal      = 6405;
//TOF CLID
const CLID CLID_Calib_TofCal      = 6406;
//Dedx CLID
const CLID CLID_Calib_DedxCal     = 6407;
//EMC CLID
const CLID CLID_Calib_EmcCal      = 6408;
//MUC CLID
const CLID CLID_Calib_MucCal      = 6409;
//EsTime CLID
const CLID CLID_Calib_EsTimeCal   = 6410;
//EstTOF CLID
const CLID CLID_Calib_EstTofCal   = 6411;
//Mdc Alignment
const CLID CLID_Calib_MdcAlign    = 6412;
//Tof Elec
const CLID CLID_TofQ_Elec         = 6413;
//TOF Simulation CLID
const CLID CLID_Calib_TofSim      = 6414;
//Dedx Simulation
const CLID CLID_Dedx_Sim          = 6415;
//Mdc Data Const
const CLID CLID_Calib_MdcDataConst= 6416;

// For everybody except the CalibModel class implementation file,
// the variables are extern.  CalibModel.cxx actually defines them.

#if defined(_CalibData_CalibModel_cxx)
#define  _EXTERN_ 
#else
#define  _EXTERN_ extern
#endif

    namespace CalibData {
    

      //   ... simple type whose "data" just come from MySQL metadata row
      //       for testing
      _EXTERN_ std::string   Test_Gen;

      // Simple xml test type
      _EXTERN_ std::string   Test_1;
      //MDC
      _EXTERN_ std::string   MdcCal;
      //Tof
      _EXTERN_ std::string   TofCal;
      //Dedx
      _EXTERN_ std::string   DedxCal;
      //Emc
      _EXTERN_ std::string   EmcCal;
      //MUC
      _EXTERN_ std::string   MucCal;
      //EsTime
      _EXTERN_ std::string   EsTimeCal;
      //EstTof
      _EXTERN_ std::string   EstTofCal;
      //MdcAlign
      _EXTERN_ std::string   MdcAlign;
      //Tof elec
      _EXTERN_ std::string   TofQElec;
      //Tof simulation
      _EXTERN_ std::string   TofSim;
      //Dedx simulation
      _EXTERN_ std::string   DedxSim;
      //MdcDataConstant
      _EXTERN_ std::string   MdcDataConstant;
      // cross-subsystem types
      _EXTERN_ std::string   NAS_TowerCfg;
      typedef  std::vector<std::pair <std::string, CLID> > CalibPairCol;
      typedef  CalibPairCol::const_iterator PairIt;
      _EXTERN_    CalibPairCol pairs;
    }
    

#undef _EXTERN_
#endif 
