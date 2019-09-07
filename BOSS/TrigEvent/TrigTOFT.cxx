#include <iostream>
#include "TrigEvent/TrigTOFT.h"

TrigTOFT::TrigTOFT(char* name) 
{
  m_name = name;
}

void TrigTOFT::initialize()
{
  for(int i = 0; i < 176; i++) 
    for(int j = 0; j < 21; j++) {
    if(i < 6) m_tofTrigCondi[i][j] = 0;
    if(i < 48) {
      m_wetofHitMap[i][j] = 0;
      m_eetofHitMap[i][j] = 0;
    }
    m_btofHitMap[i][j] = 0;
  }
}

void TrigTOFT::setTOFTTrigData( uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data) {

  if(boardId != 0xDA) std::cerr << "ERROR: You set the wrong board data to TrigTOFT!!!" << std::endl;

  m_window = window; 

  for (uint32_t i = 0; i < datasize; i++) {
    int winId = int (i/14);
    //int id = (i-winId*14)%14;
    //DW4 trigger conditions
    if(i==(1+winId*14)) {
      for(uint32_t condId = 1,loop = 0; loop < 6; condId <<=1,loop++) {
        if(data[i] & condId) m_tofTrigCondi[loop][winId] = 1;
      }
    }

    //DW9-DW12 for inner layer hit, DW13--DW16 outer layer hit
    if(i >= (6+winId*14) && i <= (13+winId*14)) {
      if(i == (6+winId*14) || i == (10+winId*14)) {
        for(uint32_t btofId = 1,loop = 0; loop < 16; btofId <<=1,loop++) {
          if(data[i] & btofId) {
            if(i == (6+winId*14))  m_btofHitMap[loop+72][winId] = 1;
            if(i == (10+winId*14)) m_btofHitMap[loop+72+88][winId] = 1;
          }
        }
      }
      else {
        for(uint32_t btofId = 1,loop = 0; loop < 24; btofId <<=1,loop++) {
          if(data[i] & btofId) {
            if(i >= (7+winId*14) && i <= (9+winId*14)) m_btofHitMap[loop+(9+winId*14-i)*24][winId] = 1;
            else m_btofHitMap[loop+(13+winId*14-i)*24+88][winId] = 1;
          }
        }
      }
    }

    //DW7-DW8 for east end cap hits
    if(i>=(4+winId*14) && i<=(5+winId*14)) {
      for(uint32_t eetofId = 1,loop = 0; loop < 24; eetofId <<=1,loop++) {
        if(data[i] & eetofId)  m_eetofHitMap[loop+(5+winId*14-i)*24][winId] = 1; 
      }
    }

    //DW5-DW6 for east end cap hits
    if(i>=(2+winId*14) && i<=(3+winId*14)) {
      for(uint32_t wetofId = 1,loop = 0; loop < 24; wetofId <<=1,loop++) {
        if(data[i] & wetofId) m_wetofHitMap[loop+(3+winId*14-i)*24][winId] = 1;
      }
    }
  }
}
