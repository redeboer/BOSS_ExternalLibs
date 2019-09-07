#ifndef EMCCALIBCONSTSVC_H_
#define EMCCALIBCONSTSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "EmcCalibConstSvc/IEmcCalibConstSvc.h"
#include <vector>


class EmcCalibConstSvc: public Service, virtual public IEmcCalibConstSvc{
 public:
  EmcCalibConstSvc( const std::string& name, ISvcLocator* svcloc );
  ~EmcCalibConstSvc();
  
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  IDataProviderSvc* m_calDataSvc;
  
  /* Access by users */
  
  double getDigiCalibConst(int No) const; 
  int  getIxtalNumber(int No) const;  
  
  //get the size of the vectors
  int getDigiCalibConstNo()  const; 
  int getIxtalNumberNo()  const; 
  
  //getPartID()=0, or 1, or 2, here endcap_east(0), barrel(1), endcap_west(2) 
  int getIndex( unsigned int PartId, 
		unsigned int ThetaIndex, 
		unsigned int PhiIndex)  const;
  unsigned int getPartID(int Index) const;
  unsigned int getThetaIndex( int Index)  const ;
  unsigned int getPhiIndex( int Index)  const ; 
  double  getCrystalEmaxData( int Index)  const;
 
  void  Dump();
 public: 
  EmcStructure *m_theEmcStruc;
  double m_CrystalEmaxData[6240];
 private:

  //  IEmcRecGeoSvc* m_emcRecGeoSvc;
 


};

#endif /* EMCCALIBCONSTSVC_H_ */
