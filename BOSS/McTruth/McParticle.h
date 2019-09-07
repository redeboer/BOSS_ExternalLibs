#ifndef Event_McParticle_H
#define Event_McParticle_H 

#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"
#include "EventModel/EventModel.h"
using namespace CLHEP;
/** @class McParticle
 * @brief The Monte Carlo particle kinematics information
 *
 *  The class McParticle uses the Class Library for HEP (CLHEP).
 *            
 * Based on LHCbEvent's MCParticle 
 *
 */
extern const CLID& CLID_McParticle;

namespace Event {  // NameSpace


class McParticle  : virtual public ContainedObject  {
  public:
    typedef int  StdHepId;
    
    //! status bits modeled

    enum StatusBits{  
        DECAYED =1 ,    //! Decayed by generator
        DECAYFLT=1<<1,  //! Decayed in flight
        PRIMARY =1<<2,  //! primary particle
        LEAF = 1<<3,    //! this particle is a leaf in the particle tree
        ERROR = 1<<4    //! error
        //MISSED=  1<<2,  //! Does not hit detector
        //NOINTER =1<<3,  //! Traverses detector w/o interacting 
        //STOPPED =1<<4,  //! Energy below cut; other bits may say why 
        //INTERACT=1<<5,  //! Interacted, no further decision to be made
        //INTSHDEP=1<<6,  //! Interacted, further decision depends on ! selection of shower deposition  
        //PRIMARY =1<<7,  //! primary particle 
        //SWERROR =1<<8,  //! Error occurred in swimming the track 
        //BCKSPL=1<<9,    //! The particle is backsplashed from the CAL back in the TKR region
        //POSHIT =1<<10,     //! The particle released a hit a PositionHit detector
        //NOTTRACK=1<<11, //! Not tracked by user request 
        //Swum =   1<<12,  //! this particle was produced by the swimmer
        //LEAF = 1<<13     //! this particle is a leaf in the particle tree 
    };
    

    virtual const CLID& clID() const   { return McParticle::classID(); }
    static const CLID& classID()       { return CLID_McParticle; }
    /// Constructors
    McParticle() :
      m_statusFlags(0)
    {}
    /// Destructor
    virtual ~McParticle() {}

    //! completely initialize a newed object. No other way to set most attributes.
    //! it will be replaced by the following methods (left here just in the transition)
    void init( McParticle* mother, 
      	StdHepId id, 
        unsigned int statusBits,
        const HepLorentzVector& initialMomentum,
        const HepLorentzVector& finalMomentum,
        const HepLorentzVector& initialPosition,
        const HepLorentzVector& finalPosition,
        const std::string process = "");

    //! Set the initial attributes of the McParticle
    void initialize(
	StdHepId id, 
        unsigned int statusBits,
        const HepLorentzVector& initialMomentum,
        const HepLorentzVector& initialPosition,
        const std::string process = "");


    void initialize( McParticle* mother, 
	StdHepId id, 
        unsigned int statusBits,
        const HepLorentzVector& initialMomentum,
        const HepLorentzVector& initialPosition,
        const std::string process = "");

    //! Set the final attributes of the McParticle
    //void finalize( const HepLorentzVector& finalMomentum,const HepLorentzVector& finalPosition);

    void finalize( const HepLorentzVector& finalPosition);

    /// Retrieve particle property
    StdHepId particleProperty() const;

    /// retrieve all of status flags for const object
    unsigned int statusFlags()const;

    /// add a new flag to the status flags
    void addStatusFlag(unsigned int flag){m_statusFlags |= flag;};

    /// Retrieve whether this is a primary particle:
    /// there's no mother for a primary particle
    bool primaryParticle() const;

    /// Retrieve whether this is a leaf particle
    bool leafParticle() const;

    /// Decayed from generator
    bool decayFromGenerator() const;

    /// Decayed in flight
    bool decayInFlight() const;

    /// methods for setting and getting vertex indexes  
    void setVertexIndex0(int index0) { m_vertexIndex0 = index0; }

    int  vertexIndex0() const {return m_vertexIndex0;}

    void setVertexIndex1(int index1) { m_vertexIndex1 = index1; }

    int  vertexIndex1() const {return m_vertexIndex1;}
    
    // Set track index
    void setTrackIndex(int trackIndex) { m_trackIndex = trackIndex; }
 
    // Get track index
    int trackIndex() const {return m_trackIndex;}

    /// Retrieve pointer to the start, end vertex positions 
    const HepLorentzVector& initialPosition() const;
    const HepLorentzVector& finalPosition() const;

    const HepLorentzVector&  initialFourMomentum()const;
    //const HepLorentzVector&  finalFourMomentum()const;

    /// access to the mother particle
    const McParticle& mother()const; 

    /// set the mother particle
    void setMother(const SmartRef<McParticle> m);

    /// add a daugther particle to this particle
    void addDaughter(const SmartRef<McParticle> d){m_daughters.push_back(d);};

    /// Used for pruning
    void removeDaughter(const SmartRef<McParticle> mcPart);

    /// access the process name
    //const std::string getProcess()const{return m_process;};

    /// set the initial and final volume identifiers
    // void setInitialId(idents::VolumeIdentifier id){m_initialId = id;};
    // void setFinalId(idents::VolumeIdentifier id){m_finalId = id;};

    /// get the initial and final volume identifier
    // idents::VolumeIdentifier getInitialId(){return m_initialId;};
    // idents::VolumeIdentifier getFinalId(){return m_finalId;};

    /// access to the list of daughters
    const SmartRefVector<McParticle>& daughterList()const{return m_daughters;};


  private:

    /// particle property (such as electron or proton or ....) ID
    StdHepId                  m_particleID;

    int m_trackIndex;

    /// the index for the vertex where the particle starts  
    int m_vertexIndex0;
    
    /// the index for the vertex where the particle stops 
    int m_vertexIndex1;

    /// Bit-field status flag
    unsigned long             m_statusFlags;
    /// Initial position
    HepLorentzVector                 m_initialPosition;
    /// Final position
    HepLorentzVector                 m_finalPosition;
    
    /// Initial 4-momentum
    HepLorentzVector           m_initialFourMomentum;
    /// Final 4-momentum
    HepLorentzVector           m_finalFourMomentum;
    /// Pointer to mother particle
    SmartRef<McParticle>       m_mother;
    /// Vector of pointers to daughter particles
    SmartRefVector<McParticle> m_daughters;
    /// String with the process name that poduces this particle
    std::string                m_process;

    /// Volume identifiers where the particle start
    // idents::VolumeIdentifier   m_initialId;

    /// Volume identifiers where the particle stop
    // idents::VolumeIdentifier   m_finalId;  
};

typedef ObjectList<McParticle>       McParticleCol;

} // NameSpace Event


#endif    // Event_McParticle_H

