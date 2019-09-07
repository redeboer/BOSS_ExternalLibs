// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Db/ResultHandle.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $

#include "rdbModel/Db/ResultHandle.h"

namespace rdbModel {

  void ResultHandle::cleanFieldPtrs(std::vector<std::string*>& fields) {
    for (unsigned i = 0; i < fields.size(); i++) {
      if (fields[i] != 0) delete fields[i];
    }
    fields.resize(0);
  }

}
