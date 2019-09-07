#include <iostream>
#include "TrigEvent/TrigGTL.h"

TrigGTL::TrigGTL(char* name) 
{
  m_name = name;
}

void TrigGTL::initialize()
{
  for(int i = 0; i < 48; i++)
  {
    m_dataGroup12_One[i]  = 0;
    m_dataGroup12_Zero[i] = 0;
    m_dataGroup34_One[i]  = 0;
    m_dataGroup34_Zero[i] = 0;
    if(i < 24) {
      m_dataGroup5_One[i]   = 0;
      m_dataGroup5_Zero[i]  = 0;
    }
  }
}

void TrigGTL::setGTLTrigData( uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data) {

  if(boardId != 0xD3) std::cerr << "ERROR: You set the wrong board data to TrigGTL!!!" << std::endl;
  if(datasize%window != 0) std::cerr << "WARNING: The data in GTL maybe not complete!!!" << std::endl;
 
  for(uint32_t i = 0; i < datasize; i++) {
    uint32_t dataId = ((data[i] >> 24) & 0x7);
    for(uint32_t j = 0, mask = 1; j < 24; j++, mask <<= 1) {
      if(dataId == 1) {
        if((data[i] & mask) != 0) {
          m_dataGroup12_One[j]    += 1;
        }
        if(m_dataGroup12_One[j] == 0) m_dataGroup12_Zero[j] += 1;
      }
      if(dataId == 2) {
        if((data[i] & mask) != 0) {
          m_dataGroup12_One[j+24] += 1;
        }
        if(m_dataGroup12_One[j+24] == 0) m_dataGroup12_Zero[j+24] += 1;
      }
      if(dataId == 3) {
        if((data[i] & mask) != 0) {
          m_dataGroup34_One[j]    += 1;
        }
        if(m_dataGroup34_One[j] == 0) m_dataGroup34_Zero[j] += 1;
      }
      if(dataId == 4) {
        if((data[i] & mask) != 0) {
          m_dataGroup34_One[j+24] += 1; 
        }
        if(m_dataGroup34_One[j+24] == 0) m_dataGroup34_Zero[j+24] += 1;
      }
      if(dataId == 5) {
        if((data[i] & mask) != 0) {
          m_dataGroup5_One[j]     += 1; 
        }
        if(m_dataGroup5_One[j] == 0) m_dataGroup5_Zero[j] += 1;
      }
    }
  }
}
