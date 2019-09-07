// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/Dedx/DedxCalibData.cxx,v 1.9 2010/10/08 01:12:53 huangb Exp $

/** @class DedxCalibData
 *    Implementation of DedxCalibData calibration TCDS class
 */

#include "CalibData/Dedx/DedxCalibData.h"
#include "GaudiKernel/MsgStream.h"
#include "CalibData/Mdc/MdcCalStruct.h"
namespace CalibData {
 
 
  StatusCode DedxCalibData::update(CalibBase1& other, MsgStream* log)
  {
    DedxCalibData& other1 = dynamic_cast<DedxCalibData& >(other);
    std::cout<<"========================================"<<"\n"<<"here is the update in the DedxCalibData"<<"\n"<<"=================================================="<<std::endl;
    CalibBase1::update(other, log);
    int i;
    int j;
    for(i=0;i<4;i++) {
      for(j=0;j<43;j++) { 
       /*  setddg(other1.m_ddg[i][j],i,j);
         setggs(other1.m_ggs[i][j],i,j);
         setzdep(other1.m_zdep[i][j],i,j);
	 setenta(other1.m_enta[i][j],i,j);
*/
  //    getddg(int i,int j)
   //  std::cout<<"other1.getddg("<<i<<","<<j<<")="<<other1.getddg(i,j)<<std::endl;
      setddg(other1.getddg(i,j),i,j);
      setggs(other1.getggs(i,j),i,j);
      setzdep(other1.getzdep(i,j),i,j);
      setenta(other1.getenta(i,j),i,j);
        }
    }

    setrunNO(other1.getrunNO());   
    int entries = other1.getrunNO();
    for(i=0;i<4;i++) {
      for(j=0;j<entries;j++) {
      setrung(other1.getrung(i,j),i,j);
        }
    }
  

    for(i=0;i<6796;i++){
    //    setwireg(other1.m_wireg[i],i);
     setwireg(other1.getwireg(i),i);
    }  
   
    for(i=0;i<43;i++){
    //    setlayerg(other1.m_layerg[i],i);
     setlayerg(other1.getlayerg(i),i);
    }  
     
  //  setgain(other1.m_gain);
  //  setresol(other1.m_resol);
  setgain(other1.getgain());
  setresol(other1.getresol());

   for(int i=0;i<1600;i++){
    m_id_doca[i]=other1.get_id_doca(i);   
    m_iner_chi[i]=other1.get_iner_chi(i);
    m_iner_gain[i]=other1.get_iner_gain(i);
    m_iner_hits[i]=other1.get_iner_hits(i);
    m_ip_eangle[i]=other1.get_ip_eangle(i);
    m_out_chi[i]=other1.get_out_chi(i);
    m_out_gain[i]=other1.get_out_gain(i);
    m_out_hits[i]=other1.get_out_hits(i);
    }

    for(int i=0;i<80;i++){
    m_costheta[i]=other1.get_costheta(i); 
    }
   for(int i=0;i<35;i++){
    m_t0[i] = other1.get_t0(i);
    m_dedx[i]=other1.get_dedx(i);
    }
  
   for(int i=0;i<20;i++){
    m_hadron[i] = other1.get_hadron(i);
    }
   
    m_enanglesize=other1.get_enanglesize();
   for(int i=0;i<m_enanglesize;i++){
    m_enangle[i]=other1.get_enangle(i);
   }
  
   std::cout<<"updata dedx calib constnts complete================="<<std::endl;
    return StatusCode::SUCCESS;  
}	
					
   //set the Calibration Data
  

  void DedxCalibData::setddg(const double ddg,int i,int j)
  { 
    m_ddg[i][j] = ddg;
  }

  void DedxCalibData::setggs(const double ggs,int i,int j)
  {
    m_ggs[i][j] = ggs;
  }
  
  void DedxCalibData::setwireg(const double wireg,int i)
  {
    m_wireg[i] = wireg;
  }
  
  void DedxCalibData::setgain(const double gain)
  {
    m_gain = gain;
  }
  
  void DedxCalibData::setzdep(const double zdep,int i,int j)
  {
    m_zdep[i][j] = zdep;
  }
  
  void DedxCalibData::setresol(const double resol)
  {
    m_resol = resol;
  }
  
  void DedxCalibData::setlayerg(const double layerg,int i)
  {
    m_layerg[i] = layerg;
  }
  
  void DedxCalibData::setenta(const double enta,int i,int j)
  {
    m_enta[i][j] = enta;
  }
 
  void DedxCalibData::setrung(const double rung,int i,int j)
  { 
   double aaa=rung;
    if(i==0) m_rung1.push_back(aaa);
    if(i==1) m_rung2.push_back(aaa);
    if(i==2) m_rung3.push_back(aaa);
    if(i==3) m_rung4.push_back(aaa);
  }

   void DedxCalibData::setrunNO(const int runNO)
  {
    m_runNO = runNO;
  }
}
