#include "ZddEvent/ZddEvent.h"
#include "ZddEvent/ZddBoard.h"
#include "ZddEvent/ZddChannel.h"

using Event::ZddEvent;

ZddEvent::ZddEvent()
    : DataObject()
{
}

ZddEvent::~ZddEvent()
{
    Boards::iterator end1 = m_bds.end();
    for ( Boards::iterator it = m_bds.begin(); it != end1; ++it ) {
	delete (*it);
    }

    Channels::iterator end2 = m_chs.end();
    for ( Channels::iterator it = m_chs.begin(); it != end2; ++it ) {
	delete (*it);
    }
}
