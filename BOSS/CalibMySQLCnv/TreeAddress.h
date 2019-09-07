// $Id: TreeAddress.h,v 1.5 2012/01/05 03:51:51 zhangy Exp $
#ifndef TREEADDRESS_H
#define TREEADDRESS_H

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "TTree.h"
#include <mysql.h>
#include "DatabaseSvc/IDatabaseSvc.h"
#include "TTree.h"
#include "TBuffer.h"

// Forward declarations
class IRegistry;

/** @class TreeAddress TreeAddress.h GaudiKernel/TreeAddress.h

    Tree Transient Address.
    The generic transient address describes the recipe to load/save
    a persistent object from/to its transient representation.

    @author Markus Frank
    @version 1.0
*/
class TreeAddress : public IOpaqueAddress{  
protected:
  /// Reference count
  unsigned long   m_refCount;
  /// Storage type
  long            m_svcType;
  /// Class id
  CLID            m_clID;
  /// String parameters to be accessed
//  std::string     m_par[3];
  /// Integer parameters to be accessed
  unsigned long  m_ipar[2];
  unsigned long  m_treesize[9];
//  MYSQL_ROW m_row;
  /// Pointer to corresponding directory
  IRegistry*      m_pRegistry;
//  char m_pp[8][1024000];
  //Trees
  DatabaseRecord* m_records;
  int m_runFrom;
  int m_runTo;
public:
  /// Dummy constructor
  TreeAddress() 
    : m_refCount(0), 
      m_svcType(0),
      m_clID(0),
      m_pRegistry(0)
  {
 //   m_ipar[0]=m_ipar[1]=m_ipar[2]=m_ipar[3]=0xFFFFFFFF;
  }
  /// Standard Constructor
  TreeAddress(const TreeAddress& copy)  
    : IOpaqueAddress(copy),
      m_refCount(0),
      m_svcType(copy.m_svcType),
      m_clID(copy.m_clID),
      m_pRegistry(copy.m_pRegistry)
  {

  }
  /// Standard Constructor

 TreeAddress( long svc,
                  const CLID& clid,
		  DatabaseRecord& records,
                  const unsigned long *ipar
                  )  
    : m_refCount(0),
      m_svcType(svc),
      m_clID(clid),
      m_pRegistry(0)
  { 
    m_records=&records;
    m_ipar[0] = ipar[0];
    m_ipar[1] = ipar[1];
  }


  /// Standard Destructor
  virtual ~TreeAddress()   {
  }

  /// Add reference to object
  virtual unsigned long addRef   ()   {
    return ++m_refCount;
  }
  /// release reference to object
  virtual unsigned long release  ()   {
    int cnt = --m_refCount;
    if ( 0 == cnt )   {
      delete this;
    }
    return cnt;
  }
  /// Pointer to directory
  virtual IRegistry* registry()   const     {
    return m_pRegistry;
  }
  /// Set pointer to directory
  virtual void setRegistry(IRegistry* pRegistry)   {
    m_pRegistry = pRegistry;
  }
  /// Access : Retrieve class ID of the link
  const CLID& clID()  const   {
    return m_clID;
  }
  /// Access : Set class ID of the link
  void setClID(const CLID& clid)   {
    m_clID = clid;
  }
  /// Access : retrieve the storage type of the class id
  long svcType()  const    {
    return m_svcType;
  }
  /// Access : set the storage type of the class id
  void setSvcType(long typ)    {
    m_svcType = typ;
  }
  ///// Retrieve string parameters
  virtual const std::string* par() const   {
    //return m_par;
  }

virtual DatabaseRecord* pp() {
    return m_records;
}

  /// Retrieve integer parameters
  virtual const unsigned long* ipar()  const  {
    return m_ipar;
  }

  /// set run from
  void setRunFrom(int runFrom){
    m_runFrom= runFrom;
  }
  /// set run to
  void setRunTo(int runTo){
    m_runTo= runTo;
  }
  /// get run from
  int getRunFrom(){
    return m_runFrom;
  }
  /// get run to
  int getRunTo(){
    return m_runTo;
  }
};
#endif // GAUDIKERNEL_GENERICADDRESS_H

