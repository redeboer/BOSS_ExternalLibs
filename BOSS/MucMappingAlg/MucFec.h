//------------------------------------------------------------------------------|
//      [File  ]:                       MucFec.h                              	|
//      [Brief ]:       Header file of class MucFec for electronics mapping     |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Jun 7, 2006                                             |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_FEC_H
#define MUC_FEC_H

#include<iostream>
#include<string>
#include<vector>

using namespace std;

static const int 	LENGTH	  		        = 16;		// Size of record section
static const int	DEFAULT_STRIP_ORDER   = -1;		// Default strip id defined order
                                                // Order of strip connected to data bit 
							                                  // 	1: low data bit corresponding to low strip ID 
							                                  // -1: low data bit corresponding to high strip ID
static const int  STRIP_NUM       = 16;  // Number of strip channel in one fec   

//  present DAQ format
//   ModuleID   ErrorFlag Slot  FEC
//  |= = = = =|= = = = = |= = |= = = =|16bit FEC data|

//  Old DAQ format
//   ErrorFlag  ModuleID  Slot  FEC
//  |= = = = =|= = = = = |= = |= = = =|16bit FEC data|    

static const int  MODULE_BIT  = 6;   // ">>" bit number for module, old DAQ format
//static const int  MODULE_BIT      = 11;  // ">>" bit number for module, present DAQ format, but used in MucBuilder not here
static const int  SOCKET_BIT  = 4;   // ">>" bit number for socket
static const int  PART_BIT    = 14;  // ">>" bit number for part
static const int  SEG_BIT     = 11;  // ">>" bit number for segment
static const int  LAY_BIT     = 7;   // ">>" bit number for layer

class MucFec {

  public:
	  MucFec( int id );
    MucFec( int id, int stripOrder, int part, int segment, int layer, int firstStripID, unsigned int vmeRecord, string chainName);
    ~MucFec( ) { ; }

		//-------------------------- Get methods --------------------------------
    int             GetID() 	      { return m_ID; 		      }
		int	            GetStripOrder()	{ return m_StripOrder; 	}
    unsigned   int	GetVmeRecord() 	{ return m_VmeRecord; 	} 
    unsigned short	GetVmeAddress() { return m_VmeAddress; 	}
    unsigned short	GetData() 	    { return m_Data; 	      }

    int GetPart()		  { return m_Part; 		}
    int GetSegment()	{ return m_Segment; }
    int GetLayer()		{ return m_Layer; 	}
    unsigned short  GetGeoAddress()	{ return m_GeoAddress;	}
		int	    GetFirstStripID() { return m_FirstStripID; 	}
	  string	GetChainName()    { return m_ChainName;		}
		int	    GetFiredStripNumber();
		vector< int >* 	GetFiredStripVect();	
    unsigned int	  GetPowerpcRecord();	
	  int	DecodeModule()  { return m_Module;		}
		int DecodeSocket()	{ return m_Socket;		}
		int	DecodeFecID()		{ return m_FecID;		  }	
		string	GetBin( unsigned int record );	
		string	GetBin( unsigned short record );
		string  GetVmeBin();
		string  GetGeoBin();
		string  GetDataBin();

		//-------------------------- Set methods --------------------------------
    void  SetStripOrder( int stripOrder );
		void	SetLocation( int part, int segment, int layer );
		void	SetFirstStripID( int firstStripID );
		void  SetChainName( string chainName );
		void 	SetData( unsigned short data );

	private:
    int	m_ID;			    // Id in chain, 0~STRIPNUM-1
    int m_StripOrder;	// Order of strip connected to data bit 
    unsigned int	m_VmeRecord;	// 32 bits record data of VME format: VME address + data 
		                            //	high 16 bits are the VME address, the low 16 bits are signals
		                            //	address contains module, socket, FEC id
    unsigned int  m_PowerpcRecord;	// 32 bits record data of PowerPC format: GEO address + data
		                            // 	high 16 bits are the FEC address, the low 16 bits are signals
		                            //	address contains part, segment, layer, and first strip id
		                            //	signals contain 16 strips status
    unsigned short	m_VmeAddress;	// High 16 bit of VME record for VME address 	
    unsigned short  m_GeoAddress;	// High 16 bit of PowerPC record for GEO address
    unsigned short 	m_Data;			  // Low  16 bit of record for data
		
		int 	m_Module;		// Module number decoded from VME address
		int 	m_Socket;		// Socket number decoded from VME address
		int 	m_FecID;		// FEC id decode from VME address, matching m_ID or (STRIPNUM-1) - m_ID 
	  string	m_ChainName;	// Chain name FEC belongs to
    int   m_Part;			// Located part
    int   m_Segment;	// located segment
    int   m_Layer;		// located layer
		int 	m_FirstStripID;		  // First strip id 
		int		m_FiredStripNumber;	// Number of fired strips
	  vector< int >   m_FiredStripVect;	// Vector of fired strip ids

		void	Initialize();		
		void 	DecodeData();
		void	DecodeVmeAddress();
		void 	SetFiredStripNumber();	
		void 	SetFiredStripVect();
		void	EncodePowerpcRecord();
		
};

#endif

