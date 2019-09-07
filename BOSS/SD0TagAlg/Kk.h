#ifndef K_K_H
#define K_K_H

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

class Kk
{
  public:
     Kk();
     ~Kk();

  public:
    inline bool   Getkkmd() { return kkmd; }
    inline double Gettagmd() { return tagmd; }
    inline double Getmass_bc() { return mass_bc; }
    inline double GetCQtm() { return cqtm; }
    inline double GetdelE_tag() { return delE_tag; }
    inline Vint Gettagtrk1() { return iGoodtag; }
    inline HepLorentzVector Gettagp1() { return ptag; }
    inline Vint GettagGam1() { return iGamtag; }
                    
                                    
  public:
     void MTotal(double event,SmartDataPtr<EvtRecTrackCol> evtRecTrkCol, Vint iGood,Vint iGam, double Ebeam, int PID_flag, int Charge_candidate_D);
  
  private:
    bool  kkmd; 
    double tagmd;
    double mass_bc;
    double cqtm;
    double delE_tag;
    Vint   iGoodtag;
    HepLorentzVector    ptag;
    Vint iGamtag;
    
};

#endif
