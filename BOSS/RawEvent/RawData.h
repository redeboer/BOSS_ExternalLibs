#ifndef RawData_H
#define RawData_H 
#include <iostream>
#include <vector>
#include <stdint.h>
#include <functional>
#include <algorithm>
#include "GaudiKernel/ContainedObject.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"
 
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"

/*
 *
 */

class RawData : virtual public ContainedObject { 
public:
  // Constructor
  RawData(const Identifier& id, const unsigned int timeChannel=0, const unsigned int chargeChannel=0);

  RawData(const unsigned int id);

  // Set time channel
  void setTimeChannel(const unsigned int timeChannel);
 
  // Set charge channel
  void setChargeChannel(const unsigned int chargeChannel);

  // Set track index
  void setTrackIndex(const int trackIndex);
  
  // Get associated id
  virtual Identifier identify() const;

  // Get associated int id 
  unsigned int getIntId() const;

  // Get time channel 
  unsigned int getTimeChannel() const; 
 
  // Get charge channel 
  unsigned int getChargeChannel() const;

  // Get track index
  int getTrackIndex() const;

  // Add two signals: if channel is the same, charge adds up, time set to the earliest value
  RawData &operator+=(const RawData &);
  

  //! Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const;
  virtual StreamBuffer& serialize( StreamBuffer& s );
	
  //! Fill the ASCII output stream
  virtual std::ostream& fillStream( std::ostream& s ) const;
 
protected:
  RawData(const RawData &);

  RawData &operator=(const RawData &);

  const Identifier m_id; //Offline ID for readout channel
  
  unsigned int m_timeChannel;
  
  unsigned int m_chargeChannel; 

  const unsigned int m_intId;

  int m_trackIndex;
};

//! Serialize the object for writing
inline StreamBuffer& RawData::serialize( StreamBuffer& s ) const {
      ContainedObject::serialize(s);
         s<< m_timeChannel
          << m_chargeChannel;
        return s;
}

inline StreamBuffer& RawData::serialize( StreamBuffer& s ) {
	return serialize(s); 
}

//! Fill the ASCII output stream
 
inline std::ostream& RawData::fillStream( std::ostream& s ) const {
        s << "class RawData :" << std::endl
          << " m_id: " << m_intId
          << " m_timeChannel: " << m_timeChannel
          << " m_chargeChannel: " << m_chargeChannel
          << std::endl;
        return s;
}

#endif
