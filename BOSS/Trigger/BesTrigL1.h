#ifndef BesTrigL1_h
#define BesTrigL1_h 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include <fstream>
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "Trigger/MdcTSF.h"
#include "Trigger/TofHitCount.h"
#include "Trigger/EmcTCFinder.h"
#include "Trigger/MucTrigHit.h"

#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"
#include "EmcCalibConstSvc/IEmcCalibConstSvc.h"

#include "TROOT.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TProfile.h"

using namespace std;

class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;
class TrigTOFT;
class TrigEACC;
class EmcWaveform;

class BesTrigL1 : public Algorithm {
public:
  /// Standard constructor
  BesTrigL1( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BesTrigL1(){ }; ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void    findSETime(multimap<int,uint32_t,less<int> > mdc_hitmap, multimap<int,int,less<int> > tof_hitmap, multimap<int,uint32_t,less<int> > emc_TC,
                           double& stime, double& etime);

  void runAclock_mdc(int iclock, double stime, multimap<int,uint32_t,less<int> > mdc_hitmap); 
  void runAclock_tof(int iclock, double stime, int& idle_status, std::multimap<int,int,less<int> > tof_hitmap); 
  void runAclock_emc(int iclock, double stime, std::multimap<int,uint32_t,less<int> > emc_TC, EmcWaveform* blockWave);
  void getEmcAnalogSig(EmcDigiCol* emcDigiCol, EmcWaveform (&blockWave)[16], multimap<int,uint32_t,less<int> >& emc_TC);
  void findEmcPeakTime(double& peak_time, EmcWaveform* blockWave);

  void stretchTrgCond(int nclock, int** & trgcond);
  void trgSAFDelay(int nclock, int** & trgcond);
  void trgGTLDelay(int nclock, int** & trgcond);

private:
  //define and initialize variables for output mdchit infor.
  double hit9[76], hit10[76], hit11[88], hit12[88]; //super layer 3
  double hit13[100], hit14[100], hit15[112], hit16[112]; //super layer 4
  double hit17[128], hit18[128], hit19[140], hit20[140]; //super layer 5
  double hit37[256], hit38[256], hit39[256], hit40[256]; //super layer 10

  BesGlobalTrigSvc* m_pIBGT;
  IBesGlobalTrigSvc* m_tmpSvc;

  IRawDataProviderSvc* m_rawDataProviderSvc;
  IMdcGeomSvc* m_MdcGeomSvc;

  // Reference to RealizationSvc
  RealizationSvc* m_RealizationSvc;
  IEmcCalibConstSvc *emcCalibConstSvc;

  MdcTSF* m_MdcTSF;
  TofHitCount* m_TofHitCount;
  EmcTCFinder* m_emcDigi;
  MucTrigHit* m_mucDigi;

  ifstream readin;
  ofstream readout;
  ifstream mdc_hit;
  std::string input;
  std::string output;
  std::string outEvtId;
  std::string indexfile;
  bool mTrigRootFlag;
  int m_runMode;

  bool ifpass;
  int passNo;
  int writeFile;
  int ifoutEvtId;

  int totalEvent;
  int totalTracks;
  
  int clock_shift;

  NTuple::Tuple*  m_tuple;
  NTuple::Item<float> m_wire_x;
  NTuple::Item<float> m_wire_y;
  NTuple::Item<long> m_wire_evtId;
  NTuple::Item<long> m_delta_tdc;

  NTuple::Tuple*  m_tuple1;
  NTuple::Item<long> m_RunId;
  NTuple::Item<long> m_EventId;
  NTuple::Item<float> m_mc_totE_all;
  NTuple::Item<float> m_data_totE_all;
  NTuple::Item<float> m_wetotE;
  NTuple::Item<float> m_eetotE;
  NTuple::Item<long> m_data_wetotE;
  NTuple::Item<long> m_data_eetotE;
  NTuple::Item<long> m_index_btc;
  NTuple::Array<float> m_btc_e;
  NTuple::Array<long> m_data_btc;
  NTuple::Item<long> m_cond_id;
  NTuple::Array<long> m_mc_cond;
  NTuple::Array<long> m_data_cond;
  NTuple::Item<long> m_block_id;
  NTuple::Array<float> m_mc_blockE;
  NTuple::Array<float> m_data_blockE;
  NTuple::Array<float> m_R_blockE;

  NTuple::Tuple* m_tuple2;
  NTuple::Item<long> m_index2;
  NTuple::Array<long> m_fireLayer;

  NTuple::Item<long> m_index3;
  NTuple::Array<long> m_hitLayer;

  NTuple::Item<long> m_index4;
  NTuple::Array<long> m_hitSeg;

  NTuple::Item<long> m_index5;
  NTuple::Array<float> m_costheta;
  NTuple::Array<float> m_phi;
  NTuple::Array<float> m_p;
  NTuple::Array<float> m_pdgcode;

  NTuple::Item<long> m_index6;
  NTuple::Array<float> m_hitphi;

  NTuple::Item<long> m_nlayerEE;
  NTuple::Item<long> m_nlayerBR;
  NTuple::Item<long> m_nlayerWE;
  NTuple::Item<long> m_nlayerTotal;

  NTuple::Item<long> m_nhitEE;
  NTuple::Item<long> m_nhitWE;
  NTuple::Item<long> m_nhitBR;
  NTuple::Item<long> m_nhitTotal;
  
  NTuple::Item<float> m_mumcostheta;
  NTuple::Item<float> m_mumphi;

  NTuple::Item<long> m_trackfindall;
  NTuple::Item<long> m_trackfind3l;
  NTuple::Item<long> m_trackb;
  NTuple::Item<long> m_tracke;
  NTuple::Item<long> m_ntrack1;
  NTuple::Item<long> m_ntrack2;
  NTuple::Item<long> m_ntrack3;
  NTuple::Item<long> m_ngoodevent;
  NTuple::Item<long> m_ngoodtrack;

  NTuple::Tuple* m_tuple3;
  NTuple::Item<long>  m_evtId;
  NTuple::Item<long> m_condNOne[48];
  NTuple::Item<long> m_condNZero[48];
  int nEvent;

  // get a handle on the Hist/TTree registration service
  ITHistSvc *m_thistsvc;
  TH1F *m_trigCondi_MC;
  TH1F *m_trigCondi_Data;

  TrigTOFT* toftrig;
  //TrigEACC* eacctrig;
};
#endif
