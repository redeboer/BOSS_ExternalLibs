/*
 * @class : MdcDedx
 *
 *  this class models "dE/dx" used in Mdc Reconstruction
 * 
 * @author: wangdy
 * 
 * ********************************************************/

#ifndef RECMDCDEDX_H
#define RECMDCDEDX_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h" 
#include "RecMdcTrack.h"
#include "RecMdcKalTrack.h"
#include "RecMdcDedxHit.h"


//#include "Identifier/Identifier.h"
//#include "ExtEvent/ExtTrack.h"
#include "DstEvent/DstMdcDedx.h" 

extern const CLID& CLID_RecMdcDedx;     
typedef SmartRefVector<RecMdcDedxHit>  DedxHitRefVec;

class RecMdcDedx : virtual public DstMdcDedx {
  
 public:
   virtual const CLID& clID() const   { 
      return RecMdcDedx::classID();
   }                                                                           
                                                                                  
   static const CLID& classID()       { 
      return CLID_RecMdcDedx;
   }                                                                           
                                                                     
   
   //ctor and dector
   RecMdcDedx();
   RecMdcDedx(const RecMdcDedx& dedxtrk);
   RecMdcDedx(const DstMdcDedx& dedxtrk);
   RecMdcDedx& operator=(const RecMdcDedx&);
   RecMdcDedx& operator=(const DstMdcDedx&);
   
   ~RecMdcDedx();
    //extractors
    // int getId(void) const { return m_id;  }
    // int getPid(void) const { return m_pid;  }
    // int getStat(void) const { return m_stat;   }
    // int getNHits(void) const { return m_nhits;   }
    // float getDedx(void) const { return m_dedx;  }
    // float getDedxStd(void) const { return m_dedx_std;  }
    // int getBitrunc(void) const { return m_bitrunc;  }
    
    bool isMdcTrackValid(void) {return (m_mdcTrack!=0);}
    bool isMdcKalTrackValid(void) {return (m_mdcKalTrack!=0);}

    DedxHitRefVec getVecDedxHits() const {return m_vecdedxhit;}
   
    double getDedxHit(void)     {return m_dedx_hit;     }
    double getDedxEsat(void)    {return m_dedx_esat;    }
    double getDedxNoRun(void)   {return m_dedx_norun;   }
    double getDedxMoment(void)      {return m_dedx_momentum;     }      
                                                   
    double getDedxExpect(int pid) const { return m_dedx_exp[pid];   }
    double getSigmaDedx(int pid) const { return m_sigma_dedx[pid];   }
    double getPidProb(int pid) const { return m_pid_prob[pid];   }
    // double getChi(int pid) const { return m_chi[pid];   }
    RecMdcTrack* getMdcTrack(void)  { return m_mdcTrack; }
    RecMdcKalTrack* getMdcKalTrack(void) {return m_mdcKalTrack; }
    
    //modifiers
    void setVecDedxHits(const DedxHitRefVec& vecdedxhit) {m_vecdedxhit = vecdedxhit;}
    
    void setDedxHit(double dedx_hit)     { m_dedx_hit = dedx_hit;     }
    void setDedxEsat(double dedx_esat)   { m_dedx_esat = dedx_esat;   }
    void setDedxNoRun(double dedx_norun) { m_dedx_norun = dedx_norun; }
    void setDedxMoment(double dedx_momentum){ m_dedx_momentum = dedx_momentum;     } 
    // void setId(int id) { m_id = id;   }
    // void setPid(int pid) { m_pid = pid;   }
    // void setStat(int stat) { m_stat = stat;    }
    // void setNHits(int nhits) { m_nhits = nhits;    }
    // void setDedx(float dedx) { m_dedx = dedx; }
    // void setDedxStd(float dedx_std) { m_dedx_std = dedx_std; }
    // void setBitrunc(int bitrunc) { m_bitrunc = bitrunc;   }
   
    void setDedxExpect(double* dedx_exp) {
      for(int i = 0; i < 5; i++)
       m_dedx_exp[i] = dedx_exp[i]  ;
    }
    void setSigmaDedx(double* sigma_dedx) {
      for(int i = 0; i < 5; i++)
       m_sigma_dedx[i] = sigma_dedx[i] ;
    }
    void setPidProb(double* pid_prob) {
      for(int i = 0; i < 5; i++)
       m_pid_prob[i] = pid_prob[i] ;
    }
    /* void setChi(double* chi) {
      for(int i = 0; i < 5; i++)
       m_chi[i] = chi[i] ;
       }
     */
    void setMdcTrack(RecMdcTrack* trk) {m_mdcTrack = trk; }
    void setMdcKalTrack(RecMdcKalTrack* trk) {m_mdcKalTrack = trk;}			
    
private:
    DedxHitRefVec m_vecdedxhit;
    
    double  m_dedx_hit;   //double trucation mean of hit-level calib
    double  m_dedx_esat;  //double trucation mean of hit-level+electron saturation calib
    double  m_dedx_norun; //double trucation mean of no run gain calib  
    double  m_dedx_momentum;   //dE/dx calib used momentum
    //   int m_bitrunc;   //bitrunction rate
    //   int m_id;
    //   int m_pid;      // partile id results
    //   int m_stat;      // status flag     			 
    //   int m_nhits;      // number of hits used in dE/dx recon
    //   float m_dedx;        // measured value of dE/dx
    //   float m_dedx_std;     // normalized dE/dx;
    double m_dedx_exp[5];    // expected value of dE/dx for 5 particle hypotheses
    double m_sigma_dedx[5]; // sigma value of dE/dx for 5 particle hypotheses
    double m_pid_prob[5]; // probability for each of  the 5 particle hypotheses
    //  double m_chi[5];     // chi of dE/dx for  each of  5 particle hypotheses
    SmartRef<RecMdcTrack> m_mdcTrack;    // reference to the   track 
    SmartRef<RecMdcKalTrack> m_mdcKalTrack;
};

typedef ObjectVector<RecMdcDedx> RecMdcDedxCol; 

#endif
   
