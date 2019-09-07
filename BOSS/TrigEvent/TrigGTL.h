#ifndef EVENT_TRIG_GTL_H
#define EVENT_TRIG_GTL_H

#include <stdint.h>
#include <string>
#include <vector>

class TrigGTL {
public:
  TrigGTL(char* name);
  virtual ~TrigGTL() {}

  void setGTLTrigData(uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data);

  void initialize();

  const std::string& getName() const { return m_name; }

  int getDataGroup12One(int index) const { return m_dataGroup12_One[index]; }
  int getDataGroup12Zero(int index) const { return m_dataGroup12_Zero[index]; }

  int getDataGroup34One(int index) const { return m_dataGroup34_One[index]; }
  int getDataGroup34Zero(int index) const { return m_dataGroup34_Zero[index]; }

  int getDataGroup5One(int index) const { return m_dataGroup5_One[index]; }
  int getDataGroup5Zero(int index) const { return m_dataGroup5_Zero[index]; }

private:
  std::string m_name;

  int m_dataGroup12_One[48];
  int m_dataGroup12_Zero[48];

  int m_dataGroup34_One[48];
  int m_dataGroup34_Zero[48];

  int m_dataGroup5_One[24];
  int m_dataGroup5_Zero[24];
};

#endif  // EVENT_TRIG_GTL_H
