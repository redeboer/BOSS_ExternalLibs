// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/CalibModel.cxx,v 1.17 2011/12/20 00:10:29 zhangy Exp $

#define _CalibData_CalibModel_cxx

#include "CalibData/CalibModel.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ClassID.h"

/**  @file CalibModel.cxx 
     Implementation for CalibModel class, which initializes strings
     for use as paths for TDDS data.  Also includes definitions
     of Gaudi class ids for TDDS DataObjects.
*/

/**  @class CalibModel
     Provides convenience definitions of strings which are paths
     to calibration data in the TDDS.  The class has no data members.
     The strings it initializes are just "there"; existing in 
     this module and extern everywhere else (see the fancy footwork
     in CalibModel.h).
     The single static instance of CalibModel declared in this file
     causes the constructor to be run, initializing the strings, and
     a vector of pairs.  The pairs are used to match each string
     with the appropriate Gaudi class id.

     NOTE:  This scheme might have to be revised.  We haven't dealt
            with different instruments, nor with flavors.  Assuming,
            as seems likely, that we *never* have to keep track of
            constants for more than one instrument simultaneously 
            (most likely not even sequentially within a single job),
            we can leave any mention of instrument out of these
            paths.  If we want to preserve the possibility of maintaining
            more than one flavor simultaneously, flavor will have to
            be part of the path string.  We could tack it on the end
            dynamically without having to do any violence to what
            we've got below.
*/

std::string root;
std::string tkr;
std::string cal;
std::string acd;
std::string test;
std::string nas;
std::string mdc;


class CalibModel {
public:
  /** The constructor sets values into the externally-accessible
      string variables */
  CalibModel () {
    // Initialize a bunch of strings here
    // First few are just for convenience in assembling the    
    // the rest; no need for them to be public.
    std::string root = "/Calib";
    std::string test = root + "/Test";
    std::string nas = root + "/NAS";
    std::string Mdc = root + "/Mdc";
    std::string Tof = root + "/Tof";
    std::string Dedx = root + "/Dedx";
    std::string Emc = root + "/Emc";
    std::string Muc = root + "/Muc";
    std::string EsTime = root + "/EsTime";
    std::string EstTof = root + "/EstTof";
    // Practically all of these don't exist in any shape or form,
    // but go ahead an reserve names anyway.


    CalibData::Test_Gen = test + "_Gen";
    CalibData::Test_1   = test + "_1";
  
    //the Mdc nodes
    
    CalibData::MdcCal = Mdc + "Cal";
    CalibData::TofCal = Tof + "Cal";
    CalibData::DedxCal = Dedx + "Cal";
    CalibData::EmcCal = Emc + "Cal";
    CalibData::MucCal = Muc + "Cal";
    CalibData::EsTimeCal = EsTime + "Cal";
    CalibData::EstTofCal = EstTof + "Cal";
    CalibData::MdcAlign = Mdc+"Align";
    CalibData::TofQElec="/Calib/TofQElec";
    CalibData::TofSim="/Calib/TofSim";
    CalibData::DedxSim="/Calib/DedxSim";
    CalibData::MdcDataConstant = Mdc+"DataConst";

    // Use same class for hot strips, dead strips or (merged) bad strips,
    // but different path in TDDS

    CalibData::pairs.push_back(std::make_pair(CalibData::MdcCal,
                                              CLID_Calib_MdcCal));
    CalibData::pairs.push_back(std::make_pair(CalibData::TofCal,
                                              CLID_Calib_TofCal));
    CalibData::pairs.push_back(std::make_pair(CalibData::DedxCal,
                                              CLID_Calib_DedxCal));
    CalibData::pairs.push_back(std::make_pair(CalibData::EmcCal,
                                              CLID_Calib_EmcCal));
    CalibData::pairs.push_back(std::make_pair(CalibData::MucCal,
                                              CLID_Calib_MucCal));
    CalibData::pairs.push_back(std::make_pair(CalibData::EsTimeCal,
                                              CLID_Calib_EsTimeCal));     
    CalibData::pairs.push_back(std::make_pair(CalibData::EstTofCal,
                                              CLID_Calib_EstTofCal));
    CalibData::pairs.push_back(std::make_pair(CalibData::MdcAlign,
                                              CLID_Calib_MdcAlign));
    CalibData::pairs.push_back(std::make_pair(CalibData::TofQElec,
                                              CLID_TofQ_Elec));
    CalibData::pairs.push_back(std::make_pair(CalibData::TofSim,
                                              CLID_Calib_TofSim));
    CalibData::pairs.push_back(std::make_pair(CalibData::DedxSim,
                                              CLID_Dedx_Sim));
    CalibData::pairs.push_back(std::make_pair(CalibData::MdcDataConstant,
                                              CLID_Calib_MdcDataConst));

   
  }

};       // end of calibModel class definition

// Instantiate an instance to get the ball rolling.
static CalibModel mod;


#undef _CalibData_CalibModel_cxx
