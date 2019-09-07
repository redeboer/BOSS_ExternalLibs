/*
 * @class : DstMdcKalTrack
 *
 *  this class models "Kalman Track" from Mdc 
 * 
 * ********************************************************/

#ifndef DSTMDCKALTRACK_H
#define DSTMDCKALTRACK_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <vector>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using namespace CLHEP;

using namespace EventModel;
using CLHEP::HepVector;
using CLHEP::HepSymMatrix;
extern const CLID& CLID_DstMdcKalTrack;  


class DstMdcKalTrack : virtual public ContainedObject {

 public:
   virtual const CLID& clID() const   {
      return DstMdcKalTrack::classID();
   }
   
   static const CLID& classID()       {
      return CLID_DstMdcKalTrack;
   }                     
   //cconstructor and destructor
   DstMdcKalTrack();
   DstMdcKalTrack(const DstMdcKalTrack& track);
   DstMdcKalTrack& operator=(const DstMdcKalTrack&);
   ~DstMdcKalTrack();

   enum PidType
   {
     null = -1,
     electron = 0,
     muon = 1,
     pion = 2,
     kaon = 3,
     proton = 4
   };
   
   static void setPidType(PidType pidType) {m_pidType = pidType; } 
   static PidType getPidType() {return m_pidType; }
   
   //extractors
   const int trackId()  const { return m_trackId;          }
   const double mass()  const { return m_mass[m_pidType];  }
   const int charge()   const { return m_charge[m_pidType];}
   const double pxy()   const { return m_pxy[m_pidType];   } 
   const double px()    const { return m_px[m_pidType];    }
   const double py()    const { return m_py[m_pidType];    }
   const double pz()    const { return m_pz[m_pidType];    }
   const double theta() const { return m_theta[m_pidType]; }
   const double phi()   const { return m_phi[m_pidType];   }
     
   const double x()     const { return m_x[m_pidType];     }   
   const double y()     const { return m_y[m_pidType];     }
   const double z()     const { return m_z[m_pidType];     }	  

   const double x(const int i)  const {return m_x[i];}
   const double y(const int i)  const {return m_y[i];}
   const double z(const int i)  const {return m_z[i];}
       
   const double r()           const {return m_r[m_pidType];}
   const double p()           const {return m_p[m_pidType];}
   const int    stat()        const {return m_stat[0][m_pidType];}
   const double chi2()        const {return m_chisq[0][m_pidType];}
   const int    ndof()        const {return m_ndf[0][m_pidType];}
   const int    nster()       const {return m_nster[0][m_pidType];}
   const int    firstLayer()  const {return m_firstLayer[0][m_pidType];}
   const int    lastLayer()   const {return m_lastLayer[0][m_pidType];}
   const int    nlayer()      const {return m_nlayer[m_pidType];}
   
   const double  dr( void )   const { return m_zhelixs[m_pidType][0];   }
   const double fi0( void )   const { return m_zhelixs[m_pidType][1];   }
   const double kappa( void ) const { return m_zhelixs[m_pidType][2];   }
   const double dz( void )    const { return m_zhelixs[m_pidType][3];   }
   const double tanl( void )  const { return m_zhelixs[m_pidType][4];   }

   const HepVector& helix()    const { return m_zhelixs[m_pidType];     }
   const HepSymMatrix& err()   const { return m_zerrors[m_pidType];     }
   const HepVector& fhelix()    const { return m_fhelixs[m_pidType];     }
   const HepSymMatrix& ferr()   const { return m_ferrors[m_pidType];     }
   const HepPoint3D poca()     const { return m_pocas[m_pidType];       }
   const Hep3Vector p3()       const;
   const HepPoint3D x3()       const;
   const HepLorentzVector p4() const;
   const HepLorentzVector p4(double mass) const;
   
//modifiers
   void setTrackId(int trackId)                    { m_trackId = trackId;   }
   void setMass(double mass, int pid)               { m_mass[pid] = mass;    }
   void setCharge(const int charge, const int pid) { m_charge[pid] = charge;}
   void setPxy(const double pxy, const int pid)    { m_pxy[pid] = pxy;      }
   void setPx(const double px, const int pid)      { m_px[pid] = px;        }
   void setPy(const double py, const int pid)      { m_py[pid] = py;        }
   void setPz(const double pz, const int pid)      { m_pz[pid] = pz;        }
   void setP(const double p, const int pid)        { m_p[pid] = p;          }
   void setTheta(const double theta,const int pid) { m_theta[pid] = theta;  }
   void setPhi(const double phi, const int pid)    { m_phi[pid] = phi;      }
   void setX(const double x, const int pid)        { m_x[pid] = x;          }
   void setY(const double y,const int pid)         { m_y[pid] = y;          }
   void setZ(const double z, const int pid)        { m_z[pid] = z;          }
   void setR(const double r, const int pid)        { m_r[pid] = r;          }
   void setNlayer(int nlayer, int pid)             { m_nlayer[pid] = nlayer;    }
   void setNster(int ns,  int i, int pid)          { m_nster[i][pid] = ns;     }
   void setStat(int stat, int i, int pid)          { m_stat[i][pid] = stat;    }
   void setChisq(double chisq, int i, int pid)      { m_chisq[i][pid] = chisq;  }
   void setFirstLayer(int fL, int i, int pid)      { m_firstLayer[i][pid] = fL; }
   void setLastLayer(int lL, int i, int pid)       { m_lastLayer[i][pid] = lL;  }
   void setNhits(int nhits,  int pid)              { m_nhits[pid] = nhits;   }  
   void setNdf(int ndf, int i, int pid)            { m_ndf[i][pid] = ndf;    }

   void setPoca(const HepPoint3D& poca, const int pid){
     m_pocas[pid] = poca;
   }

   void setPoca(double*  poca, const int pid){
     for(int i=0; i<3; i++){
       m_pocas[pid][i] = poca[i];
     }                                                              
   }
   
   void setZHelix(const HepVector& helix, const int pid)  {
     m_zhelixs[pid] = helix;
   }

   void setZError(const HepSymMatrix& error, const int pid) { 
      m_zerrors[pid] = error;
   }
   void setZHelix(double* helix, const int pid)  {
      for(int i=0; i<5; i++) {
        m_zhelixs[pid][i] = helix[i]; 
      }
   }

   void setZError(double* error, const int pid) {
      int k=0;
      HepSymMatrix mat(5);
      for(int i=0; i<5 ; i++) {
	 for(int j=0; j<=i; j++,k++) {
         mat[i][j] = error[k];
         mat[j][i] = error[k];	    
        }      
      }
      m_zerrors[pid] = mat;
   }

   void setFHelix(const HepVector& fhelix, const int pid)  {
     m_fhelixs[pid] = fhelix;
   }

   void setFError(const HepSymMatrix& ferror, const int pid) { 
      m_ferrors[pid] = ferror;
   }
   void setFHelix(double* fhelix, const int pid)  {
      for(int i=0; i<5; i++) {
        m_fhelixs[pid][i] = fhelix[i]; 
      }
   }

   void setFError(double* ferror, const int pid) {
      int k=0;
      HepSymMatrix mat(5);
      for(int i=0; i<5 ; i++) {
	 for(int j=0; j<=i; j++,k++) {
         mat[i][j] = ferror[k];
         mat[j][i] = ferror[k];	    
        }      
      }
      m_ferrors[pid] = mat;
   }
  const int getTrackId()                 const { return m_trackId;        } 
  const int getCharge(const int pid)     const { return m_charge[pid];    }   
  const int getStat( const int pid)      const { return m_stat[0][pid];      }
  const int  getNster(const int pid)     const { return m_nster[0][pid];     }
  const double getChisq(const int pid)   const { return m_chisq[0][pid];     }
  const int getNdf(const int pid)        const { return m_ndf[0][pid];       }
  const int getFirstLayer(const int pid) const { return m_firstLayer[0][pid];}
  const int getLastLayer(const int pid)  const { return m_lastLayer[0][pid]; }
  const int getNlayer(const int pid)  const { return m_nlayer[pid]; }

  
  const HepPoint3D& getPoca(const int pid) const{ 
    return m_pocas[pid];
  }
  const HepVector& getZHelix(const int pid) const{
     return m_zhelixs[pid];
   } 
  const HepSymMatrix& getZError(const int pid) const{
     return m_zerrors[pid];
   }
  const HepVector& getFHelix(const int pid) const{
     return m_fhelixs[pid];
   } 
  const HepSymMatrix& getFError(const int pid) const{
     return m_ferrors[pid];
   }
 
   
 protected:
   static PidType m_pidType; 
   int    m_trackId;   // Track ID wensp add 2005-10-18
   int    m_charge[5];
   int    m_stat[2][5];
   int    m_nster[2][5];
   int    m_firstLayer[2][5];
   int    m_lastLayer[2][5];
   double m_mass[5];
   double m_chisq[2][5]; 
   double m_pxy[5];
   double m_px[5];
   double m_py[5];
   double m_pz[5];
   double m_p[5];
   double m_theta[5];
   double m_phi[5];
   double m_x[5];
   double m_y[5];
   double m_z[5];
   double m_r[5];
   //double m_chi2[2][5];  // chi square of forward filter 
   int    m_ndf[2][5];    // degree of freedom of forward filter
   int    m_nhits[5];
   int    m_nlayer[5];
     
   std::vector<HepPoint3D>   m_pocas;   // 
   std::vector<HepVector>    m_zhelixs; // 
   std::vector<HepSymMatrix> m_zerrors; // 
   std::vector<HepVector>    m_fhelixs; // 
   std::vector<HepSymMatrix> m_ferrors; // 
};

typedef ObjectVector<DstMdcKalTrack> DstMdcKalTrackCol;

#endif
   
