#ifndef DEDXCALIBDOCAEANG_HH
#define DEDXCALIBDOCAEANG_HH

#include "TH1F.h"
#include "TH2F.h"

#include "DedxCalibAlg/DedxCalib.h"

using namespace std;

class DedxCalibDocaEAng: public DedxCalib
{
    public:
        DedxCalibDocaEAng(const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalibDocaEAng() {};
        void initializing() {}
        void BookHists();
        void genNtuple() {}
        void FillHists();
        void AnalyseHists();
        void WriteHists();

    private:
        TH1F*** m_docaeangle;
        TH2F* m_DocaEAngAverdE;
	bool m_debug;
	int m_idoca_test, m_ieang_test;
};
#endif
