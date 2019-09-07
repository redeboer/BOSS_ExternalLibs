#ifndef DEDXCALIBWIREGAIN_HH
#define DEDXCALIBWIREGAIN_HH

#include "TH1F.h"

#include "DedxCalibAlg/DedxCalib.h"

using namespace std;
class DedxCalibWireGain: public DedxCalib
{
    public:
        DedxCalibWireGain(const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalibWireGain() {};
        void initializing() {}
        void BookHists();
        void genNtuple() {}
        void FillHists();
        void AnalyseHists();
        void WriteHists();

    private:
        TH1F** m_wiregain;
};
#endif
