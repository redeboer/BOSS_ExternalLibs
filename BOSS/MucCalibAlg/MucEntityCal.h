//------------------------------------------------------------------------------|
//      [File  ]:                       MucEntityCal.h                          |
//      [Brief ]:       Head file of MUC geometry entity class for calibration	|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_ENTITY_CAL_H
#define MUC_ENTITY_CAL_H

#include<iostream>

using namespace std;

class MucEntityCal 
{
  public:
    MucEntityCal( int part, int segment, int layer ); 
    MucEntityCal( int part, int segment, int layer, int id );
    MucEntityCal( const MucEntityCal &other );
    MucEntityCal &operator =( const MucEntityCal &other );
    virtual	~MucEntityCal() ;
    
    int     GetPart();
    int     GetSegment();
    int     GetLayer();
    int     GetID();
    
    double  GetTheta(); // degree
    double  GetRin();   // mm
    double  GetRout();  // mm
    double  GetRc();    // mm
    double  GetThin();  // mm
    double  GetW();     // mm
    double  GetH();     // mm
    double  GetL();     // mm
    double  GetWu();    // mm
    double  GetWd();    // mm
    double  GetArea();  // cm^2
		
  protected:

    virtual void  Init();     // all geometry information initilization 
    
    virtual void  SetTheta(); // set theta
    virtual void  SetRin();   // set r inner
    virtual void  SetRout();  // set r outer
    virtual void  SetRc();    // set r center	
    virtual void  SetThin();  // set thickness
    virtual void  SetW();     // set width  
    virtual void  SetH();     // set height 
    virtual void  SetL();     // set stretching length 
    virtual void  SetWu();    // set up width  
    virtual void  SetWd();    // set down width 
    virtual void  SetArea();  // set area of entity(cm^2)

  protected:
    int     m_Part;     // part id
    int     m_Segment;  // segment id
    int     m_Layer;    // layer id
    int     m_ID;       // object id
    
    double  m_Theta;    // barrel: inclination angle of R and Bes_X
                        // endcap: inclination angle of panel boudary and Bes_X
    
    double  m_Rin;      // r of the inner surface relative to BES origin
    double  m_Rout;     // r of the outer surface relative to BES origin
    double  m_Rc;       // r of the center surface relative to BES orign	
    double  m_Thin;     // thickness of entity		
    double  m_W;        // width of cross section
    double  m_H;        // height of cross section
    double  m_L;        // stretching length of cross section
    double  m_Wu;       // up width of trapezoid cross section
    double  m_Wd;       // down width of trapezoid cross section
    double  m_Area;     // area of entity,mm^2
};

#endif

