#include "ClientErrHandler/ClientErrHandler.h"
#include "DistBossUtil/DimErrorMsg.h"
#include "dic.hxx"
#include <iostream>

ClientErrHandler* ClientErrHandler::s_instance = 0;

void ClientErrHandler::registerInstance()
{
   if ( s_instance == 0 ) {
      s_instance = new ClientErrHandler();
      DimClient::addErrorHandler( s_instance );
   }
}

ClientErrHandler::ClientErrHandler()
{
}

void ClientErrHandler::errorHandler(int severity, int code, char* msg)
{
   std::cerr << '[' << DimError::toSeverity(severity) << "] " << DimError::toMsg(code) << std::endl;
   if ( msg != 0 ) {
      std::cout << '[' << DimError::toSeverity(severity) << "] " << msg << std::endl;
   }
}
