/***************************************************************************
 EventInfo package
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TriggerInfo.cxx,v 1.1.1.1 2007/04/25 11:46:57 zoujh Exp $
//<version>	$Name: HltDataTypes-01-01-03 $

//<<<<<< INCLUDES                                                       >>>>>>

#include "HltDataTypes/TriggerInfo.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

TriggerInfo::TriggerInfo()
    :
    m_extendedLevel1ID(0),
    m_level1TriggerType(0),
    m_level2TriggerInfo(0)
{}

TriggerInfo::TriggerInfo(number_type extendedLevel1ID,
			 number_type level1TriggerType,
			 number_type level2TriggerInfo,
			 const std::vector<number_type>& eventFilterInfo)
    :
    m_extendedLevel1ID(extendedLevel1ID),
    m_level1TriggerType(level1TriggerType),
    m_level2TriggerInfo(level2TriggerInfo),
    m_eventFilterInfo(eventFilterInfo)
{}

TriggerInfo::~TriggerInfo()
{}

TriggerInfo::number_type 
TriggerInfo::extendedLevel1ID 	() const
{
    return (m_extendedLevel1ID);
}

TriggerInfo::number_type 
TriggerInfo::level1TriggerType	() const
{
    return (m_level1TriggerType);
}

TriggerInfo::number_type 
TriggerInfo::level2TriggerInfo	() const
{
    return (m_level2TriggerInfo);
}

const std::vector<TriggerInfo::number_type>& 
TriggerInfo::eventFilterInfo () const
{
    return (m_eventFilterInfo);
}


