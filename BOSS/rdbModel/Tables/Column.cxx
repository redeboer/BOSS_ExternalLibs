// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/Column.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $

#include "rdbModel/Tables/Column.h"
#include "rdbModel/Tables/Datatype.h"
#include "rdbModel/Tables/Table.h"
#include "facilities/Timestamp.h"

#include <algorithm>
namespace rdbModel {

  Column::~Column() {
    delete m_type;
  }

  Enum* Column::getEnum() const {return m_type->getEnum();}

  const std::string& Column::getTableName() const {
    return m_myTable->getName();
  }

  bool Column::okValue(const std::string& val, bool set) const {
    // auto increment and datetime values are established by rdbms
    if (set) {

      if ( (m_from == FROMautoIncrement) || 
           (m_from == FROMnow)) return false;
    }

    return m_type->okValue(val);
  }

  bool Column::isCompatible(const Column* otherCol) const {
    return m_type->isCompatible(otherCol->m_type);
  }


  bool Column::isAutoIncrement() const {
    return (m_from == FROMautoIncrement);
  }

  bool Column::interpret(const std::string& interpType, std::string& val) {
    // Currently only interpretation is for timestamp-like columns.
    // Value of interpType must be "time" and val must be "NOW".
    // In this case, substitute ascii current time
    if (interpType.compare(std::string("time")) != 0) return false;

    Datatype::TYPES dtype = m_type->getType();
    if ((dtype != Datatype::TYPEdatetime) && 
        (dtype != Datatype::TYPEtimestamp)) {
      return false;
    }
    if (val.compare(std::string("NOW")) != 0) return false;

    val = facilities::Timestamp().getString();
    return true;
  }

  Visitor::VisitorState Column::accept(Visitor* v) {

    Visitor::VisitorState state = v->visitColumn(this);
    if (state == Visitor::VBRANCHDONE) return Visitor::VCONTINUE;
    return state;
  }

  void Row::rowSort() {
    if (m_sorted) return;

    FieldValCompare cmp;
    std::sort(m_fields.begin(), m_fields.end(), cmp);
    m_sorted = true;
  }

  FieldVal* Row::find(std::string colname) {
    unsigned nField = m_fields.size();
    unsigned minI = 0;
    unsigned maxI = nField;

    unsigned guess = maxI/2;
    unsigned oldGuess = nField;

    int cmp = colname.compare(m_fields[guess].m_colname);

    while (cmp != 0)  {
      if (guess == oldGuess) return 0;        // not found

      if (cmp < 0 ) {    // thing we tried is > colName, so decrease maxI
        maxI = guess;
      }  else {   // thing we tried is > colName, so increase minI
        minI =  guess;
      }
      oldGuess = guess;
      guess = (minI + maxI)/2;
      cmp = colname.compare(m_fields[guess].m_colname);
    }
    return &m_fields[guess];
  }

   void  Row::regroup(std::vector<std::string>& colNames, 
                      std::vector<std::string>& colVals, 
                      std::vector<std::string>& nullCols) const {
     unsigned nFields = m_fields.size();
     colNames.reserve(nFields);
     colVals.reserve(nFields);

     for (unsigned i = 0; i < nFields; i++) {
       if (m_fields[i].m_null) {
         nullCols.push_back(m_fields[i].m_colname);
       }
       else {
         colNames.push_back(m_fields[i].m_colname);
         colVals.push_back(m_fields[i].m_val);
       }
     }
   }
 
}
