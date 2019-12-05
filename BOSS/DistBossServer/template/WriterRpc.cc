#ifndef DISTBOSS_WRITER_RPC_CC
#define DISTBOSS_WRITER_RPC_CC

#include "DistBossUtil/DistBossCode.h"
#include <iostream>

template<class Writer>
WriterRpc<Writer>::WriterRpc(const std::string& svrName, const std::string& fname)
   : DimRpc(svrName.c_str(), "I", "I"),
     m_evtDone(0)
{
   m_writer = new PthrWriterBufPool<Writer>(fname);

   m_svrName = svrName.substr( svrName.find_last_of('/')+1 );
}

template<class Writer>
WriterRpc<Writer>::~WriterRpc()
{
   if ( m_writer->stat() == 0 ) {
      while ( m_evtBak.size() > 0 ) {
	 clearBak(-1);
      }
      if ( dis_get_n_clients(itsIdOut) > 0 ) {
	 std::cout << "[" << m_svrName << "] PROBLEMS @ TERMINATING. FORCE TO QUIT" << std::endl;
      }
      int code = DistBossCode::StatusFinalize;
      m_writer->writeEvent(&code, 4);
   }

   delete m_writer;

   for (std::map<int, AutoEnlargeBuffer*>::iterator it = m_evtBak.begin(); it != m_evtBak.end(); ++it) {
      delete it->second;
   }

   std::cout << "[" << m_svrName << "] Terminated. Total events processed: " << m_evtDone << std::endl;
}

template<class Writer>
int WriterRpc<Writer>::wait_to_end()
{
   int nClients = 0;
   int i = 0;

   do {
      // wait for all the clients dis-connecting to this server
      // and force the server to stop when the total waiting time
      // is greater than 10s(5*1s)
      if ( ++i > 5 ) break;
      sleep(2);
      nClients = dis_get_n_clients(itsIdOut);
   }
   while ( nClients > 0 );

   return nClients;
}

template<class Writer>
void WriterRpc<Writer>::rpcHandler()
{
   m_size = getSize();
   m_pEvt = getData();

   int clientId = DimServer::getClientId();
   int  outCode = DistBossCode::StatusSuccess;

   if ( m_size == 4 ) {
      unsigned int inCode = *(unsigned int*)m_pEvt;

      if ( inCode == DistBossCode::StatusFinalize ) {
	 std::cout << "[" << m_svrName << "] Client [" << DimServer::getClientName() << "] finalized" << std::endl;
	 clearBak(clientId);
	 if ( dis_get_n_clients(itsIdOut) <= 1 ) {
	    m_writer->writeEvent(m_pEvt, m_size);
	 }
      }
      else if ( inCode == DistBossCode::ClientReady ) {
	 std::cout << "[" << m_svrName << "] Client [" << DimServer::getClientName() << "] connected" << std::endl;
      }
   }
   else if ( m_size > 4 ) {
      std::map<int, AutoEnlargeBuffer*>::iterator it = m_evtBak.find( clientId );
      if ( it == m_evtBak.end() ) {
	 m_evtBak[clientId] = new AutoEnlargeBuffer();
	 m_evtBak[clientId]->copy(m_pEvt, m_size );
      }

      void* pbak = m_evtBak[clientId]->data();
      if ( *(int*)pbak != *(int*)m_pEvt ) {
	 m_writer->writeEvent(pbak, m_evtBak[clientId]->size());
	 ++m_evtDone;
      }

      m_evtBak[clientId]->copy(m_pEvt, m_size );
   }
   else {
      outCode = DistBossCode::ServerTimeout;
   }

   setData(outCode);
}

template<class Writer>
void WriterRpc<Writer>::clearBak(int clientId)
{
   std::map<int, AutoEnlargeBuffer*>::iterator it = (clientId<0) ? m_evtBak.begin() : m_evtBak.find( clientId );
   if ( it != m_evtBak.end() ) {
      m_writer->writeEvent( it->second->data(), it->second->size() );
      ++m_evtDone;

      delete it->second;
      m_evtBak.erase(it);
   }
}

#endif
