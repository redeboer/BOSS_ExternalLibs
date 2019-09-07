//------------------------------------------------------------------------------|
//      [File  ]:                       MucBoxCal.h                             |
//      [Brief ]:       Head file of MUC geometry Box class for calibration     |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_BOX_CAL_H
#define MUC_BOX_CAL_H

#include<iostream>

#include "MucCalibAlg/MucEntityCal.h"

using namespace std;

class MucBoxCal : public MucEntityCal
{
  public:
    MucBoxCal( int part, int segment, int layer );
    MucBoxCal( const MucBoxCal &other );
    MucBoxCal &operator =( const MucBoxCal &other );
    ~MucBoxCal();
		
  protected:			
        	
    virtual void    Init();

    virtual void    SetTheta();
    virtual void    SetRin();
    virtual void    SetRout();
    virtual void    SetRc();

    virtual void    SetThin();
    virtual void    SetW();
    virtual void    SetH();
    virtual void    SetL();
    virtual void    SetWu();
    virtual void    SetWd();
    virtual void    SetArea();
};

#endif

