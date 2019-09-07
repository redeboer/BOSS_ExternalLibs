#ifndef BesMdcTrig_h
#define BesMdcTrig_h 1
class MdcTCount;
class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;

class BesMdcTrig{
public:
	BesMdcTrig();
	~BesMdcTrig();
	void startMdcTrig();
private:
	MdcTCount* m_MdcTCount;
	bool NLtrk1,NLtrk2,NLtrkN,NStrk1,NStrk2,NStrkN,STrk_BB,LTrk_BB,NItrk1,NItrk2;

        BesGlobalTrigSvc* m_pIBGT;
        IBesGlobalTrigSvc* m_tmpSvc;
};
#endif
