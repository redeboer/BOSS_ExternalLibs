//------------------------------------------------------------------------------|
//      [File  ]:                       MucStripCal.h  	                        |
//      [Brief ]:       Head file of class MucStripCal for MUC calibration      |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 25, 2006                                            |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_STRIP_H
#define MUC_STRIP_H

#include<iostream>

#include "MucCalibAlg/MucEntityCal.h"

using namespace std;

const int     TURN_STR_ID[8][2]       = { {11,29},{11,30},{12,30},{12,32},{12,33},{12,34},{13,36},{14,38} };
const double  FIRST_TURN_HEAD[8][2]   = { {1092,1092}, {1126,1120}, {1158,1158}, {1193,1179},
                                          {1228,1220}, {1267,1267}, {1327,1327}, {1388,1385} };
const double  SECOND_TURN_HEAD[2]     = {56,66}; // odd layer: 56, even layer: 66;
const int     TURN_POINT_NUM          = 26;
const double  ODD_TURN_POINT_TAIL[2][2] = { {2317,2304},{2329,2321} };
const double  EVEN_TURN_POINT_TAIL[2]   = {2329,2329};

//-------------------------------Class definition -------------------------------
class MucStripCal : public MucEntityCal
{
  public:

    MucStripCal( int part, int segment, int layer,int id ); 
    MucStripCal( const MucStripCal &other );
    MucStripCal& operator =( const MucStripCal &other );
    virtual ~MucStripCal(); 
    
    int     GetType();	
    double  GetCenterLine();
    double  GetHead();
    double  GetTail();
    double  GetPhi();
    int     GetPadNumber();

  protected:

    virtual void  Init();

    void  SetType();
    void  SetCenterLine();
    void  SetHead();
    void  SetTail();
    void  SetPhi();

    virtual void  SetThin();
    virtual void  SetW();
    virtual void  SetWu();
    virtual void  SetWd();
    virtual void  SetH();
    virtual void  SetL();
    virtual void  SetArea();
    
    void  SetPadNumber();
	
  private:
   	       
    // Geometry info 
    int     m_Type;       // strip readout type
    double  m_CenterLine; // strip center line position, endcap only
    double  m_Head;       // strip head coordinate, endcap only
    double  m_Tail;       // strip tail coordinate, endcap only
    double  m_Phi;        // the inclination angle of local x axis and BES x axis
    int     m_PadNumber;  // the number of pads in the strip
};

#endif
