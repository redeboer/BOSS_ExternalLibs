#ifndef G4SIM_G4SVC_H
#define G4SIM_G4SVC_H

#include "GaudiKernel/Service.h"
#include "G4Svc/IG4Svc.h"
#include "GaudiKernel/NTuple.h"

// G4 includes
#include  "G4UImanager.hh"

#ifdef G4VIS_USE
 #include "G4VisManager.hh"
#endif

#include <vector>
#include <string>
#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"

// Forward declarations
template <class TYPE> class SvcFactory;

class G4VUserPhysicsList;
class G4RunManager;
class G4SvcRunManager;

// ClassName:   G4Svc
// Description: This service provides access to Geant 4
//

class G4Svc: public Service, virtual public IG4Svc 
{
public:

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Query the interfaces.
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  G4RunManager * runMgr();

  // Detector init
  void SetUserInitialization(G4VUserDetectorConstruction* userInit);

  // Physics init
  void SetUserInitialization(G4VUserPhysicsList* physInit);

  // User Action
  void SetUserAction(G4UserRunAction *);
  void SetUserAction(G4UserEventAction *);
  void SetUserAction(G4VUserPrimaryGeneratorAction*);
  void SetUserAction(G4UserStackingAction*);
  void SetUserAction(G4UserTrackingAction*);
  void SetUserAction(G4UserSteppingAction*);

  // G4Event
  const G4Event* GetCurrentEvent() const;
  const G4Run*   GetCurrentRun() const;
 
#ifdef G4VIS_USE
  G4VisManager * visMgr() { return p_visMgr; }
#endif
  G4UImanager  * uiMgr();
  void  StartUISession();
 
  //for vertex parameters
  double GetBeamPosX() {return m_beamPosX;}
  double GetBeamPosY() {return m_beamPosY;}
  double GetBeamPosZ() {return m_beamPosZ;}

  double GetBeamSizeX() {return m_beamSizeX;}
  double GetBeamSizeY() {return m_beamSizeY;}
  double GetBeamSizeZ() {return m_beamSizeZ;}

  double GetBeamShiftPx(){return m_beamShiftPx;}
  double GetBeamShiftPy(){return m_beamShiftPy;}
  double GetBeamShiftPz(){return m_beamShiftPz;}

  double GetBeamStartTime() {return m_beamStartTime;}
  double GetBeamDeltaTime() {return m_beamDeltaTime;}
  double GetNBunch() {return m_nBunch;} 
  double GetBunchTimeSigma() {return m_bunchTimeSigma;}
 
  double GetBeamTime() {return m_beamTime;}
  void   SetBeamTime(double value) {m_beamTime = value;}
   std::string GetMdcNoiseFile(){return m_mdcNoiseFile;}
  
  double GetBeamAngle() {return m_beamAngle;}
  bool   GetBoostLab() {return m_boostLab;}  
  bool   GetSetBeamShift() {return m_setBeamShift;} 

  int GetMdcDataInput() { return m_mdcDataInput; }
  int GetMdcDedxFlag(){return m_mdcDedxFlag;}

  NTuple::Tuple* GetTupleMdc() {return m_tupleMdc;}
  void SetTupleMdc (NTuple::Tuple* tuple) {m_tupleMdc = tuple;}

  NTuple::Tuple* GetTupleTof1() {return m_tupleTof1;}
  void SetTupleTof1 (NTuple::Tuple* tuple) {m_tupleTof1 = tuple;}
  
  NTuple::Tuple* GetTupleTof2() {return m_tupleTof2;}
  void SetTupleTof2 (NTuple::Tuple* tuple) {m_tupleTof2 = tuple;}

  NTuple::Tuple* GetTupleTof3() {return m_tupleTof3;}
  void SetTupleTof3 (NTuple::Tuple* tuple) {m_tupleTof3 = tuple;}

  NTuple::Tuple* GetTupleEmc1() {return m_tupleEmc1;}
  void SetTupleEmc1 (NTuple::Tuple* tuple) {m_tupleEmc1 = tuple;}

  NTuple::Tuple* GetTupleEmc2() {return m_tupleEmc2;}
  void SetTupleEmc2 (NTuple::Tuple* tuple) {m_tupleEmc2 = tuple;}

  NTuple::Tuple* GetTupleMuc() {return m_tupleMuc;}
  void SetTupleMuc (NTuple::Tuple* tuple) {m_tupleMuc = tuple;}  

  bool MdcRootFlag() {return m_mdcRootFlag;}
  bool TofRootFlag() {return m_tofRootFlag;}
  bool EmcRootFlag() {return m_emcRootFlag;}
  bool MucRootFlag() {return m_mucRootFlag;}  

  bool TofSaturationFlag() {return m_tofSaturationFlag;} // Tof Q Saturation in bhabha events

  bool EmcLightOutput() {return m_emcLightOutput;}
  double EmcIncoherentNoise() {return m_emcIncoherentNoise;}
  double EmcCoherentNoise() {return m_emcCoherentNoise;}
  double EmcNoiseMean() {return m_emcNoiseMean;}
  double EmcNoiseSigma() {return m_emcNoiseSigma;}
  double EmcNoiseThreshold() {return m_emcNoiseThreshold;}
  int EmcNoiseLevel() {return m_emcNoiseLevel;}
  int EmcTime() {return m_emcTime;}
  int EmcElecSaturation(){return m_emcELecSaturation;}


  int MucNoiseMode() {return m_mucNoiseMode;}

  int LogLevel() {return m_logLevel;}

protected:

  G4Svc( const std::string& name, ISvcLocator* svc );
  
  // Destructor.
  virtual ~G4Svc();

public:

  void G4Init();
  
  // Run initialization, termination and single event generation for G4SimAlg
  void RunInitialize();
  void RunTerminate();
  void SimulateEvents(int);
  
private:

  // Allow SvcFactory to instantiate the service.
  friend class SvcFactory<G4Svc>;

  G4SvcRunManager *p_runMgr;
  G4UImanager  *p_uiMgr;
  
  //std::string m_PhysicsList;
  //double m_PhysicsCut;

  // Visualization
  bool m_vis;
  //std::string m_visType;
  
  // Initialization macro file
  
  std::string m_macroName;
  std::string m_mdcNoiseFile; 

#ifdef G4VIS_USE
  G4VisManager *p_visMgr;
#endif

  // Save Hits and Tracks in StoreGate
  //bool m_saveHits;
  //bool m_saveTracks;
  int m_logLevel;

  // Verbosity
  int m_runVerb;
  int m_eventVerb;
  int m_trackVerb;

  bool _init;
  
  bool m_interactiveG4;
  
  bool m_besGenAction;

  int m_runID;
  
  int m_mdcDataInput;  
  int m_mdcDedxFlag;

  //vertex parameters
  double m_beamPosX;
  double m_beamPosY;
  double m_beamPosZ;

  double m_beamSizeX;
  double m_beamSizeY;
  double m_beamSizeZ;

  double m_beamStartTime;
  double m_beamDeltaTime;
  double m_nBunch;
  
  double m_bunchTimeSigma;
  double m_beamTime;  

  double m_beamShiftPx;
  double m_beamShiftPy;
  double m_beamShiftPz;

  bool   m_boostLab;
  bool   m_setBeamShift;
  double m_beamAngle;

  // MC Monitor
  NTuple::Tuple* m_tupleMdc;
  bool m_mdcRootFlag;

  NTuple::Tuple* m_tupleTof1;
  NTuple::Tuple* m_tupleTof2;
  NTuple::Tuple* m_tupleTof3;
  bool m_tofRootFlag;
  bool m_tofSaturationFlag;
  
  NTuple::Tuple* m_tupleEmc1; //digi level
  NTuple::Tuple* m_tupleEmc2; //event level
  bool m_emcRootFlag;

  // Emc Crystal Light Output
  bool m_emcLightOutput;
  // Incoherent Noise
  double  m_emcIncoherentNoise;
  // Coherent Noise
  double  m_emcCoherentNoise;
  // Noise for fast simulation
  double  m_emcNoiseMean;
  double  m_emcNoiseSigma;
  // Only energy above noise threshold will be recorded
  double  m_emcNoiseThreshold;
  // Noise Level:
  // 0: No noise, 1: Only singal crystals have noise,
  // 2: Only 5x5 matrix has noise, 3: All noise.
  int m_emcNoiseLevel;
  // Time
  int m_emcTime;
  //MC electronics saturation of crystal:
  //0:using Bhabha calibration constants;
  //1:using EmaxData from data;
  int m_emcELecSaturation;

  // Muc noise mode
  int m_mucNoiseMode;   

  NTuple::Tuple* m_tupleMuc;
  bool m_mucRootFlag;

  RealizationSvc* m_RealizationSvc;
};

#endif
