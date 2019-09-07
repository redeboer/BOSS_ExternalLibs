// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Rdb.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $ 
#include "rdbModel/Rdb.h"
#include "rdbModel/Tables/Table.h"
#include "rdbModel/RdbException.h"

namespace rdbModel {

  Rdb::~Rdb() {
    while (m_tables.size() ) {
      Table* table = m_tables.back();
      m_tables.pop_back();
      delete table;
    }
  }

  Table* Rdb::getTable(const std::string& name) const {
    unsigned nTable = m_tables.size();
    for (unsigned iTable = 0; iTable < nTable; iTable++) {
      Table* table = m_tables[iTable];
      if (table->getName() == name) return table;
    }
    return 0;
  }

  Column* Rdb::getColumn(const std::string& tableName, 
                         const std::string& colName) const {
    Table* table = getTable(tableName);
    if (!table) return 0;

    return table->getColumnByName(colName);

  }

  Index* Rdb::getIndex(const std::string& tableName, 
                       const std::string& indexName) const {
    Table* table = getTable(tableName);
    if (!table) return 0;

    return table->getIndexByName(indexName);

  }

  void Rdb::setConnection(Connection* connection) {
    m_connection = connection;

    // propagate to all our tables as well
    for (unsigned i = 0; i < m_tables.size(); i++) {
      m_tables[i]->setConnection(connection);
    }
  }

  int Rdb::insertRow(const std::string& tName, Row& row, int* serial) const {
    Table* t = getTable(tName);
    if (!t) {
      std::string msg("Rdb::insertRow unknown table ");
      msg = msg + tName;
      throw RdbException(msg);
    }
    return (t->insertRow(row, serial));
  }

  int Rdb::updateRows(const std::string& tName, Row& row, Assertion* where) const {
    Table* t = getTable(tName);
    if (!t) {
      std::string msg("Rdb::insertRow unknown table ");
      msg = msg + tName;
      throw RdbException(msg);
    }
    return (t->updateRows(row, where));
  }

  int Rdb::insertLatest(Table* t, Row& row, int* serial) const {
    return (t->insertLatest(row, serial));
  }

  int Rdb::insertLatest(const std::string& tName, Row& row, int* serial) 
    const {
    Table* t = getTable(tName);
    if (!t) {
      std::string msg("Rdb::insertLatest unknown table ");
      msg = msg + tName;
      throw RdbException(msg);
    }
    return (t->insertLatest(row, serial));
  }

  int Rdb::supersedeRow(const std::string& tName, Row& row, int oldKey, 
                        int* newKey) const {
    Table* t = getTable(tName);
    if (!t) {
      std::string msg("Rdb::supersedeRow unknown table ");
      msg = msg + tName;
      throw RdbException(msg);
    }
    return (t->supersedeRow(row, oldKey, newKey));
  }

  unsigned int Rdb::accept(Visitor* v) {
    Visitor::VisitorState state = v->visitRdb(this);
    if (state != Visitor::VCONTINUE) return state;

    unsigned nTable = m_tables.size();

    for (unsigned i = 0; i < nTable; i++) {
      state = m_tables[i]->accept(v);
      if (state != Visitor::VCONTINUE) return state;
    }
    return state;
  }


}
    
