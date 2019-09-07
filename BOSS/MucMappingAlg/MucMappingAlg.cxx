//------------------------------------------------------------------------------|
//      [File  ]:                       MucMappingAlg.cxx                       |
//      [Brief ]:       Muon detector electronics mapping algrithom for BESIII  |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#include "GaudiKernel/MsgStream.h"

#include "MucMappingAlg/MucMappingAlg.h"

// -----------------------------------Declaration----------------------------------------
MucMappingAlg::MucMappingAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
	Algorithm(name, pSvcLocator), m_fFecOrder(1)
{
	// Declear the properties
	declareProperty("FecOrder",   	m_fFecOrder);
	declareProperty("ChainMap",   	m_sFileName[0]);
	declareProperty("FecMap",      	m_sFileName[1]);
	declareProperty("OutputOption", m_sOutputOption);
}

// ----------------------------------Initialize-----------------------------------------
StatusCode MucMappingAlg::initialize()
{
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << endreq << "In initialize()" << endreq;
	log << MSG::INFO << "Fec order:\t\t"      << m_fFecOrder    << endreq;
	log << MSG::INFO << "Chain map:\t"        << m_sFileName[0] << endreq;
	log << MSG::INFO << "Output map:\t\t"     << m_sFileName[1] << endreq;	
	log << MSG::INFO << "Output option:\t\t"  << m_sOutputOption << endreq;

	ifstream fin(m_sFileName[0].c_str(), ios::in);

	log << MSG::INFO << "-------Chain map--------" << endreq;
        if( m_sFileName[0].size() == 0 || fin.bad() )
        {
         
                log << MSG::INFO << endreq << "Load default map!" << endreq;

                for(int i=0; i<MODULE_MAX; i++)
                {
                        for(int j=0; j<SOCKET_MAX; j++)
                        {
                                m_sChainMap[i][j] = DEFAULT_MAP[i][j];
                             //   strcpy( m_ChainMap[i][j], DEFAULT_MAP[i][j] );
                                log << MSG::INFO << m_sChainMap[i][j] << "\t";
                        }

                        log << MSG::INFO << endreq;
                }
        }
        else
        {
                string tempMark;
                for(int i=0; i<MODULE_MAX; i++)
                {
                        for(int j=0; j<SOCKET_MAX; j++)
                        {
                                fin >> tempMark;
                               // if( strlen( tempMark ) > MARK_NUM )
                                if( tempMark.size() > MARK_NUM )
                                {
                                        log << MSG::INFO <<  endreq
                                            << "Mark:\t" << "Module\t" << i << "Socket\t" << j << "\t"
                                            << tempMark  << "\terror!"<< endreq;
                                        continue;
                                }

                                m_sChainMap[i][j] = tempMark;
                           //     strcpy( m_sChainMap[i][j], tempMark );
                                log << MSG::INFO << m_sChainMap[i][j] << "\t";
                        }
                        log << MSG::INFO << endreq;
                }

                fin.close();
        }
	log << MSG::INFO << "------------------------" << endreq;
	return StatusCode::SUCCESS;
}

// ----------------------------------Execute--------------------------------------------
StatusCode MucMappingAlg::execute()
{
        MsgStream log(msgSvc(), name());
        log << MSG::INFO << endreq << "In execute()" << endreq;


        return StatusCode::SUCCESS;
}


// ----------------------------------Finalize------------------------------------------
StatusCode MucMappingAlg::finalize()
{
	MsgStream log(msgSvc(), name());
  log << MSG::INFO << endreq << "In finalize()" << endreq << endreq;
	
	ofstream fout(m_sFileName[1].c_str(), ios::out);
  if( fout.bad() )
  {
  	log << MSG::INFO << "Output file created error!" << endreq;
    exit(-1);
  }
	
	for(int i=0; i<DATA_NUM; i++)
	{	
		if( m_sOutputOption[i] == '1' )
     	fout << DATA_NAME[i] << "\t";
	}
	fout << endl;	

  for(int i=0; i<MODULE_MAX; i++)
  {
  	log << MSG::INFO << "----------------------Module [ " << i << " ]----------------------" << endreq;
    for(int j=0; j<SOCKET_MAX; j++)
    {
    	int id = i * SOCKET_MAX + j;
      m_mucChain[id] = new MucChain( id, m_sChainMap[i][j], i, j, m_fFecOrder );

			for( int k=0; k<m_mucChain[id]->GetFecTotal(); k++ )
			{
        MucFec* aFec = m_mucChain[id]->GetFec( k );
				if( m_sOutputOption[0] == '1' )
        	fout << aFec->GetChainName()    << "\t";
				if( m_sOutputOption[1] == '1' )
          fout << aFec->DecodeModule()    << "\t";
				if( m_sOutputOption[2] == '1' )
          fout << aFec->DecodeSocket()    << "\t";
				if( m_sOutputOption[3] == '1' )
          fout << aFec->DecodeFecID()     << "\t";
				if( m_sOutputOption[4] == '1' )					
          fout << aFec->GetVmeAddress()   << "\t";
				if( m_sOutputOption[5] == '1' )
          fout << aFec->GetVmeBin()       << "\t";
				if( m_sOutputOption[6] == '1' )
          fout << aFec->GetPart()         << "\t";
				if( m_sOutputOption[7] == '1' )
          fout << aFec->GetSegment()      << "\t";
				if( m_sOutputOption[8] == '1' )
          fout << aFec->GetLayer()        << "\t";
				if( m_sOutputOption[9] == '1' )
          fout << aFec->GetFirstStripID() << "\t";
				if( m_sOutputOption[10] == '1' )					
          fout << aFec->GetStripOrder()   << "\t";				
				if( m_sOutputOption[11] == '1' )
          fout << aFec->GetGeoAddress()   << "\t";
				if( m_sOutputOption[12] == '1' )
          fout << aFec->GetGeoBin()       << "\t";
				if( m_sOutputOption[13] == '1' )
          fout << aFec->GetDataBin()      << "\t";
				fout << endl;
      }

			log << MSG::INFO << "Chain ID: " << id << "\tName: " << m_sChainMap[i][j];
			log << MSG::INFO << "\tSocket: " << j << "\tFecTotal: " << m_mucChain[id]->GetFecTotal() 
			    << "\tFecOrder: " << m_fFecOrder << "\tdone!" << endreq;
		} // end socket 
	} // end module

	fout.close();
		
	return StatusCode::SUCCESS;	
}







