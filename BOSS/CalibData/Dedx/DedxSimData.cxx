// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/Dedx/DedxSimData.cxx,v 1.3 2016/06/16 03:16:14 zhaog Exp $

/** @class DedxSimData
 *    Implementation of DedxSimData calibration TCDS class
 */

#include "CalibData/Dedx/DedxSimData.h"
#include "GaudiKernel/MsgStream.h"
#include "CalibData/Mdc/MdcCalStruct.h"
namespace CalibData {
 
 
    StatusCode DedxSimData::update(CalibBase1& other, MsgStream* log)
    {
        DedxSimData& other1 = dynamic_cast<DedxSimData& >(other);
        std::cout << "========================================"
                  << "\n"
                  << "here is the update in the DedxSimData"
                  << "\n"
                  << "=================================================="
                  << std::endl;
        CalibBase1::update(other, log);
        setVersion(other1.getVersion());
        setHistNo(other1.gethistNo());
        setRangeNo(other1.getRangeNo());
        if (m_hist) delete m_hist;
        if (m_hRange) delete m_hRange;

        m_hist = new vector<TH1F>(*other1.m_hist);
        m_hRange = new vector<double>(*other1.m_hRange);
 
  
        std::cout<<"updata dedx Sim constnts complete================="<<std::endl;
        return StatusCode::SUCCESS;  
    }	
					
    //set the Calibration Data
    void DedxSimData::setHist(const vector<TH1F>* hist){
        if(m_hist) delete m_hist;
        m_hist = new vector<TH1F>(*hist);
    }

    void DedxSimData::setRange(const vector<double>* hRange){
        if(m_hRange) delete m_hRange;
        m_hRange = new vector<double>(*hRange);
    }

}
