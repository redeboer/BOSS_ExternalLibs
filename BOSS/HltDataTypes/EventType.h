/***************************************************************************
 EventInfo Package
 -----------------------------------------
 Copyright (C) 2000 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: EventType.h,v 1.1.1.1 2007/04/25 11:46:57 zoujh Exp $
//<version>	$Name: HltDataTypes-01-01-03 $

#ifndef EVENTINFO_EVENTTYPE_H
# define EVENTINFO_EVENTTYPE_H 1

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include <string>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  class EventType
 **
 **  This class represents the "type of event" where the type is given
 **  by one or more "characteristics".
 **
 **  Standard characteristics:
 **  -------------------------
 **
 **      IS_SIMULATION  // false means IS_DATA
 **      IS_TESTBEAM    // false means IS_FROM_ATLAS_DET
 **      IS_CALIBRATION // false means IS_PHYSICS
 **    
 **    Since an event may have MORE than one characteristic, a testbeam
 **    simulation event would respond true to first two of the above
 **    characteristics, whereas an offline simulation event would
 **    respond true to ONLY IS_SIMULATION.
 **
 **    These are set with:
 **
 **      void 		add_type	(EventTypeCode type_code);
 **
 **    where the possible EventTypeCode's are provided as constants,
 **    e.g.:
 **
 **      static const EventTypeCode	IS_SIMULATION;
 **
 **    Thus, one would set IS_SIMULATION by:
 **
 **      an_event_type.set_type_bit(EventType::IS_SIMULATION);
 **
 **
 **  User-defined characteristics:
 **  -----------------------------
 **
 **    There is a possible to set and get a "user-defined"
 **    characteristic in terms of a string:
 **
 **    void 		add_type	(const string& user_type);
 **    const string&	user_type	(void) const;
 **
 **    
 **  Access to the full set of characteristics:
 **  ------------------------------------------
 **
 **    This is possible via:
 **
 **      BitMaskIterator 	bit_mask_begin	(void) const;
 **      BitMaskIterator 	bit_mask_end	(void) const;
 **
 **
 **  Implementation details:
 **  -----------------------
 **
 **    The full set of characteristics is provided by static
 **    constants. One may add new event characteristics BOTH by adding
 **    more static constants AND by providing the cooresponding new
 **    boolean methods.
 **/
class EventType {
public:

    // public typedefs:
    typedef	std::vector<bool>                   BitMask;  
    typedef     BitMask::const_iterator             BitMaskIterator;  
    typedef     BitMask::size_type                  EventTypeCode;  
    typedef     std::pair<std::string,std::string>  NameTagPair;
    typedef     std::vector<NameTagPair>            NameTagPairVec;

    EventType();
    virtual ~EventType();

    // Set characteristics:
    void 		add_type	(EventTypeCode type_code);
    void 		set_user_type	(const std::string& user_type);
    void                set_detdescr_tags(const NameTagPairVec& pairs);

    // Tests for standard characteristics
    bool 		test		(EventTypeCode type_code) const;

    // Access to user type
    const std::string&	user_type	(void) const;

    // Access DetDescr tags
    void                get_detdescr_tags(NameTagPairVec& pairs);


    // The set of possible characteristic codes:
    static const EventTypeCode	IS_SIMULATION;  // false: IS_DATA
    static const EventTypeCode	IS_TESTBEAM;    // false: IS_FROM_ATLAS_DET
    static const EventTypeCode	IS_CALIBRATION; // false: IS_PHYSICS
    

    // Access to full set of standard characteristics
    BitMaskIterator 	bit_mask_begin	(void) const;
    BitMaskIterator 	bit_mask_end	(void) const;

private:
    BitMask 		m_bit_mask; 
    std::string 	m_user_type;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // EVENTINFO_EVENTTYPE_H
