// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Tables/Supersede.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_SUPSERSEDE_H
#define RDBMODEL_SUPERSEDE_H
#include <vector>
#include <string>
#include "rdbModel/Management/Visitor.h"

namespace rdbModel{

  class Set;
  class Table;
  class Assertion;

  /** 
      Supersede operation needs to
          o change one or more column values in row being superseded
          o set values in new, superseding row.  Default is to assume
            values come from the old row; only specify those that don't
  */
  class Supersede {
  public:
    Supersede(Table* table, Assertion* onlyIf=0);

    ~Supersede();

    const std::vector<Set*>&  getSetOld() const {return m_setOld;}
    const std::vector<Set*>&  getSetNew() const {return m_setOld;}

    void addSet(Set* s);

    const Table* getTable() const {return m_myTable;}

    Visitor::VisitorState accept(Visitor* v);

    Assertion* getOnlyIf() {return m_onlyIf;}

    const std::vector<std::string>& getAsk() const {
      return m_ask; }

    const std::vector<std::string>& getOldDefaulted() const {
      return m_oldDefaults; }

    const std::vector<std::string>& getFromOld() const {
      return m_fromOld;}

    const std::vector<FieldVal>& getFixed() const { return m_fixed;}
    const std::vector<std::string>& getFixedInterp() const { 
      return m_fixedInterp;}

    const std::vector<std::string>& getForced() const { return m_oldForced;}


    

    // Might want to get rid of this; should know assertion by constructor
    // time.
    void setOnlyIf(Assertion* onlyIf) {m_onlyIf = onlyIf;}

    /**
       Initialize m_oldDefaults, m_oldForced, m_ask and m_fixed if it 
       hasn't been done already
     */
    void normalize();

  private:
    Table* m_myTable;
    //    bool   m_keepOp;
    Assertion* m_onlyIf;   // conditions row must meet to make it supersedable.
    std::vector<Set*> m_setOld;
    std::vector<Set*> m_setNew;
    /** Always need to fetch the same set of columns from the row to be
        superseded.  Some of the values must be used in the new row;
        others are just defaults.  
    */
    /// column names where old-row value is to be used as default
    std::vector<std::string> m_oldDefaults;  
    /// column names where old-row value is to be copied to new row
    std::vector<std::string> m_oldForced;

    /// Names of columns for which client must supply a value (no default)
    std::vector<std::string> m_ask;

    /// Names of columns for which <supersede> supplies fixed value for new row
    std::vector<FieldVal> m_fixed;

    /// Parallel  vector to keep track of which among fixed columns
    /// require further interpretation.  May want to move this into FieldVal
    std::vector<std::string> m_fixedInterp;

    /// Name of all columns to be fetched from old row by Table::supersedeRow
    std::vector<std::string> m_fromOld;

    bool m_normalized; 


  };
}
#endif

