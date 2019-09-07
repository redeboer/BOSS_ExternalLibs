#include "NetDataWriter/DimRpcWriter.h"
#include "ClientErrHandler/ClientErrHandler.h"
#include "DistBossUtil/DistBossCode.h"
#include "IRawFile/RawFileExceptions.h"
#include "dic.hxx"
#include <iostream>

pthread_mutex_t DimRpcWriter::m_rpcLock = PTHREAD_MUTEX_INITIALIZER;

DimRpcWriter::DimRpcWriter(WriterArgType& name)
   : m_inCode(0)
{
   if ( name.empty() ) {
      throw RawExMessage("[NetDataWriter] The name of DistBoss Server was not set!");
   }

   ClientErrHandler::registerInstance();

   m_rpc = new DimRpcInfo(name.c_str(), 5, DistBossCode::ServerTimeout);
}

DimRpcWriter::~DimRpcWriter()
{
   delete m_rpc;
}

int DimRpcWriter::writeEvent(void *pevt, int size)
{
   static int nn = 0;
   ++nn;

   for ( int i = 1; i < 7; ++i ) {
      pthread_mutex_lock( &m_rpcLock );

      m_rpc->setData(pevt, size);
      int inCode = m_rpc->getInt();

      pthread_mutex_unlock( &m_rpcLock );


      if ( inCode != DistBossCode::StatusSuccess) {
	 if ( inCode == DistBossCode::ServerTimeout) {
	    if ( i < 6 ) {
	       int sec = i;
	       std::cout << "[NetDataWriter] Event " << nn << " timeout. Sleep " << sec << "s before retry." << std::endl;
	       sleep(sec);
	       std::cout << "[NetDataWriter] Event " << nn << " now retry time " << i << " ..." << std::endl;
	       continue;
	    }
	    else {
	       m_inCode = inCode;
	       throw RawExMessage("[NetDataWriter] Failed to retry server. Stop this client!");
	    }
	 }
	 else if ( inCode == DistBossCode::ServerError) {
	    m_inCode = inCode;
	    throw RawExMessage("[NetDataWriter] Received server ERROR code!");
	 }
	 else {
	    m_inCode = inCode;
	    throw RawExMessage("[NetDataWriter] Unknown server code!");
	 }
      }

      if ( size == 4 && *((int*)pevt) == DistBossCode::StatusFinalize) {
	 throw ReachEndOfFileList();
      }

      break;
   }

   return 0;
}

int DimRpcWriter::close()
{
   return 0;
}

int DimRpcWriter::stat()
{
   return m_inCode;
}
