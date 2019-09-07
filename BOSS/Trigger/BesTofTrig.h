#ifndef BesTofTrig_h
#define BesTofTrig_h 1
#include <vector>

using namespace std;
#include "Trigger/TofHitCount.h"

class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;

class BesTofTrig{
public:
	BesTofTrig();
	~BesTofTrig();
	void startTofTrig();

	inline std::vector<int>& GetbHitpos() { return bHitpos; }
	inline std::vector<int>& GetecapHitpos() { return ecapHitpos; }
	inline std::vector<int>& GetwcapHitpos() { return wcapHitpos; }
private:
	TofHitCount* m_TofHitCount;
	bool NBTOF1,NBTOF2,NETOF1,NETOF2,NTOF1;
	bool TBB,ETBB;
	std::vector<int> bHitpos;
	std::vector<int> ecapHitpos;
	std::vector<int> wcapHitpos;

        BesGlobalTrigSvc* m_pIBGT;
        IBesGlobalTrigSvc* m_tmpSvc;
 
};
#endif
