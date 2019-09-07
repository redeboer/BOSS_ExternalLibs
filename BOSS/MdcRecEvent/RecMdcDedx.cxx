/*                                                                              
 * @class : RecMdcDedx                                                             
 *                                                                              
 *  this class models "dE/dx" used in Mdc Reconstruction                        
 *                                                                              
 * @author: xcao                                                              
 *                                                                              
 * ********************************************************/


#include "MdcRecEvent/RecMdcDedx.h"

RecMdcDedx::RecMdcDedx() {
    m_dedx_hit=0;
    m_dedx_esat =0;
    m_dedx_norun =0;   
    m_dedx_momentum =0; 
    for( int i=0; i<5; i++) {
        m_dedx_exp[i] = 0.0;
        m_sigma_dedx[i] = 0.0;
        m_pid_prob[i] = 0.0;
    }
    m_mdcTrack = 0;
    m_mdcKalTrack = 0;
}         

RecMdcDedx::RecMdcDedx( const RecMdcDedx& recdedx)
: DstMdcDedx(recdedx)
{ 
    m_dedx_hit=recdedx.m_dedx_hit;
    m_dedx_esat =recdedx.m_dedx_esat;
    m_dedx_norun =recdedx.m_dedx_norun;
    m_dedx_momentum =recdedx.m_dedx_momentum;
    for( int i=0; i<5; i++) {
        m_dedx_exp[i] = recdedx.m_dedx_exp[i];
        m_sigma_dedx[i] = recdedx.m_sigma_dedx[i];
        m_pid_prob[i] = recdedx.m_pid_prob[i];
    }
    m_mdcTrack = recdedx.m_mdcTrack;
    m_mdcKalTrack = recdedx.m_mdcKalTrack;
    m_vecdedxhit = recdedx.m_vecdedxhit;
}

RecMdcDedx::RecMdcDedx( const DstMdcDedx& dstdedx) 
: DstMdcDedx(dstdedx)
{
    m_dedx_hit=0;
    m_dedx_esat =0;
    m_dedx_norun =0;
    m_dedx_momentum =0;
    for( int i=0; i<5; i++) {
        m_dedx_exp[i] = 0.0;
        m_sigma_dedx[i] = 0.0;
        m_pid_prob[i] = 0.0;
    }
    m_mdcTrack = 0;
    m_mdcKalTrack = 0;
}

RecMdcDedx& RecMdcDedx::operator=(const RecMdcDedx& recdedx)
{
    if( &recdedx != this ){
        DstMdcDedx::operator=(recdedx);
        m_dedx_hit=recdedx.m_dedx_hit;
        m_dedx_esat =recdedx.m_dedx_esat;
        m_dedx_norun =recdedx.m_dedx_norun;
        m_dedx_momentum =recdedx.m_dedx_momentum;
        for( int i=0; i<5; i++) {
            m_dedx_exp[i] = recdedx.m_dedx_exp[i];
            m_sigma_dedx[i] = recdedx.m_sigma_dedx[i];
            m_pid_prob[i] = recdedx.m_pid_prob[i];
        }
        m_mdcTrack = recdedx.m_mdcTrack;
        m_mdcKalTrack = recdedx.m_mdcKalTrack;
        m_vecdedxhit = recdedx.m_vecdedxhit;
    }
    return  *this;
}

RecMdcDedx& RecMdcDedx::operator=(const DstMdcDedx& dstdedx)
{
    if( &dstdedx != this ){
        DstMdcDedx::operator=(dstdedx);  
        m_dedx_hit=0;
        m_dedx_esat =0;
        m_dedx_norun =0;
        m_dedx_momentum =0;
        for( int i=0; i<5; i++) {
            m_dedx_exp[i] = 0;
            m_sigma_dedx[i] = 0;
            m_pid_prob[i] = 0;
        }
    m_mdcTrack = 0;
    m_mdcKalTrack = 0;
    }
    return  *this;
}  



RecMdcDedx::~RecMdcDedx() {       
}       
