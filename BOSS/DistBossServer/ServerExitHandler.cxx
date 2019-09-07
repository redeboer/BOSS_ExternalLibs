#include "DistBossServer/ServerExitHandler.h"
#include <iostream>

ServerExitHandler::ServerExitHandler()
{
}

void ServerExitHandler::exitHandler(int code) {    
   std::cerr << "[WARNING] Received EXIT command from ["
             << DimServer::getClientName() << "] with code: " << code << std::endl;
   std::cerr << "[WARNING] EXIT command from client is FORBIDDEN, ignore it!" << std::endl;
}
