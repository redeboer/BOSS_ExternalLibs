#ifndef IHLTINTERFACE_H
#define IHLTINTERFACE_H

#include "Identifier/Identifier.h"
#include <stdint.h> //uint32_t
#include <vector>

class IHltInterface{
 public:
  IHltInterface(){};
  virtual ~IHltInterface(){};

  virtual void push_mdc(uint32_t data=0)=0;
  virtual void push_tof(uint32_t data=0)=0;
  virtual void push_emc(uint32_t data=0)=0;
  virtual void push_muc(uint32_t data=0)=0;
  virtual void push_con(uint32_t data=0)=0;
  virtual void push_mdc(float data=0)=0;
  virtual void push_tof(float data=0)=0;
  virtual void push_emc(float data=0)=0;
  virtual void push_muc(float data=0)=0;
  virtual void push_con(float data=0)=0;
// virtual bool setData(const Identifier& id=0, uint32_t data=0)=0;
// virtual bool setData(const Identifier& id=0, float data=0)=0;
// virtual void setMdcData(const std::vector<uint32_t>& data=0)=0;
// virtual void setTofData(const std::vector<uint32_t>& data=0)=0;
// virtual void setEmcData(const std::vector<uint32_t>& data=0)=0;
// virtual void setMucData(const std::vector<uint32_t>& data=0)=0;
// virtual void setConData(const std::vector<uint32_t>& data=0)=0;
// virtual bool getData(const Identifier& id=0, uint32_t* data=0)=0;

  virtual bool setData(const Identifier& id, uint32_t data=0)=0;
  virtual bool setData(const Identifier& id, float data=0)=0;
  virtual void setMdcData(const std::vector<uint32_t>& data)=0;
  virtual void setTofData(const std::vector<uint32_t>& data)=0;
  virtual void setEmcData(const std::vector<uint32_t>& data)=0;
  virtual void setMucData(const std::vector<uint32_t>& data)=0;
  virtual void setConData(const std::vector<uint32_t>& data)=0;
  virtual bool getData(const Identifier& id, uint32_t* data=0)=0;
 
  virtual void print() const=0;
};
#endif
