#include "TFile.h"
#include "TTree.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include <cstdlib>

namespace DetVerSvcPack {

    int getRunFromFileList(const std::string& flist)
    {
        std::string::size_type p1 = flist.find('"') + 1;
        std::string::size_type p2 = flist.find('"', p1);
        std::string fn = flist.substr(p1, p2-p1);

        TFile* file = TFile::Open(fn.c_str(), "READ");
        TTree* tree = (TTree*)file->Get("Event");
        tree->SetMakeClass(1);
        int run;
        tree->SetBranchAddress("m_runId", &run);
        tree->GetEntry(0);

        return abs(run);
    }

    int funcInputRoot(ISvcLocator* svcLct)
    {
        SmartIF<IJobOptionsSvc> iSvc(svcLct->service("JobOptionsSvc"));
        if ( iSvc.isValid() ) {
            const std::vector<const Property*>* ps = iSvc->getProperties("EventCnvSvc");
            std::vector<const Property*>::const_iterator it, end = ps->end();
            for ( it = ps->begin(); it != end; ++it ) {
                if ( (*it)->name() == "digiRootInputFile" ) {
                    return getRunFromFileList( (*it)->toString() );
                }
            }
        }
        return -1;
    }

    extern int (*pf_helper)(ISvcLocator*);

    //initialize function pointer: pf_helper
    struct InputRootInit {
        InputRootInit() {
            pf_helper = funcInputRoot;
        }
    };

    InputRootInit init;
}
