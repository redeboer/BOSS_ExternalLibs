#ifndef MdcTCount_h 
#define MdcTCount_h 1
#include <vector>

class MdcTF;
class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;

class MdcTCount{
public:
	MdcTCount();
	~MdcTCount();
	void GetTCount();
	inline int GetstrackNo() { return strackNo; }
	inline int GetltrackNo() { return ltrackNo; }
        inline int GetnItrkSL1() { return nItrkSL1; }
        inline int GetnItrkSL2() { return nItrkSL2; }
	inline std::vector<int>& GetstrackId() { return strackId; }
	inline std::vector<int>& GetltrackId() { return ltrackId; }
        inline std::vector<int>& GetintrkId1() { return intrkId1; }
        inline std::vector<int>& GetintrkId2() { return intrkId2; }
private:
	MdcTF* m_MdcTF;
        BesGlobalTrigSvc* m_pIBGT;
        IBesGlobalTrigSvc* m_tmpSvc;

	int strackNo;
	int ltrackNo;
        int nItrkSL1;
        int nItrkSL2;
	std::vector<int> strackId;
	std::vector<int> ltrackId;
        std::vector<int> intrkId1;
        std::vector<int> intrkId2;
};
#endif
