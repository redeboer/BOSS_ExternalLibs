#ifndef IDATABASESVC_H
#define IDATABASESVC_H

// Base classes
#include "GaudiKernel/IService.h"
#include <string>
#include <vector>
#include <map>

#include "DatabaseSvc/DatabaseRecord.h"

static const InterfaceID IID_IDatabaseSvc("IDatabaseSvc", 1, 0);

class IDatabaseSvc : virtual public IService
{

 public:
  /* Virtual destructor */
  virtual ~IDatabaseSvc() {};

 public:
  static const InterfaceID& interfaceID() { return IID_IDatabaseSvc; }
  static const std::string& serviceInUse() { 
    return g_serviceInUse; 
  };

  virtual int query(const std::string& dbName, const std::string& sql, DatabaseRecordVector& res)=0;

 protected:

  static std::string g_serviceInUse;

};

#endif
