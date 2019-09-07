#ifndef Event_RecZddChannel_H
#define Event_RecZddChannel_H

#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ContainedObject.h"
#include <vector>
#include <memory>  //std::pair

extern const CLID & CLID_RecZddChannel;

class RecZddChannel : virtual public ContainedObject
{
    public :

	typedef std::pair<int, float> FragType;  //time and energy

	virtual const CLID& clID() const { return RecZddChannel::classID(); }
	static  const CLID& classID() { return CLID_RecZddChannel; }

	RecZddChannel();
	virtual ~RecZddChannel();

	RecZddChannel(const RecZddChannel&);
	RecZddChannel& operator=(const RecZddChannel&);

	int  channelId() const { return m_chId; }
	int  scanCode()  const { return m_scanCode; }
	int  baseLine()  const { return m_baseLine; }
	int  phase()     const { return m_phase; }
	const std::vector<FragType>& fragments() const { return m_frags; }

	void setChannelId(int chId) { m_chId = chId; }
	void setScanCode(int scanCode) { m_scanCode = scanCode; }
	void setBaseLine(int baseLine) { m_baseLine = baseLine; }
	void setPhase(int phase) { m_phase = phase; }
	void setFragments(const std::vector<FragType>& frags) { m_frags = frags; }

	void addFragment(int time, float energy) { m_frags.push_back(std::make_pair(time, energy)); }
	void addFragment(const FragType& fragment) { m_frags.push_back(fragment); }

    private :

	int  m_chId;
	int  m_scanCode;
	int  m_baseLine;
	int  m_phase;
	std::vector<FragType>  m_frags;
};

typedef ObjectVector<RecZddChannel> RecZddChannelCol;
typedef RecZddChannelCol::iterator  RecZddChannelIterator;

#endif
