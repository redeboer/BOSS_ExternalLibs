#include <iostream>
#include <fstream>
#include "TrigEvent/TrigEACC.h"
using namespace std;

TrigEACC::TrigEACC(const char* name)
{
  m_name = name;
}

TrigEACC::~TrigEACC() {
}

void TrigEACC::initialize(){
  //initialize
  for(uint32_t i = 0; i < 11; i++) 
    for(uint32_t j = 0; j < 32; j++) 
      for(uint32_t k = 0; k < 32; k++) {
        if(j < 30) TC[i][j][k] = 0;
        if(i == 0) { TC_E[j][k] = 0; TC_W[j][k] = 0; }
        if(i == 0 && j < 16) { midCondi[j] = 0; emcTrigCondi[j][k] = 0; }
        if(i == 0 && j < 12) { E_blk[j] = 0; T_blk[j] = 0; }
        if(i == 0 && j < 2)  { E_blkE[j] = 0; T_blkE[j] = 0; E_blkW[j] = 0; T_blkW[j] = 0; }
  }
  E_B_Etot = 0;
  T_B_Etot = 0;
  E_E_Etot = 0;
  T_E_Etot = 0;
  E_blkAll = 0;
  T_blkAll = 0;
  E_eastHalf = 0;
  T_eastHalf = 0;
  E_westHalf = 0;
  T_westHalf = 0;
  E_eastEndHalf = 0;
  T_eastEndHalf = 0;
  E_westEndHalf = 0;
  T_westEndHalf = 0;
}

void TrigEACC::setEACCTrigData( uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data) {
  if(boardId != 0xd7) std::cerr << "ERROR: You set the wrong board data to TrigEACC!!!" << std::endl;

  m_window = window;

  uint32_t charge_mask = 0xFFFF;
  uint32_t time_mask   = 0xFF0000;
  for(uint32_t i = 0; i < datasize; i++) {
    if(i < 12) { 
      E_blk[i] = ((data[i]) & charge_mask);
      T_blk[i] = ((data[i]) & time_mask) >> 16;
    }
    if(i >= 12 && i < 14) { 
      E_blkE[i-12] = ((data[i]) & charge_mask);
      T_blkE[i-12] = ((data[i]) & time_mask) >> 16;
    }
    if(i >= 14 && i < 16) {
      E_blkW[i-14] = ((data[i]) & charge_mask);
      T_blkW[i-14] = ((data[i]) & time_mask) >> 16;
    }
    if(i >= 16 && i < 23) {
      if(i == 16) { E_B_Etot = ((data[i]) & charge_mask); T_B_Etot = ((data[i]) & time_mask) >> 16; }
      if(i == 17) { E_E_Etot = ((data[i]) & charge_mask); T_E_Etot = ((data[i]) & time_mask) >> 16; }
      if(i == 18) { E_blkAll = ((data[i]) & charge_mask); T_blkAll = ((data[i]) & time_mask) >> 16; }
      if(i == 19) { E_eastHalf = ((data[i]) & charge_mask); T_eastHalf = ((data[i]) & time_mask) >> 16; }
      if(i == 20) { E_westHalf = ((data[i]) & charge_mask); T_westHalf = ((data[i]) & time_mask) >> 16; }
      if(i == 21) { E_eastEndHalf = ((data[i]) & charge_mask); T_eastEndHalf = ((data[i]) & time_mask) >> 16; }
      if(i == 22) { E_westEndHalf = ((data[i]) & charge_mask); T_westEndHalf = ((data[i]) & time_mask) >> 16; }
    }
    if(i == 23) {
      for(uint32_t j = 0, mask = 1; j < 16; j++, mask <<= 1) {
        if(data[i] & mask) midCondi[j] = 1;
      }
    }
    if(i >= 24 && i < 24 + 20*window) {
      uint32_t winId = uint32_t ((i-24)/20);
      uint32_t id = i - (24+winId*20);
      if(i >= (24+winId*20) && i < (39+winId*20)) {
        for(uint32_t j = 0, mask = 1; j < 24; j++, mask <<= 1) {
          if(j < 11) { 
            if(data[i] & mask) TC[j][2*id+1][winId] = 1;
          }
          if(j >= 12 && j < 23) {
            if(data[i] & mask) TC[j-12][2*id][winId] = 1;
          }
        }
      }
      if(i == (39+winId*20)) {
        for(uint32_t j = 0, mask = 1; j < 16; j++, mask <<= 1) {
          if(data[i] & mask) TC_E[j][winId] = 1;
        }
      }
      if(i == (40+winId*20)) {
        for(uint32_t j = 0, mask = 1; j < 16; j++, mask <<= 1) {
          if(data[i] & mask) TC_E[j+16][winId] = 1;
        }
      }
      if(i == (41+winId*20)) {
        for(uint32_t j = 0, mask = 1; j < 16; j++, mask <<= 1) {
          if(data[i] & mask) TC_W[j][winId] = 1;
        }
      }
      if(i == (42+winId*20)) {
        for(uint32_t j = 0, mask = 1; j < 16; j++, mask <<= 1) {
          if(data[i] & mask) TC_W[j+16][winId] = 1;
        }
      }
      if(i == (43+winId*20)) {
        for(uint32_t j = 0, mask = 1; j < 16; j++, mask <<= 1) {
          if(data[i] & mask) emcTrigCondi[j][winId] = 1;
        }
      }
    }
  }
  if(24 + 20*window != datasize) std::cerr << "ERROR: The data in EACC are not complete" << std::endl;
}

void TrigEACC::print() {
}
