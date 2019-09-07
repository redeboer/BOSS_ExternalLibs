#ifndef MDCCALEVENT_H
#define MDCCALEVENT_H

#include "MdcCalibAlg/MdcCalRecTrk.h"
#include "MdcCalibAlg/MdcCalRecHit.h"
#include "MdcCalibAlg/MdcCalParams.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcUtilitySvc/MdcUtilitySvc.h"

#include <vector>

class StatusCode;
class BesEvent;

class MdcCalEvent{
public:
     MdcCalEvent();
     ~MdcCalEvent();

     void setParam(MdcCalParams& param) { m_param = param; }
     void setGeomSvc(IMdcGeomSvc* mdcGeomSvc) { m_mdcGeomSvc = mdcGeomSvc; }
     void setUtilSvc(IMdcUtilitySvc* mdcUtilitySvc) { m_mdcUtilitySvc = mdcUtilitySvc; }

     StatusCode setRecEvent();
     StatusCode setKalEvent();

     int getNRawHitTQ() const{ return m_nhitTQ; }
     int getEsFlag() const { return m_esFlag; }
     bool getEsCutFlag() const { return m_esCutFg; }
     int getNesCutFlag() const { return m_nEsCutFg; }
     double getTes() const { return m_tes; }

     int getNTrk() const { return m_rectrk.size(); }
     MdcCalRecTrk* getRecTrk(int index) const { return m_rectrk[index]; }

     void setEvtNoOnline(int evtNo) { m_nEvtOnline = evtNo; }
     int getEvtNoOnline() const { return m_nEvtOnline; }

     void setEvtNoOffline(int evtNo) { m_nEvtOffline = evtNo; }
     int getEvtNoOffline() const { return m_nEvtOffline; }

     void clear();

private:
     MdcCalParams m_param;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcUtilitySvc* m_mdcUtilitySvc;

     int m_nEvtOnline;
     int m_nEvtOffline;
     int m_nhitTQ;
     int m_esFlag;
     bool m_esCutFg;
     int m_nEsCutFg;
     double m_tes;
     std::vector<MdcCalRecTrk*> m_rectrk;
};

#endif /* MDCCALEVENT_H */
