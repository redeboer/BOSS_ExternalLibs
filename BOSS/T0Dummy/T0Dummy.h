#ifndef T0DUMMY_H
#define T0DUMMY_H

#include "GaudiKernel/Algorithm.h"


class T0Dummy :public Algorithm{

  public:

    T0Dummy(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

};

#endif
