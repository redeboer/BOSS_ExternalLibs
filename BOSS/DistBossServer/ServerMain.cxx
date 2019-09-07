#include "DistBossServer/DistBossServer.h"
#include <iostream>

int main(int argc, char* argv[])
{
   if ( argc != 2 ) {
      std::cout << "Usage: " << argv[0] << " jobOptions.txt" << std::endl;
      exit(1);
   }

   DistBossServer server( argv[1] );

   server.run();

   return 0;
}
