// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Tables/Column.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_COLUMN_H
#define RDBMODEL_COLUMN_H
#include <vector>
#include <string>
#include <utility>
#include "rdbModel/Management/Visitor.h"
// #include "rdbModel/Tables/Table.h"

namespace rdbModel {

  //  class Table;
  class Datatype;
  class Enum;
  class Table;
  class XercesBuilder;



  /** 
   * rdbModel representation of a(n SQL-like) table description
   */
  class Column {
    //    class ColumnSource;      // embedded class, described below

  public:
    /// Source of value.  Note timestamp with value current time should
    /// be indicated by contents value CONTENTSupdateTime or (if only
    /// upon insert) CONTENTS enterTime
    enum FROM {
      FROMdefault = 1,          // enduser can override default, however
      FROMautoIncrement,
      FROMnow,                  // datatype must be timestamp - deprecated
      FROMprogram,
      FROMendUser
    };

    /// Hints to program in case FROM field is FROMprogram
    enum CONTENTS {
      CONTENTSunspecified = 0,
      CONTENTSserviceName = 1,
      CONTENTSusername    = 2,
      CONTENTSinsertTime   = 3,
      CONTENTSupdateTime = 4

    };

    Column(Table* myTable=0) : m_myTable(myTable), m_type(0), 
                               m_isPrimaryKey(false) {
      m_contents = CONTENTSunspecified;
      m_default = std::string("");};
    // Column(Table* myTable=0) : m_myTable(myTable), m_type(0), m_source(0) {};
    ~Column();


    const std::string& getName() const {return m_name; };
    const std::string& getComment() const {return m_comment;};

    const std::string& getDefault() const {return m_default;}

    const std::string& getTableName() const;

    Datatype* getDatatype() const {return m_type;};

    /// Return pointer to Enum object associated with this column (if
    /// none, return null pointer).
    Enum* getEnum() const;

    /** See if supplied value meets constraints of column definition
     *   @arg  val    std::string representation of value to be checked
     *   @arg  set    true if value is to be written to column; false
     *                if just being used, e.g. in "where" clause 
     */
    bool okValue(const std::string& val, bool set=true) const;

    /// Return true if otherCol and this have compatible datatypes
    bool isCompatible(const Column* otherCol) const;

    /// Returns true if column may take on value NULL
    bool nullAllowed() const { return m_null;}
   
    bool stickyInsert() const { return m_stickyInsert;}

    bool isPrimaryKey() const {return m_isPrimaryKey;}

    bool isAutoIncrement() const;  

    FROM getSourceType() const {return m_from;}
    CONTENTS getContentsType() const {return m_contents;}

    /**
      Handle special literal values, depending loosely on column datatype.
      Most Column objects won't do any interpretation, but, for example,
      timestamp-like columns may substitute for "NOW"
      Return true if any substitution was done
     */
    bool interpret(const std::string& interpType, std::string&val);
                               
    Visitor::VisitorState accept(Visitor* v);

  private:
    friend class rdbModel::XercesBuilder; // needs access to add.. methods 

    Table* m_myTable;

    std::string m_name;
    std::string m_comment;
    Datatype*  m_type;
      
    std::string m_default;
    FROM m_from;
    CONTENTS m_contents;

    /// Can this field have the value NULL?
    bool m_null;
    ///  For multi-insert, does this column normally keep same value
    /// for all the inserts?
    bool m_stickyInsert;  
    
    /// Is this column a primary key?
    bool m_isPrimaryKey;

  };

  // Do we want Column* for first component or just the column name?
  class FieldVal {
    //    Column* m_pCol;
  public:
    FieldVal(std::string colname, std::string val, bool isNull=false) :
      m_colname(colname), m_val(val), m_null(isNull) { }
    std::string m_colname;
    std::string m_val;
    bool        m_null; // true if field val is NULL; then will ignore m_val
  };

  /// Function object used to sort FieldValPar objects by column name
  class FieldValCompare {
  public:
    //    bool operator()  (const Column* a, const Column*  b) {
    bool operator()  (const FieldVal& a, const FieldVal&  b) {
      return ((a.m_colname).compare(b.m_colname) < 0);
      //      return (a.first->getName().compare(b.first->getName()) < 0);
    }
  };

  /**
          @class Row
     A collection of column names and values. 
   */
  class Row {
  public:
    Row() : m_sorted(false) {m_fields.clear();}
    Row(std::vector<FieldVal>& fields) : m_fields(fields), m_sorted(false) {}

    ~Row() { m_fields.clear(); }
    void rowSort();
    void addField(const FieldVal& f) {m_fields.push_back(f); m_sorted = false;}
    void clear() {m_fields.clear();}

    FieldVal* find(std::string colname);

    /// Reorder information suitable for Connection::insert
    void  regroup(std::vector<std::string>& colNames, 
                  std::vector<std::string>& colVals, 
                  std::vector<std::string>& nullCols) const;
  private:
    std::vector<FieldVal> m_fields;
    bool     m_sorted;
  };
}
#endif
