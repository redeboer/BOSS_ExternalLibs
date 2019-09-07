#ifndef EVENT_TRIG_TOFT_H
#define EVENT_TRIG_TOFT_H

#include <stdint.h>
#include <string>
#include <vector>

class TrigTOFT {
public:
  TrigTOFT(char* name);
  virtual ~TrigTOFT() {}

  void setTOFTTrigData(uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data);

  void initialize();

  const std::string& getName() const { return m_name; }

  inline unsigned int getTimeWindow() const { return m_window; }

  int getTofTrigCondi(int index, int win) const { return m_tofTrigCondi[index][win]; }
  int getBTofHitMap(int index, int win) const { return m_btofHitMap[index][win]; }

  int getWETofHitMap(int index, int win) const { return m_wetofHitMap[index][win]; }
  int getEETofHitMap(int index, int win) const { return m_eetofHitMap[index][win]; }

private:
  std::string m_name;

  unsigned int m_window;

  int m_tofTrigCondi[6][21];
  int m_btofHitMap[176][21];

  int m_wetofHitMap[48][21];
  int m_eetofHitMap[48][21];
};

#endif  // EVENT_TRIG_TOFT_H
