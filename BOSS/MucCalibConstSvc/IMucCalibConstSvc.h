//------------------------------------------------------------------------------|
//      [File  ]:                       IMucCalibConstSvc.h                     |
//      [Brief ]:       Head file of MucCalibConstSvc interface                 |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Aug 22, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef IMUC_CALIB_CONST_SVC_H_
#define IMUC_CALIB_CONST_SVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"

/* Declaration of the interface ID */
static const InterfaceID IID_IMucCalibConstSvc("IMucCalibConstSvc",1,0);

class IMucCalibConstSvc: virtual public IInterface
{
  public:
    static const InterfaceID& interfaceID() { return IID_IMucCalibConstSvc; }
    
    //--------- Access by users -------------
    virtual int    getLevel() const = 0;
    
    virtual double getEff( int part, int segment, int layer, int strip ) const = 0;
    virtual double getCnt( int part, int segment, int layer, int strip ) const = 0;
    virtual double getNos( int part, int segment, int layer, int strip ) const = 0;
    virtual double getNosRatio( int part, int segment, int layer, int strip ) const = 0;
    virtual double getClst(int part, int segment, int layer, double prob ) const = 0;
    
    virtual double getUniformEff()  const = 0;
    virtual double getUniformCnt()  const = 0;
    virtual double getUniformNos()  const = 0;
    virtual double getUniformNosRatio()  const = 0;
    virtual double getUniformClst() const = 0; // probability = 1(biggest strip window for tracking)
    
    virtual double getLayerEff( int layer ) const = 0; 
    virtual double getLayerCnt( int layer ) const = 0; 
    virtual double getLayerNos( int layer ) const = 0;
    virtual double getLayerNosRatio( int layer ) const = 0; 
    virtual double getLayerClst( int layer, double prob ) const = 0;
    
    virtual double getBoxEff( int part, int segment, int layer ) const = 0;
    virtual double getBoxCnt( int part, int segment, int layer ) const = 0;
    virtual double getBoxNos( int part, int segment, int layer ) const = 0;
    virtual double getBoxNosRatio( int part, int segment, int layer ) const = 0;
    virtual double getBoxClst( int part, int segment, int layer, double prob ) const = 0;
    
    virtual double getStripEff( int part, int segment, int layer, int strip ) const = 0;
    virtual double getStripCnt( int part, int segment, int layer, int strip ) const = 0;
    virtual double getStripNos( int part, int segment, int layer, int strip ) const = 0;
    virtual double getStripNosRatio( int part, int segment, int layer, int strip ) const = 0;
};

#endif /* IMUC_CALIB_CONST_SVC_H_ */
