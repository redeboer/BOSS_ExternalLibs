/**
 * @class McPrimaryParticle
 *
 *
 */

#ifndef McPrimaryParticle_h
#define McPrimaryParticle_h

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "McTruth/McParticle.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_McPrimaryParticle;


namespace Event {

typedef SmartRefVector<Event::McParticle> McParticleRefVec; 
typedef SmartRef<Event::McParticle>       McParticleRef;

class McPrimaryParticle : public ContainedObject
{
public:
    //! Define bits to help classify the event
    enum ClassificationBits{  
        NOPRIMARY  = 1 ,    //! No primary particle found (can't happen?)
        CHARGED    = 1<<1,  //! Primary particle is charged
        NEUTRAL    = 1<<2,  //! Primary particle is neutral
        GAMMA      = 1<<3,  //! Primary is a gamma
        CONVERT    = 1<<4,  //! Secondaries from gamma conversion
        BREM       = 1<<5,  //! Secondaries from Bremstrahlung
        COMPT      = 1<<6,  //! Secondaries from Compton scatter
        PHOT       = 1<<7,  //! Secondaries from Photoelectric effect (?)
        OTHER      = 1<<8,  //! Secondaries from some other process 
        TRKCONVERT = 1<<12, //! Secondaries from gamma conversion in tracker
        TRKBREM    = 1<<13, //! Secondaries from Bremstrahlung in tracker
        TRKCOMPT   = 1<<14, //! Secondaries from Compton scatter in tracker
        TRKPHOT    = 1<<15, //! Secondaries from Photoelectric effect in tracker
        TRKOTHER   = 1<<16, //! Secondaries from some other process in tracker
        RUNBIT     = 1<<24  //! Indicates code was called (does nothing)
    };

    //! Dataobject compliant constructor
    McPrimaryParticle() : ContainedObject()
    {
        m_classification = 0;
        m_primary = 0;
        m_secondaries.clear();
        m_associated.clear();
    }

    //McPrimaryParticle(Event::McParticle* mcPart, unsigned long classBits);
   ~McPrimaryParticle() {}

    virtual const CLID& clID() const   { return McPrimaryParticle::classID(); }
    static const CLID& classID()       { return CLID_McPrimaryParticle; }

    //! Retrieve classification bits (see above definitions)
    const unsigned long                     getClassificationBits() const {return m_classification;}
    
    //! Retrieve reference to the primary particle
    const Event::McParticleRef              getPrimaryParticle()    const {return m_primary;}

    //! Retrieve number and iterators to the daughters of the primary with hits in tracker
    const int                               getNumSecondaries()     const {return m_secondaries.size();}
    Event::McParticleRefVec::const_iterator beginSecondaries()      const {return m_secondaries.begin();}
    Event::McParticleRefVec::const_iterator endSecondaries()        const {return m_secondaries.end();}

    //! Retrieve number and iterators to the particles associated with primary 
    const int                               getNumAssociated()      const {return m_associated.size();}
    Event::McParticleRefVec::const_iterator beginAssociated()       const {return m_associated.begin();}
    Event::McParticleRefVec::const_iterator endAssociated()         const {return m_associated.end();}

    //! Return an McParticle reference vector of all McParticles which leave hits in tracker
    inline Event::McParticleRefVec          getTrackVector();

    //! For building 
    void setPrimaryParticle(const Event::McParticleRef mcPart) {m_primary = mcPart;}
    void addSecondary(const Event::McParticleRef mcPart)       {m_secondaries.push_back(mcPart);}
    void addAssociated(const Event::McParticleRef mcPart)      {m_associated.push_back(mcPart);}
    void setClassificationBits(const unsigned long bits)       {m_classification |= bits;}

private:
    /// Bit-field for classification
    unsigned long             m_classification;

    Event::McParticleRef      m_primary;
    Event::McParticleRefVec   m_secondaries;
    Event::McParticleRefVec   m_associated;

};

typedef ObjectVector<McPrimaryParticle> McPrimaryParticleCol;

} // NameSpace Event

#endif
