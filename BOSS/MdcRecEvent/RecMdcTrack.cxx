/*
* @class : RecMdcTrack 
*  this class models "Track" used in Mdc Reconstruction                        
*                                                                              
* ********************************************************/

#include "MdcRecEvent/RecMdcTrack.h"

  const CLID& RecMdcTrack::clID() const {
    return RecMdcTrack::classID();
  }
                                                                               
  const CLID& RecMdcTrack::classID() {
    return CLID_RecMdcTrack;
  }

  RecMdcTrack::RecMdcTrack() 
  : m_pivot(0,0,0),
    m_fiTerm(0.0),	         
    m_nhits(0)
   {   
   }

  RecMdcTrack::RecMdcTrack( const RecMdcTrack& rectrk)
  : DstMdcTrack(rectrk),
    m_pivot(rectrk.m_pivot),
    m_fiTerm(rectrk.m_fiTerm),
    m_nhits(rectrk.m_nhits),
    m_vechits(rectrk.m_vechits) 
  {
  }

  RecMdcTrack::RecMdcTrack( const DstMdcTrack& dstrk) 
  : DstMdcTrack(dstrk),
    m_pivot(-99.,-99.,-99.),
    m_fiTerm(-99.),
    m_nhits(-99)
  {   
  }

  RecMdcTrack& RecMdcTrack::operator=(const RecMdcTrack& rectrk) 
  {
    if( &rectrk != this ){
      DstMdcTrack::operator=(rectrk);
      m_pivot = rectrk.m_pivot;
      m_fiTerm = rectrk. m_fiTerm;
      m_nhits = rectrk. m_nhits;
      m_vechits = rectrk.m_vechits;
    }
    return  *this;
  }
 
  RecMdcTrack& RecMdcTrack::operator=(const DstMdcTrack& dstrk)
  {
    if( &dstrk != this ){
      DstMdcTrack::operator=(dstrk);  
      m_pivot[0] = -99.;
      m_pivot[1] = -99.;
      m_pivot[2] = -99.;
      m_fiTerm   = -99.;
      m_nhits    = -99;
    }
    return  *this;
  }  


  RecMdcTrack::~RecMdcTrack() {
  }               

