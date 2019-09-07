// $Id: HepHistID.h,v 1.1.1.1 2005/04/21 01:15:12 zhangy Exp $
//------------------------------------------------------------------------
//
// Description:
//	Class HepHistID :
//	This class allows ID numbers, ID strings or both to be used
//	to uniquely identify histograms.  The actual use of this
//	class depends on the concrete package that implements HepTuple
//
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Scott Metzler		Original author
//
//------------------------------------------------------------------------

#ifndef _HepHistID_H
#define _HepHistID_H

#include <string>

class HepHistID {
public: 
  // destructor
  virtual ~HepHistID() {}

  // constructors
  HepHistID(int theNumber);
    // constructor with number
  HepHistID(const char * theString);
    // constructor with string
  HepHistID(int theNumber, std::string theString);
    // constructor with both number and string
  HepHistID(const HepHistID & other);
    // copy constructor
  
  // accessors
  inline bool isIDnumberSet() const { return _numSet; }
    // number used in constructor
  inline bool isIDstringSet() const { return _stringSet; }
    // string used in constructor
  inline int getIDnumber() const { return _idNumber; }
    // the id number
  std::string getIDstring() const { return _idString; }
    // the id string

  // operations
  HepHistID & operator++(); // prefix ++
  const HepHistID operator++(int); // postfix ++

  HepHistID & operator--(); // prefix -- 
  const HepHistID operator--(int); // postfix -- 

private:
  bool _numSet;
  bool _stringSet;
  int _idNumber;
  std::string _idString;
};

#endif
