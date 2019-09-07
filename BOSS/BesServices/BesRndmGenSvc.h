#ifndef BESRNDMGENSVC_H
#define BESRNDMGENSVC_H

#ifndef _CPP_MAP
 #include <map>
#endif

//base classes
#ifndef BESKERNEL_IBESRNDMGENSVC_H
 #include "BesKernel/IBesRndmGenSvc.h"
#endif
#ifndef GAUDIKERNEL_IINCIDENTLISTENER_H
 #include "GaudiKernel/IIncidentListener.h"
#endif
#ifndef GAUDIKERNEL_SERVICE_H
 #include "GaudiKernel/Service.h"
#endif

#ifndef KERNEL_STATUSCODES_H
 #include "GaudiKernel/StatusCode.h"
#endif

//#include "IBesRndmGenSvc.h"
#include "CLHEP/Random/RanecuEngine.h"
#include "CLHEP/Random/RanluxEngine.h"

class IAlgorithm; 
class ISvcLocator;
class IIncident;


template <class TYPE> class SvcFactory;

class BesRndmGenSvc : virtual public IBesRndmGenSvc,
		     virtual public IIncidentListener,
		     public Service
{
public:
    /// Interface to the CLHEP engine
    //@{
  CLHEP::HepRandomEngine*	GetEngine	( const std::string& StreamName );
    void		CreateStream	( long seed, const std::string& StreamName );
  //@{

    /// CLHEP engines typedefs:
    //typedef	std::map<std::string, CLHEP::RanecuEngine*>	engineMap;
    typedef   std::map<std::string, CLHEP::RanluxEngine*>     engineMap;
    typedef	engineMap::iterator 			engineIter;
    typedef	engineMap::const_iterator		engineConstIter;  
    typedef	engineMap::value_type			engineValType;

    engineConstIter	begin			(void)	const;
    engineConstIter	end			(void)	const;
    unsigned int	number_of_streams	(void)	const;

    //caogf for random seed
    void setGenseed(long i) { genSeed = i; }
    long getGenseed() { return genSeed; }
    
    /// Print methods
    void		print		( const std::string& StreamName );
    void		print		( void );

  ///set the seeds for an engine. First param will usually be the event number
      CLHEP::HepRandomEngine* setOnDefinedSeeds (int EventNumber, 
					const std::string& StreamName);
  
    /// Gaudi Service Implementation
    //@{
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
    //@}

    /// IIncidentListener implementation. Handles EndEvent incident
    void handle(const Incident&);


private:
    /// Service Properties
    typedef	std::vector< std::string >	VStrings;
    VStrings	m_streams_seeds;

    /// Random engine copy (for output to a file)
    std::map<std::string, std::vector<long int> >	m_engines_copy;

    bool 		m_read_from_file;
    std::string		m_file_to_read;
    std::string         ofile;
    long genSeed; //caogf for random seed   
    engineMap*	m_engines;

    /// Default seed values
    //@{
    long        m_default_seed;
    long	m_EVTGEN_default_seed;
    long	m_PYTHIA_default_seed;
    long	m_HERWIG_default_seed;
    long        m_LUNDCRM_default_seed;
    long        m_SIM_default_seed;
    long        m_MIX_default_seed;
    long        m_KKMC_default_seed;
    std::string m_StreamName;
    std::string Stream_EVTGEN;
    std::string Stream_PYTHIA;
    std::string Stream_HERWIG;
    std::string Stream_LUNDCRM;
    std::string Stream_SIM;
    std::string Stream_MIX;
    std::string Stream_KKMC;
    //@}
 
    void		SetStreamSeeds	( const std::string& StreamName );
    
protected:
    
    friend class SvcFactory<BesRndmGenSvc>;

    // Standard Constructor
    BesRndmGenSvc(const std::string& name, ISvcLocator* svc);
        
    // Standard Destructor
    virtual ~BesRndmGenSvc();

};
 
inline	BesRndmGenSvc::engineConstIter
BesRndmGenSvc::begin			(void)	const
{ return m_engines->begin(); }

inline	BesRndmGenSvc::engineConstIter
BesRndmGenSvc::end			(void)	const
{ return m_engines->end(); }

inline	unsigned int
BesRndmGenSvc::number_of_streams		(void)	const
{ return m_engines->size(); }

#endif // BESRNDMGENSVC_H


