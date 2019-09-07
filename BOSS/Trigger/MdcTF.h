#ifndef MdcTF_h
#define MdcTF_h 1
#include <vector>
#include "Trigger/MdcTSF.h"

class MdcTF{
public:
	MdcTF();
	~MdcTF();
	void GetTrack();
	void ReadTFLut();
        void Dump();

	inline int GetstrackId(int i) { return strack[i]; }
	inline int GetltrackId(int i) { return ltrack[i]; }
        inline std::vector<int>& GetintrkSL1() { return intrkSL1; }
        inline std::vector<int>& GetintrkSL2() { return intrkSL2; }
private:
	MdcTSF* m_MdcTSF;
	int strack[128];
	int ltrack[128];
        std::vector<int> intrkSL1;
        std::vector<int> intrkSL2;
	std::vector<std::vector<int> > TF;
};
#endif
