#ifndef IEMCCALIBCONSTSVC_H_
#define IEMCCALIBCONSTSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "EmcRecGeoSvc/IEmcRecGeoSvc.h"
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "EmcGeneralClass/EmcStructure.h"

/* Declaration of the interface ID */
static const InterfaceID IID_IEmcCalibConstSvc("IEmcCalibConstSvc",1,0);

class IEmcCalibConstSvc: virtual public IInterface{
 public:
  static const InterfaceID& interfaceID() { return IID_IEmcCalibConstSvc; }
  
  virtual double getDigiCalibConst(int No) const =0; 

   virtual int getIxtalNumber(int No) const =0; 
  
  //get the size of the vectors
  virtual int getDigiCalibConstNo() const =0; 
  virtual int getIxtalNumberNo() const =0; 
  
  //getPartID()=0, or 1, or 2, here endcap_east(0), barrel(1), endcap_west(2) 
  virtual int getIndex( unsigned int PartId, 
			unsigned int ThetaIndex, 
			unsigned int PhiIndex)  const =0;
  virtual unsigned int getPartID(int Index) const=0;
  virtual unsigned int getThetaIndex( int Index)  const =0;
  virtual unsigned int getPhiIndex( int Index)  const =0;
 
  virtual double getCrystalEmaxData( int Index)  const =0;
 
  virtual void Dump() = 0;
 public:
  EmcStructure *m_theEmcStruc; 
  double m_CrystalEmaxData[6240];
 private:

   //    IEmcRecGeoSvc* m_emcRecGeoSvc;
};

#endif /* IEMCCALIBCONSTSVC_H_ */
