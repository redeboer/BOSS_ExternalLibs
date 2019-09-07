#ifndef BUILDER_H
#define BUILDER_H

#include <stdint.h>
#include <map>
#include <string>

#include "GaudiKernel/PropertyMgr.h"    
#include "GaudiKernel/StatusCode.h" 
#include "RawDataCnv/EventManagement/RawEvent.h"

using namespace std;

class Builder
{

 protected:

  Builder();
  virtual  ~Builder();

  class DataErrException{};

 public: 
  typedef std::map<uint32_t, uint32_t> TE2REMAP; 

  static bool expect( ifstream &f, string msg, string fname );
  static bool expectInt( ifstream &f, string msg, string fname, uint32_t &val1, uint32_t &val2 );
  static bool expectLong( ifstream &f, string msg, string fname, uint64_t &val );  //zoujh
  static bool find( ifstream &f, string msg, string fname );   //zoujh

  virtual StatusCode  initialize(string &initFile) {return StatusCode::SUCCESS;};
  virtual uint32_t getTEID(uint32_t teid) = 0;
  virtual uint32_t getREID(uint32_t reid) = 0;
  /*virtual void unPack (uint32_t reDigi, uint32_t &TEId, uint32_t &TEData, 
                       uint32_t &overFlow, uint32_t &TorQ) {};*/

 protected:
  void append2event(WriteRawEvent*& re, uint32_t source_id, uint32_t size, uint32_t shift = 0);

 protected: 

  PropertyMgr m_propMgr;

  // zoujh
  uint32_t*       m_buf;
  static string   m_confFile;
  // fucd
  uint32_t        m_status;
};

#endif
