#ifndef GENERATOROBJECTS_HEPMCPARTICLELINK_H
#define GENERATOROBJECTS_HEPMCPARTICLELINK_H
/** @class HepMcParticleLink
 * @brief a link optimized in size for a GenParticle in a McEventCollection 
 *
 * @see McEventCollection, GenEvent, ElementLink
 * @author Paolo Calafiura
 * $Id: HepMcParticleLink.h,v 1.1.1.1 2004/09/28 06:40:36 liwd Exp $
 **/
#include <cassert>

namespace HepMC {
  class GenParticle;
  class GenEvent;
}

class HepMcParticleLink {
public:  
  typedef unsigned int index_type;

  /// \name structors
  //@{
  HepMcParticleLink() : m_particle(0), m_extBarcode() {}
  HepMcParticleLink(index_type barCode, index_type eventIndex = 0) :
    m_particle(0), m_extBarcode(barCode, eventIndex) {}
  HepMcParticleLink(const HepMC::GenParticle* p, index_type eventIndex = 0);
  HepMcParticleLink(const HepMcParticleLink& rhs) : 
    m_particle(rhs.m_particle),
    m_extBarcode(rhs.m_extBarcode) {}
  HepMcParticleLink(const HepMC::GenParticle* part, 
		    const HepMC::GenEvent* pevt); //FIXME NOT YET 
  //@}
  
  /// \name pointer interface
  //@{
  /// @throws std::runtime_error when the element is not found
  const HepMC::GenParticle& operator* () const { return *cptr(); }//FIXME
  const HepMC::GenParticle* operator->() const { return cptr(); }
  operator const HepMC::GenParticle* () const { return cptr(); }
  bool operator!() const {return !isValid();}
  //@}

  /// \name indexing accessors (e.g. for writing)
  //@{
  int barcode() const { return m_extBarcode.barcode(); }
  index_type eventIndex() const { return m_extBarcode.eventIndex(); }
  //@}

  bool isValid() const { return (0 != cptr()); }
  const HepMC::GenParticle* cptr() const;

private:
  class ExtendedBarCode {
  public:
    ExtendedBarCode() : m_extBC(0) {}
    ExtendedBarCode(index_type barcode, index_type eventIndex) {
      assert(barcode < 0x1FFFFF); // this is (1 << 21) - 1
      assert(eventIndex < 0x7FF); // this is (1 << 11) - 1
      m_extBC = barcode + (eventIndex << 21);
    }
    ExtendedBarCode(const ExtendedBarCode& rhs) :
      m_extBC(rhs.m_extBC) {}

    int barcode() const { return m_extBC & 0x1FFFFF; }
    index_type eventIndex() const { return m_extBC >> 21; }
  private:
    //    mutable int m_barcode : 21;   //FIXME HepMC
    //    unsigned int m_eventIndex: 11;
    unsigned int m_extBC;
  };
  mutable HepMC::GenParticle* m_particle; /* transient */
  ExtendedBarCode m_extBarcode;
};

#endif
