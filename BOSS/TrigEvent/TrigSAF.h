#ifndef EVENT_TRIG_SAF_H
#define EVENT_TRIG_SAF_H

#include <stdint.h>
#include <string>
#include <vector>

class TrigSAF {
public:
  TrigSAF(char* name);
  virtual ~TrigSAF() {}

  void setSAFTrigData(uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data);

  void initialize();

  const std::string& getName() const { return m_name; }

  int getDataGroup1One(int index) const { return m_dataGroup1_One[index]; }
  int getDataGroup1Zero(int index) const { return m_dataGroup1_Zero[index]; }

  int getDataGroup2One(int index) const { return m_dataGroup2_One[index]; }
  int getDataGroup2Zero(int index) const { return m_dataGroup2_Zero[index]; }

  int getDataGroup3(int index) const { return m_dataGroup3[index]; }
  int getDataGroup4(int index) const { return m_dataGroup4[index]; }

private:
  std::string m_name;

  int m_dataGroup1_One[48];
  int m_dataGroup1_Zero[48];

  int m_dataGroup2_One[48];
  int m_dataGroup2_Zero[48];

  int m_dataGroup3[48];
  int m_dataGroup4[48];
};

#endif  // EVENT_TRIG_SAF_H
