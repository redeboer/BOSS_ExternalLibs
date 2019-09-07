#ifndef Event_ZDD_EVENT_H
#define Event_ZDD_EVENT_H

#include "GaudiKernel/DataObject.h"
#include <vector>

extern const CLID& CLID_ZddEvent;

class ZddBoard;
class ZddChannel;

namespace Event {

class ZddEvent : public DataObject
{
    public :

	typedef std::vector<ZddBoard*>   Boards;
	typedef std::vector<ZddChannel*> Channels;

	ZddEvent();
	virtual ~ZddEvent();

	/// Retrieve reference to class definition structure
	virtual const CLID& clID() const { return ZddEvent::classID(); }
	static const CLID& classID() { return CLID_ZddEvent; }

	//user interfaces
	const Boards&   boards() const { return m_bds; }
	const Channels& channels() const { return m_chs; }


    private :

	friend class ::ZddBoard;

	Boards    m_bds;
	Channels  m_chs;
};

}
#endif
