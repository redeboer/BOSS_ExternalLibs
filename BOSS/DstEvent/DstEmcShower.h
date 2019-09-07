#ifndef DST_EMCSHOWER_H
#define DST_EMCSHOWER_H
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include "EventModel/EventModel.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include <vector>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using namespace CLHEP;

using namespace std;
using namespace EventModel;
extern  const CLID  &CLID_DstEmcShower;

class DstEmcShower : virtual public  ContainedObject {
   public:
     DstEmcShower() {}
     virtual ~DstEmcShower() {}

     virtual const CLID& clID()  const { return  DstEmcShower::classID();}
     static  const CLID& classID() { return CLID_DstEmcShower; }
     
     int trackId() const {return m_trackId ;}
     int numHits() const { return m_numHits;   }  
     int status() const { return m_status; }
     int cellId() const { return m_cellId; }
     int module() const { return m_module; }
     HepPoint3D position() const { return m_position; }
     double x() const { return m_position.x(); }
     double y() const { return m_position.y(); }
     double z() const { return m_position.z(); }
     double theta() const { return  m_position.theta(); }
     double phi() const { return m_position.phi(); }
     double dx() const; 
     double dy() const; 
     double dz() const; 
     double dtheta() const {  return  m_dTheta; }
     double dphi()   const {  return  m_dPhi;   }
     double energy()      const {  return  m_energy;      }
     double dE()     const { return  m_dE;     }
     double eSeed()  const { return m_eSeed; }
     double e3x3()  const { return m_e3x3; }
     double e5x5()  const { return m_e5x5; }
     double time()  const { return m_time; }
     double secondMoment() const {return m_secondMoment;}
     double latMoment() const {return m_latMoment;}
     double a20Moment() const {return m_a20Moment;}
     double a42Moment() const {return m_a42Moment;}
     HepSymMatrix errorMatrix() const { return m_errorMatrix; }
     
     void setTrackId(int trackId) {m_trackId = trackId;}
     void setNumHits( int hit )  { m_numHits   = hit;     }
     void setStatus(int st )  { m_status = st;      }
     void setCellId(int id )  { m_cellId = id;      }
     void setModule(int mod)  { m_module = mod;     }
     void setPosition(const HepPoint3D& pos) { m_position=pos; }
     void setEnergy(double e    )  { m_energy  = e ;     }
     void setDE(double de  )  { m_dE = de;          }
     void setDtheta(double dt    )  { m_dTheta = dt;    } 
     void setDphi(double dpi     )  { m_dPhi    = dpi ;  }
     void setESeed(double eSeed     )  { m_eSeed   = eSeed ;  }
     void setE3x3(double e3x3     )  { m_e3x3   = e3x3 ;  }
     void setE5x5(double e5x5     )  { m_e5x5   = e5x5 ;  }
     void setTime(double time     )  { m_time   = time ;  }
     void setSecondMoment(double secondMoment) { m_secondMoment = secondMoment; }
     void setLatMoment(double latMoment) { m_latMoment = latMoment; }
     void setA20Moment(double a20Moment) { m_a20Moment = a20Moment; }
     void setA42Moment(double a42Moment) { m_a42Moment = a42Moment; }
     void setErrorMatrix(const HepSymMatrix& error) { m_errorMatrix = error; }
     
   private:
    int m_trackId;    // Track ID wensp Add 2005-10-19   
    int m_numHits;    // Total number of hits
    int m_status;     // Status: 1:single seed cluster; 2:splitted from multi-seeds cluster
    int m_cellId;     // Cell ID
    int m_module;     // Module: 0:east endcap; 1:barrel; 2:west endcap
    HepPoint3D m_position;  // Shower position
    double m_dTheta;
    double m_dPhi;
    double m_energy;  // Shower energy after correction
    double m_dE;      
    double m_eSeed; //// Energy of seed, only one
    double m_e3x3; //Energy of 3x3 crystals, totally 9 ones
    double m_e5x5; //Energy of 5x5 crystals, totally 25 ones
    double m_time;  //Time measurement
    //The following four moments describe the shower shape
    double m_secondMoment; //add 2006-07-03
    double m_latMoment; //Lateral moment
    double m_a20Moment; //Zernike moment
    double m_a42Moment;
    HepSymMatrix m_errorMatrix;
};

ostream& operator<<(ostream & os, const DstEmcShower& aShower);

typedef ObjectVector<DstEmcShower> DstEmcShowerCol;
#endif //DST_EMCSHOWER_H

