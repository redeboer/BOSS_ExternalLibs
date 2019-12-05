#ifndef DISTBOSS_READER_RPC_CC
#define DISTBOSS_READER_RPC_CC

#include "DistBossUtil/DistBossCode.h"
#include "IRawFile/RawFileExceptions.h"
#include <iostream>

template<class Reader>
ReaderRpc<Reader>::ReaderRpc(const std::string& svrName, const std::vector<std::string>& fnames, int evtMax)
   : DimRpc(svrName.c_str(), "I", "I"),
     m_evtDone(0),
     m_evtMax(evtMax),
     m_stopFlag(0)
{
   sem_init(&m_sem, 0, 0);
   m_freader = new PthrReaderBufPool<Reader>( fnames );

   m_svrName = svrName.substr( svrName.find_last_of('/')+1 );
}

template<class Reader>
ReaderRpc<Reader>::~ReaderRpc()
{
   sem_destroy(&m_sem);

   delete m_freader;

   for (std::map<int, AutoEnlargeBuffer*>::iterator it = m_evtBak.begin(); it != m_evtBak.end(); ++it) {
      delete it->second;
   }

   std::cout << "[" << m_svrName << "] Terminated. Total events processed: " << m_evtDone << std::endl;
}

template<class Reader>
int ReaderRpc<Reader>::wait_to_end()
{
   sem_wait(&m_sem);

   int nClients = dis_get_n_clients(itsIdOut);

   int i = 0;
   while ( nClients > 0 ) {
      // wait for all the clients dis-connecting to this server
      // and force the server to stop when the total waiting time
      // is greater than 5s(5*1s)
      if ( ++i > 5 ) break;
      sleep(1);
      nClients = dis_get_n_clients(itsIdOut);
   }

   return nClients;
}

template<class Reader>
void ReaderRpc<Reader>::rpcHandler()
{
   int clientId = DimServer::getClientId();
   m_inCode = getInt();

   switch (m_inCode) {

      //--------------------------------------------------------
      case (DistBossCode::GetEvent) :
      //--------------------------------------------------------
	 {
	    try {
	       if ( m_stopFlag != 0 ) {
		  throw RawExMessage(("["+ m_svrName+"] Server stopflag is set, now waiting clients to exit!").c_str());
	       }
	       m_pEvt = m_freader->nextEvent();
	    }
	    catch ( RawFileException& e ) {
	       if ( ++m_stopFlag < 3 ) e.print();
	       m_outCode = DistBossCode::NoMoreEvents;
	       setData( (int&)m_outCode );
	       if ( m_stopFlag == 1 ) sem_post(&m_sem);
	       return;
	    }

	    setData((void*)m_pEvt, (m_pEvt[1]*4) );

	    std::map<int, AutoEnlargeBuffer*>::iterator it = m_evtBak.find( clientId );
	    if ( it == m_evtBak.end() ) {
	       m_evtBak[clientId] = new AutoEnlargeBuffer();
	    }
	    m_evtBak[clientId]->copy((void*)m_pEvt, (m_pEvt[1]*4) );

	    ++m_evtDone;

	    if ( m_evtMax >= 0 && m_evtDone >= m_evtMax ) {
	       ++m_stopFlag;
	       std::cout << "[" << m_svrName << "] We have reach the EvtMax!" << std::endl;
	       sem_post(&m_sem);
	    }
	 }

	 break;

      //--------------------------------------------------------
      case (DistBossCode::RetryEvent) :
      //--------------------------------------------------------
	 {
	    //std::cout << "[" << m_svrName << "] RESEND event to: " << DimServer::getClientName() << std::endl;
	    m_pEvt = (uint32_t*)m_evtBak[clientId]->data();
	    setData((void*)m_pEvt, (m_pEvt[1]*4) );
	 }
	 break;

      //--------------------------------------------------------
      case (DistBossCode::GetFileName) :
      //--------------------------------------------------------
	 {
	    std::string fname = m_freader->currentFile();
	    setData( (void*)fname.c_str(), (fname.length()+1) );
	 }
	 break;

      //--------------------------------------------------------
      case (DistBossCode::ClientError) :
      //--------------------------------------------------------
	 std::cout << "[" << m_svrName << "] Error code from client["
	           << clientId << "]: "
		   << DimServer::getClientName()
		   << std::endl;
	 break;

      //--------------------------------------------------------
      default :
      //--------------------------------------------------------
	 std::cout << "[" << m_svrName << "] Unknown code(0x"
	           << std::hex << m_inCode << std::dec
		   << ") from client[" << clientId << "]: " 
		   << DimServer::getClientName()
		   << std::endl;
   }
}

#endif
