#ifndef SERVER_ERROR_HANDLER_H
#define SERVER_ERROR_HANDLER_H

#include "dim.hxx"

class ServerErrorHandler : public DimErrorHandler
{
   public :

      ServerErrorHandler();

   private :

      void errorHandler(int severity, int code, char* msg);
};

#endif
