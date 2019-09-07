#ifndef tofcalgsec_Header
#define tofcalgsec_Header

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "tofcalgsec/TofCalibCheck.h"

using namespace std;
class tofcalgsec:public Algorithm {

  public:
    tofcalgsec(const std::string& name, ISvcLocator* pSvcLocator);
    ~tofcalgsec();
    StatusCode initialize();
    StatusCode beginRun();   
    StatusCode execute();
    StatusCode endRun();
    StatusCode finalize();

 protected:
    bool testbit( unsigned int n ) {
      if( n>m_calibItem.size() ) return false;
      return m_calibItem[n]=='0'?false:true;
    }

 private:

    bool        m_online;       // online,offline
    bool        m_calibration;  // online and calibration or offline
    std::string m_workdir;      // by default workdir=.
    std::string m_datafile_dir; // dir of data files are in
    std::vector<std::string> m_datafile_barrel;  //specify input data file
    std::vector<std::string> m_datafile_endcap;  //specify input data file
    std::vector<std::string> m_datafile_etf;     //specify input data file
    std::vector<std::string> m_barrelfile;       //specify input data file
    std::vector<std::string> m_endcapfile;       //specify input data file
    std::vector<std::string> m_etffile;          //specify input data file
    std::string m_calibItem;
    // bit   1 1 1 1  1 1 1 1  1 1 1 1;
    //       Endcap   Barrel: Q0, EffV, AttenLength, Time
    unsigned int m_tcorrzbin;    // the bin for time correltation
    unsigned int m_barrelzbin;   // the bin for barrel sigma calibration
    unsigned int m_endcaprbin;   // the bin for endcap sigma calibration
    unsigned int m_endcapQrbin;  // the bin for endcap Q calibration
    
    bool FILE_HAS_BEEN_READ;         // indicate that 
    
    TofCalibCheck* m_checkbarrel;
    NTuple::Tuple* mtuple_barrel;
    TofCalibCheck* m_checkendcap;
    NTuple::Tuple* mtuple_endcap;
    TofCalibCheck* m_checketf;
    NTuple::Tuple* mtuple_etf;
};

#endif//tofcalgsec_Header
