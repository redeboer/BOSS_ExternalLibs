#include <vector>

#include "TruthHelper/IsGenerator.h"
#include "HepMC/GenParticle.h"
  using std::vector;
  using std::abs;

 //****************************************************************
  //*                    IsGenerator                             *
  //****************************************************************
  IsGenerator::IsGenerator()  {}

  bool IsGenerator::operator()( const Particle* const p ) const {
    return ( p->barcode() < 100000 && ( p->status()<200 ||
					p->status()%1000 == 1 || 
					p->status()%1000 == 2  ) ) ? true:false;
    // conforms to PDG standard and accomodates the generated partilces that are copied to kine
  }

  bool IsGenerator::operator() ( const Particle& p ) const {
    return this->operator()(&p);
  } 
  GenIMCselector* IsGenerator::create() const {return new IsGenerator(*this);}

