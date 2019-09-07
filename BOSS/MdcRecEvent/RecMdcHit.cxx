/*                                                                              
 * @class : RecMdcHit                                                           
 *                                                                              
 *  this class models "Hit" used in Mdc Reconstruction                          
 *                                                                              
 * ********************************************************/          

#include "MdcRecEvent/RecMdcHit.h"

RecMdcHit::RecMdcHit():
    m_id(-1),
    m_trkid(-1),   
    m_ddl(0.0),
    m_ddr(0.0),
    m_erddl(0.0),
    m_erddr(0.0),
    m_pChisq(999.0),
    m_lr(0),
    m_stat(0),
    m_mdcid(0),
    m_tdc(0.0),
    m_adc(0.0),
    m_driftT(0.0),
    m_doca(999.0),
    m_entra(0.0),
    m_zhit(999.0),
    m_fltLen(0.0)
 {
 
 }                                     

RecMdcHit::RecMdcHit( const RecMdcHit& rechit )
  : m_id( rechit.m_id),
    m_trkid(rechit.m_trkid),
    m_ddl( rechit.m_ddl),
    m_ddr( rechit.m_ddr),
    m_erddl( rechit.m_erddl),
    m_erddr( rechit.m_erddr),
    m_pChisq( rechit.m_pChisq),
    m_lr( rechit.m_lr),
    m_stat( rechit.m_stat),
    m_mdcid( rechit.m_mdcid),
    m_tdc( rechit.m_tdc),
    m_adc( rechit.m_adc),
    m_driftT( rechit.m_driftT),
    m_doca( rechit.m_doca),
    m_entra( rechit.m_entra),
    m_zhit( rechit.m_zhit), 
    m_fltLen( rechit.m_fltLen) {
} 

bool
RecMdcHit::isGrouped() {
   if(m_trkid == -1 ) return false;
   return true;
}                

RecMdcHit::~RecMdcHit() {
}                
