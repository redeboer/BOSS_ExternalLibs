#ifndef Event_ZDD_CHANNEL_H
#define Event_ZDD_CHANNEL_H

#include <vector>

/////////////////////////////////////
typedef struct {
    int   start_index;  //the index of the 1st sample in this fragment
    int   length;       //the length (in bytes) of this fragment
    unsigned char* sample;       //data samples in this fragment
}
// the data model for a ZDD fragment
ZddFragment;


/////////////////////////////////////
class ZddChannel
{
    public :

	typedef std::vector<ZddFragment> Fragments;

	ZddChannel();
	virtual ~ZddChannel();

	int  getChId() const { return m_chId; }

	int  getScanCode() const { return m_scanCode; }

	const Fragments& fragments() const { return m_frags; }

	void setChId(int chId) { m_chId = chId; }

	void setScanCode(int scanCode) { m_scanCode = scanCode; }

	void addFragments(const ZddFragment& frag) { m_frags.push_back(frag); }

    private :

	int  m_chId;
	int  m_scanCode;
	Fragments  m_frags;
};

#endif
