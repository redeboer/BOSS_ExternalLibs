//
//  Package: TofRec
//  BESIII Tof Reconstruction Algorithm
//  Created by Sun Shengsen (EPC IHEP)
//
#ifndef TOF_REC_A_H
#define TOF_REC_A_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "TofRec/TofCheckDigi.h"
#include "TofRec/TofCheckData.h"
#include "TofRec/TofTrack.h"
#include "TofRec/TofCount.h"
#include <string>

class TofRec:public Algorithm {
 public:
   TofRec(const std::string& name, ISvcLocator* pSvcLocator);
   StatusCode initialize();
   StatusCode beginRun();   
   StatusCode execute();
   StatusCode finalize();

   void clearTofTrackVec( std::vector<TofTrack*>*& tofTrackVec );

 private:

// switchs
   std::string m_acceleratorStatus;  // swich controls Colliding data or Cosmic Ray
   bool m_magneticField;    // switch controls With or Without magneticfield
   bool m_forCalibration;   // switch cnotrols for calibration or not
   std::string m_data;      // jpsi, psip or psipp
   std::string m_calibData; // bhabha or dimu
   //   bool m_calibDataBarrel;  // barrel or endcap
   bool m_firstIteration;   // switch controls which t0 would be used
   bool m_checkTrigger;     // switch controls fill trigger information
   bool m_saveRootFile;     // switch controls ntuple
   bool m_printOutInfo;     // switch controls print out information
   bool m_checkDigi;        // switch controls check digi
   bool m_checkDigiRaw;
   bool m_checkDigiExt;
   bool m_checkMcTruth;     // switch controls check MC Truth information

   TofCheckDigi* m_checkdigi_tuple;
   NTuple::Tuple*  m_tuple_digi;
   NTuple::Tuple*  m_tuple_raw;
   NTuple::Tuple*  m_tuple_barrel;
   NTuple::Tuple*  m_tuple_endcap;
   NTuple::Tuple*  m_tuple_mrpc;
   NTuple::Tuple*  m_tuple_ext;
   NTuple::Tuple*  m_tuple_tof;
   NTuple::Tuple*  m_tuple_tdiff;
   NTuple::Tuple*  m_tuple_bb;

   TofCheckData* m_checkdata_tuple;
   NTuple::Tuple*  m_tuple_trk;
   NTuple::Tuple*  m_tuple_cbtrk;
   NTuple::Tuple*  m_tuple_cetrk;
   NTuple::Tuple*  m_tuple_cetftrk;

   TofCount* m_printOut;

};
#endif // Tof_REC_A_H
