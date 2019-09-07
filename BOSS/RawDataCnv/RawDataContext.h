#ifndef RAWDATACONTEXT_H
#define RAWDATACONTEXT_H

#include "GaudiKernel/IEvtSelector.h"

class IOpaqueAddress;
class RawDataSelector;

/** @class RawDataContext
 * @brief This class provides the Context for RawDataSelector
 **/

class RawDataContext :  public IEvtSelector::Context {

public:
   /// Constructor
  RawDataContext(const RawDataSelector* pSelector);

  RawDataContext(const RawDataContext& ctxt);
   /// Destructor
  virtual ~RawDataContext();

   /// Inequality operator.
  virtual void* identifier() const;

private:
  const IEvtSelector*  m_evtSelector;

};
#endif
