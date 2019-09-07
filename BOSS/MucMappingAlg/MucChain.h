//------------------------------------------------------------------------------|
//      [File  ]:                       MucChain.h                              |
//      [Brief ]:       Header file of class MucChain for electronics mapping	|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Jun 7, 2006                                             |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_CHAIN_H
#define MUC_CHAIN_H

#include<iostream>
#include<vector>
#include<string>

using namespace std;

#include "MucFec.h"
#include "MucGeoConst.h"

static const int  FEC_NUM     = 16;     // Maximun of FECs in a chain
static const int  DEFAULT_FEC_ORDER 	= 1;	// Default FEC id defined order
                                        	// 1: from chain tail(the detector end), 

const int FIRST_STRIP_ID[7]		    = {0, 16, 32, 48, 64, 80, 96};  // 1st stripid list of chain 
const int FIRST_STRID_SQC_EC[4]		= {48, 32, 16, 0};  // 1st stripid list of chain in Endcap
const int FIRST_STRID_SQC_BETOP[4]= {64, 96, 80, 48}; // 1st stripid list of chain in Barrel east Seg2
const int FIRST_STRID_SQC_BEA[3]	= {48, 64, 80} ;    // 1st stripid list of chain in Barrel east Seg0,1,5
const int FIRST_STRID_SQC_BEB[3]	= {80, 64, 48} ;    // 1st stripid list of chain in Barrel east Seg3,4,6,7
const int FIRST_STRID_SQC_BWA[3]	= {32, 16, 0}  ;    // 1st stripid list of chain in Barrel west 
							                                        // Seg0,1,2,5 of even layers,Seg3,4,6,7 of odd layers, 
const int FIRST_STRID_SQC_BWB[3]	= {0, 16, 32}  ;    // 1st stripid list of chain in Barrel west 
							                                        // Seg0,1,2,5 of odd layers,Seg3,4,6,7 of even layers,
const int STRORDER_ECA[E_LAY_NUM] = {-1, 1, -1, 1, -1, 1, -1, 1};	// Order sequence A for Endcap
									                                                // East Seg0 and Seg2, West Seg1 and Seg3;
const int STRORDER_ECB[E_LAY_NUM] = {1, -1, 1, -1, 1, -1, 1, -1};	// Order sequence B for Endcap
									                                                // East Seg1 and Seg3, West Seg0 and Seg2;
const int STRORDER_BETOP[4]	= {1, -1, 1, -1} ;  // Strip order of FECs in Barrel east Seg2
const int STRORDER_BEA[3]	= {-1, -1, 1} ;       // Strip order of FECs in Barrel east Seg0,1,5, 
						                                    // and west Seg3,4,6,7 for even layers
const int STRORDER_BEB[3]	= {1, -1, -1} ;       // Strip order of FECs in Barrel east Seg3,4,6,7, 
						                                    // and west Seg0,1,2,5 for even layers
const int STRORDER_BWO[3]	= {-1, -1, -1} ;      // Strip order of FECs in Barrel west for odd layers

class MucChain {

  public:
	  MucChain( int id, string name, int module, int socket, int fecOrder);
		~MucChain( );
		//--------------------------- Get methods ----------------------------------
		int GetID()	      { return m_ID; 		  }
		int GetPart() 	  { return m_Part; 	  }
		int	GetSegment() 	{ return m_Segment; }
		int	GetFecTotal() { return m_FecTotal;}
		int GetFecOrder() { return m_FecOrder;}
		string	GetName() { return m_Name; 	  }
	  vector< MucFec> GetFecVect() 	{ return m_FecVect; 	}

		MucFec* GetFec( int fecId )		    { return &m_FecVect[fecId]; 	  }
		int	GetFecLayerID( int fecID )	  { return m_FecLayerID[fecID];	  }
		int	GetFirstStripID( int fecID ) 	{ return m_FirstStripID[fecID]; }
		
		//---------------------------- Set methods ----------------------------------
		void	ReMap( string name, int module, int socket );
		void	SetFecOrder( int fecOrder );
		void 	SetStripOrder( int stripOrder );		        // All FECs, 0 as default order: -1
		void 	SetStripOrder( int fecID, int stripOrder );	// One FEC
		void	ArrayInvert( int* array, int number );

  private:
		int m_ID;					      // Chain id
		string 	m_Name;					// Name of chain based on hardware
		int	m_Module;				    // Module id connected by chain
		int m_Socket;				    // Socket id connected by chain
		int	m_FecOrder;				  // FEC id defined order, 
		int m_Part;					    // Part chain belongs to 
		int m_Segment;				  // Segment chain belongs to 
		int m_FecLayerID[FEC_NUM];	// Array of the layer id for each FEC in chain 
		int m_FecTotal;				  // Total FECs in chain, 0 ~ FEC_NUM 
		int	m_FecPerLayer;  		// Number of FECs in each layer
		int m_FirstStripID[FEC_NUM];	// Array of the first strip id for each FEC in chain
		int	m_StripOrder[FEC_NUM];		// Strip order corresponding to data bit
	  vector< MucFec> m_FecVect;		// Vector of the FECs in chain
	
		void 	Mapping();
		void	InitPart();
		void 	InitSegment();
		void	InitFecTotal();
		void 	InitFecPerLayer();
		void	InitFecLayerID();
		void	InitFirstStripID();
		void	InitStripOrder();
	  unsigned int	EncodeVmeRecord( int module, int socket, int fecId, unsigned short data);
		void 	InitFecVect();
};

#endif
