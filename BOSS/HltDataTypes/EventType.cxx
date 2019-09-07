/***************************************************************************
 Event Info Package
 -----------------------------------------
 Copyright (C) 2000 by ATLAS Collaboration
 ***************************************************************************/


//<<<<<< INCLUDES                                                       >>>>>>

#include "HltDataTypes/EventType.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

// List of EventTypeCodes
//   To add on a new code, simply extend at the end 
const EventType::EventTypeCode  EventType::IS_SIMULATION        = 0;
const EventType::EventTypeCode  EventType::IS_TESTBEAM          = 1;
const EventType::EventTypeCode  EventType::IS_CALIBRATION       = 2;


EventType::EventType()
{}


EventType::~EventType()
{}


void 		
EventType::add_type (EventTypeCode type_code)
{
    if(m_bit_mask.size() <= type_code) m_bit_mask.resize(type_code + 1, false);
    m_bit_mask[type_code] = true;
}

void 		
EventType::set_user_type	(const std::string& user_type)
{
    m_user_type = user_type;
}

void                
EventType::set_detdescr_tags(const NameTagPairVec& pairs)
{
    // We must save in m_user_type the dd tags for the moment to avoid
    // schema evolution. Overwrite if existing "sep" is found.

    // Force overwrite:
    m_user_type = user_type(); 

    char sep = '#';
    for (unsigned int i = 0; i < pairs.size(); ++i) {
	m_user_type += sep;
	m_user_type += pairs[i].first;
	m_user_type += sep;
	m_user_type += pairs[i].second;
    }
}


bool 		
EventType::test		(EventTypeCode type_code) const
{
    if (m_bit_mask.size() <= type_code) return false;
    return m_bit_mask[type_code];
}

const std::string&	
EventType::user_type	(void) const
{
    char sep = '#';
    std::string::size_type beg = m_user_type.find(sep);
    static std::string user_type;
    if (beg != std::string::npos) {
	user_type = m_user_type.substr(0, beg);
    }
    else {
	user_type = m_user_type;
    }
    return user_type;
}


void       
EventType::get_detdescr_tags(NameTagPairVec& pairs)
{
    // We must extract from m_user_type the dd tags for the moment to
    // avoid schema evolution.

    char sep = '#';
    bool done = false;
    std::string::size_type beg = m_user_type.find(sep);
    do {
	if (beg != std::string::npos) {
	  std::string::size_type end1 = m_user_type.find(sep, beg+1);
	    if (end1 != std::string::npos) {
	      std::string::size_type end2 = m_user_type.find(sep, end1+1);
		if (end2 != std::string::npos) {
		    // end2 is a new separator
		    std::string first  = m_user_type.substr(beg+1, end1-beg-1);
		    std::string second = m_user_type.substr(end1+1, end2-end1-1);
		    pairs.push_back(NameTagPair(first,second));

		    // continue with new beg
		    beg = end2;
		}
		else {
		    // end2 is the end of the string
		    std::string first  = m_user_type.substr(beg+1, end1-beg-1);
		    std::string second = m_user_type.substr(end1+1, m_user_type.size()-1);
		    pairs.push_back(NameTagPair(first,second));
		    done = true; // finished
		}
	    }
	    else {
		done = true; // finished
	    }
	}
	else {
	    done = true; // finished
	}
    } while (!done);
}


EventType::BitMaskIterator 	
EventType::bit_mask_begin	(void) const
{
    return m_bit_mask.begin();
}

EventType::BitMaskIterator 	
EventType::bit_mask_end		(void) const
{
    return m_bit_mask.end();
}
