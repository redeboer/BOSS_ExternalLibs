//
#ifndef ACTION_H
#define ACTION_H 1

#include "GdmlStatusCode.hh"
#include "SXComponentObject.hh"

class ProcessingContext;

class Action : virtual public SAXComponentObject
{
public:
  virtual const SAXComponentObject* Build() const
  {
    return 0;
  }
  virtual const SAXComponentObject::EType Type() const
  {
    return SAXComponentObject::eAction;
  }

public:
  virtual GdmlStatusCode Run( const ProcessingContext* const context ) = 0;
};

#endif // ACTION_H

