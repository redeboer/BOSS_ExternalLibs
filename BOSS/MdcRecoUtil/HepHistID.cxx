#include "MdcRecoUtil/HepHistID.h"
using std::string;

HepHistID::HepHistID(int theNumber) : 
	_numSet(true), 
	_stringSet(false),
	_idNumber(theNumber),
	_idString() {}

HepHistID::HepHistID(const char * theString) :
	_numSet(false), 
	_stringSet(true),
	_idNumber(0),
	_idString(theString) {}

HepHistID::HepHistID(int theNumber, string theString) :
	_numSet(true), 
	_stringSet(true),
	_idNumber(theNumber),
	_idString(theString) {}

HepHistID::HepHistID(const HepHistID & other) :
	_numSet(other._numSet), 
	_stringSet(other._stringSet),
	_idNumber(other._idNumber),
	_idString(other._idString) {}

// prefix increment number ID
HepHistID & HepHistID::operator++() { 
  if ( _numSet ) {
    ++_idNumber;
  };
  // return this
  return *this; 
}

// postfix increment number ID
const HepHistID HepHistID::operator++(int) { 
  // initialize result
  HepHistID result(*this);
  // increment this 
  ++(*this);
  // return result
  return result; 
}

// prefix decrement number ID
HepHistID & HepHistID::operator--() { 
  if ( _numSet ) {
    --_idNumber;
  };
  // return this
  return *this; 
}

// postfix decrement number ID
const HepHistID HepHistID::operator--(int) { 
  // initialize result
  HepHistID result(*this);
  // increment this 
  --(*this);
  // return result
  return result; 
}
