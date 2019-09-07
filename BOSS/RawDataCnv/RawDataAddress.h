//====================================================================
//	RawDataAddress.h
//====================================================================
//
// Description: An address class used to access Event Filter packed
//              raw event persistent data from within the Athena 
//              framework.
//
//--------------------------------------------------------------------

#ifndef EF_TDR_EVENT_ADDRESS_H
#define EF_TDR_EVENT_ADDRESS_H

// Include files.
#include <string>
#include "GaudiKernel/GenericAddress.h"

// Namespaces.
using namespace std;

// Forward declarations.
class EVENT;

class RawDataAddress : public GenericAddress
{

public:

   // Standard Constructor.
   RawDataAddress(const CLID& clid, const string& fname, 
      const string& cname);

   // Standard Destructor.
   virtual ~RawDataAddress();

  ///Event accessors
  void setEvent(const EVENT* evt);
  inline const EVENT* event() const  {return m_Event;}
  virtual const CLID& clID()  const;

private:

  // Pointer to Event Filter raw event data wrapper class.
  const EVENT* m_Event;
  /// Class id
  CLID            m_clID;


};

#endif // EF_TDR_EVENT_ADDRESS_H
