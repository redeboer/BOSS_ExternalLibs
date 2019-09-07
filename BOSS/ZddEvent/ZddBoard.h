#ifndef Event_ZDD_BOARD_H
#define Event_ZDD_BOARD_H

#include "ZddEvent/ZddChannel.h"

//pre-declarations
namespace Event {
    class ZddEvent;
}

//class defination
class ZddBoard
{
    public :

	typedef std::vector<ZddChannel*> Channels;

	ZddBoard(Event::ZddEvent* p_evt);
	virtual ~ZddBoard();

	//user interfaces
	int getBoardId() const { return m_boradId; }
	int getCounter() const { return m_counter; }
	int getTimeTag() const { return m_timeTag; }

	const Channels& channels() const { return m_chs; }

	//interfaces for setting data
	void setBoardId(int boardId) { m_boradId = boardId; }
	void setCounter(int counter) { m_counter = counter; }
	void setTimeTag(int timeTag) { m_timeTag = timeTag; }

	void addChannel(ZddChannel* ch);

    private :

	int  m_boradId;
	int  m_counter;
	int  m_timeTag;

	Event::ZddEvent*  m_evt;
	Channels   m_chs;
};

#endif
