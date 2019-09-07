#include "ZddEvent/ZddChannel.h"

ZddChannel::ZddChannel()
    : m_chId(-1),m_scanCode(0)
{
}

ZddChannel::~ZddChannel()
{
    Fragments::iterator end = m_frags.end();
    for ( Fragments::iterator it = m_frags.begin(); it != end; ++it ) {
	delete [] it->sample;
    }
}
