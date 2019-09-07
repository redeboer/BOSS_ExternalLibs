#include "ZddEvent/RecZddChannel.h"

RecZddChannel::RecZddChannel()
    : m_chId(-1),m_scanCode(-1),m_baseLine(-1),m_phase(-1)
{
}

RecZddChannel::~RecZddChannel()
{
}

RecZddChannel::RecZddChannel(const RecZddChannel& val)
    : ContainedObject(val),
      m_chId(val.m_chId),
      m_scanCode(val.m_scanCode),
      m_baseLine(val.m_baseLine),
      m_phase(val.m_phase),
      m_frags(val.m_frags)
{
}

RecZddChannel& RecZddChannel::operator=(const RecZddChannel& val)
{
    this->ContainedObject::operator=(val);
    m_chId = val.m_chId;
    m_scanCode = val.m_scanCode;
    m_baseLine = val.m_baseLine;
    m_phase = val.m_phase;
    m_frags = val.m_frags;

    return (*this);
}
