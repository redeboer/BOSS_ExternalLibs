#ifndef TofHitCount_h
#define TofHitCount_h 1
#include <vector>

#include "TofRawEvent/TofDigi.h"

class TofHitCount{
public:
	TofHitCount();
	~TofHitCount();
        void setTofDigi(std::vector<int>& vtofHit);
	inline std::vector<int>& GetbarrelHit1() { return barrelHit1; }
	inline std::vector<int>& GetbarrelHit2() { return barrelHit2; }
	inline std::vector<int>& GetecapHit() { return ecapHit; }
	inline std::vector<int>& GetwcapHit() { return wcapHit; }

        static TofHitCount* get_Tof(void);

private:
	std::vector<int> barrelHit1;
	std::vector<int> barrelHit2;
	std::vector<int> ecapHit;
	std::vector<int> wcapHit;

        static TofHitCount* tof_Pointer;
 
};
#endif
