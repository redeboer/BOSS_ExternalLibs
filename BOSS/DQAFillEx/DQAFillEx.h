#ifndef Physics_Analysis_DQAFillEx_H
#define Physics_Analysis_DQAFillEx_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class ITHistSvc;

/**********************************************************************/
/** <project name> - 
 ** <description> Head file for histogram(tuple) filling algorithm
 **
 **  \file   
 **
 **  \author Ji Xiaobin 
 **
 **  \version $Id: DQAFillEx.h,v 1.1.1.1 2009/05/27 03:43:05 jixb Exp $
 **
 **
*************************************************************************/

class DQAFillEx : public Algorithm {

public:
    DQAFillEx(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();  

private:

    ITHistSvc* m_thsvc;

    NTuple::Tuple*  m_tuple;    
    NTuple::Item<long>  m_runNo;
    NTuple::Item<long>  m_event;
};


#endif
