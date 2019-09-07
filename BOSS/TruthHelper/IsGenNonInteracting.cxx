#include <vector>

#include "TruthHelper/IsGenNonInteracting.h"
#include "HepMC/GenParticle.h"
  using std::vector;
  using std::abs;

  //****************************************************************
  //*                    IsGenNonInteracting                              *
  //****************************************************************
  IsGenNonInteracting::IsGenNonInteracting()  {}

 bool IsGenNonInteracting::operator()( const Particle* const p ) const {
    return (abs(p->pdg_id())==12 || abs(p->pdg_id())==14 
	    || abs(p->pdg_id())==16 ||
	    (abs(p->pdg_id())==1000039 &&  p->status()%1000==1 ) ||
	    (abs(p->pdg_id())==1000022 &&  p->status()%1000==1 ) ) ? true:false;
 }

  bool IsGenNonInteracting::operator() ( const Particle& p ) const {
    return this->operator()(&p);
  } 
  GenIMCselector* IsGenNonInteracting::create() const {return new IsGenNonInteracting(*this);}

