#include <vector>

#include "TruthHelper/IsGenStable.h"
#include "HepMC/GenParticle.h"
  using std::vector;
  using std::abs;

  //****************************************************************
  //*                    IsGenStable                             *
  //****************************************************************
  IsGenStable::IsGenStable()  {}

  bool IsGenStable::operator()( const Particle* const p ) const {
    return ( ((p->status()%1000 == 1) || (p->status()%1000 == 2 && p->status() > 1000)) &&
 (p->barcode()<100000)  ) ? true:false;    
  }
  bool IsGenStable::operator() ( const Particle& p ) const {
    return this->operator()(&p);
  } 
  GenIMCselector* IsGenStable::create() const {return new IsGenStable(*this);}
