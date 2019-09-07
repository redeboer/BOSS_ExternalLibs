#ifndef RAWDATA_ZDD_EVENT_CNV_H
#define RAWDATA_ZDD_EVENT_CNV_H

#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnv/ClassID_temp.h" 

// Forward declarations.
class IRawDataInputSvc;
class ZddConverter;

// Converter Factory to create instances of this class.
template <class TYPE> class CnvFactory;

class RawDataZddEventCnv : public RawDataBaseCnv
{
    protected :
	// Standard Constructor.
	RawDataZddEventCnv(ISvcLocator* svc);
	virtual ~RawDataZddEventCnv();

    public :

	StatusCode  initialize();

	// Return the class type of this converter.
	static const CLID& classID();

	virtual long repSvcType() const {
	    return PACKEDRAWEVENT_StorageType;
	}

	static const unsigned char storageType() {
	    return PACKEDRAWEVENT_StorageType;
	}

	// Create a converted object in the Transient Data Store.
	StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

	/// override the RawDataBaseCnv version
	virtual StatusCode updateObj(IOpaqueAddress*, DataObject*);

	virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

    private:

	IRawDataInputSvc*  m_inputSvc; 
	/// Define the interface to raw data cnv
	//IRawDataCnvSvc* m_RawDataAccess;

	ZddConverter*  m_cnv;
	friend class CnvFactory<RawDataZddEventCnv>;
};

#endif
