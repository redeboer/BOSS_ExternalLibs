/*
 * @class : MdcDedx
 *
 *  this class models "dE/dx hit" used in Mdc Reconstruction
 * 
 * @author: xcao
 * 
 * ********************************************************/

#ifndef RECMDCDEDXHIT_H
#define RECMDCDEDXHIT_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h" 
#include "Identifier/Identifier.h"

#include "RecMdcKalHelixSeg.h"
#include "RecMdcHit.h"

extern const CLID& CLID_RecMdcDedxHit;     
class RecMdcDedx;

class RecMdcDedxHit : virtual public ContainedObject {
  
 public:
   virtual const CLID& clID() const   { 
      return RecMdcDedxHit::classID();
   }                                                                           
                                                                                  
   static const CLID& classID()       { 
      return CLID_RecMdcDedxHit;
   }                                                                           
                                                                     
   
   //ctor and dector
   RecMdcDedxHit(); 
   RecMdcDedxHit(const RecMdcDedxHit& recdedxhit );
   ~RecMdcDedxHit();
  // is this hit grouped to any track?  
   bool isGrouped(void);
   bool isMdcHitValid() {return (m_mdcHit!=0);}
   bool isMdcKalHelixSegValid() {return (m_mdcKalHelixSeg!=0);} 
   
  //extractors
  
   RecMdcKalHelixSeg*  getMdcKalHelixSeg() { return m_mdcKalHelixSeg; } 
   RecMdcHit* getMdcHit() {return m_mdcHit;}
   
   const int getTrkId(void) const { return m_trkid; }
   const int getFlagLR(void) const { return m_lr;    }
   const double getDedx(void) const {return m_dedx; }
   double getPathLength(void) const { return m_pathlength;  }
   const Identifier getMdcId(void)         const { return m_mdcid; }

   //modifiers 
   void setMdcKalHelixSeg(const RecMdcKalHelixSeg* mdcKalHelixSeg) {m_mdcKalHelixSeg  =  mdcKalHelixSeg;}
   void setMdcHit(const RecMdcHit* mdcHit) {m_mdcHit = mdcHit;}
   
   void setTrkId(int trkid) { m_trkid = trkid;  }
   void setFlagLR(int lr) { m_lr = lr;   }
   void setDedx(double dedx ) {m_dedx = dedx; }
   void setPathLength(double pathlength) { m_pathlength = pathlength;   }
   void setMdcId( Identifier mdcid )       { m_mdcid = mdcid;  }  

    private:
   //RecMdcKalHelixSeg m_kalHelixSeg; 
   int m_trkid;      // id of track containing this hit,if this hit belongs to no track, set as -1; 
   int m_lr;           // flag indicating left or right 0:left 1:right 2:ambig
   double m_pathlength;  //path length of track cross a cell
   double m_dedx;
   Identifier m_mdcid; // MDC identifier   

   SmartRef<RecMdcKalHelixSeg> m_mdcKalHelixSeg;
   SmartRef<RecMdcHit> m_mdcHit;
};

typedef ObjectVector<RecMdcDedxHit> RecMdcDedxHitCol; 

#endif

