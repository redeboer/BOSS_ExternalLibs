/** @class MucCalibData
 *    Implementation of MucCalibData calibration TCDS class
 */

#include "CalibData/Muc/MucCalibData.h"
#include "GaudiKernel/MsgStream.h"
namespace CalibData {
  MucCalibData::MucCalibData(){}

   StatusCode MucCalibData::update(CalibBase1& other, MsgStream* log)
  {
    MucCalibData& other1 = dynamic_cast<MucCalibData& >(other);
    std::cout<<"========================================"<<"\n"
	     <<"here is the update in the MucCalibData"<<"\n"
	     <<"=================================================="<<std::endl;
    CalibBase1::update(other, log);
    setUniformClst(other1.getUniformClst());
    setUniformEff(other1.getUniformEff());
    setUniformClst(other1.getUniformClst());
    setUniformCnt(other1.getUniformCnt());
    setUniformNosRatio(other1.getUniformNosRatio());
    //set the Calibration Data
    for(int i=0;i<LAYER_MAX;i++) {
      setLayerEff(other1.getLayerEff(i),i);
      setLayerNos(other1.getLayerNos(i),i);
      setLayerCnt(other1.getLayerCnt(i),i);
      setLayerNosRatio(other1.getLayerNosRatio(i),i);
      for(int j=0;j<CLST_MAX;j++){
      setLayerClstPro(other1.getLayerClstPro(i,j),i,j);
      }
    }
    for(int i=0;i<PART_MAX;i++) {
      for(int j=0;j<SEGMENT_MAX;j++){
	for(int k=0;k<LAYER_MAX;k++){
          setBoxEff(other1.getBoxEff(i,j,k),i,j,k);
          setBoxNos(other1.getBoxNos(i,j,k),i,j,k);
          setBoxCnt(other1.getBoxCnt(i,j,k),i,j,k);
          setBoxNosRatio(other1.getBoxNosRatio(i,j,k),i,j,k);
          for(int l=0;l<CLST_MAX;l++){
            setBoxClstPro(other1.getBoxClstPro(i,j,k,l),i,j,k,l);
          }
        }
      }
    }
   for(int i=0;i<PART_MAX;i++){
     for(int j=0;j<SEGMENT_MAX;j++){
       for(int k=0;k<LAYER_MAX;k++){
	 for(int l=0;l<STRIP_INBOX_MAX;l++){
	   setStripEff(other1.getStripEff(i,j,k,l),i,j,k,l);
	   setStripNos(other1.getStripNos(i,j,k,l),i,j,k,l);
           setStripCnt(other1.getStripCnt(i,j,k,l),i,j,k,l);
           setStripNosRatio(other1.getStripNosRatio(i,j,k,l),i,j,k,l);
	 }
       }
     }
   }
   std::cout<<"updata complete================="<<std::endl;
    return StatusCode::SUCCESS;  
}	
					
   //set the Calibration Data

/*  int MucCalibData::setDefault() 
 {

        m_uniformEff = DEFAULT_EFF;
        m_uniformNos = DEFAULT_NOS;
        m_uniformClst = DEFAULT_CLST;

        for(int i=0; i<LAYER_MAX; i++)
        {
                m_layerEff[i] = DEFAULT_EFF;
                m_layerNos[i] = DEFAULT_NOS;
                for(int j=0; j<CLST_MAX; j++)
                        m_layerClstPro[i][j] = DEFAULT_CLST_PRO[j];
        }

        for(int i=0; i<PART_MAX; i++)
        {
	  for(int j=0; j<((i==1)?B_SEG_NUM:E_SEG_NUM); j++)
	  {
	    for(int k=0; k<((i==1)?B_LAY_NUM:E_LAY_NUM); k++)
	    {
                m_boxEff[i][j][k] = DEFAULT_EFF;
                m_boxNos[i][j][k] = DEFAULT_NOS;
                for(int n=0; n<CLST_MAX; n++)
                        m_boxClstPro[i][j][k][n] = DEFAULT_CLST_PRO[n];
	    }
	  }	
        }

	int strip_max;
        for(int i=0; i<PART_MAX; i++)
        {
	  for(int j=0; j<((i==1)?B_SEG_NUM:E_SEG_NUM); j++)
	  {
	    for(int k=0; k<((i==1)?B_LAY_NUM:E_LAY_NUM); k++)
	    {
                strip_max = m_ptrIdTr->GetStripMax(i, j, k);
		for(int n=0; n<strip_max; n++)
		{
                	m_stripEff[i][j][k][n] = DEFAULT_EFF;
                	m_stripNos[i][j][k][n] = DEFAULT_NOS;
		}
	    }
	  }			
        }	     

     	return 1;
}

*/

  
}
