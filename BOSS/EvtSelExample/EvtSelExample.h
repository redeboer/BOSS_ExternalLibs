#ifndef Physics_ControlSample_EvtSelExample_H
#define Physics_ControlSample_EvtSelExample_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"


/**********************************************************************/
/** <project name> - 
 ** <description> Head file for event selection program 
 **
 **  \file   
 **
 **  \author Ji Xiaobin 
 **
 **  \version $Id: EvtSelExample.h,v 1.1.1.1 2009/05/25 06:21:26 jixb Exp $
 **
 **
*************************************************************************/

class EvtSelExample : public Algorithm {

public:

    EvtSelExample(const std::string& name, ISvcLocator* pSvcLocator);
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

    NTuple::Tuple*  m_tuple;     // rhopi 4C
    NTuple::Item<long>  m_runNo;
    NTuple::Item<long>  m_event;

};


#endif
