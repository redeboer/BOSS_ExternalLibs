#ifndef DEDXCALIBMOMENTUM_HH
#define DEDXCALIBMOMENTUM_HH

#include "TH1F.h"
#include "TTree.h"
#include "vector"

#include "DedxCalibAlg/DedxCalib.h"

using namespace std;

class DedxCalibMomentum: public DedxCalib
{
    public:
        DedxCalibMomentum(const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalibMomentum() {};
        void initializing() {}
        void BookHists();
        void genNtuple() {}
        void FillHists();
        void AnalyseHists();
        void WriteHists();

    private:
        TH1F** m_chi;
        TH1F** m_pos_chi;
        TH1F** m_neg_chi;
        TH1F** m_dedx;
        TH1F** m_pos_dedx;
        TH1F** m_neg_dedx;
        TH1F** m_hits;

        vector<double> Vec_dedx;
        vector<double> Vec_ptrk;
};
#endif
