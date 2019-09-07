#include "ZddEvent/ZddBoard.h"
#include "ZddEvent/ZddEvent.h"

ZddBoard::ZddBoard(Event::ZddEvent* p_evt)
    : m_evt(p_evt)
{
    m_evt->m_bds.push_back(this);
}

ZddBoard::~ZddBoard()
{
}

void ZddBoard::addChannel(ZddChannel* ch)
{
    m_chs.push_back(ch);
    m_evt->m_chs.push_back(ch);
}
