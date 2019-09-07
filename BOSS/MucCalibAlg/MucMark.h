//------------------------------------------------------------------------------|
//      [File  ]:                       MucMark.h                               |
//      [Brief ]:       Header file of MucMark class for encapsulation          |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Oct 19, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_MARK_H
#define MUC_MARK_H

#include<iostream>
#include<vector>

using namespace std;

class MucMark;
typedef vector< MucMark* > mark_col;

class MucMark {
  public:
    MucMark() {};
    MucMark( int part, int segment, int layer, int strip );
    ~MucMark();

    bool operator ==( MucMark &other );
    
    bool SetPart( int part );
    bool SetSegment( int segment );
    bool SetLayer( int layer );
    bool SetStrip( int strip );

    int Part();
    int Segment();
    int Layer();
    int Strip();
    
    int  NumInCol( mark_col &aCol );
    int  IsInCol( int part, int segment, int layer, int strip, mark_col &aCol );
    int  IsInCol( mark_col &aCol );
    int  IsInCol( vector< mark_col > &aClusterCol );
    bool IsInSegWith( MucMark &other );
    bool IsInBoxWith( MucMark &other );
    bool IsNeighborWith( MucMark &other );
    bool IsNeighborWith( mark_col &aCol );
    
    vector< mark_col > CreateClusterCol( int buildMode, mark_col &aMarkCol ); 
    vector< mark_col > CreateClusterCol_A( mark_col &aMarkCol ); // basic, fastest
    vector< mark_col > CreateClusterCol_B( mark_col &aMarkCol ); 
    vector< mark_col > CreateClusterCol_C( mark_col &aMarkCol );
    vector< mark_col > CreateClusterCol_D( mark_col &aMarkCol );
    
    void Print();
    void Print( mark_col &aMarkCol );
    void Print( vector< mark_col > &aClusterCol );

  private:

    int m_Part;
    int m_Segment;
    int m_Layer;
    int m_Strip;
};

#endif

