//------------------------------------------------------------------------------|
//      [File  ]:                       MucMark.cxx                             |
//      [Brief ]:       Source file of MucMark class for encapsulation          |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Oct 19, 2006                                            |
//------------------------------------------------------------------------------|


#include<iostream>
#include<vector>
#include<cmath>

#include "MucCalibAlg/MucStructConst.h"
#include "MucCalibAlg/MucMark.h"

using namespace std;

// Constructor
MucMark::MucMark( int part, int segment, int layer, int strip )
{
  m_Part    = part;
  m_Segment = segment;
  m_Layer   = layer;
  m_Strip   = strip;
}

// Destructor
MucMark::~MucMark() { ; }

// Operator ==
bool MucMark::operator ==( MucMark &other )
{
  if( this == &other ) return true;
  
  if( (*this).Part()    == other.Part()    &&
      (*this).Segment() == other.Segment() &&
      (*this).Layer()   == other.Layer()   &&
      (*this).Strip()   == other.Strip()       )
    return true;
  else
    return false;
}

// Set properties
bool MucMark::SetPart( int part ) 
{
  if( part > PART_MAX || part < 0 ) {
    cout << "part overflow:\t" << part << endl;
    return false;
  }
  else {
    m_Part = part;
    return true;
  }
} 

bool MucMark::SetSegment( int segment )
{
  if( segment > ((m_Part == BRID)?B_SEG_NUM:E_SEG_NUM) || segment < 0 ) {  
    cout << "segment overflow:\t" << segment << endl;
    return false;
  }
  else {
    m_Segment = segment;
    return true;
  }
}

bool MucMark::SetLayer( int layer )
{
  if( layer > ((m_Part == BRID)?B_LAY_NUM:E_LAY_NUM) || layer < 0 ) {  
    cout << "layer overflow:\t" << layer << endl;
    return false;
  }
  else {
    m_Layer = layer;
    return true;
  }
}

bool MucMark::SetStrip( int strip )
{
  if( strip > STRIP_INBOX_MAX || strip < 0 ) {  
    cout << "strip overflow:\t" << strip << endl;
    return false;
  }
  else {
    m_Strip = strip;
    return true;
  }
}

// Get properties 
int MucMark::Part()    { return m_Part;    }
int MucMark::Segment() { return m_Segment; }
int MucMark::Layer()   { return m_Layer;   }
int MucMark::Strip()   { return m_Strip;   }

// Judge whether mark itself is in a Col or not
int MucMark::IsInCol( int part, int segment, int layer, int strip, mark_col &aCol )
{
  for( unsigned int i=0; i<aCol.size(); i++ ) {
    if( part    == aCol[i]->Part()    &&
        segment == aCol[i]->Segment() &&
        layer   == aCol[i]->Layer()   &&
        strip   == aCol[i]->Strip()     )
    return i;
  }
  
  return -1;
}

int MucMark::IsInCol( mark_col &aCol )
{
  for( unsigned int i=0; i<aCol.size(); i++ ) {
    if( (*this) == (*aCol[i]) ) return i;
  }
  
  return -1;
}

int MucMark::IsInCol( vector< mark_col >  &aClusterCol )
{ 
  for( unsigned int i=0; i<aClusterCol.size(); i++ ) {
    if( (*this).IsInCol ( aClusterCol[i] ) ) return i;
  }
  
  return -1;
}

// Return the number of mark itself in a Col
int MucMark::NumInCol( mark_col &aCol )
{
  int num = 0;
  
  for( unsigned int i=0; i<aCol.size(); i++ ) {
    if( (*this) == (*aCol[i]) ) num ++;
  }
  
  return num;
}

// Judge whether mark itself is in the same segment with the other
bool MucMark::IsInSegWith( MucMark &other )
{
  if( (*this).Part() == other.Part() &&
      (*this).Segment() == other.Segment()  )
    return true;
  else
    return false;
}

// Judge whether mark itself is in the same box with the other
bool MucMark::IsInBoxWith( MucMark &other )
{
  if( (*this).Part() == other.Part()       &&
      (*this).Segment() == other.Segment() &&
      (*this).Layer() == other.Layer()        )
    return true;
  else
    return false;
}

// Judge whether mark itself is a neighbor with the other
bool MucMark::IsNeighborWith( MucMark &other )
{
  if( (*this).Part()    == other.Part()    &&
      (*this).Segment() == other.Segment() &&
      (*this).Layer()   == other.Layer()   &&
      fabs( (*this).Strip() - other.Strip() ) == 1 )
    return true;
  else 
    return false;
}

// Judge whether mark itself is a neighbor with a multi hit
bool MucMark::IsNeighborWith( mark_col &aCluster )
{
  for( unsigned int i=0; i<aCluster.size(); i++ ) {
    if( (*this).IsNeighborWith((*aCluster[i])) )
      return true;
  }
  
  return false;
}

// Return a cluster Col from a given mark Col
vector< mark_col > MucMark::CreateClusterCol_A( mark_col &aMarkCol )
{
  vector< mark_col > aClusterCol;
  mark_col	checkedMarkCol;
  
  int recordFlag      = 0;
  unsigned int circle = 0;
  
  for( unsigned int i=0; i<aMarkCol.size(); i++ )
  {
    recordFlag = 0;
    for( unsigned int j=0; j< aClusterCol.size(); j++ )
    {
      if( (*aMarkCol[i]).IsInCol(aClusterCol[j]) !=-1 ) {
        recordFlag = 1;
        break;
      }
      else if( (*aMarkCol[i]).IsNeighborWith(aClusterCol[j]) )
      {
        aClusterCol[j].push_back( aMarkCol[i] );
        recordFlag = 1;
        break;
      }
    }
    
    if( recordFlag == 0 )
    {
      mark_col aCluster;
      aCluster.push_back( aMarkCol[i] );
      aClusterCol.push_back( aCluster );
      
      circle =0;
      do {
        circle ++;
        for( unsigned int j = i+1; j<aMarkCol.size(); j++ )
        {
          if( (*aMarkCol[j]).IsInCol(aClusterCol.back()) !=-1 ) continue;
          else if( (*aMarkCol[j]).IsNeighborWith( aClusterCol.back()) )
          aClusterCol.back().push_back( aMarkCol[j] );
        }
      } while( circle < aMarkCol.size()-i-1);
    }
  }
  
  return aClusterCol;
}

vector< mark_col > MucMark::CreateClusterCol_B( mark_col &aMarkCol)
{
  vector< mark_col > aClusterCol;
  mark_col  checkedMarkCol;
  
  unsigned int circle = 0;
  for( unsigned int i=0; i<aMarkCol.size(); i++ )
  {
    if( (*aMarkCol[i]).IsInCol(checkedMarkCol) !=-1 ) continue;
    
    mark_col aCluster;
    aCluster.push_back( aMarkCol[i] );
    aClusterCol.push_back( aCluster );
    checkedMarkCol.push_back( aMarkCol[i] );
    
    circle = 0;
    do {
      for( unsigned int j = i+1; j<aMarkCol.size(); j++ )
      {
        if( (*aMarkCol[j]).IsInCol(checkedMarkCol) != -1) continue;
        else if( (*aMarkCol[j]).IsNeighborWith( aClusterCol.back() ) ) {
          aClusterCol.back().push_back( aMarkCol[j] );
          checkedMarkCol.push_back( aMarkCol[j] );
        }
      }
      circle ++;
    } while(circle < aMarkCol.size()-i-1);
  }
  
  return aClusterCol;
}

vector< mark_col > MucMark::CreateClusterCol_C( mark_col &aMarkCol)
{
  MucMark tmpMark;
  vector< mark_col > aClusterCol;
  mark_col  copyMarkCol;
  copyMarkCol = aMarkCol;
  
  mark_col::iterator it1;
  while( copyMarkCol.size() !=0 ) // while 1
  {
    it1 = copyMarkCol.begin();
    mark_col aCluster;
    aCluster.push_back( copyMarkCol[0] );
    aClusterCol.push_back( aCluster );
  
    copyMarkCol.erase( it1 );
    unsigned int circle = 0;
    unsigned int size   = 0;
    unsigned int beginsize = copyMarkCol.size();
    while( circle < beginsize ) // while 2
    {
      mark_col::iterator it2;
      size = copyMarkCol.size();
      for( unsigned int j=0, headNum=0; j < size; j++ )
      {
        it2 = copyMarkCol.begin() + headNum;
        if( (copyMarkCol[headNum])->IsNeighborWith( aClusterCol.back() ) ) {
          aClusterCol.back().push_back( (copyMarkCol[headNum]) );
          copyMarkCol.erase(it2);
        }
        else headNum ++;
      } // end for
  
      circle ++;
    }; // End while 2
  
  }; // End while 1
  
  return aClusterCol;
}

vector< mark_col > MucMark::CreateClusterCol_D( mark_col &aMarkCol )
{
  vector< mark_col > aClusterCol;

  bool recordFlag     = false;
  unsigned int circle = 0;

  for( unsigned int i=0; i<aMarkCol.size(); i++ )
  {
    recordFlag = false;
    for( int j= aClusterCol.size()-1; j>-1; j-- )
    {
      if( (*aMarkCol[i]).IsInCol(aClusterCol[j]) !=-1 ) {
        recordFlag = true;
        break;
      }
      else if( (*aMarkCol[i]).IsNeighborWith(aClusterCol[j]) )
      {
        aClusterCol[j].push_back( aMarkCol[i] );
        recordFlag = true;
        break;
      }
    }

    if( recordFlag == false )
    {
      mark_col aCluster;
      aCluster.push_back( aMarkCol[i] );
      aClusterCol.push_back( aCluster );

      circle =0;
      do {
        circle ++;
        for( unsigned int j = i+1; j<aMarkCol.size(); j++ )
        {
          if( (*aMarkCol[j]).IsInCol(aClusterCol.back()) !=-1 ) continue;
          else if( (*aMarkCol[j]).IsNeighborWith( aClusterCol.back()) )
            aClusterCol.back().push_back( aMarkCol[j] );
        }
      } while( circle < aMarkCol.size()-i-1);
    } // End if recordflag
  } // End fist for

  return aClusterCol;
}


vector< mark_col > MucMark::CreateClusterCol( int buildMode, mark_col &aMarkCol )
{
  vector< mark_col > aClusterCol;
  switch( buildMode ) 
  {
    case 1: return CreateClusterCol_A( aMarkCol );
            break;
    case 2: return CreateClusterCol_B( aMarkCol );
            break;
    case 3: return CreateClusterCol_C( aMarkCol );
            break;
    case 4: return CreateClusterCol_D( aMarkCol );
            break;
    default: return (aClusterCol);
  }
}

// Show mark itself
void MucMark::Print()
{
  cout << "prt: "   << m_Part
       << "\tseg: " << m_Segment
       << "\tlay: " << m_Layer
       << "\tstr: " << m_Strip << endl;
}

void MucMark::Print( mark_col &aMarkCol )
{
  for( unsigned int i=0; i< aMarkCol.size(); i++ ) aMarkCol[i]->Print();
}

void MucMark::Print( vector< mark_col > &aClusterCol )
{
  for( unsigned int i=0; i< aClusterCol.size(); i++ )
    for( unsigned int j=0; j< aClusterCol[i].size(); j++ ) {
      aClusterCol[i][j]->Print();
    }
}

// END

