#include "GeneratorObject/HepMcParticleLink.h"
#include "GeneratorObject/McEventCollection.h"
#include "StoreGate/StoreGate.h"
#include "CLHEP/HepMC/GenParticle.h"
#include "CLHEP/HepMC/GenEvent.h"
#include <cassert>

HepMcParticleLink::HepMcParticleLink(const HepMC::GenParticle* part, 
				     index_type eventIndex) :
    m_particle(const_cast<HepMC::GenParticle*>(part)), 
    m_extBarcode((0 != part) ? part->barcode() : 0, eventIndex) {
  assert(part);
}

const HepMC::GenParticle* HepMcParticleLink::cptr() const {
  if (!m_particle) {
    const McEventCollection* pEvtColl;
    if ((StoreGate::instance().retrieve(pEvtColl)).isSuccess()) { //FIXME SLOW
      m_particle = pEvtColl->at(eventIndex())->barcode_to_particle(barcode());
    }
  }
  return m_particle;
}
