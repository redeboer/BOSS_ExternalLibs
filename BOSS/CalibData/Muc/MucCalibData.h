#ifndef CalibData_Muc_MucCalibData_h
#define CalibData_Muc_MucCalibData_h

/* @class CalibData_MucCalibData_h

  calibration data class  
  calibration infrastructure

  @author ChunXiu Liu
*/
#include "CalibData/Muc/MucCalibConst.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include <vector>
#include <cmath>

using namespace std;

namespace CalibData {
  class MucCalibData:  public CalibBase1{
    
  public:
    MucCalibData();
    virtual ~MucCalibData(){}
    
    // Re-implemented from DataObject
    inline virtual const CLID& clID() const { return classID(); } 
    
    inline static  const CLID& classID() { return CLID_Calib_MucCal; }
    
    virtual StatusCode   update(CalibBase1& other, MsgStream* log);
     
 //   int setDefault(); 			// set default uniform constants value

  	//--------- Access by users -------------
    inline double getUniformEff()  const { return m_uniformEff;}
    inline double getUniformNos()  const { return m_uniformNos;}
    inline double getUniformClst() const { return m_uniformClst;} // probability = 1(biggest strip window for tracking)
    inline double getUniformCnt() const {return m_uniformCnt;}
    inline double getUniformNosRatio() const{return m_uniformNosRatio;}

    inline double getLayerEff( int layer ) const { return m_layerEff[layer];} 
    inline double getLayerNos( int layer ) const {return m_layerNos[layer];}
    inline double getLayerNosRatio( int layer ) const{return m_layerNosRatio[layer];}
    inline double getLayerCnt( int layer ) const{return m_layerCnt[layer];}
    inline double getLayerClstPro( int layer, int size ) const{ return m_layerClstPro[layer][size]; } 
    inline double getLayerClst( int layer, double prob ) const{
        double 	diff = 2.0;
        int 	size = 0;
	for(int i=0; i<CLST_MAX; i++)
	{       	
		if( fabs(prob - m_layerClstPro[layer][i] ) < diff ) {
                  diff = fabs(prob - m_layerClstPro[layer][i]);
		  size = i+1;
		}
	}
	return size;
}

    inline double getBoxEff( int part, int segment, int layer ) const{	return m_boxEff[part][segment][layer];}
    inline double getBoxNos( int part, int segment, int layer ) const{	return m_boxNos[part][segment][layer];}
    inline double getBoxCnt( int part, int segment, int layer ) const{ return m_boxCnt[part][segment][layer];}
    inline double getBoxNosRatio( int part, int segment, int layer ) const{return m_boxNosRatio[part][segment][layer];}
    inline double getBoxClstPro( int part, int segment, int layer, int size ) const{ return m_boxClstPro[part][segment][layer][size]; }
    inline double getBoxClst( int part, int segment, int layer, double prob ) const{
      	double 	diff = 2.0;
	int 	size = 0;
        for(int i=0; i<CLST_MAX; i++)
	{	
		if( fabs(prob - m_boxClstPro[part][segment][layer][i]) < diff ) {
		  diff = fabs(prob - m_boxClstPro[part][segment][layer][i]);
		  size = i+1;
		}  
	}
	return size;
}

    inline double getStripEff( int part, int segment, int layer, int strip ) const{ return m_stripEff[part][segment][layer][strip];}
    inline double getStripNos( int part, int segment, int layer, int strip ) const{ return m_stripNos[part][segment][layer][strip];}
    inline double getStripCnt( int part, int segment, int layer, int strip ) const{ return m_stripCnt[part][segment][layer][strip];}
    inline double getStripNosRatio( int part, int segment, int layer, int strip ) const{ return m_stripNosRatio[part][segment][layer][strip];}

   // set uniform constants for all detector units manually
    void setUniformEff( double eff ) {m_uniformEff=eff;}  
    void setUniformNos( double nos ) {m_uniformNos=nos;} 
    void setUniformClst( double clst ) {m_uniformClst=clst;}	
    void setUniformCnt( double cnt ){m_uniformCnt=cnt;}
    void setUniformNosRatio(double nosratio){m_uniformNosRatio=nosratio;}

  //set value
   void setLayerEff(const double layereff,int i){ m_layerEff[i]=layereff;}
   void setLayerNos(const double layernos,int i){ m_layerNos[i]=layernos;}
   void setLayerCnt(const double layercnt,int i){ m_layerCnt[i]=layercnt;}
   void setLayerNosRatio(const double layernosratio,int i){ m_layerNosRatio[i]=layernosratio;}
   void setLayerClstPro(const double layerpro,int i,int j){m_layerClstPro[i][j]= layerpro;}

   void setBoxEff(const double boxeff,int i,int j, int k){m_boxEff[i][j][k] = boxeff;}
   void setBoxNos(const double boxnos,int i,int j, int k){m_boxNos[i][j][k] = boxnos;}
   void setBoxCnt(const double boxcnt,int i,int j, int k){m_boxCnt[i][j][k] = boxcnt;}
   void setBoxNosRatio(const double boxratio,int i,int j, int k){m_boxNosRatio[i][j][k]=boxratio;}
   void setBoxClstPro(const double boxpro,int i,int j,int k,int l){m_boxClstPro[i][j][k][l]=boxpro;}

   void setStripEff(const double stripeff,int i,int j,int k,int l){m_stripEff[i][j][k][l]=stripeff;}
   void setStripNos(const double stripnos,int i,int j,int k,int l){m_stripNos[i][j][k][l]=stripnos;}
   void setStripCnt(const double stripcnt,int i,int j,int k,int l){m_stripCnt[i][j][k][l]=stripcnt;}
   void setStripNosRatio(const double stripnosratio,int i,int j,int k,int l){m_stripNosRatio[i][j][k][l]=stripnosratio;}

  private:
 //       MucIdTransform* m_ptrIdTr;

	double m_uniformEff;
	double m_uniformNos;
	double m_uniformClst;
        double m_uniformCnt;
        double m_uniformNosRatio;
//	double m_average[3][3];

	double m_layerEff[LAYER_MAX];	
	double m_layerNos[LAYER_MAX];
	double m_layerClstPro[LAYER_MAX][CLST_MAX];
        double m_layerCnt[LAYER_MAX];
        double m_layerNosRatio[LAYER_MAX];

	double m_boxEff[PART_MAX][SEGMENT_MAX][LAYER_MAX];
	double m_boxNos[PART_MAX][SEGMENT_MAX][LAYER_MAX];
	double m_boxClstPro[PART_MAX][SEGMENT_MAX][LAYER_MAX][CLST_MAX];
        double m_boxCnt[PART_MAX][SEGMENT_MAX][LAYER_MAX];
        double m_boxNosRatio[PART_MAX][SEGMENT_MAX][LAYER_MAX];

	double m_stripEff[PART_MAX][SEGMENT_MAX][LAYER_MAX][STRIP_INBOX_MAX];
	double m_stripNos[PART_MAX][SEGMENT_MAX][LAYER_MAX][STRIP_INBOX_MAX];
        double m_stripCnt[PART_MAX][SEGMENT_MAX][LAYER_MAX][STRIP_INBOX_MAX];
        double m_stripNosRatio[PART_MAX][SEGMENT_MAX][LAYER_MAX][STRIP_INBOX_MAX];
  };
}


#endif //CalibData_MucCalibData_h
