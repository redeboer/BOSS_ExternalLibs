#include <iostream>
#include "TrigEvent/TrigCondition.h"

const std::string TrigCondition::s_CONDITIONS[] = {
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
  "12: Barrel Energy difference Balance",
  "13: Endcap Energy difference Balance",
  "14: Energy Block Balance",
  "15: Endcap Energy Balance",
  "16: Endcap TOF back to back",
  "17: Barrel TOF back to back",
  "18: NETOF >= 2",
  "19: NETOF >= 1",
  "20: NBTOF >= 2",
  "21: NBTOF >= 1",
  "22: Not defined yet",
  "23: Not defined yet",
  "24: Not defined yet",
  "25: Not defined yet",
  "26: Not defined yet",
  "27: Not defined yet",
  "28: Not defined yet",
  "29: Not defined yet",
  "30: Not defined yet",
  "31: Not defined yet",
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
  "46: NItrk >= 1",
  "47: NItrk >= 2"
};

TrigCondition::TrigCondition(char* name, uint32_t window, const uint32_t* data) :
  m_name(name)
{
  for (uint32_t group = 0; group < 2; group++) {
    uint32_t iBegin = group * 24;
    uint32_t iEnd   = (group + 1) * 24;
    uint32_t jBegin = group * window;
    uint32_t jEnd   = (group + 1) * window;
    for (uint32_t i = iBegin, mask = 1; i < iEnd; i++, mask <<= 1) {
      uint32_t num_One = 0, num_Zero = 0;
      for (uint32_t j = jBegin; j < jEnd; j++) {
	if ((data[j] & mask) != 0) {
	  num_One++;
	}
	else {
	  if (num_One == 0) num_Zero++;
	}
      }
      m_num_One[i]  = num_One;
      m_num_Zero[i] = num_Zero;
    }
  }

  for (uint32_t i = 0; i < 48; i++) {
    if (m_num_One[i] != 0) m_condList.push_back(i);
  }
}

void TrigCondition::print() const {
  int size = m_condList.size();
  std::cout << "TrigCondition-" << m_name << " contains the following conditions:" << std::endl;
  for (int i = 0; i < size; i++) {
    std::cout << '\t' << s_CONDITIONS[m_condList[i]] << std::endl;
  }
}
