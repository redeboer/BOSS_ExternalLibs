#ifndef DEDXCALIBLAYERGAIN_HH
#define DEDXCALIBLAYERGAIN_HH

#include "TH1F.h"

#include "DedxCalibAlg/DedxCalib.h"

using namespace std;
class DedxCalibLayerGain: public DedxCalib
{
    public:
        DedxCalibLayerGain(const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalibLayerGain() {};
        void initializing() {}
        void BookHists();
        void genNtuple() {}
        void FillHists();
        void AnalyseHists();
        void WriteHists();

    private:
        vector<float> m_vector[43];
        TH1F** m_laygain;
        TH1F** m_laygain_gaus;
};
#endif
