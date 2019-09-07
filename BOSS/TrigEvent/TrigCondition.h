#ifndef EVENT_TRIG_CONDITION_H
#define EVENT_TRIG_CONDITION_H

#include <stdint.h>
#include <string>
#include <vector>

class TrigCondition {
public:
  TrigCondition(char* name, uint32_t window, const uint32_t* data);
  virtual ~TrigCondition() {}

  const std::string& getName() const { return m_name; }
  const std::vector<uint32_t>& getCondList() const { return m_condList; }
  uint32_t getNumOne(int index) const { return m_num_One[index]; }
  uint32_t getNumZero(int index) const { return m_num_Zero[index]; }

  void print() const;

private:
  std::string m_name;
  std::vector<uint32_t> m_condList;

  uint32_t m_num_One[48];
  uint32_t m_num_Zero[48];

  static const std::string s_CONDITIONS[48];
};

#endif  // EVENT_TRIG_CONDITION_H
