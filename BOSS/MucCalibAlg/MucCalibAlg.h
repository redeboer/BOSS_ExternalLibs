//------------------------------------------------------------------------------|
//      [File  ]:                       MucCalibAlg.h                           |
//      [Brief ]:       Head file of MUC calibration algrithom                  |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_CALIB_ALG_H
#define MUC_CALIB_ALG_H

#include<iostream>

#include "GaudiKernel/Algorithm.h"

#include "MucCalibAlg/MucCalibMgr.h"

using namespace std;

class MucCalibAlg : public Algorithm
{
  public:
    MucCalibAlg( const std::string& name, ISvcLocator* pSvcLocator );

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    std::vector<double>  m_vJobInfo;     // Job information vector
    std::vector<int>    m_vConfigInfo;  // Config information vector
    
    std::string m_sOutputFile;    // Possable specified output file of ROOT or ASCII format by users 
    
    MucCalibMgr*  m_pMucCalibMgr; // Pointer of calibration manager
    
};

#endif

