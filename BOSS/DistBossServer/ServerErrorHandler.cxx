#include "DistBossServer/ServerErrorHandler.h"
#include "DistBossUtil/DimErrorMsg.h"
#include <iostream>

ServerErrorHandler::ServerErrorHandler()
{
}

void ServerErrorHandler::errorHandler(int severity, int code, char* msg)
{
   std::cerr << '[' << DimError::toSeverity(severity) << "] " << DimError::toMsg(code) << std::endl;
   if ( msg != 0 ) {
      std::cout << '[' << DimError::toSeverity(severity) << "] " << msg << std::endl;
   }
}
