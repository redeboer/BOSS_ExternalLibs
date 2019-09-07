/*                                                                              
 * @class : RecETofCalHit                                                           
 *                                                                              
 *  this class used to do calibration output in Tof Reconstruction                          
 *                                                                              
 * @author: jiangll                                                              
 *                                                                              
 * ********************************************************/          
#include "TofRecEvent/RecETofCalHit.h"
RecETofCalHit::RecETofCalHit(){
  m_imod=-1;
  m_qual=-1;
   m_tpred=-1.;
   m_tdc=-1.;
   m_adc=-1.;
   m_rHit=-1.;
   m_drHit=-1.;
   m_costheta=-1;
  for(int i=0;i<5;i++)
       m_texp[i]=0.;
  m_p=-1.;
  m_Q=-1.;
  m_path=-1.;
}
