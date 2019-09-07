#ifndef MdcTSF_h
#define MdcTSF_h 1
#include <vector>
#include "MdcRawEvent/MdcDigi.h"

class MdcTSF{

public:
	MdcTSF();
	~MdcTSF();
        void setMdcDigi(std::vector<int>& vmdcHit);
	void CountTS();
	void GetTS();
	void ReadLut();
        void Dump();	

        static MdcTSF* get_Mdc(void);

        inline int GetSL1TSF(int i) { return SL1TSF[i]; }
        inline int GetSL2TSF(int i) { return SL2TSF[i]; }
	inline int GetSL3TSF(int i) { return SL3TSF[i]; }
	inline int GetSL4TSF(int i) { return SL4TSF[i]; }
	inline int GetSL5TSF(int i) { return SL5TSF[i]; }
	inline int GetSL10TSF(int i) { return SL10TSF[i]; }
private:
	std::vector<int> digiId;

	std::vector<std::vector<int> > SL1;
	std::vector<std::vector<int> > SL2;
	std::vector<std::vector<int> > SL3;
	std::vector<std::vector<int> > SL4;
	std::vector<std::vector<int> > SL5;
	std::vector<std::vector<int> > SL10;

        int SL1PivotCellHit[48];
        int SL2PivotCellHit[80];
        int SL3PivotCellHit[88];
        int SL4PivotCellHit[112];
        int SL5PivotCellHit[128];
        int SL10PivotCellHit[128];

	int SL1HitCount[48][8];
	int SL2HitCount[80][12];
	int SL3HitCount[88][16];
	int SL4HitCount[112][22];
	int SL5HitCount[128][31];
	int SL10HitCount[128][81];

        int SL1TSF[48];
        int SL2TSF[80];
        int SL3TSF[88];
        int SL4TSF[112];
        int SL5TSF[128];
        int SL10TSF[128];

        static MdcTSF* mdc_Pointer;
				
};
#endif
