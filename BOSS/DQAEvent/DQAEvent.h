
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef DQAEvent_DQAEvent_H
#define DQAEvent_DQAEvent_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/boost_allocator.h"

// Forward declarations

namespace DQAEvent 
{

  // Forward declarations

  /** @class DQAEvent DQAEvent.h
   *
   * DQA Event Model 
   *
   * @author Ji Xiaobin
   * created Tue May 26 09:05:58 2009
   * 
   */

  class DQAEvent: public DataObject
  {
  public:

    /// Default Constructor
    DQAEvent() : m_EventTag(0) {}
  
    /// Default Destructor
    virtual ~DQAEvent() {}
  
    /// Retrieve const  
    /// Event Tag
  unsigned int EventTag() const;
  
    /// Update  
    /// Event Tag
  void setEventTag(unsigned int value);
  
    /// Retrieve
    /// Bhabha event
  bool Bhabha() const;
  
    /// Update
    /// Bhabha event
  void setBhabha(bool value);
  
    /// Retrieve
    /// di-muon event
  bool Dimu() const;
  
    /// Update
    /// di-muon event
  void setDimu(bool value);
  
    /// Retrieve
    /// inclusive hadronic event
  bool Hadron() const;
  
    /// Update
    /// inclusive hadronic event
  void setHadron(bool value);
  
    /// Retrieve
    /// cosmic event
  bool Cosmic() const;
  
    /// Update
    /// cosmic event
  void setCosmic(bool value);
  
    /// Retrieve
    /// psi'->pi+ pi- J/psi, J/psi->Rho pi
  bool PipiRhopi() const;
  
    /// Update
    /// psi'->pi+ pi- J/psi, J/psi->Rho pi
  void setPipiRhopi(bool value);
  
    /// Retrieve
    /// J/psi->Rho pi
  bool Rhopi() const;
  
    /// Update
    /// J/psi->Rho pi
  void setRhopi(bool value);
  
    /// Retrieve
    /// J/psi->Kstar K
  bool Kstark() const;
  
    /// Update
    /// J/psi->Kstar K
  void setKstark(bool value);
  
    /// Retrieve
    /// J/psi->P Pbar
  bool Ppbar() const;
  
    /// Update
    /// J/psi->P Pbar
  void setPpbar(bool value);
  
    /// Retrieve
    /// J/psi->Lambda Lambdabar
  bool Lambdalambdabar() const;
  
    /// Update
    /// J/psi->Lambda Lambdabar
  void setLambdalambdabar(bool value);
  
    /// Retrieve
    /// J/psi->p pbar pi+ pi-
  bool Ppbarpipi() const;
  
    /// Update
    /// J/psi->p pbar pi+ pi-
  void setPpbarpipi(bool value);
  
  
  #ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(DQAEvent) == size ? 
               boost::singleton_pool<DQAEvent, sizeof(DQAEvent)>::malloc() :
               ::operator new(size) );
    }
  
    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }
  
    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<DQAEvent, sizeof(DQAEvent)>::is_from(p) ?
      boost::singleton_pool<DQAEvent, sizeof(DQAEvent)>::free(p) :
      ::operator delete(p);
    }
  
    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
  #endif
  protected:

  private:

    /// Offsets of bitfield EventTag
    enum EventTagBits{BhabhaBits          = 0,
                      DimuBits            = 1,
                      HadronBits          = 2,
                      CosmicBits          = 3,
                      PipiRhopiBits       = 4,
                      RhopiBits           = 5,
                      KstarkBits          = 6,
                      PpbarBits           = 7,
                      LambdalambdabarBits = 8,
                      PpbarpipiBits       = 9};
  
    /// Bitmasks for bitfield EventTag
    enum EventTagMasks{BhabhaMask          = 0x1L,
                       DimuMask            = 0x2L,
                       HadronMask          = 0x4L,
                       CosmicMask          = 0x8L,
                       PipiRhopiMask       = 0x10L,
                       RhopiMask           = 0x20L,
                       KstarkMask          = 0x40L,
                       PpbarMask           = 0x80L,
                       LambdalambdabarMask = 0x100L,
                       PpbarpipiMask       = 0x200L
                      };
  
  
    unsigned int m_EventTag; ///< Event Tag
  
  }; // class DQAEvent

} // namespace DQAEvent;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline unsigned int DQAEvent::DQAEvent::EventTag() const 
{
  return m_EventTag;
}

inline void DQAEvent::DQAEvent::setEventTag(unsigned int value) 
{
  m_EventTag = value;
}

inline bool DQAEvent::DQAEvent::Bhabha() const
{
  return 0 != ((m_EventTag & BhabhaMask) >> BhabhaBits);
}

inline void DQAEvent::DQAEvent::setBhabha(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~BhabhaMask;
  m_EventTag |= ((((unsigned int)val) << BhabhaBits) & BhabhaMask);
}

inline bool DQAEvent::DQAEvent::Dimu() const
{
  return 0 != ((m_EventTag & DimuMask) >> DimuBits);
}

inline void DQAEvent::DQAEvent::setDimu(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~DimuMask;
  m_EventTag |= ((((unsigned int)val) << DimuBits) & DimuMask);
}

inline bool DQAEvent::DQAEvent::Hadron() const
{
  return 0 != ((m_EventTag & HadronMask) >> HadronBits);
}

inline void DQAEvent::DQAEvent::setHadron(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~HadronMask;
  m_EventTag |= ((((unsigned int)val) << HadronBits) & HadronMask);
}

inline bool DQAEvent::DQAEvent::Cosmic() const
{
  return 0 != ((m_EventTag & CosmicMask) >> CosmicBits);
}

inline void DQAEvent::DQAEvent::setCosmic(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~CosmicMask;
  m_EventTag |= ((((unsigned int)val) << CosmicBits) & CosmicMask);
}

inline bool DQAEvent::DQAEvent::PipiRhopi() const
{
  return 0 != ((m_EventTag & PipiRhopiMask) >> PipiRhopiBits);
}

inline void DQAEvent::DQAEvent::setPipiRhopi(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~PipiRhopiMask;
  m_EventTag |= ((((unsigned int)val) << PipiRhopiBits) & PipiRhopiMask);
}

inline bool DQAEvent::DQAEvent::Rhopi() const
{
  return 0 != ((m_EventTag & RhopiMask) >> RhopiBits);
}

inline void DQAEvent::DQAEvent::setRhopi(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~RhopiMask;
  m_EventTag |= ((((unsigned int)val) << RhopiBits) & RhopiMask);
}

inline bool DQAEvent::DQAEvent::Kstark() const
{
  return 0 != ((m_EventTag & KstarkMask) >> KstarkBits);
}

inline void DQAEvent::DQAEvent::setKstark(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~KstarkMask;
  m_EventTag |= ((((unsigned int)val) << KstarkBits) & KstarkMask);
}

inline bool DQAEvent::DQAEvent::Ppbar() const
{
  return 0 != ((m_EventTag & PpbarMask) >> PpbarBits);
}

inline void DQAEvent::DQAEvent::setPpbar(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~PpbarMask;
  m_EventTag |= ((((unsigned int)val) << PpbarBits) & PpbarMask);
}

inline bool DQAEvent::DQAEvent::Lambdalambdabar() const
{
  return 0 != ((m_EventTag & LambdalambdabarMask) >> LambdalambdabarBits);
}

inline void DQAEvent::DQAEvent::setLambdalambdabar(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~LambdalambdabarMask;
  m_EventTag |= ((((unsigned int)val) << LambdalambdabarBits) & LambdalambdabarMask);
}

inline bool DQAEvent::DQAEvent::Ppbarpipi() const
{
  return 0 != ((m_EventTag & PpbarpipiMask) >> PpbarpipiBits);
}

inline void DQAEvent::DQAEvent::setPpbarpipi(bool value)
{
  unsigned int val = (unsigned int)value;
  m_EventTag &= ~PpbarpipiMask;
  m_EventTag |= ((((unsigned int)val) << PpbarpipiBits) & PpbarpipiMask);
}


#endif ///DQAEvent_DQAEvent_H
