#ifndef DecayMode_H
#define DecayMode_H

#include "GaudiKernel/DataObject.h"

extern const  CLID  &CLID_DecayMode;

class DecayMode : public DataObject {

  public:
  DecayMode()
  {
    for (unsigned int index=0; index<m_size; index++)
      m_data[index] = 0;
  }
  virtual ~DecayMode() {;}
  void getData (int* data, unsigned int size);
  void putData (int* data, unsigned int size);	
  virtual const CLID& clID()  const { return  DecayMode::classID();}
  static  const CLID& classID() { return CLID_DecayMode; }

  private:
  bool  values_ok(unsigned int size);

  static const unsigned int m_size = 10;
  int m_data[m_size];
	
};

#endif //DecayMode_H
