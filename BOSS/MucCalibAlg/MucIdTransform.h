//------------------------------------------------------------------------------|
//      [File  ]:                      MucIdTransform.h	                        |
//      [Brief ]:       Header file of MucIdTransform class for encapsulation  	|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Oct 19, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_IDTRANSFORM_H
#define MUC_IDTRANSFORM_H

#include<iostream>

using namespace std;

class MucIdTransform{
  public:
    MucIdTransform();
    ~MucIdTransform();
    
    int GetId();
    int GetPart();
    int GetSegment();
    int GetLayer();
    int GetStrip();
    
    int GetStripMax( int part, int segment, int layer );
    int GetBoxId( int part, int segment, int layer );
    int GetStripId( int part, int segment, int layer, int subid );
    
    bool SetBoxPos( int boxid, int* part, int* segment, int* layer );
    bool SetStripPos(  int stripid, int* part, int* segment, int* layer, int* subid );
    
    void Print(int);

  private:
    int m_Id;
    int m_Part;
    int m_Segment;
    int m_Layer;
    int m_Strip;
};

#endif

