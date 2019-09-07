// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Tables/Set.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_SET_H
#define RDBMODEL_SET_H
#include <vector>
#include <string>
#include "rdbModel/Rdb.h"
#include "rdbModel/Management/Visitor.h"
#include "rdbModel/Tables/Column.h"   // for FIELDTYPE
#include "rdbModel/Tables/Table.h"
namespace rdbModel{
  class Table;
  /** 
      Describe update to be made to a single column.  Must be to an existing
      row.  Value can be any of the three possible FIELDTYPEs.

           @destColName     column to get updated
           @destType        defines whether dest row is existing or proposed
           @to              string for value to go into dest column
           @toType          may be literal, old column or proposed column
  */
  class Set {
  public:
    Set(Table* table, const std::string& destColName, 
        FIELDTYPE destType, 
        const std::string& srcValue, FIELDTYPE srcType, 
        const std::string& interp = std::string("")) : 
      m_myTable(table), m_destCol(destColName), m_destType(destType),
      m_srcValue(srcValue), m_srcType(srcType), m_interp(interp) {}
    /**  
         Normally, operator associated with the assertion will be deleted
         when the assertion itself is deleted, but this won't happen if
         keepOp is set to true.
     */

    ~Set() {}
    //    WHEN getWhen() const {return m_when;}
    Visitor::VisitorState accept(Visitor* v);
    const std::string& getDestColName() const {return m_destCol;}
    FIELDTYPE getDestType() const {return m_destType;}

    const std::string& getSrcValue() const {return m_srcValue;}
    FIELDTYPE getSrcType() const {return m_srcType;}
    const std::string& getInterp() const {return m_interp;}

    bool hasInterp() const {return (m_interp.size() > 0);}

  private:
    Table*      m_myTable;
    // The target is a column name; may refer to existing column or new one
    std::string m_destCol;
    FIELDTYPE   m_destType;
    /// source value may be literal string or column name or empty
    std::string m_srcValue;   
    /// Describes how to interpret m_srcValue
    FIELDTYPE   m_srcType;
    std::string m_interp;
  };
}
#endif

