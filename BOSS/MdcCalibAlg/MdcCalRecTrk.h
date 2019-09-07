#ifndef MDCCALRECTRK_H
#define MDCCALRECTRK_H

#include "MdcCalibAlg/MdcCalRecHit.h"

#include "EventModel/Event.h"
#include "RawEvent/RawDataUtil.h"

#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ReconEvent/ReconEvent.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcUtilitySvc/MdcUtilitySvc.h"

class MdcCalRecTrk {
public:
     MdcCalRecTrk(int pid);
     ~MdcCalRecTrk();

     void    setGeomSvc(IMdcGeomSvc* mdcGeomSvc) { m_mdcGeomSvc = mdcGeomSvc; }
     void    setUtilSvc(IMdcUtilitySvc* mdcUtilitySvc) { m_mdcUtilitySvc = mdcUtilitySvc; }

     void    setRecTrk(RecMdcTrackCol::iterator it_trk);
     void    setKalTrk(RecMdcKalTrackCol::iterator it_trk);

     double  getDr()    const { return m_dr; }
     double  getPhi0()  const { return m_phi0; }
     double  getKappa() const { return m_kappa; }
     double  getDz()    const { return m_dz; }
     double  getTanLamda() const { return m_tanl; }
     double  getPt()    const { return m_pt; }
     double  getP()     const { return m_p; }
     double  getChisq() const { return m_chisq; }
     int     getNHits() const { return m_nhits; }
     HepLorentzVector getP4() const {return m_p4;}
     MdcCalRecHit* getRecHit(int index) const { return m_rechit[index]; }
     HepVector getHelix() const { return m_helix; }
     HepSymMatrix getHelixErr() const { return m_helixerr; }

     bool m_fgNoiseRatio;
     bool fgNoiseRatio(double phi0);
     bool getFgNoiseRatio() const { return m_fgNoiseRatio; }

private:
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcUtilitySvc* m_mdcUtilitySvc;

     int    m_pid;
     double m_dr;
     double m_phi0;
     double m_kappa;
     double m_dz;
     double m_tanl;
     double m_pt;
     double m_p;
     double m_chisq;
     int    m_nhits;
     HepLorentzVector m_p4;
     std::vector<MdcCalRecHit*> m_rechit;
     HepVector m_helix;
     HepSymMatrix m_helixerr;
};

#endif /* MDCCALRECTRK_H */
