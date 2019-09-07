/*
 * @class : RecMdcKalHelixSeg
 *
 * @author: wangjk
 * ********************************************************/

#ifndef RECMDCKALHELIXSEG_H
#define RECMDCKALHELIXSEG_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/ObjectVector.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "EventModel/EventModel.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif


using namespace CLHEP;

extern const CLID& CLID_RecMdcKalHelixSeg;  

class RecMdcKalTrack;

class RecMdcKalHelixSeg : virtual public ContainedObject {

 public:
   virtual const CLID& clID() const   {
      return RecMdcKalHelixSeg::classID();
   }
   
   static const CLID& classID()       {
      return CLID_RecMdcKalHelixSeg;
   }                     
   //constrcctor and destructor
   RecMdcKalHelixSeg();
   RecMdcKalHelixSeg(RecMdcKalHelixSeg& seg);
   ~RecMdcKalHelixSeg();

   //extractors
   int  getTrackId(void) const { return m_trackid; }
   Identifier  getMdcId(void)  const { return m_mdcid; }
   int  getFlagLR(void) const { return m_flagLR; }
   double getTdc(void) const { return m_tdc; }
   double getAdc(void) const { return m_adc; }
   double getZhit(void) const { return m_zhit; }
   double getTof(void) const{ return m_tof; }
   double getDocaIncl(void) const { return m_doca_incl ; }
   double getDocaExcl(void) const { return m_doca_excl ; }
   double getDD(void) const  { return m_dd; };
   double getEntra(void) const  { return m_entra; }
   double getDT(void) const { return m_dt; }
   int  getLayerId(void) const {return m_layerId; }

   /// get every component of helix parameter
   double getDrIncl  ( void ) const { return m_helix_incl[0];   }
   double getFi0Incl ( void ) const { return m_helix_incl[1];   }
   double getCpaIncl ( void ) const { return m_helix_incl[2];   }
   double getDzIncl  ( void ) const { return m_helix_incl[3];   }
   double getTanlIncl( void ) const { return m_helix_incl[4];   }

   double getDrExcl  ( void ) const { return m_helix_excl[0];   }
   double getFi0Excl ( void ) const { return m_helix_excl[1];   }
   double getCpaExcl ( void ) const { return m_helix_excl[2];   }
   double getDzExcl  ( void ) const { return m_helix_excl[3];   }
   double getTanlExcl( void ) const { return m_helix_excl[4];   }
   
   HepVector& getHelixIncl( void )    { return m_helix_incl;   }
   HepVector& getHelixExcl( void )    { return m_helix_excl;   }
   
   HepSymMatrix& getErrorIncl( void ) { return m_error_incl;   }
   HepSymMatrix& getErrorExcl( void ) { return m_error_excl;   }
   double getResIncl(void)            { return m_res_incl;     }
   double getResExcl(void)            { return m_res_excl;     }

   
   // modifiers
   void setTrackId(int trackid)  { m_trackid = trackid ;}
   void setMdcId(Identifier mdcid)  { m_mdcid = mdcid;}
   void setFlagLR(int flagLR) { m_flagLR =flagLR;}
   void setTdc(double tdc)  { m_tdc = tdc; }
   void setAdc(double adc)  { m_adc = adc; }
   void setZhit(double zhit) { m_zhit = zhit; } 
   void setTof(double tof)  { m_tof = tof; }
   void setDocaIncl(double doca) { m_doca_incl = doca; }
   void setDocaExcl(double doca) { m_doca_excl = doca; }
   
   void setDD(double dd) { m_dd = dd; }
   void setEntra(double entra) { m_entra = entra; }
   void setDT(double dt) { m_dt = dt; }
   void setLayerId(int layerId)  {  m_layerId = layerId;}
   
   void setDrIncl(double dr)     { m_helix_incl[0] = dr;     }
   void setFi0Incl(double fi0)   { m_helix_incl[1] = fi0;    }
   void setCpaIncl(double cpa)   { m_helix_incl[2] = cpa;    }
   void setDzIncl(double dz)     { m_helix_incl[3] = dz;     }
   void setTanlIncl(double tanl) { m_helix_incl[4] = tanl;   }
 
   void setDrExcl(double dr)     { m_helix_excl[0] = dr;     }
   void setFi0Excl(double fi0)   { m_helix_excl[1] = fi0;    }
   void setCpaExcl(double cpa)   { m_helix_excl[2] = cpa;    }
   void setDzExcl(double dz)     { m_helix_excl[3] = dz;     }
   void setTanlExcl(double tanl) { m_helix_excl[4] = tanl;   }
   
   void setResIncl(double res) {m_res_incl = res; }
   void setResExcl(double res) {m_res_excl = res; }

   void setHelixIncl(const HepVector& helix)  {
      m_helix_incl = helix;
   }
   void setErrorIncl(const HepSymMatrix& error) { 
      m_error_incl = error;
   }
   void setHelixExcl(const HepVector& helix)  {
     m_helix_excl = helix;
   }
   void setErrorExcl(const HepSymMatrix& error) {
     m_error_excl = error;
   }
   
   void setHelixIncl(double* helix)  {
      for(int i=0; i<5; i++) {
        m_helix_incl[i] = helix[i]; 
      }
   }
   void setErrorIncl(double* error) {
      int k=0;
      for(int i=0; i<5 ; i++) {
	 for(int j=0; j<=i; j++,k++) {
         m_error_incl[i][j] = error[k];
         m_error_incl[j][i] = error[k];	    
        }      
      }
   }
   
   void setHelixExcl(double* helix)  {
     for(int i=0; i<5; i++) {
       m_helix_excl[i] = helix[i];
     }
   }
   
   void setErrorExcl(double* error) {
     int k=0;
     for(int i=0; i<5 ; i++) {
       for(int j=0; j<=i; j++,k++) {
	 m_error_excl[i][j] = error[k];
	 m_error_excl[j][i] = error[k];
       }
     }
   }
 
 
 private:

   int m_trackid;
   Identifier m_mdcid;
   int m_flagLR;
   double m_tdc;
   double m_adc;
   double m_zhit;
   double m_tof;
   double m_doca_incl;
   double m_doca_excl;
   double m_dd;
   double m_entra;
   double m_dt;
   double m_res_incl;
   double m_res_excl;
   HepVector m_helix_incl; // 5 track parameters 
   HepVector m_helix_excl; // 5 track parameters 
  
   HepSymMatrix m_error_incl; // error matrix 
   HepSymMatrix m_error_excl; // error matrix 
   
   int m_layerId;

};
typedef ObjectVector<RecMdcKalHelixSeg> RecMdcKalHelixSegCol;
 

#endif
   
