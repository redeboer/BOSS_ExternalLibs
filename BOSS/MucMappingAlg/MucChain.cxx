//------------------------------------------------------------------------------|
//      [File  ]:                       MucChain.cxx                            |
//      [Brief ]:       Source file of class MucChain for electronics mapping   |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Jun 7, 2006                                             |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#include<iostream>
#include<vector>

using namespace std;

#include "MucMappingAlg/MucChain.h"

// Constructor
MucChain :: MucChain( int id, std::string name, int module, int socket, int fecOrder )
{
//	cout << "MucChain:: Create chain : " << endl;
//	cout << "ID: " << id << "\tModule: " << module << "\tSocket: " << socket << "\tFecOrder: " << fecOrder << endl;
	m_ID	    = id;
	m_Name 	    = name;
	m_Module    = module;
	m_Socket    = socket;
	m_FecOrder  = fecOrder;
	
	Mapping();
}

// Destructor
MucChain :: ~MucChain()
{
//	cout << "MucChain:: Destructor()." <<endl;
	delete []m_FirstStripID; 
	delete []m_FecLayerID;
	delete []m_StripOrder;

	m_FecVect.clear();
}  

//----------------------- Set properties methods for public---------------
void MucChain :: ReMap( string name, int module, int socket )
{
	m_Name 	 = name;
	m_Module = module;
	m_Socket = socket;

	MucChain::Mapping();
}

void MucChain :: SetFecOrder( int fecOrder )
{
	m_FecOrder = fecOrder;
	
	MucChain::InitFecVect();
}

// All FECs
void MucChain :: SetStripOrder( int stripOrder )
{
//      cout << "MucChain:: SetStripOrder( int )." << endl;
	int order;
	if( stripOrder == 0 ) order = DEFAULT_STRIP_ORDER;
	else 		      order = stripOrder;

        for(int i=0; i<m_FecTotal; i++)
	        m_StripOrder[ i ] = order;
}

void MucChain :: SetStripOrder( int fecID, int stripOrder )
{
	// Decode FecVect member number by fecID
	int i = m_FecOrder * ( fecID - ((1-m_FecOrder)/2)*(m_FecTotal - 1) );
	
	m_FecVect[i].SetStripOrder( stripOrder ); 
} 

void MucChain :: ArrayInvert( int* array, int number )
{
	int temp;
	for(int i=0; i<number/2; i++)
	{
		temp     	  = array[i];
		array[i] 	  = array[number-1-i];
		array[number-1-i] = temp;
	}
}

//------------------------ set properties methods for private---------------
void MucChain :: Mapping()
{	
	InitPart();
  InitSegment();
  InitFecTotal();
	InitFecPerLayer();
  InitFecLayerID();
	InitFirstStripID();
	InitStripOrder();
  InitFecVect();
}

void MucChain :: InitPart()
{	
//	cout << "MucChain:: InitPart()." <<endl;
	if( m_Name[0] == 'B' )		m_Part = BRID;
	else if( m_Name[1] == 'E' )	m_Part = EEID;
	else				m_Part = WEID;	
}

void MucChain :: InitSegment()
{
//	cout << "MucChain:: InitSegment()." <<endl;
	if( m_Part == BRID )
	{	
		switch( m_Name[2] )
		{
			case '1' : m_Segment = 2; break; 
			case '2' : m_Segment = 1; break;
			case '3' : m_Segment = 0; break;
			case '4' : m_Segment = 7; break;
			case '5' : m_Segment = 6; break;
			case '6' : m_Segment = 5; break;
			case '7' : m_Segment = 4; break;
			case '8' : m_Segment = 3; break;
			default  : ;
		}	
	}
	else if( m_Part == EEID )
	{
		switch( m_Name[2] )
		{
			case '1' : m_Segment = 0; break;
      case '2' : m_Segment = 3; break;
      case '3' : m_Segment = 2; break;
      case '4' : m_Segment = 1; break;
		}
	}
  else
  { 
    switch( m_Name[2] )
    {
      case '1' : m_Segment = 1; break;
      case '2' : m_Segment = 2; break;
      case '3' : m_Segment = 3; break;
      case '4' : m_Segment = 0; break;
    }
  }   
}

void MucChain :: InitFecTotal()
{
//	cout << "MucChain:: InitFecTotal()." <<endl;
	if( m_Part == BRID )				// Barrel 
	{
		if( m_Name[1] == 'O' ) 			// Odd layer chain
		{
			m_FecTotal = FEC_NUM - 1; 	// 15 FECs
		}
		else					// Even layer chain
		{
			if( m_Segment == BRTOP && m_Name[1] == 'E' ) 	// Top segment
			{
				m_FecTotal = FEC_NUM ; 	// 16 FECs
			}	
			else				// Not top segment
			{
				m_FecTotal = FEC_NUM - 4; // 12 FECs
			}
		}
	}
	else						// Endcap
	{
		m_FecTotal = FEC_NUM;			// 16 FECs
	}
}

void MucChain :: InitFecPerLayer()
{
//	cout << "MucChain:: InitFecPerLayer()." <<endl;
	if( m_FecTotal != 0 )
	{
		if( m_FecTotal == FEC_NUM )
			m_FecPerLayer = 4;
		else 
			m_FecPerLayer = 3;
	}
	else
		m_FecPerLayer = 0;
}

void MucChain :: InitFecLayerID()
{
  //	cout << "MucChain:: InitLayer()." <<endl;
	// Init array
	for(int i=0; i<FEC_NUM; i++)
		m_FecLayerID[i] = 0;
	
	// Set FEC layer id according to default order
	if( m_Part == BRID ) 		// Barral chains
	{	
		if( m_Name[1] == 'O' )  // Odd layer chain, layer id: 0, 2, 4, 6, 8 
		{
			for(int i=0; i<m_FecTotal; i++) {
				m_FecLayerID[i] = (i/m_FecPerLayer) * 2;	
			}
		}
		else			// Even layer chain, layer id: 1, 3, 5, 7
		{
			for(int i=0; i<m_FecTotal; i++)  {
				m_FecLayerID[i] = (i/m_FecPerLayer) * 2 + 1;
			}
		}
	}
	else				// Endcap chains
	{
		if( m_Name[3] == 'F' )  // First chain, layer id: 0, 1, 2, 3
		{
			for(int i=0; i<m_FecTotal; i++) {
				m_FecLayerID[i] = 3 - (i/m_FecPerLayer);
			}
		}
		else			// Second chain, layer id: 4, 5, 6, 7
		{
			for(int i=0; i<m_FecTotal; i++) {
				m_FecLayerID[i] = 7 - (i/m_FecPerLayer);
			}
		}
	}

	// If inverting order  
	if( m_FecOrder == -1 )
		MucChain::ArrayInvert( &m_FecLayerID[0], m_FecTotal );

}

void MucChain :: InitFirstStripID()
{
//	cout << "MucChain:: InitFirstStripID()." <<endl;
	// Init array
	for(int i=0; i<FEC_NUM; i++)
		m_FirstStripID[i] = 0;

	// Set first strip ID according to default fecOrder
	if( m_Part== BRID )		// Barrel chains
	{
		if( m_Name[1] == 'E' ) 	// East end, only even number layer chain, layer id: 1, 3, 5, 7
		{
			// Section number is defined by m_FecTotal/m_FecPerLayer
			// Some sections of chain inverting default firstStripID sequence
			
			if(m_Segment==BRTOP)	// BRTOP segment is exceptive, FirstStripID sequence is 64,96,80,48
			{
				for(int i=0; i<m_FecTotal; i++)
					m_FirstStripID[i] = FIRST_STRID_SQC_BETOP[i%m_FecPerLayer ];
			}
			else switch(m_Segment)
			{
				case 0: ;		// Segment 0, 1, 5, section 0, 2: (+), 1, 3: (-)
				case 1: ;
				case 5:
					for(int i=0; i<m_FecTotal; i++)
						m_FirstStripID[i] = FIRST_STRID_SQC_BEA[i%m_FecPerLayer ];
					break;
				default :	// Other segments 
					for(int i=0; i<m_FecTotal; i++)
						m_FirstStripID[i] = FIRST_STRID_SQC_BEB[i%m_FecPerLayer ];
					
			}
			
		}
		else if( m_Name[1] == 'W' ) // West end, even number layer chain, layer id: 1, 3, 5, 7
		{
    	switch(m_Segment)
      {
      	case 0: ;               // Segment 0, 1, 2, 5
        case 1: ;
        case 2: ;
				case 5:
            for(int i=0; i<m_FecTotal; i++)
							m_FirstStripID[i] = FIRST_STRID_SQC_BWA[i%m_FecPerLayer ];
            break;
        default:       	// Other segments 
            for(int i=0; i<m_FecTotal; i++)
							m_FirstStripID[i] = FIRST_STRID_SQC_BWB[i%m_FecPerLayer ];
      }
		} 
		else			// West end, odd number layer chain, layer id: 0, 2, 4, 6
		{	
      switch(m_Segment)
      {
      	case 0: ;               // Segment 0, 1, 2, 5
        case 1: ;
        case 2: ;
				case 5:
          	for(int i=0; i<m_FecTotal; i++)
							m_FirstStripID[i] = FIRST_STRID_SQC_BWB[ i%m_FecPerLayer ];
            break;
        default :       	// Other segments 
            for(int i=0; i<m_FecTotal; i++)
							m_FirstStripID[i] = FIRST_STRID_SQC_BWA[ i%m_FecPerLayer ];
      }
		}

		// for all chains in Barrel, section 0, 2, 4: (+), 1, 3: (-), inverting 1, 3;
		for(int j=1; j<m_FecTotal/m_FecPerLayer; j+=2) 
			MucChain::ArrayInvert( &m_FirstStripID[j*m_FecPerLayer], m_FecPerLayer );
	}		// End barrel chains
	else		// Endcap chains
	{	
		// Set default firstStripID order(+), no inverting
		for(int i=0; i<m_FecTotal; i++)
			m_FirstStripID[i] = FIRST_STRID_SQC_EC[ i%m_FecPerLayer ];
	}

	// If fecOrder inverting
	if( m_FecOrder == -1 )
	{
		MucChain::ArrayInvert( m_FirstStripID, m_FecTotal );
	}
}

unsigned int MucChain :: EncodeVmeRecord( int module, int socket, int fecID, unsigned short data)
{
//	cout << "MucChain:: EncodeVmeRecord()." <<endl;
	unsigned int record = ((module << MODULE_BIT) | (socket << SOCKET_BIT) | fecID);
	return ( (record << LENGTH) | data );
}

// All FECs
void MucChain :: InitStripOrder()
{
//      cout << "MucChain:: InitStripOrder()." << endl;

	
  for(int i=0; i<m_FecTotal; i++)
	{	
		if( m_Part == BRID ) // Barrel			
		{
			m_StripOrder[i] = -1;
/*			
			if( m_Name[1] == 'E' ) 	// East end, only even number layer chain, layer id: 1, 3, 5, 7
			{	
				// Section number is defined by m_FecTotal/m_FecPerLayer
				// Some sections of chain inverting default strip order
			
				if(m_Segment==BRTOP)	//Strip order is +1, -1, +1, -1
					
				{
					for(int i=0; i<m_FecTotal; i++)
						m_StripOrder[i] = STRORDER_BETOP[i%m_FecPerLayer ];
				}
				else switch(m_Segment)
				{
					case 0: ;		// Segment 0, 1, 5, section 0, 2: (--+), 1, 3: (+--)
					case 1: ;
					case 5:
						for(int i=0; i<m_FecTotal; i++)
							m_StripOrder[i] = STRORDER_BEA[i%m_FecPerLayer ];
						break;
					default :	// Other segments 
						for(int i=0; i<m_FecTotal; i++)
							m_StripOrder[i] = STRORDER_BEB[i%m_FecPerLayer ];
				}			
			}
			else if( m_Name[1] == 'W' ) // West end, even number layer chain, layer id: 1, 3, 5, 7
			{
                        	switch(m_Segment)
                        	{
                                	case 0: ;               // Segment 0, 1, 2, 5, --+
                                	case 1: ;
                                	case 2: ;
					case 5:
                                        	for(int i=0; i<m_FecTotal; i++)
							m_StripOrder[i] = STRORDER_BEA[i%m_FecPerLayer ];
                                        	break;
                                	default :       	// Other segments, +-- 
                                        	for(int i=0; i<m_FecTotal; i++)
							m_StripOrder[i] = STRORDER_BEB[i%m_FecPerLayer ];
                        	}
			} 
			else			// West end, odd number layer chain, layer id: 0, 2, 4, 6
			{	
                        	for(int i=0; i<m_FecTotal; i++)
					m_StripOrder[i] = STRORDER_BWO[ i%m_FecPerLayer ];
			}

			// for all chains in Barrel, section 0, 2, 4: (+), 1, 3: (-), inverting 1, 3;
			for(int j=1; j<m_FecTotal/m_FecPerLayer; j+=2) 
				MucChain::ArrayInvert( &m_StripOrder[j*m_FecPerLayer], m_FecPerLayer );
*/
		} // End Barrel 	
		else if( (m_Part==EEID && (m_Segment==0 || m_Segment==2)) 
			 || (m_Part==WEID && (m_Segment==1 || m_Segment==3)) )	
	        	m_StripOrder[ i ] = STRORDER_ECA[ m_FecLayerID[i] ];
		else 
			m_StripOrder[ i ] = STRORDER_ECB[ m_FecLayerID[i] ];

	} // End FecTotal	

} 

void MucChain :: InitFecVect()
{
//	cout << "MucChain:: InitFecVect()." << endl;

	unsigned short data = 0;

	int part     	= m_Part;
	int segment  	= m_Segment;

	int id;
	int layer;
	int firstStripID;
	int stripOrder;
	string chainName;

	unsigned int vmeRecord;

	for(int i=0; i<m_FecTotal; i++)
	{	
		// Encode FEC id by FecVect member number	
		id =( (1 - m_FecOrder)/2 ) * ( m_FecTotal - 1 ) + (m_FecOrder * i);
				
 		layer 				= m_FecLayerID[ id ];
		firstStripID 	= m_FirstStripID[ id ];
		stripOrder 		= m_StripOrder[ id ];
		chainName			= MucChain::m_Name;
		vmeRecord 		= EncodeVmeRecord( m_Module, m_Socket, id , data);
//    cout << "FEC:\t" << id << "\t" << (vmeRecord>>LENGTH) << endl;
		
		MucFec aFec( id, stripOrder, part, segment, layer, firstStripID, vmeRecord, chainName );
		m_FecVect.push_back( aFec );
	}					
}

// END



