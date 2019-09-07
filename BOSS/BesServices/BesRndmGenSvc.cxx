#include "BesServices/BesRndmGenSvc.h"

#ifndef HAVE_NEW_IOSTREAMS /*gnu-specific*/
#define BOOST_NO_STRINGSTREAM 1 /*FIXME should come from boost config */
#endif
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
using namespace std;

bool interpretSeeds(const string& buffer, 
		    string& stream, std::vector<unsigned long>& seed) {
  bool status(false);
  seed.clear();
  //split the space-separated string in 3 words:	
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(" ");
  tokenizer tokens(buffer, sep);	      	    
  if (status = (distance(tokens.begin(), tokens.end()) == 32)) {
    tokenizer::iterator token(tokens.begin());
    stream = *token++;
    try {
      for(int i=0; i<31; i++) {
	long tmp = boost::lexical_cast<long>(*token++);
        seed.push_back(tmp);
      }
    } catch (boost::bad_lexical_cast e) {
      status = false;
    }
  }
  return status;
}

/// Standard Constructor
BesRndmGenSvc::BesRndmGenSvc(const std::string& name,ISvcLocator* svc)
    : Service(name,svc)
{
    // Property Default values
    m_read_from_file	=	false;
    m_file_to_read	=	"BesRndmGenSvc.out";
    ofile = "BesRndmGenSvc.out";

     // Set Default values       
    m_default_seed              =       11000;
    m_EVTGEN_default_seed       =       35910;
    m_PYTHIA_default_seed       =       93531;
    m_HERWIG_default_seed       =       35391;
    m_LUNDCRM_default_seed      =       12456;
    m_SIM_default_seed          =       23569;
    m_MIX_default_seed          =       76543;
    m_KKMC_default_seed         =       26877;
    // Get user's input
    declareProperty("Seeds",   	    m_streams_seeds);
    declareProperty("StreamName",   m_StreamName = "EVTGEN");
    declareProperty("ReadFromFile", m_read_from_file);
    declareProperty("FileToRead",   m_file_to_read);
    declareProperty("RdmOutFile",      ofile);
    declareProperty("RndmSeed",  m_default_seed);

    Stream_EVTGEN = "EVTGEN";
    Stream_PYTHIA = "PYTHIA";
    Stream_HERWIG = "HERWIG";
    Stream_LUNDCRM = "LUNDCRM";
    Stream_SIM    = "SIM";
    Stream_MIX    = "MIX";
    Stream_KKMC = "KKMC";

    m_engines			=	new	engineMap();
}


/// Standard Destructor
BesRndmGenSvc::~BesRndmGenSvc()  
{
    delete	m_engines;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
BesRndmGenSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
    if ( IBesRndmGenSvc::interfaceID().versionMatch(riid) )    {
        *ppvInterface = (IBesRndmGenSvc*)this;
    }
    else  {
	// Interface is not directly available: try out a base class
	return Service::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

StatusCode 
BesRndmGenSvc::initialize()
{
  MsgStream log( messageService(), name() );
  log << MSG::INFO << "Initializing " << name()
      << " - package version " << PACKAGE_VERSION 
      << "\n INITIALISING RANDOM NUMBER STREAMS. " << endreq;


    if (!(Service::initialize().isSuccess())) {
      return StatusCode::FAILURE;
    }

    //set up different stream seeds by default seed
    m_EVTGEN_default_seed       =       m_default_seed+35910;
    m_PYTHIA_default_seed       =       m_default_seed+93531;
    m_HERWIG_default_seed       =       m_default_seed+35391;
    m_LUNDCRM_default_seed      =       m_default_seed+12456;
    m_SIM_default_seed          =       m_default_seed+23569;
    m_MIX_default_seed          =       m_default_seed+76543;
    m_KKMC_default_seed         =       m_default_seed+26877;

    /// Incident Service
    IIncidentSvc* pIncSvc(0);

    // set up the incident service:
    if (!(service("IncidentSvc", pIncSvc, true)).isSuccess()) {
      log << MSG::ERROR 
	  << "Could not locate IncidentSvc "
	  << endreq;
      return StatusCode::FAILURE;
    }
  
    //start listening to "EndEvent"
    static const int PRIORITY = 100;
    pIncSvc->addListener(this, "EndEvent", PRIORITY);

    if (m_read_from_file)
    {
	// Read from a file
	ifstream infile( m_file_to_read.c_str() );
	if ( !infile )
	{
	    log << MSG::ERROR << " Unable to open: " << m_file_to_read <<endreq;
	    return StatusCode::FAILURE;
	}
	else
	{
	    std::string buffer;
	    while (std::getline(infile, buffer))
	    {
	      string stream; 
	      std::vector<unsigned long> v;
	      //split the space-separated string in 3 words:
	      if (interpretSeeds(buffer, stream, v)) {
		log << MSG::DEBUG << " INITIALISING " << stream << " stream, "<< " read from file " <<m_file_to_read << endreq;
		CLHEP::HepRandomEngine* engine = GetEngine(stream);
		engine->get(v);
	      } else {		
		log << MSG::ERROR << "bad line\n" << buffer 
		    << "\n in input file " << m_file_to_read << endreq;
		return StatusCode::FAILURE;
	      }		
	    }
	    
	}
    }

    // Create the various streams according to user's request
    for (VStrings::const_iterator i = m_streams_seeds.begin(); i != m_streams_seeds.end(); ++i)
    {
        string stream; 
	std::vector<unsigned long> v;
        //split the space-separated string in 3 words:
        if (interpretSeeds(*i, stream, v)) {
	  log << MSG::VERBOSE << "Seeds property: stream " << stream << endreq; 
	} else {
	  log << MSG::ERROR << "bad Seeds property\n" << *i << endreq; 
	  return StatusCode::FAILURE;
	}		
    	
	// Check if stream already generated (e.g. from reading a file)
	bool				not_found	=	true;
	if ( number_of_streams() != 0 )
	{
	    BesRndmGenSvc::engineConstIter sf 		= 	begin();
	    do
	    {
		if ((*sf).first == stream) not_found = false;
		++sf;
	    } while (sf != end() && not_found);
	}
	
	if (not_found)
	{
	    log << MSG::DEBUG << " INITIALISING " << stream << endreq;
	    CreateStream(v[0], stream);
	}
	
    }
    
    return StatusCode::SUCCESS;
}

void 
BesRndmGenSvc::handle(const Incident &inc) {
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << " Handle EndEvent " << endreq;

  if ( inc.type() == "EndEvent") 
  {

    m_engines_copy.clear();
    for (engineConstIter i = begin(); i != end(); ++i)
    {
      CLHEP::HepRandomEngine* engine = GetEngine((*i).first);
      std::vector<unsigned long> s = engine->put();
      std::vector<long int> tseeds;
      for(unsigned int j=0; j<s.size(); j++) {
        tseeds.push_back(s[j]);
      }
      m_engines_copy.insert( 
	std::map<std::string, std::vector<long int> >::value_type( (*i).first,
						                   tseeds ) );
    }
    // Write the status of the Service into a file
/*
    ofstream    outfile( ofile.c_str() );
    if ( !outfile )
    {
      log <<MSG::ERROR << "error: unable to open: " << ofile << endreq;
    }
    else
    {
        for (std::map<std::string, std::vector<long int> >::const_iterator i = m_engines_copy.begin();
             i != m_engines_copy.end();
             ++i)
        {
            outfile << (*i).first;
            for(unsigned int j=0; j<((*i).second).size(); j++)
              outfile << " " << (*i).second[j];
            outfile <<"\n";
        }
    }
*/
//    print();    
  }
}

StatusCode 
BesRndmGenSvc::finalize()
{
    MsgStream log(messageService(), name());
    log << MSG::INFO << " FINALISING " << endreq;
    return Service::finalize();
}

CLHEP::HepRandomEngine*
BesRndmGenSvc::GetEngine	( const std::string& StreamName )
{
    engineConstIter citer = m_engines->find(StreamName);
    if ( citer == m_engines->end() )
    {
	m_engines->insert( engineValType( StreamName, new CLHEP::RanluxEngine() ) );
	SetStreamSeeds	( StreamName );
    }
    
    engineIter	iter	=	m_engines->find(StreamName);
    return	(CLHEP::HepRandomEngine*)(*iter).second;
}

void
BesRndmGenSvc::CreateStream	( long seed, const std::string& StreamName )
{
    engineConstIter citer = m_engines->find(StreamName);
    if ( citer == m_engines->end() ) m_engines->insert( engineValType(	StreamName,
									new CLHEP::RanluxEngine() ) );
    engineIter	iter	=	m_engines->find(StreamName);
    ((*iter).second)->setSeed( seed, 3 );
}

void
BesRndmGenSvc::SetStreamSeeds	( const std::string& StreamName )
{
    long theseed;
    if (StreamName == "PYTHIA")
    {
	theseed = m_PYTHIA_default_seed;
    }
    else if (StreamName == "HERWIG")
    {
	theseed = m_HERWIG_default_seed;
    }
    else if (StreamName == "EVTGEN")
    {
	theseed = m_EVTGEN_default_seed;
    }
    else if (StreamName == "LUNDCRM")
    {
	theseed = m_LUNDCRM_default_seed;
    }
    else if (StreamName == "SIM")
    {
	theseed = m_SIM_default_seed;
    }
    else if (StreamName == "MIX")
    {
        theseed = m_MIX_default_seed;
    }
    else if (StreamName == "KKMC")
    {
      theseed = m_KKMC_default_seed;
    }
    else
    {
        theseed = m_default_seed;
    }
    MsgStream log(messageService(), name());
    log << MSG::WARNING << " INITIALISING " << StreamName << " stream with DEFAULT seed "
	<< theseed << endreq;
    
    engineIter	iter	=	m_engines->find(StreamName);
    ((*iter).second)->setSeed( theseed, 3);
}

void
BesRndmGenSvc::print	( const std::string& StreamName )
{
    MsgStream log(messageService(), name());
    engineConstIter citer = m_engines->find(StreamName);
    if ( citer == m_engines->end() )
    {
	log << MSG::WARNING << " Stream =  " << StreamName << " NOT FOUND" << endreq;
    }
    else
    {
	const long s	=	((*citer).second)->getSeed();
	log << MSG::INFO << " Stream =  " << StreamName << ", Seed =  "
	    << s << endreq;
    }
}

void
BesRndmGenSvc::print	( void )
{
	print( m_StreamName );
}

CLHEP::HepRandomEngine*
BesRndmGenSvc::setOnDefinedSeeds	(int EventNumber, const std::string& StreamName )
{
  engineConstIter citer = m_engines->find(StreamName);
  if ( citer == m_engines->end() ) m_engines->insert( engineValType(	StreamName,
									new CLHEP::RanluxEngine() ) );
  engineIter	iter	=	m_engines->find(StreamName);
  long hashedStream;
  // (SG::simpleStringHash(StreamName));
  long theseed = 10000*EventNumber + hashedStream;
  ((*iter).second)->setSeed( theseed, 3 );
  return	(CLHEP::HepRandomEngine*)(*iter).second;
}

