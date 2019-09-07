#ifndef MDCAlign_H
#define MDCAlign_H

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"

#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"

#include "MdcAlignAlg/MdcAlignPar.h"
#include "MdcAlignAlg/MdcAliEvent.h"

#include "TObjArray.h"

class MdcAlign {
public:
    MdcAlign();
    virtual ~MdcAlign();
    virtual void clear() = 0;
    virtual void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			    IMdcCalibFunSvc* mdcFunSvc) = 0;
		
    virtual void setParam(MdcAliParams& param) = 0;
    virtual bool fillHist(MdcAliEvent* event) = 0;
    virtual void updateConst(MdcAlignPar* alignPar) = 0;
public:
     std::string fixMomLab;
private:
     MdcAliParams m_param;

};

inline void MdcAlign::setParam(MdcAliParams& param){
     m_param = param;
}

#endif /* MDCAlign_H */
