//------------------------------------------------------------------------------|
//      [File  ]:                       MucFec.cxx                              |
//      [Brief ]:       Source file of class MucFec for electronics mapping     |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Jun 7, 2006                                             |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#include<iostream>
#include<vector>

using namespace std;

#include "MucMappingAlg/MucFec.h"
#include "MucMappingAlg/MucGeoConst.h"

// Constructor
MucFec :: MucFec(int id )
{
	//cout << "MucFec:: Create FEC : " << id << endl;

	if( (id < 0) || (id > STRIP_NUM-1) )
  {
		cout << "Initialize failure, check id!" << endl;
		m_ID = 0;
	}
	else	m_ID = id;
	
 	m_Part          = 0;
  m_Segment       = 0;
  m_Layer         = 0;
  m_FirstStripID  = 0;
  m_StripOrder    = DEFAULT_STRIP_ORDER;
	m_ChainName	= "NoName";
  m_VmeRecord     = 0;

	MucFec::Initialize();
}

MucFec :: MucFec( int id, int stripOrder, int part, int segment, int layer, int firstStripID, 
		  unsigned int vmeRecord, string chainName)
{
//	cout << "MucFec:: Create FEC : " << endl;
//	cout << "ID: " << id << "\tPart: " << part << "\tSegment: " << segment << "\tLayer: " << layer 
//	     << "\tfirstStripID: " << firstStripID << "\tvmeRecord: " << vmeRecord<<endl;

	// Check parameters	
	if( (id < 0) || (id > STRIP_NUM-1) ) 
	{
		m_ID =  0; 
		cout << "MucFec::Check id:\t" << id << "!" << endl;
	}else
	if( (stripOrder != 1) && (stripOrder != -1) )
	{
		m_StripOrder = 0;
		cout << "MucFec::Check stripOrder:\t" << stripOrder << "!" << endl;
	}else
	if( (part < 0) || ( part > PART_MAX-1) )
	{
		m_Part =  0;
		cout << "MucFec::Check part:\t" << part << "!" << endl;
	}else
	if( (segment < 0) || ( segment > ((part==BRID)?(B_SEG_NUM-1):(E_SEG_NUM-1)) ) )
	{
		m_Segment =  0;
		cout << "MucFec::Check segment:\t" << segment << "!" << endl;
	}else
	if( (layer <0) || ( layer > ((part==BRID)?(B_LAY_NUM-1):(E_LAY_NUM-1)) ) )
	{
		m_Layer = 0;
		cout << "MucFec::Check layer:\t" << layer << "!" << endl;
	}else
	if( sizeof(vmeRecord) < 4 )
	{
		m_VmeRecord = 0;
		cout << "MucFec::Check record:\t" << vmeRecord << "!" << endl;
	}else
	{
    m_ID            = id;
	  m_StripOrder    = stripOrder;
		m_Part					= part;
		m_Segment				= segment;
		m_Layer					= layer;
		m_FirstStripID  = firstStripID;
		m_ChainName			= chainName;
    m_VmeRecord     = vmeRecord;
	}
	
	MucFec::Initialize();	
}

void MucFec :: Initialize()
{
	MucFec::DecodeData();
  MucFec::DecodeVmeAddress();
	MucFec::SetFiredStripNumber();
  MucFec::SetFiredStripVect();
  MucFec::EncodePowerpcRecord();
}

//----------------------------------Set methods---------------------------------------
void MucFec :: DecodeData()
{
	m_Data = (unsigned short)m_VmeRecord;
}	

void MucFec :: DecodeVmeAddress() 
{
  m_VmeAddress = (unsigned short)(m_VmeRecord >> LENGTH);	// high 16 bits, 10 bits used
	m_Module     = (m_VmeAddress >> MODULE_BIT) & 0xF;	// 4 bits
	m_Socket     = (m_VmeAddress >> SOCKET_BIT) & 0x3;	// 2 bits
	m_FecID      = m_VmeAddress & 0xF;			// 4 bits	
}

void MucFec :: SetFiredStripNumber() 
{
	m_FiredStripNumber = 0;
	for(int i=0; i<STRIP_NUM; i++)
	{
		if( (m_Data>>i) & 1 ) 
			m_FiredStripNumber++;
	}	
}

void MucFec :: SetFiredStripVect()
{
	m_FiredStripVect.clear();
	int tempID[STRIP_NUM];
	int tempFired=0;

  for(int i=0; i<STRIP_NUM; i++)
  {
  	if( (m_Data>>i) & 1 )
    {
	   	tempID[tempFired] = i;	
			tempFired++;
		}	
  }
	
	int stripID = 0;	
	for(int i=0; i<tempFired; i++)
	{
		stripID = ((1 - m_StripOrder)/2) * (STRIP_NUM - 1) + (m_StripOrder * tempID[i]);
		stripID += m_FirstStripID;
		m_FiredStripVect.push_back( stripID );
	}	
}

void MucFec :: EncodePowerpcRecord()
{	
	unsigned int address = ( (m_Part<<PART_BIT) | (m_Segment<<SEG_BIT) | (m_Layer<<LAY_BIT) | m_FirstStripID );
	m_GeoAddress = (unsigned short)address;
	m_PowerpcRecord = ( address << LENGTH ) | m_Data;
}

void MucFec :: SetStripOrder( int stripOrder )
{
	m_StripOrder = stripOrder;
	
	MucFec::SetFiredStripVect();	
}

void MucFec :: SetLocation( int part, int segment, int layer )
{
	m_Part = part;
	m_Segment = segment;
	m_Layer = layer;

	MucFec::Initialize();
}

void MucFec :: SetFirstStripID( int firstStripID )
{
	m_FirstStripID = firstStripID;
	MucFec :: EncodePowerpcRecord();
	MucFec :: SetFiredStripVect();
}

void MucFec :: SetData( unsigned short data )
{
	m_Data 	    = data;
	m_VmeRecord = (m_VmeRecord | m_Data);
	MucFec :: EncodePowerpcRecord();
	MucFec :: SetFiredStripNumber();
	MucFec :: SetFiredStripVect();
}



//-------------------------- Get methods --------------------------------
int          MucFec :: GetFiredStripNumber()   { return m_FiredStripNumber;    }

unsigned int MucFec :: GetPowerpcRecord()      { return m_PowerpcRecord;       }

vector< int >*  MucFec :: GetFiredStripVect()  { return &m_FiredStripVect;     }

string MucFec :: GetBin( unsigned int record )
{
	const int SIZE = sizeof( unsigned int ) * 8;

  string Bin;
  unsigned int temp = 0;
  for(int i=0; i<SIZE; i++)
  {
  	temp = record >> SIZE-i-1;
    Bin += ( temp & 1 ) ? "1" : "0";
  }

  return Bin;
}

string MucFec :: GetBin( unsigned short record )
{
	const int SIZE = sizeof( unsigned short ) * 8;

  string Bin;
  unsigned short temp = 0;
  for(int i=0; i<SIZE; i++)
  {
  	temp = record >> SIZE-i-1;
    Bin += ( temp & 1 ) ? "1" : "0";
  }

  return Bin;
}

string MucFec :: GetVmeBin()
{
	return MucFec::GetBin( m_VmeAddress );
}

string MucFec :: GetGeoBin()
{
	return MucFec::GetBin( m_GeoAddress );
}

string MucFec :: GetDataBin()
{
	return MucFec::GetBin( m_Data );
}



// END
