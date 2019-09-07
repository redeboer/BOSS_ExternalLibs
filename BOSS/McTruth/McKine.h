#ifndef McKine_H
#define McKine_H 
#include <vector>
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_McKine;

/*
 *
 */

class McKine : virtual public ContainedObject{ 
public:
  // Constructor 
  McKine() {};

  // Retrieve reference to class definition structure
  virtual const CLID& clID() const   { return McKine::classID(); }
  static  const CLID& classID()       { return CLID_McKine; }
 
  void setId (int id);
  int  getId ();

  void setMotherId (int motherId);
  int  getMotherId ();

  void setNumDaughter (int nDaughter);
  int  getNumDaughter ();

  void setDaughterIdMin (int daughterIdMin);
  int  getDaughterIdMin ();

  void setDaughterIdMax (int daughterIdMax);
  int  getDaughterIdMax ();

  void setParticleId (int particleId);
  int  getParticleId ();

  void setCharge (int q);
  int  getCharge (); 

  void setPt (double pt);
  double  getPt (); 

  void setTheta0 (double theta0);
  double  getTheta0 (); 
   
  void setPhi0 (double phi0);
  double  getPhi0 (); 
 
  void setD0 (double d0);
  double  getD0 (); 
 
  void setZ0 (double z0);
  double  getZ0 (); 
 
  void setXv (double xv);
  double  getXv (); 
   
  void setYv (double yv);
  double  getYv (); 

  void setZv (double zv);
  double  getZv (); 

private:
    int	m_id;
    int	m_motherId;
    int	m_numDaughter;
    int	m_daughterIdMin;
    int	m_daughterIdMax;
    int	m_particleId;
    int	        m_q;
    double	m_pt;
    double	m_theta0;
    double	m_phi0;
    double	m_d0;
    double	m_z0;
    double	m_xv;
    double	m_yv;
    double	m_zv;

};

typedef ObjectVector<McKine> McKineCol;

#endif
