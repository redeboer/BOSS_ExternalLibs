#ifndef AthenaHitsCollectionHelper_H
#define AthenaHitsCollectionHelper_H

#include <string>
// Generic interface to a hit collection. it should be used to
// remove any dependency between sensitive detectors and the 
// underlying storing system..
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

// #include "StoreGate/StoreGateSvc.h"

class AthenaHitsCollectionHelper {
public:
  AthenaHitsCollectionHelper();
  virtual ~AthenaHitsCollectionHelper();
  // following can't be virtual
  /*
  template <typename collectionType> 
  void ExportCollection(collectionType* collection)
  {      
      	StatusCode status = m_storeGate->record(collection,collection->Name(),false);
      	if (status.isFailure()) 
	{
	  	std::cout 
	      	<< "Failed to record and lock HitCollection in StoreGate!"
	      	<< std::endl;
	} 
      	else 
	{
	}
  }
  */
  /*
  template <typename collectionType> 
  const DataHandle< collectionType > RetrieveCollection(std::string name="")
  {
	const DataHandle< collectionType > collMDT;
	if (name.empty())
		StatusCode status = m_storeGate->retrieve( collMDT );
	else
		StatusCode status = m_storeGate->retrieve( collMDT , name);
	return collMDT;
  }
  */
private:
  // StoreGateSvc* 	m_storeGate;
};

#endif

