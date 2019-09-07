//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//	Copyright (C) 2005              IHEP
//
//------------------------------------------------------------------------

#ifndef EMCSELBHAEVENT_H
#define EMCSELBHAEVENT_H

//-------------
// C Headers --
//-------------
#include <ctime>

#include <cstring>
#include<list>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"

//Ntuple
#include "GaudiKernel/NTuple.h"
#include "EmcGeneralClass/EmcStructure.h"
#include "EmcBhaCalib/EmcShower.h"
#include "EmcBhaCalib/EmcShDigi.h"
#include "EmcBhaCalib/EmcBhabhaEvent.h"
#include "EmcBhaCalib/EmcBhabha.h"
#include "EmcBhaCalib/EmcBhaCalibData.h"
#include "EmcBhaCalib/BhabhaType.h"
//#include "EmcBhaCalib/ReadBeamInfFromDb.h"

#include "EmcCalibConstSvc/IEmcCalibConstSvc.h"
#include "EmcCalibConstSvc/EmcCalibConstSvc.h"

#include "BeamEnergySvc/IBeamEnergySvc.h"
#include "BeamEnergySvc/BeamEnergySvc.h"

using namespace std;

class IEmcRecGeoSvc;
class IBeamEnergySvc;
//		---------------------
// 		-- Class Interface --
//		---------------------
//
//  package EmcSelBhaEvent - Select Bhabha events(MCdata) for Emc-digi Calibration
//
//  @author Chunxiu Liu	       (originator/contributor etc.);
//

class EmcSelBhaEvent:public Algorithm {
  
 public:
  
  //selected type
  enum {m_oneProng=1, m_twoProng=2};
  
  EmcSelBhaEvent(const std::string& name, ISvcLocator* pSvcLocator);
  
  
  //--------------
  // Destructor
  //--------------
  ~EmcSelBhaEvent(); 
  
  StatusCode initialize();
  StatusCode execute();  
  StatusCode finalize();
  
  //-----------------
  bool passed() { return m_passed;}
  void setPassed( bool passed) { m_passed = passed;}
  
  int selectedType() const
    {
      return m_selectedType; 
    }

  int selectedTrkID1() const
    {
      return m_selectedTrkID1;
    }

  int selectedTrkID2() const
    {
      return m_selectedTrkID2;
    }

  //-----------------
  // matrix to convert theta,phi <-> index 
  int index(int theta, int phi) const {
    int val = ((m_index)[theta][phi]);
    return (val); }

  // init Emc geometry (convertion matrix theta,phi <-> index) 
  void initGeom(); 

  //--------------
  StatusCode SelectBhabha();
  
  StatusCode SelectFillBhabha();

  void FillBhabha();

  //collect Bhabha event and fill matrix and vector of system of linear equations
  void CollectBhabha();

  //output matrix and vector to files 
  void OutputMV();

  double findPhiDiff( double phi1, double phi2);


  //read correction function f(theta) itheta=0:55
  void readCorFun();

   //read Esigma function sigma(theta) itheta=0:55
  void readEsigma(); 

  // energy deposition function 56(0:55) for shower selection of Bhabha calibration 
  void readDepEneFun();

 // energy resolution function 56(0:55) for shower selection of Bhabha calibration 
  void readSigmaExp();

  void readRawPeakIxtal();

 
  double Angle2ClosestShower( int ShowerID ); 

  private :
    
   // Declare r0, z0 cut for charged tracks
  double m_vr0cut;
  double m_vz0cut;

  double m_lowEnergyShowerCut;
  double m_highEnergyShowerCut;
  double m_matchThetaCut ;
  double m_matchPhiCut ;

  double m_highMomentumCut;
  double m_EoPMaxCut;
  double m_EoPMinCut;
  double m_minAngShEnergyCut;
  double m_minAngCut;
  double m_acolliCut;
  double m_eNormCut;
  double m_pNormCut;
  double m_oneProngMomentumCut;

/**selected event type */
  int m_selectedType;
  int m_selectedTrkID1;
  int m_selectedTrkID2; 

  int m_Nothing; 
  int m_oneProngsSelected; 
  int m_twoProngsMatchedSelected;
  int m_twoProngsOneMatchedSelected;
  
  // calculate the expected energy of a Bhabha depending on theta 
  double expectedEnergy( long int ixtal );  //not been used now
  
  //Correction funtion 56(0:55) parameter
  double m_corFun[56];

  //Esigma 56(0:55) as function of Itheta
  double m_eSigma[56];

  // energy deposition function 56(0:55) for shower selection of Bhabha calibration 
  double m_eDepEne[56];

  double m_eRawMean[6240];
  double m_eRawRMS[6240];

  double m_eRawPeak[6240];
  double m_eMcPeak[6240];

  double m_eDepMean[6240];
  double m_eDepPeak[6240];
  double m_eDepSigma[6240];
  //energy resolution
  double m_eSigmaExp[56];
  //cut off digis further away from the maxima digi
  // of a shower in theta and phi 
  
  int m_digiRangeCut;   //be not used now for MCdata
  
  //low energy shower cut
  double m_ShEneThreshCut;
  
  // high energy shower cut 
  double m_ShEneLeptonCut;

  // cut on minimum number of crystals of a shower in the event 
  int m_minNrXtalsShowerCut;   //be not used now for MCdata

  // cut on maximum number of crystals of a shower in the event 
  int m_maxNrXtalsShowerCut; //be not used now for MCdata

  // cut on minimum difference of the two showers in phi 
  double m_phiDiffMinCut;   //be not used now for MCdata

  // cut on maximum difference of the two showers in phi 
  double m_phiDiffMaxCut;   //be not used now for MCdata

  // cut on number of showers above low energy threshold less this 
  int m_nrShThreshCut;     //be not used now for MCdata

  // cut on ratio measured to expected energy of the event 
  //double m_eNormCut;  //be not used now for MCdata

  // cut on minimum difference of the two showers in theta 
  double m_thetaDiffCut;  //be not used now for MCdata

  double m_LATCut;  //be not used now for MCdata

  // no shower closer than this angle  
  //double m_minAngCut;   //be not used now for MCdata
  
  //number of events passed 
  long int m_events;

  //number of events selected 
  long int m_taken;

  //number of events with  OneProng

  long int m_OneProng;
  //number of events with   TwoProngMatched
  long int m_TwoProngMatched;
  //number of events with   TwoProngOneMatched
  long int m_TwoProngOneMatched;

  //number of events rejected 
  long int m_rejected;

  //number of showers accepted
  long m_showersAccepted;

  // int m_oneProngsSelelected; //be not used now for MCdata
  //int m_twoProngsSelected;  //be not used now for MCdata


  list<EmcShower> m_showerList;
  
  //-------------
  // fill Matrix of system of linear equations 
  void fillMatrix();

   //write the matrix and vector also to file ? 
  bool m_writeMVToFile;
  //file name extensions
  std::string m_fileExt;

  //file directory
  std::string m_fileDir;

  //input file directory
  std::string m_inputFileDir;

  //Selection method with "Ithe" and "Ixtal"
  std::string m_selMethod;

  //calibration data: matrix M, vector R and other stuff 
  EmcBhaCalibData* myCalibData;
  //the Bhabha event that is filled with the two showers 
  EmcBhabhaEvent* myBhaEvt;

  int m_nXtals;
 
  //SigmaCut : selecting Bhabha events for digi-calibration
  double m_sigmaCut;
  //beam energy GeV
  double m_beamEnergy;


  bool m_ReadBeamEFromDB;
  bool m_elecSaturation;
  IBeamEnergySvc *m_BeamEnergySvc;
  // ReadBeamInfFromDb m_readDb;
  int m_irun;

  //message output in the tool classes
  int m_MsgFlag;

  // matrix to convert theta,phi <-> index 
  int **m_index;
  int m_measure[6240];
  bool m_passed;
  double m_inputConst[6240];
  int m_event;
  int m_run;
  IEmcRecGeoSvc* m_iGeoSvc;

  //IEmcCalibConstSvc* m_emcCalibConstSvc;

};


#endif // EMCSELBHAEVENT_H



















