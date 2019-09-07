#ifndef RAWDATA_BUFFER_HOLDER_H
#define RAWDATA_BUFFER_HOLDER_H

#include <stdint.h>
#include <vector>

class BufferHolder
{
public:
  BufferHolder() : m_nbuf(0) {}
  virtual ~BufferHolder() {};

  void reset();

  uint32_t nBuf() const { return m_nbuf; }
  uint32_t bufSize(int i) const { return m_bufSize[i]; }
  uint32_t* operator()(int ibuf) const;

  void addBuffer(uint32_t* buffer, uint32_t size);

private:
  std::vector<uint32_t*> m_buffer;
  std::vector<uint32_t>  m_bufSize;
  uint32_t               m_nbuf;
};

#endif
