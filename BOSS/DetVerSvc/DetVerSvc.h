#ifndef DET_VER_SVC_H
#define DET_VER_SVC_H

#include "GaudiKernel/Service.h"
#include "DetVerSvc/IDetVerSvc.h"

class DetVerSvc : public Service, virtual public IDetVerSvc
{
    public :

        DetVerSvc(const std::string& name, ISvcLocator* svcloc);
        virtual ~DetVerSvc();

        virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
        virtual StatusCode initialize ( );
        virtual StatusCode finalize ( );

        int phase();
        int fromRun(unsigned int run);

    private :

        int   m_phase;
        std::string  m_conf;
};

#endif
