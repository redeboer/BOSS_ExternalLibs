#include "DetVerSvc/DetVerSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <fstream>
#include <dlfcn.h>

namespace DetVerSvcPack {
    int (*pf_helper)(ISvcLocator*);
}

DetVerSvc::DetVerSvc(const std::string& name, ISvcLocator* svcloc)
    : Service(name, svcloc),
      m_phase(-1)
{
     declareProperty("Config", m_conf = std::string(getenv("DETVERSVCROOT")) + "/share/config.txt");
}

DetVerSvc::~DetVerSvc()
{
}

StatusCode DetVerSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
    if ( IID_IDetVerSvc.versionMatch(riid) ) { 
        *ppvInterface = static_cast<IDetVerSvc*> (this); 
    } else { 
        return Service::queryInterface(riid, ppvInterface) ; 
    }
    return StatusCode::SUCCESS;
}

StatusCode DetVerSvc::initialize()
{
    Service::initialize();

    MsgStream log(messageService(), name());
    log << MSG::INFO << name() << ": Start of run initialisation" << endreq;

    //assert m_conf is a valid file
    if ( access( m_conf.c_str(), F_OK ) < 0 ) {
        log << MSG::FATAL << "Cann't find config file: " << m_conf << endreq;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode DetVerSvc::finalize ( ) {
    MsgStream log(messageService(), name());
    log << MSG::INFO << name() << ": End of Run finalize" << endreq;

    return StatusCode::SUCCESS;
}

int DetVerSvc::phase()
{
    if ( m_phase < 0) {  //phase has not been retrieved
        SmartIF<IJobOptionsSvc> iSvc(serviceLocator()->service("JobOptionsSvc"));
        if ( iSvc.isValid() ) {
            std::string  dll;  //sub so name
            const std::vector<const Property*>* ps = 0;
            if ( (ps = iSvc->getProperties("RawDataInputSvc")) != 0 ) {
                dll = "libDetVerSvc_IRaw.so";  //case 0: Input RAW
            }
            else if ( (ps = iSvc->getProperties("EventCnvSvc")) != 0 ) {
                dll = "libDetVerSvc_IRoot.so";  //case 1: Input root
            }
            else if ( (ps = iSvc->getProperties("RealizationSvc")) != 0 ) {
                dll = "libDetVerSvc_Sim.so";  //case 2: simulation
            }
            //else //    //Unknown job type !!!!!

            void *dl_handler = dlopen(dll.c_str(), RTLD_LAZY|RTLD_GLOBAL);
            if ( dl_handler != 0 ) {
                m_phase = fromRun( (*DetVerSvcPack::pf_helper)(serviceLocator()) );
            }
            dlclose(dl_handler);
        }
    }
    return m_phase;
}

int DetVerSvc::fromRun(unsigned int run)
{
    std::vector<int>  runList;
    //parse m_conf
    int iTmp;
    std::ifstream  fs(m_conf.c_str());
    fs >> iTmp;
    while ( ! fs.eof() ) {
        runList.push_back(iTmp);
        fs >> iTmp;
    }

    //return phase from run
    int iPhase = 1;
    for ( std::vector<int>::iterator it = runList.begin(); it != runList.end(); ++it ) {
        if ( run < (*it) ) {
            break;
        }
        ++iPhase;
    }
    return iPhase;
}
