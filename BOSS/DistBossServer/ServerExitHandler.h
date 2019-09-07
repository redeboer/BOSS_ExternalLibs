#ifndef SERVER_EXIT_HANDLER_H
#define SERVER_EXIT_HANDLER_H

#include "dis.hxx"

class ServerExitHandler : public DimExitHandler
{
   public :

      ServerExitHandler();

   private :

      void exitHandler( int code );
};

#endif
