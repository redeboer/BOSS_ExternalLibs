// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Tables/Table.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_TABLE_H
#define RDBMODEL_TABLE_H
#include <vector>
#include <string>
#include <iostream>
#include "rdbModel/Management/Visitor.h"
#include "rdbModel/Tables/Column.h"

namespace rdbModel {

  //  class Column;
  class Index;
  class Assertion;

  class XercesBuilder;
  class Connection;

  class InsertNew;
  class Supersede;

  /// Function object used to sort columns by column name
  class ColCompare {
  public:
    bool operator()  (const Column* a, const Column*  b) {
      return (a->getName().compare(b->getName()) < 0);
    }
  };

  /** 
   * rdbModel representation of a(n SQL-like) table description
   */
  class Table {
  public:
    Table();
    ~Table();

    void setConnection(Connection* connect);

    const std::string& getName() const { return m_name;}
    Column* getColumnByName(const std::string& name) const;
    Index* getIndexByName(const std::string& name) const;
    Assertion* getAssertionByName(const std::string& name) const;

    /**
       insertLatest is smart in the following respects:
          o Makes some checks to see if row is self-consistent
          o Fills in all fields which are to be supplied by "service"
          o If row satisfies conditions for being "official"
            - queries table to see if any pre-existing rows need 
              adjustment; if so, they are adjusted
          o Finally inserts the row 

       If @a test is true, just output description of what would be
       done without actually doing it.
       Note @a row may be modified by the function.
       If a non-zero pointer is supplied for @a serial
       it will contain the primary key value (if there is one) of
       the new row on exit.
    */
    int insertLatest(Row& row, int* serial=0) const;

    /**
       The less smart insertRow function fills in all fields which are to
       be supplied by "service" and inserts the row, without any further
       checks.
       Good return value is 0
     */
    int insertRow(Row& row, int* serial=0) const;

    /**
       Given some input values and primary key of an existing row,
       insert a new row which will supersede the old, according to
       prescription laid down in <supersede> element of the 
       xml description of the dbs.  
       Good return value is 0
     */
    int supersedeRow(Row& row, int oldKey, int* newKey=0) const;

    /**
       Silently fills in "service" fields as well as requested updates
       Good return value is 0
     */
    int updateRows(Row &row, Assertion* where) const;

    // Do we need these for anything?  
    InsertNew* getInsertNew() const {return m_iNew;}
    Supersede* getSupersede() const {return m_sup;}

    Visitor::VisitorState accept(Visitor* v);
    //     Visitor::VisitorState acceptNotRec(Visitor* v);

    void  sortColumns();

  private:
    friend class rdbModel::XercesBuilder; // needs access to add.. methods
    friend class rdbModel::Rdb;           // to set connection
    void setValidRow(Assertion* v) {m_validRow = v;}
    std::vector<Column* > m_cols;
    std::vector<Column* > m_sortedCols;
    std::vector<Assertion* > m_asserts;
    std::vector<Index* > m_indices;

    /// Subset of columns which are responsibility of Program (us) to fill
    std::vector<Column* > m_programCols;

    /// Subset of columns which user *must* supply
    std::vector<Column* > m_userCols;

    /// Subset of columns with defaults (includes those defaulting to null)
    std::vector<Column*> m_mayDefault;

    std::string m_name;
    std::string m_version;
    std::string m_comment;

    std::string m_primaryKeyCol;  // cache this for use in supersedeRow

    bool m_sorted;  // set to true once columns have been sorted by name
    unsigned m_nEndUser;  // #columns whose value must be supplied by user

    Assertion* m_validRow;
    InsertNew* m_iNew;
    Supersede* m_sup;

    Connection* m_connect;
    std::ostream* m_out;
    std::ostream* m_err;

    /// For use by builders
    void addColumn(Column* c);

    /// For use by builders
    void addAssert(Assertion* a) {m_asserts.push_back(a);}
    /// For use by builders
    void addIndex(Index* i) {m_indices.push_back(i); }

    /// Service routine for insertLatest
    bool doInterUpdate(const std::vector<Set>& sets, Assertion* subsAssert,
                       Row& toBe) const;

    /** Service routine when doing insert or update. Augment Row with
        values for those columns which are responsibility of Program 
        (not end-user).
        @arg newRow should be true for insert, false for update.
    */
    bool fillProgramCols(Row& row, bool newRow) const;

    /// Service routine when doing insert
    void fillDefaults(Row& row) const;

    /// Service routine for supersedeRow.  Check supersedable condition
    bool isSupersedable(std::string oldKeyStr) const;

    /// Service routine for supersedeRow.  Need to know table's primary key
    const std::string& setPrimaryKeyCol();


  };

}
#endif
