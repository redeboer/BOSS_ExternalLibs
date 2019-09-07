#ifndef MDCALIEVENT_H
#define MDCALIEVENT_H

#include "MdcAlignAlg/MdcAliRecTrk.h"
#include "MdcAlignAlg/MdcAliRecHit.h"
#include "MdcAlignAlg/MdcAliParams.h"

#include <vector>

using namespace std;

class StatusCode;
class BesEvent;

class MdcAliEvent{
public:
     void setParam(MdcAliParams& param) { m_param = param; }
     StatusCode setRecEvent();
     StatusCode setKalEvent();

     int getEsFlag() const { return m_esFlag; }
     bool getEsCutFlag() const { return m_esCutFg; }
     double getTes() const { return m_tes; }

     int getNTrk() const { return m_rectrk.size(); }
     MdcAliRecTrk* getRecTrk(int index) const { return m_rectrk[index]; }

     void clear();

private:
     MdcAliParams m_param;
     int m_esFlag;
     bool m_esCutFg;
     double m_tes;
     std::vector<MdcAliRecTrk*> m_rectrk;
};

#endif /* MDCAliEVENT_H */
