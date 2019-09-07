#include "TruthHelper/GenAccessIO.h"
#include <cmath> 
#include <algorithm>
#include "TruthHelper/GenIMCselector.h"
#include "TruthHelper/GenAll.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

StatusCode GenAccessIO::getMC(MCparticleCollection& mcParticles) const {
  const GenIMCselector* selector = new GenAll();
  StatusCode stat = this->getMC(mcParticles, selector);
  return stat;
}


StatusCode GenAccessIO::getMC(MCparticleCollection& mcParticles, 
			      const GenIMCselector* selector) const {
    
  const McEventCollection* mcCollptr;
  StatusCode stat = this->getDH(mcCollptr);
  if(!stat){
    return StatusCode::FAILURE;
  }

  // Iterate over all McEvent records
  McEventCollection::const_iterator itr;
  for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
    // Access the HepMC record which is wrapped within McEvent
    const HepMC::GenEvent* genEvt = (*itr);
    if(genEvt == 0) return StatusCode::FAILURE;
      
    HepMC::GenEvent::particle_const_iterator it= genEvt->particles_begin();
    HepMC::GenEvent::particle_const_iterator en= genEvt->particles_end();
    for(; it!=en; ++it){
      if((*selector)(*it)){mcParticles.push_back(*it);}
    }
  }
    
  return StatusCode::SUCCESS;
    
}
