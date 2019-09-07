#ifndef DISTBOSS_RPC_INTERFACE_H
#define DISTBOSS_RPC_INTERFACE_H

class RpcInterface
{
   public :

      virtual ~RpcInterface() {};

      virtual int wait_to_end() = 0;
};

#endif
