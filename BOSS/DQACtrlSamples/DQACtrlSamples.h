#ifndef Physics_Analysis_DQACtrlSamples_H
#define Physics_Analysis_DQACtrlSamples_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"


/**********************************************************************/
/** <project name> - DQACtrlSamples
 ** <description> Head file for DQA Control Samples selection
 **
 **  \file   
 **
 **  \author Ji Xiaobin 
 **
 **  \version $Id: DQACtrlSamples.h,v 1.4 2010/01/20 21:39:35 jixb Exp $
 **
 **
*************************************************************************/

class DQACtrlSamples : public Algorithm {

public:
    DQACtrlSamples(const std::string& name, ISvcLocator* pSvcLocator);
    
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();  

private:

    BooleanProperty m_selBhabha;
    BooleanProperty m_selDimu;
    BooleanProperty m_selHadron;
    BooleanProperty m_selRhopi;
    BooleanProperty m_selPpbar;
    BooleanProperty m_selKstark;
    BooleanProperty m_selLambdalambda;
    BooleanProperty m_selPPpipi;
    BooleanProperty m_selKsKpiDedx;

    BooleanProperty m_selpipiJpsi;
    
    Algorithm* m_dqaBhabha;
    Algorithm* m_dqaDimu;
    Algorithm* m_dqaHadron;
    Algorithm* m_dqaRhopi;
    Algorithm* m_dqaPpbar;
    Algorithm* m_dqaKstark;
    Algorithm* m_dqaLambdalambda;
    Algorithm* m_dqaPPpipi;
    Algorithm* m_dqaKsKpiDedx;

    Algorithm* m_dqapipiJpsi;

    // output data
    BooleanProperty m_OutBhabha;
    BooleanProperty m_OutDimu;
    BooleanProperty m_OutHadron;
    BooleanProperty m_OutRhopi;
    BooleanProperty m_OutPpbar;
    BooleanProperty m_OutKstark;
    BooleanProperty m_OutLambdalambda;
    BooleanProperty m_OutPPpipi;
    BooleanProperty m_OutKsKpiDedx;

    BooleanProperty m_OutpipiJpsi;
    
    Algorithm* m_dqaoutBhabha;
    Algorithm* m_dqaoutDimu;
    Algorithm* m_dqaoutHadron;
    Algorithm* m_dqaoutRhopi;
    Algorithm* m_dqaoutPpbar;
    Algorithm* m_dqaoutKstark;
    Algorithm* m_dqaoutLambdalambda;
    Algorithm* m_dqaoutPPpipi;
    Algorithm* m_dqaoutKsKpiDedx;

    Algorithm* m_dqaoutpipiJpsi;
    
};


#endif
