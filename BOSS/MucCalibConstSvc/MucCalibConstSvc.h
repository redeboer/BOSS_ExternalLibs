//------------------------------------------------------------------------------|
//      [File  ]:                       MucCalibConstSvc.h                      |
//      [Brief ]:       Head file of MUC calibration constants service          |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Aug 22, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_CALIB_CONST_SVC_H_
#define MUC_CALIB_CONST_SVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "MucCalibConstSvc/IMucCalibConstSvc.h"

class MucCalibConstSvc: public Service, virtual public IMucCalibConstSvc
{
  public:
    MucCalibConstSvc( const std::string& name, ISvcLocator* svcloc );
    ~MucCalibConstSvc();
    
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    //--------- Access by users -------------
    inline int    getLevel() const;
    
    inline double getEff( int part, int segment, int layer, int strip ) const;
    inline double getCnt( int part, int segment, int layer, int strip ) const; // all background
    inline double getNos( int part, int segment, int layer, int strip ) const; // noise to tracking
    inline double getNosRatio( int part, int segment, int layer, int strip ) const; 
    inline double getClst(int part, int segment, int layer, double prob ) const;
    
    inline double getUniformEff()  const;
    inline double getUniformCnt()  const;
    inline double getUniformNos()  const;
    inline double getUniformNosRatio()  const;
    inline double getUniformClst() const; // probability = 1(biggest strip window for tracking)
    
    inline double getLayerEff( int layer ) const; 
    inline double getLayerCnt( int layer ) const; 
    inline double getLayerNos( int layer ) const; 
    inline double getLayerNosRatio( int layer ) const;
    inline double getLayerClst( int layer, double prob ) const;
    
    inline double getBoxEff( int part, int segment, int layer ) const;
    inline double getBoxCnt( int part, int segment, int layer ) const;
    inline double getBoxNos( int part, int segment, int layer ) const;
    inline double getBoxNosRatio( int part, int segment, int layer ) const;
    inline double getBoxClst( int part, int segment, int layer, double prob ) const;
    
    inline double getStripEff( int part, int segment, int layer, int strip ) const;
    inline double getStripCnt( int part, int segment, int layer, int strip ) const;
    inline double getStripNos( int part, int segment, int layer, int strip ) const;
    inline double getStripNosRatio( int part, int segment, int layer, int strip ) const;

  private:

    IDataProviderSvc* m_pCalibDataSvc;
    int     m_fConfigMode;
    double  m_fUniformEff;
    double  m_fUniformCnt;
    double  m_fUniformNos;
    double  m_fUniformNosRatio;
    double  m_fUniformClst;
};

#endif // MUC_CALIB_CONST_SVC_H_ 
