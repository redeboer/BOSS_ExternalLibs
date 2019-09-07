#include <vector>

#include "TruthHelper/GenAll.h"
#include "HepMC/GenParticle.h"
  using std::vector;
  using std::abs;
  //****************************************************************
  //*                    All                                       *
  //****************************************************************
  bool GenAll::operator()( const Particle* const) const {
    return true;
  }
  bool GenAll::operator() ( const Particle& p ) const {
    return this->operator()(&p);
  } 
  GenIMCselector* GenAll::create() const {return new GenAll();}
