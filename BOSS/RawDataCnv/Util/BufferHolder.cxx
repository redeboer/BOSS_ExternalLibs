#include "RawDataCnv/Util/BufferHolder.h"

void BufferHolder::reset() {
  m_nbuf = 0;
  m_buffer.clear();
  m_bufSize.clear();
}

uint32_t* BufferHolder::operator()(int ibuf) const {
  return m_buffer[ibuf];
}

void BufferHolder::addBuffer(uint32_t* buffer, uint32_t size) {
  m_nbuf++;
  m_buffer.push_back(buffer);
  m_bufSize.push_back(size);
}

