#include <iostream>
#include "TrigEvent/TrigSAF.h"

TrigSAF::TrigSAF(char* name) 
{
  m_name = name;
}

void TrigSAF::initialize()
{
  for(int i = 0; i < 48; i++)
  {
    m_dataGroup1_One[i]  = 0;
    m_dataGroup1_Zero[i] = 0;
    m_dataGroup2_One[i]  = 0;
    m_dataGroup2_Zero[i] = 0;
    m_dataGroup3[i] = 0;
    m_dataGroup4[i] = 0;
  }
}

void TrigSAF::setSAFTrigData( uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data) {

  if((boardId != 0xd2)&&(boardId != 0xd4)&&(boardId != 0xd6)) std::cerr << "ERROR: You set the wrong board data to TrigSAF!!!" << std::endl;
  
  uint32_t jBegin = 0, jEnd = 0;
  if(boardId == 0xd2) { jBegin = 32; jEnd = jBegin+16; } //SAF1
  if(boardId == 0xd4) { jBegin = 16; jEnd = jBegin+16; } //SAF2
  if(boardId == 0xd6) { jBegin =  0; jEnd = jBegin+16; } //SAF3

  for(uint32_t i = 0; i < datasize; i++) {
    uint32_t dataId = ((data[i] >> 16) & 0xFF);
    for(uint32_t j = jBegin, mask = 1; j < jEnd; j++, mask <<= 1) {
      if(dataId == 1) {
        if((data[i] & mask) != 0) {
          m_dataGroup1_One[j]    += 1;
        }
        if(m_dataGroup1_One[j] == 0) m_dataGroup1_Zero[j] += 1;
      }
      if(dataId == 2) {
        if((data[i] & mask) != 0) {
          m_dataGroup2_One[j] += 1;
        }
        if(m_dataGroup2_One[j] == 0) m_dataGroup2_Zero[j] += 1;
      }
      if(dataId == 3) {
        if((data[i] & mask) != 0) {
          m_dataGroup3[j]    = 1;
        }
      }
      if(dataId == 4) {
        if((data[i] & mask) != 0) {
          m_dataGroup4[j]    = 1; 
        }
      }
    }
  }
}
