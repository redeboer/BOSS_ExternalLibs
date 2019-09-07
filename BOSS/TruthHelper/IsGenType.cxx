#include <vector>

#include "TruthHelper/IsGenType.h"
#include "HepMC/GenParticle.h"
using std::vector;
using std::abs;

//****************************************************************
//*                    IsGenType                             *
//****************************************************************
IsGenType::IsGenType(int ParticleType){
  m_TypeList.push_back(ParticleType); 
}
IsGenType::IsGenType(std::vector<int> ParticleTypeList){
  vector<int>::const_iterator itype = ParticleTypeList.begin();
  for(; itype !=ParticleTypeList.end(); ++itype ){ 
    m_TypeList.push_back((*itype));
  }
}
IsGenType::IsGenType(const IsGenType& rhs):GenIMCselector(),m_TypeList(rhs.m_TypeList){}

bool IsGenType::operator()( const Particle* const p ) const {
  vector<int>::const_iterator itype = m_TypeList.begin();
  for(; itype != m_TypeList.end(); ++itype ){ 
    if( abs(p->pdg_id()) == (*itype) ) return true ;
  }
  return false;
}

bool IsGenType::operator() ( const Particle& p ) const {
  return this->operator()(&p);
} 
GenIMCselector* IsGenType::create() const {return new IsGenType(*this);}
