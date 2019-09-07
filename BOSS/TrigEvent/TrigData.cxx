#include <iostream>
#include "TrigEvent/TrigData.h"

const std::string TrigData::s_CONDITIONS[] = {
  "00: NCluster >= 1",
  "01: NCluster >= 2",
  "02: Barrel Cluster back to back",
  "03: Endcap Cluster back to back",
  "04: Cluster Balance in Z direction",
  "05: Barrel Cluster Balance in Phi direction",
  "06: Endcap Cluster phi Balance",
  "07: Barrel total Energy Higher threshold",
  "08: Endcap total Energy Higher threshold",
  "09: Total Energy Lower threshold",
  "10: Total Energy Middle threshold",
  "11: Energy Balance in \"Z\" direction",
  "12: NBCluster >= 1",
  "13: NECluster >= 1",
  "14: Barrel Energy Block Balance",
  "15: Endcap Energy Block Balance",
  "16: Endcap TOF back to back",
  "17: Barrel TOF back to back",
  "18: NETOF >= 2",
  "19: NETOF >= 1",
  "20: NBTOF >= 2",
  "21: NBTOF >= 1",
  "22: NTOF  >= 1",
  "23: C Track Back to Back",
  "24: NCTrk >= 2",
  "25: NCTrk >= 1",
  "26: B Track Back to Back",
  "27: NBTrk >= 2",
  "28: NBTrk >= 1",
  "29: A Track Back to Back",
  "30: NATrk >= 2",
  "31: NATrk >= 1",
  "32: MUON Back to Back 3478",
  "33: MUON Back to Back 37",
  "34: Not defined yet",
  "35: Not defined yet",
  "36: Not defined yet",
  "37: Not defined yet",
  "38: Short Track back to back",
  "39: NStrk >= N",
  "40: NStrk >= 2",
  "41: NStrk >= 1",
  "42: Long Track back to back",
  "43: NLtrk >= N",
  "44: NLtrk >= 2",
  "45: NLtrk >= 1",
  "46: NItrk >= 2",
  "47: NItrk >= 1"
};

TrigData::TrigData(int window, int timing, const int* trigCond, const int* trigChan, bool preScale)
{
  m_timeWindow = window;
  m_Timing = timing;
  m_preScale = preScale;
  for(int i = 0; i < 48; i++) {
    if(i<16) {
      m_trigChan[i] = trigChan[i];
    }
    m_trigcond[i] = trigCond[i];
  }
}

void TrigData::print() const {
  std::cout << "TrigData contains the following effective conditions:" << std::endl;
  for (int i = 0; i < 48; i++) {
    if(m_trigcond[i] == 1) std::cout << '\t' << s_CONDITIONS[i] << std::endl;
  }
}
