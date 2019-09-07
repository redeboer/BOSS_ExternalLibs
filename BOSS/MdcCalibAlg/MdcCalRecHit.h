#ifndef MDCCALRECHIT_H
#define MDCCALRECHIT_H

#include "EventModel/Event.h"
#include "RawEvent/RawDataUtil.h"
#include "EventModel/Event.h"

#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ReconEvent/ReconEvent.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

class MdcCalRecHit {
public:
     void   setRecHit(HitRefVec::iterator it_hit);
     void   setKalHit(HelixSegRefVec::iterator it_hit);

     int    getLayid()  const { return m_layid; }
     int    getCellid() const { return m_cellid; }
     int    getLR()     const { return m_lr; }
     int    getStat()  const { return m_stat; }

     double getDocaInc()   const { return m_docaInc; }
     double getDocaExc()   const { return m_docaExc; }

     double getDmeas() const { return m_dmeas; }
     double getErrDmeas() const { return m_errdmeas; }

     double getResiInc()    const { return m_resiInc; }
     double getResiIncLR()  const { return m_resilrInc; }

     double getResiExc()    const { return m_resiExc; }
     double getResiExcLR()  const { return m_resilrExc; }

     double getTdc()  const { return m_tdc; }
     double getTdrift()   const { return m_tdrift; } 
     double getQhit()     const { return m_qhit; }
     double getZhit()     const { return m_zhit; }
     double getEntra()    const { return m_entra; }
     double getChisqAdd() const { return m_chisqadd; }

     double getTof()      const { return m_tof; }

     double getDr()       const { return m_Dr; }
     double getFi0()      const { return m_Fi0; }
     double getCpa()      const { return m_Cpa; }
     double getDz()       const { return m_Dz; }
     double getTanl()     const { return m_Tanl; }
     HepVector getHelix() const { return m_helix; }
     HepSymMatrix getHelixErr() const { return m_helixerr; }

private:
     int m_layid;
     int m_cellid;
     int m_lr;
     int m_stat;

     double m_docaInc;
     double m_docaExc;

     double m_dmeas;
     double m_errdmeas;

     double m_resiInc;
     double m_resilrInc;

     double m_resiExc;
     double m_resilrExc;

     double m_tdc;
     double m_tdrift;
     double m_qhit;
     double m_zhit;
     double m_entra;
     double m_chisqadd;

     double m_tof;

     double m_Dr;
     double m_Fi0;
     double m_Cpa;
     double m_Dz;
     double m_Tanl;
     HepVector m_helix;
     HepSymMatrix m_helixerr;

     static const double m_df;
     static const double m_qf;
};

#endif /* MDCCALRECHIT_H */
