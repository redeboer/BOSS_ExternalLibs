#ifndef MDCAliRECTRK_H
#define MDCAliRECTRK_H

#include "MdcAlignAlg/MdcAliRecHit.h"

#include "EventModel/Event.h"
#include "RawEvent/RawDataUtil.h"

#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ReconEvent/ReconEvent.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"


class MdcAliRecTrk {
public:
     ~MdcAliRecTrk();

     void    setRecTrk(RecMdcTrackCol::iterator it_trk);
     void    setKalTrk(RecMdcKalTrackCol::iterator it_trk);
     int     getStat()  const {return m_stat;}
     double  getDr()    const { return m_dr; }
     double  getPhi0()  const { return m_phi0; }
     double  getKappa() const { return m_kappa; }
     double  getDz()    const { return m_dz; }
     double  getTanLamda() const { return m_tanl; }
     HepVector getHelix() const { return m_helix; }
     HepSymMatrix getHelixErr() const { return m_helixerr; }
     double  getPt()    const { return m_pt; }
     double  getP()     const { return m_p; }
     double  getChisq() const { return m_chisq; }
     int     getNHits() const { return m_nhits; }
     MdcAliRecHit* getRecHit(int index) const { return m_rechit[index]; }

private:
     double m_dr;
     double m_phi0;
     double m_kappa;
     double m_dz;
     double m_tanl;
     HepVector m_helix;
     HepSymMatrix m_helixerr;
     double m_pt;
     double m_p;
     double m_chisq;
     int    m_stat;//added by lut
     int    m_nhits;
     std::vector<MdcAliRecHit*> m_rechit;
     static const double m_df;
};

#endif /* MDCAliRECTRK_H */
