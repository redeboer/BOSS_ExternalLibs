#ifndef DEDXCALIBCOSTHETA_HH
#define DEDXCALIBCOSTHETA_HH

#include "TH1F.h"
#include "TTree.h"
#include "vector"

#include "DedxCalibAlg/DedxCalib.h"

using namespace std;

class DedxCalibCostheta: public DedxCalib
{
    public:
        DedxCalibCostheta(const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalibCostheta() {};
        void initializing() {}
        void BookHists();
        void genNtuple() {}
        void FillHists();
        void AnalyseHists();
        void WriteHists();

    private:
        TH1F** m_costheta;
        TH1F** m_pos_costheta;
        TH1F** m_neg_costheta;
        TH1F** m_chi;
        TH1F** m_pos_chi;
        TH1F** m_neg_chi;
        TH1F* m_dEdxVsCostheta;
        TH1F* m_pos_dEdxVsCostheta;
        TH1F* m_neg_dEdxVsCostheta;

        vector<double> Vec_dedx;
        vector<double> Vec_costheta;
	bool m_debug;
	int m_debug_min, m_debug_max;
};
#endif
