#ifndef Physics_ControlSample_JsiLL_H
#define Physics_ControlSample_JsiLL_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class ITHistSvc;

/**********************************************************************/
/** <project name> - 
 ** <description> Head file for event selection program 
 **
 **  \file   
 **
 **  \author Ji Xiaobin 
 **
 **  \version $Id: JsiLL.h,v 1.1.1.1 2009/06/02 00:13:41 yanghx Exp $
 **
 **
*************************************************************************/

class JsiLL : public Algorithm {

public:

    JsiLL(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();  

private:

    double m_vr0cut;
    double m_vz0cut;
    double m_vr1cut;
    double m_vz1cut;  
    double m_cthcut;

    double m_energyThreshold;
    double m_gammaAngCut;

    ITHistSvc* m_thsvc;

    NTuple::Tuple*  m_tuple;    
    NTuple::Item<long>  m_runNo;
    NTuple::Item<long>  m_event;
    NTuple::Item<double>  m_mLambda;
    NTuple::Item<double>  m_mLambdabar;
    NTuple::Item<double>  m_pLambda;
    NTuple::Item<double>  m_pLambdabar;
    NTuple::Item<double>  m_chisq;
    

};


#endif
