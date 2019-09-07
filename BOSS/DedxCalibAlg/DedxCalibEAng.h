#ifndef DEDXCALIBEANG_HH
#define DEDXCALIBEANG_HH

#include "TH1F.h"

#include "DedxCalibAlg/DedxCalib.h"

using namespace std;

class DedxCalibEAng: public DedxCalib
{
    public:
        DedxCalibEAng(const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalibEAng() {};
        void initializing() {}
        void BookHists();
        void genNtuple() {}
        void FillHists();
        void AnalyseHists();
        void WriteHists();

    private:
        TH1F** m_eangle;
        TH1F** m_pos_eangle;
        TH1F** m_neg_eangle;
        TH1F* m_EAngAverdE;
        TH1F* m_pos_EAngAverdE;
        TH1F* m_neg_EAngAverdE;
};
#endif
