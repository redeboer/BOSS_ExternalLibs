/*                                                                              
 * @class : TofCalHit                                                           
 *                                                                              
 *  this class used to do calibration output in Tof Reconstruction                          
 *                                                                              
 * @author: jiangll                                                              
 *                                                                              
 * ********************************************************/          
#include "TofRecEvent/RecBTofCalHit.h"
RecBTofCalHit::RecBTofCalHit(){
    m_imod=-1;
    m_qual=-1;
    m_tpred=-1.;
    m_tdc1=-1.;
    m_tdc2=-1.;
    m_adc1=-1.;
    m_adc2=-1.;
    m_zHit=-1.;
    m_dzHit=-1.;
    m_sintheta=-1.;
    m_bch=NULL;
    for(int i=0;i<5;i++)
       m_texp[i]=0.;
   m_p=-1.;
   m_Q=-1.;
   m_path=-1.;
}
