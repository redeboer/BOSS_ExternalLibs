/*
 *
 *  this class models "Hit" used in Mdc Reconstruction
 * 
 * 
 * ********************************************************/

#ifndef RECMDCHIT_H
#define RECMDCHIT_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"  
#include "GaudiKernel/ObjectVector.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"


extern const CLID& CLID_RecMdcHit;

class RecMdcTrack;

class RecMdcHit : virtual public ContainedObject {

 public:
   virtual const CLID& clID() const   {
      return RecMdcHit::classID();     
   }                                   

   static const CLID& classID()       {
      return CLID_RecMdcHit;
   }
   
 //ctor and dector
   RecMdcHit();
   RecMdcHit(const RecMdcHit& rechit );
   ~RecMdcHit();
 // is this hit grouped to any track?  
   bool isGrouped(void); 
 //extractors
   const int    getId(void)                const { return m_id;    }
   const  int   getTrkId(void)             const { return m_trkid; }   
   const double getDriftDistLeft(void)     const { return m_ddl;   }
   const double getDriftDistRight(void)    const { return m_ddr ;  }
   const double getErrDriftDistLeft(void)  const { return m_erddl; }
   const double getErrDriftDistRight(void) const { return m_erddr; }
   const double getChisqAdd(void)          const { return m_pChisq;}
   const int    getFlagLR(void)            const { return m_lr;    }
   const int    getStat(void)              const { return m_stat;  }
   const Identifier getMdcId(void)         const { return m_mdcid; }
   const double getTdc(void)               const { return m_tdc;   }
   const double getAdc(void)               const { return m_adc;   }
   const double getDriftT(void)            const { return m_driftT;}      
   const double getDoca(void)              const { return m_doca;  }      
   const double getEntra(void)             const { return m_entra; }     
   const double getZhit(void)              const { return m_zhit;  }      
   const double getFltLen(void)            const { return m_fltLen;}
//modifiers
   void setId(int id)                      { m_id = id;        }
   void setTrkId(int trkid)                { m_trkid = trkid;  }
   void setDriftDistLeft(double ddl)       { m_ddl = ddl;      }
   void setDriftDistRight(double ddr)      { m_ddr = ddr ;     }
   void setErrDriftDistLeft(double erddl)  { m_erddl = erddl;  }
   void setErrDriftDistRight(double erddr) { m_erddr = erddr;  }
   void setChisqAdd(double pChisq)         { m_pChisq = pChisq;}
   void setFlagLR(int lr)                  { m_lr = lr;        }
   void setStat(int stat)                  { m_stat = stat;    }
   void setMdcId( Identifier mdcid )       { m_mdcid = mdcid;  }
   void setTdc(double tdc)                 { m_tdc = tdc;      }
   void setAdc(double adc)                 { m_adc = adc;      }
   void setDriftT(double driftT)           { m_driftT = driftT;}      
   void setDoca(double doca)               { m_doca = doca;    }      
   void setEntra(double entra)             { m_entra = entra;  }      
   void setZhit(double zhit)               { m_zhit = zhit;    }      
   void setFltLen(double fltLen)           { m_fltLen = fltLen;}
 
   
 private:

   int m_id;           // the id of thie hit
   int m_trkid;        // id of track containing this hit,if this hit 
                       // belongs to no track, set as -1;
   double m_ddl;       // drift distance left
   double m_ddr;       // drift distance right
   double m_erddl;     // error of drift distance left
   double m_erddr;     // error of drift distance right
   double m_pChisq;    // contribution to chisquare
   int m_lr;           // flag indicating left or right 0:left 1:right 2:ambig
   int m_stat;         // status flag
   Identifier m_mdcid; // MDC identifier
   double m_tdc;       // corrected TDC
   double m_adc;       // corrected ADC
   double m_driftT;    // drift time
   double m_doca;      // distance of closesest approach for helix in the cell
   double m_entra;     // entrance angle in azimuth
   double m_zhit;      // z coordinate of the hit
   double m_fltLen;    // flight length of this hit
};

typedef ObjectVector<RecMdcHit> RecMdcHitCol; 

#endif
   
