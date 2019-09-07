#include "McTruth/DecayMode.h"
#include <iostream>
void DecayMode::getData (int* data, unsigned int size) {
  for (unsigned int index=0; index<m_size; index++) {
     data[index] = m_data[index];
  }
}

void DecayMode::putData (int* data, unsigned int size) {
  assert(size); 
  for (unsigned int index=0; index<m_size; index++) {
    m_data[index] = data[index];
  }
}

bool  DecayMode::values_ok(unsigned int size) {
  if (size > m_size) {
    std::cout << " DecayMode:: error: size beyond the range!" <<std::endl;
    return false;
  } else {
    return true;
  }
}
