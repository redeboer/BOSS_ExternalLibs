#ifndef Event_McTrajectory_H
#define Event_McTrajectory_H 1

#include <iostream>
#include <vector>
#include <utility>
#include "CLHEP/Geometry/Point3D.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"
#include "McTruth/McParticle.h"

extern const CLID& CLID_McTrajectory;

namespace Event {  // NameSpace

class McTrajectory : virtual public ContainedObject {
  public:

    virtual const CLID& clID() const   { return McTrajectory::classID(); }
    static const CLID& classID()       { return CLID_McTrajectory; }

    McTrajectory(){}
    ~McTrajectory(){}
    
    /// Add the 3d points to the trajectory
    void addPoints(std::vector<Hep3Vector>& points);

    /// Set the pointer to the McParticle
    void setMcParticle(SmartRef<McParticle> value);
    void setMcParticle( McParticle* value );

    /// Get the pointer to the McParticle
    const McParticle* getMcParticle() const;
    McParticle* getMcParticle();

    /// Get the 3d points
    std::vector<Hep3Vector>& getPoints(){return m_points;}

    /// get, set charge
    int getCharge() const { return m_charge; }
    void setCharge(int charge){ m_charge=charge;}

  private:
    /// Pointer to McParticle of this trajectory
    SmartRef<McParticle>    m_mcParticle;
    /// The point of the trajectory
    std::vector<Hep3Vector> m_points;
    /// the (redundant?) charge
    int m_charge;

};

// Definition of all container types of McTrajectory
//template <class TYPE> class ObjectVector;
typedef ObjectVector<McTrajectory>     McTrajectoryVector;
typedef ObjectVector<McTrajectory>     McTrajectoryCol;
//template <class TYPE> class ObjectList;
typedef ObjectList<McTrajectory>       McTrajectoryList;


}

#endif // Event_McTrajectory_H

