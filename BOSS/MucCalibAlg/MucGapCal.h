//------------------------------------------------------------------------------|
//      [File  ]:                       MucGapCal.h                                |
//      [Brief ]:       Head file of MUC geometry gap of Gap class              |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_GAP_H
#define MUC_GAP_H

#include<iostream>

#include "MucCalibAlg/MucEntityCal.h"
#include "MucCalibAlg/MucBoxCal.h"

using namespace std;

class MucGapCal : public MucEntityCal
{
  public:
    MucGapCal( int part, int segment, int layer );
    MucGapCal( const MucGapCal &other );
    MucGapCal& operator =( const MucGapCal &other );
    ~MucGapCal();
    
    MucBoxCal* GetBox();

  protected:			
    virtual void  Init();
    
    virtual void  SetTheta();
    virtual void  SetRin();
    virtual void  SetRout();
    virtual void  SetRc();
    
    virtual void  SetThin();
    virtual void  SetW();
    virtual void  SetH();
    virtual void  SetL();
    virtual void  SetWu();
    virtual void  SetWd();
    
  private:

    MucBoxCal* m_MucBoxCal; 
};

#endif

