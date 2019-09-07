#ifndef MixerAlg_h
#define MixerAlg_h

#include "RawDataCnv/EventManagement/RAWEVENT.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "RawFile/RawFileReader.h"
#include "RawDataCnv/EventManagement/EmcBuilder.h"
#include "RawDataCnv/EventManagement/MdcBuilder.h"
#include "RawDataCnv/EventManagement/MucBuilder.h"
#include "RawDataCnv/EventManagement/TofBuilder.h"
#include "EmcRawEvent/EmcDigi.h"
#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "MucRawEvent/MucDigi.h"
#include "RawEvent/RawDataUtil.h"
#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"
#include "DataInfoSvc/DataInfoSvc.h"
#include "DataInfoSvc/IDataInfoSvc.h"
#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimerSvc.h"
#include "GaudiKernel/NTuple.h"
//
// An algorithm to combine MC signal and background from raw data file at digi level
//
// A.Zhemchugov (zhemchugov@jinr.ru), 09/08/2006
// A.Zhemchugov: modified to new data format, 29.07.08

#define MAX_LOOP_TIMES 1000000

class MsgStream;
//class IDatabaseSvc;
class IBesRndmGenSvc;

class MdcConverter;
class MucConverter;
class EmcConverter;
class TofConverter;

template <class T1, class T2> 
void combineDigits (SmartDataPtr<T1>& mcDigits, T1& bgDigits, int verbosity);
void combineMdcDigits (SmartDataPtr<MdcDigiCol>& mcDigits, MdcDigiCol& bgDigits, int verbosity);
void combineTofDigits (SmartDataPtr<TofDigiCol>& mcDigits, TofDigiCol& bgDigits, int verbosity);

class MixerAlg:public Algorithm {
public:
  MixerAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  ~MixerAlg() {}

 protected:
  bool nextEvent(int nskip = 0, int evtbyte = 0, int eventsToEnd = 0);
  
  void mixDigi(SmartDataPtr<MdcDigiCol>& mdcMcDigits, 
	       SmartDataPtr<EmcDigiCol>& emcMcDigits, 
	       SmartDataPtr<MucDigiCol>& mucMcDigits, 
	       SmartDataPtr<TofDigiCol>& tofMcDigits);

  void decodeMdc(MdcDigiCol* digiCol);
  void decodeMuc(MucDigiCol* digiCol);
  void decodeEmc(EmcDigiCol* digiCol);
  void decodeTof(TofDigiCol* digiCol);

  int getTiming();
  std::string eventType();

  std::string prepareDbQuery();

  bool file_sort(std::vector<std::string>& files, std::vector<int>& ranEvtNums);

 private:
  MsgStream* log;

  RAWEVENT*                m_raw_event;
  RawFileReader*  m_fr;
 
  //IDatabaseSvc* m_db;

  // Converters to create Digi objects
  MdcConverter* m_mdcCnv;
  MucConverter* m_mucCnv;
  EmcConverter* m_emcCnv;
  TofConverter* m_tofCnv;

  // Algorithm properties to be set in data card
  std::string m_run;
  std::vector<std::string> m_runs;
  std::vector<std::string> m_dates;

  // Flags to choose subdetectors for which background should be mixed in
  bool b_mdc;
  bool b_emc;
  bool b_muc;
  bool b_tof;
  
  // Number of background events to be mixed in one signal event
  // bool b_loop;
  bool m_skip;
  bool m_dbUserRequest;
  int m_NSkip;
  int m_skipCount;
  int nskipped;
  int m_mixingMethod;

  int m_nevent;
  int m_runNo;
  int m_totRanEvtNum;
  int m_totalEvent;
  int m_totEvtNumInCurFile;
  int m_nEventsToEnd;

  std::vector<int> m_ranTrgEvents;
  std::vector<int> m_numSets; //No. of sets of each bg file, for example: run_**_RandomTrg_file001_SF0-1.raw, 001 is set No.
  std::vector<int> m_vRanEvtNumInSubSet;
  std::vector<int> m_vStreamNumInSubSet;
  std::vector<int> m_vStepLength;
  std::vector<int> m_ranStepLenInCurrentFile;

  std::map<int,std::vector<int> > map_stepLength;
  
  int m_exRunFrom;
  int m_exRunTo;

  int m_maxLoop;
  bool m_ifSmearT0;

  int m_readBGMethod;

  bool m_usingFilter;

  // Current bg file
  std::string currentBGFile;

  // Current MC file
  std::string currentMCFile;

  // Input file name of background data (raw data format)
  std::vector<std::string> m_bgfiles;
  std::vector<std::string> m_bgfilesIndex;
  //std::vector<std::string>::iterator m_bgfile;

  // Replace  /besfs/offline/data/randomtrg in the file name from the database to the user defined value
  std::string m_pattern;
  // Use the user defined value of directory which contains random trigger files
  std::string m_newdatadir;

  // Reference to RealizationSvc
  RealizationSvc* m_RealizationSvc;
  DataInfoSvc* m_jobInfoSvc;
 
  NTuple::Tuple*        m_tuple1;
  NTuple::Item<float> m_time1;
  NTuple::Item<float> m_time2;
  NTuple::Item<float> m_time3;
  NTuple::Item<float> m_time4;
  NTuple::Item<float> m_time5;

  NTuple::Tuple*        m_tuple2;
  NTuple::Tuple*        m_tuple3;
  NTuple::Item<long> m_tdc;

  IBesTimerSvc* m_timersvc;
  BesTimer* m_timer; 
  BesTimer* m_timer1; 
  bool m_ifOutPut;

  //caogf for random seed
  IBesRndmGenSvc* p_BesRndmGenSvc;
};

#endif
