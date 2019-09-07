#ifndef DEDXCALIBRUNBYRUN_HH
#define DEDXCALIBRUNBYRUN_HH

#include "TH1F.h"

#include "DedxCalibAlg/DedxCalib.h"

using namespace std;
class DedxCalibRunByRun: public DedxCalib
{
    public:
        DedxCalibRunByRun(const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalibRunByRun() {};
        void initializing() {}
        void BookHists();
        void genNtuple() {}
        void FillHists();
        void AnalyseHists();
        void WriteHists();

    private:
        vector<float> m_runNOVector;
        TH1F** m_rungain;
        TH1F* m_hist;
};
#endif
