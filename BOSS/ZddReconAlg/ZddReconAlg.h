#ifndef ZDD_RECON_ALG_H
#define ZDD_RECON_ALG_H

#include "GaudiKernel/Algorithm.h"
#include "ZddEvent/ZddEvent.h"
#include "ZddEvent/ZddChannel.h"

class ZddReconAlg : public Algorithm
{
    public:

	ZddReconAlg(const std::string& name, ISvcLocator* pSvcLocator);

	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

    private :

	double getEK(int chId);
	int zddDataStat(const Event::ZddEvent* zddEvt, int evtId);
	int calFragEandT(const ZddFragment& frag, int& efrag, int& tfrag);

	bool m_errStat;
	bool m_errQuit;
};

#endif
