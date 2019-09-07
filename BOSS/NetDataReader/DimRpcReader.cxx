#include "NetDataReader/DimRpcReader.h"
#include "ClientErrHandler/ClientErrHandler.h"
#include "DistBossUtil/AutoEnlargeBuffer.h"
#include "DistBossUtil/DistBossCode.h"
#include "IRawFile/RawFileExceptions.h"
#include "dic.hxx"
#include <iostream>

pthread_mutex_t DimRpcReader::m_rpcLock = PTHREAD_MUTEX_INITIALIZER;

DimRpcReader::DimRpcReader(ReaderArgType& name)
{
   if ( name.empty() ) {
      throw RawExMessage("[NetDataReader] The name of DistBoss EvtServer was not set!");
   }

   ClientErrHandler::registerInstance();

   m_buffer = new AutoEnlargeBuffer(128*1024);

   m_rpc = new DimRpcInfo(name.c_str(), 5, DistBossCode::ServerTimeout);
}

DimRpcReader::~DimRpcReader()
{
   delete m_rpc;
}

const uint32_t* DimRpcReader::nextEvent()
{
   static int nn = 0;
   ++nn;

   int theCode = DistBossCode::GetEvent;

   for ( int i = 1; i < 7; ++i ) {
      pthread_mutex_lock( &m_rpcLock );

      m_outCode = theCode;
      m_rpc->setData(m_outCode);
      int size = m_rpc->getSize();
      void* data = m_rpc->getData();
      m_buffer->copy(data, size);

      pthread_mutex_unlock( &m_rpcLock );

      if ( size > 4 ) {
	 return (const uint32_t*)m_buffer->data();
      }
      else if ( size == 4 ) {
	 m_inCode = *((const uint32_t*)m_buffer->data());
	 if ( m_inCode == DistBossCode::NoMoreEvents ) {
	    throw RawExMessage("[NetDataReader] Reach the end, no more events left.");
	 }
	 else if ( m_inCode == DistBossCode::ServerTimeout) {
	    if ( i < 6 ) {
	       int sec = i;
	       std::cout << "[NetDataReader] Event " << nn << " timeout. Sleep " << sec << "s before retry." << std::endl;
	       sleep(sec);
	       std::cout << "[NetDataReader] Event " << nn << " now retry time " << i << " ..." << std::endl;
	       theCode = DistBossCode::RetryEvent;
	       continue;
	    }
	    else {
	       throw RawExMessage("[NetDataReader] Failed to retry server. Stop this client!");
	    }
	 }
	 else if ( m_inCode == DistBossCode::ServerError ) {
	    throw RawExMessage("[NetDataReader] DistBossServer ERROR !!!");
	 }
	 else {
	    throw RawExMessage("[NetDataReader] Unknown server code !!!");
	 }
      }
      else {
	 throw RawExMessage("[NetDataReader] Invalid data from server !!!");
      }

      break;
   }

   return 0;
}

const uint32_t* DimRpcReader::currentEvent() const
{
   return (const uint32_t*)m_buffer->data();
}

uint32_t DimRpcReader::runNo()
{
    //FIXME: this is a place holder for runNo()
    //fill it in the future
    return 0xFFFFFFFF;
}

std::string DimRpcReader::currentFile()
{
   pthread_mutex_lock( &m_rpcLock );

   m_outCode = DistBossCode::GetFileName;
   m_rpc->setData(m_outCode);
   // should check status code here, correct it in future
   std::string fname((char*)m_rpc->getData());

   pthread_mutex_unlock( &m_rpcLock );

   return fname;
}

uint32_t DimRpcReader::stat()
{
   return 0;
}
