/*                                                                              
 * @class : MdcDedxHit                                                             
 *                                                                              
 *  this class models "dE/dx hit" used in Mdc Reconstruction                        
 *                                                                              
 * @author:xcao                                                              
 *                                                                              
 * ********************************************************/

#include "MdcRecEvent/RecMdcDedxHit.h"

RecMdcDedxHit::RecMdcDedxHit() {
   m_trkid = -1;
   m_lr = 0;
   m_pathlength = -1;
   m_dedx = 0;
   m_mdcid = 0;
   m_mdcKalHelixSeg = 0;
   m_mdcHit = 0;
}  

RecMdcDedxHit::RecMdcDedxHit( const RecMdcDedxHit& recdedxhit ):
    m_trkid  (recdedxhit.m_trkid), 
    m_lr  (recdedxhit.m_lr),
    m_pathlength (recdedxhit.m_pathlength),
    m_dedx  (recdedxhit.m_dedx),
    m_mdcid  (recdedxhit.m_mdcid),
    m_mdcKalHelixSeg (recdedxhit.m_mdcKalHelixSeg),
    m_mdcHit (recdedxhit.m_mdcHit)
{
} 

bool
RecMdcDedxHit::isGrouped() {
    if(m_trkid == -1 ) return false;
    return true;
}                


RecMdcDedxHit::~RecMdcDedxHit() {       
} 
