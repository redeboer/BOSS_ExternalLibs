#ifndef CLIENT_ERR_HANDLER_H
#define CLIENT_ERR_HANDLER_H

#include "dim.hxx"

class ClientErrHandler : public DimErrorHandler
{
   public :

      static void registerInstance();

      ClientErrHandler();


   private :

      void errorHandler(int severity, int code, char* msg);

      static ClientErrHandler* s_instance;
};

#endif
