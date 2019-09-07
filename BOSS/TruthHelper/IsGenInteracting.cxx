#include <vector>

#include "TruthHelper/IsGenInteracting.h"
#include "HepMC/GenParticle.h"
  using std::vector;
  using std::abs;

  //****************************************************************
  //*            IsGenInteracting                                     *
  //****************************************************************
  IsGenInteracting::IsGenInteracting()  {}

  bool IsGenInteracting::operator()( const Particle* const p ) const {
    return ( (((p->status()%1000 == 1) ||
	      (p->status()%1000 == 2 && p->status() > 1000)) &&
	     (p->barcode()<100000)) &&
	     !(abs(p->pdg_id())==12 || abs(p->pdg_id())==14 
	    || abs(p->pdg_id())==16 ||
	    (abs(p->pdg_id())==1000039 &&  p->status()%1000==1 ) ||
	    (abs(p->pdg_id())==1000022 &&  p->status()%1000==1 )) ) ? true:false;    
  }

  bool IsGenInteracting::operator() ( const Particle& p ) const {
    return this->operator()(&p);
  } 
  GenIMCselector* IsGenInteracting::create() const {return new IsGenInteracting(*this);}
