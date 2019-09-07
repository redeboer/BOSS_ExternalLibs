#ifndef SING_TAG_H
#define SING_TAG_H

#include "GaudiKernel/SmartDataPtr.h"
#include "EvtRecEvent/EvtRecEvent.h" 
#include "EvtRecEvent/EvtRecTrack.h" 
#include "DstEvent/TofHitStatus.h" 

#include "ParticleID/ParticleID.h"
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Point3D.h"

typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<HepLorentzVector> Vp4;

class Sing
{
  public:
    Sing();
   ~Sing();

  public:
    inline bool   Getoktg() { return oktg; }
    inline double Gettagmd() { return tagmd_temp; }
    inline double Getmass_bc() { return mass_bc_temp; } 
    inline double GetCQtm() { return cqtm_temp; }
    inline double GetdelE_tag() { return delE_tag_temp; }
    inline Vint Gettagtrk1() { return iGoodtag_temp; }
    inline HepLorentzVector Gettagp1() { return ptag_temp; }
    inline Vint GettagGam1() { return iGamtag_temp; }

  public:
    void Mdset(double event, SmartDataPtr<EvtRecTrackCol> evtRecTrkCol,Vint iGood,Vint iGam,int mdset, double Ebeam, int PID_flag, int Charge_candidate_D);
  
  protected:
    bool   oktg;
    double  tagmd_temp;
    double  mass_bc_temp;
    double  cqtm_temp;
    double  delE_tag_temp;
    Vint iGoodtag_temp;
    HepLorentzVector ptag_temp;
    Vint iGamtag_temp;

};

#endif
