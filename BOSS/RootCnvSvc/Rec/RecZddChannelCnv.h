#ifndef RecZddChannelCnv_H
#define RecZddChannelCnv_H

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecZddChannelCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class RecZddChannelCnv : public RootEventBaseCnv
{
    friend class CnvFactory<RecZddChannelCnv>;

    public :

	static const CLID& classID() { return CLID_RecZddChannelCol; }

	virtual ~RecZddChannelCnv() {}

    protected :

	RecZddChannelCnv(ISvcLocator* svc);

	/// transformation to root
	virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

	/// transformation from root
	virtual StatusCode TObjectToDataObject(DataObject*& obj);

    private :
	/// relational maps
	commonData m_common;

	TObjArray*   m_recZddChannelCol;
};

#endif
