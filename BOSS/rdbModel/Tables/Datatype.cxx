// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Tables/Datatype.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#include <iostream>
#include "rdbModel/Tables/Datatype.h"
#include "facilities/Util.h"
#include "facilities/Timestamp.h"

namespace {

  using rdbModel::Datatype;

  bool initDone = false;
  const unsigned int N_SUPPORTED_TYPES = rdbModel::Datatype::TYPEchar + 1;
  std::string typenames[N_SUPPORTED_TYPES];
  void init() {
    if (!initDone) {
      typenames[Datatype::TYPEenum] = std::string("enum");
      typenames[Datatype::TYPEdatetime] = std::string("datetime");
      typenames[Datatype::TYPEtimestamp] = std::string("timestamp");
      typenames[Datatype::TYPEint] = std::string("int");
      typenames[Datatype::TYPEmediumint] = std::string("mediumint");
      typenames[Datatype::TYPEsmallint] = std::string("smallint");
      typenames[Datatype::TYPEreal] = std::string("real");
      typenames[Datatype::TYPEdouble] = std::string("double");
      typenames[Datatype::TYPEvarchar] = std::string("varchar");
      typenames[Datatype::TYPEchar] = std::string("char");
      initDone = true;
    }
  }
  int findType(std::string aType) {
    if (!initDone) init();
    for (unsigned int i = 0; i < N_SUPPORTED_TYPES; i++) {
      if (aType == typenames[i]) return i;
    }
    return (int) Datatype::TYPEnotFound;
  }
  enum TYPE_OF_TYPE {
    TOTinteger = 0,
    TOTreal,
    TOTchar,
    TOTdate
  };
  int findTOT(Datatype::TYPES aType) {
    if ((aType == Datatype::TYPEint) || (aType == Datatype::TYPEmediumint) ||
        (aType == Datatype::TYPEsmallint)) return TOTinteger;
    else if ((aType == Datatype::TYPEreal) || (aType == Datatype::TYPEdouble))
      return TOTreal;
    else if ((aType == Datatype::TYPEdatetime) || 
             (aType == Datatype::TYPEtimestamp))
      return TOTdate;
    else return TOTchar;
  }
    
}

namespace rdbModel {

  int Datatype::setType(std::string name) {
    m_type = (TYPES) findType(name);
    if (m_type >=0 ) {
      m_typename = name;
    }

    // Set up initial restrictions for integer-like types.  These
    // values come from the MySQL doc.  See e.g. 
    // http://www.mysql.com/documentation/mysql/bychapter/manual_Column_types.html#Numeric_types
    switch (m_type) {
      case TYPEint: {
        m_maxInt = 2147483647;
        m_minInt = -2147483647;
        m_isInt = true;
        break;
      }
      case TYPEmediumint: {
        m_maxInt = 8388607;
        m_minInt = -8388608;
        m_isInt = true;
        break;
      }
      case TYPEsmallint: {
        m_maxInt = 32767;
        m_minInt = -32768;
        m_isInt = true;
        break;
      }
    default:
      break;
    }
    return m_type;
  }

  // Makes sense only for numeric types or for datetime [or timestamp]
  bool Datatype::setInterval(const std::string& min, const std::string& max) {
    bool ret = false;
    //    if ((m_type == TYPEtimestamp) || (m_type == TYPEenum) || 
    if ( (m_type == TYPEenum) || 
        (m_type == TYPEchar) || (m_type == TYPEvarchar)) {
      std::cerr << "From rdbModel::Datatype::setInterval " << std::endl;
      std::cerr << "Cannot set interval restriction for type " <<
        typenames[m_type] << std::endl;
      return false;
    }

    m_restrict = RESTRICTinterval;
    m_min = min;
    m_max = max;

    // In case data is some integer type, convert min and max to integers,
    // store.  Return false if anything about them is unreasonable.
    if (m_isInt) {
      try {
        int minInt = facilities::Util::stringToInt(min);
        int maxInt = facilities::Util::stringToInt(max);
        if (minInt > m_minInt) m_minInt = minInt;
        if (maxInt < m_maxInt) m_maxInt = maxInt;
      }
      catch (facilities::WrongType ex) {
        std::cerr << "Error detected in XercesBuilder::buildDatatype " 
                  << std::endl;
        std::cerr << ex.getMsg() << std::endl;
        return false;
      }
      ret = (m_min < m_max);
    }
    // Don't expect to encounter interval restriction for floating point,
    // so don't bother to cache values, but at least check for validity
    if ((m_type == TYPEdouble) || (m_type == TYPEreal)) {
      try {
        double minFloat = facilities::Util::stringToDouble(min);
        double maxFloat = facilities::Util::stringToDouble(max);
        ret =  (minFloat < maxFloat);
      }
      catch (facilities::WrongType ex) {
        std::cerr << "Error detected in XercesBuilder::buildDatatype " 
                  << std::endl;
        std::cerr << ex.getMsg() << std::endl;
        return false;
      }
    }
    if (m_type == TYPEdatetime) {
      try {
        facilities::Timestamp minTime(min);
        facilities::Timestamp maxTime(max);
        ret =  (minTime < maxTime);
      }
      catch (facilities::BadTimeInput ex) {
        std::cerr << "From rdbModel::Datatype::setInterval" << std::endl;
        std::cerr << ex.complaint << std::endl;
        return false;
      }
    }
    return ret;
  }

  bool Datatype::getInterval(std::string& min, std::string& max) {
    if (m_restrict == RESTRICTinterval) {
      min = m_min;
      max = m_max;
      return true;
    }
    return false;
  }


  bool Datatype::okValue(const std::string& val) const {
    using facilities::Util;

    switch (m_type) {
    case TYPEreal:
    case TYPEdouble: {
      double doubleVal;
      try {
        doubleVal = Util::stringToDouble(val);
      }
      catch (facilities::WrongType ex) {
        return false;
      }
      if (m_restrict == RESTRICTnonneg) return (doubleVal  >= 0.0 );
      else if (m_restrict == RESTRICTpos) return (doubleVal > 0.0);
      else if (m_restrict == RESTRICTinterval) {
        double min = Util::stringToDouble(m_min);
        double max = Util::stringToDouble(m_max);
        return ((min <= doubleVal) && (doubleVal <= max));
      }
      return true;

    }

    case TYPEint:
    case TYPEmediumint:
    case TYPEsmallint: {
      int intVal;

      try {
        intVal = Util::stringToInt(val);
      }
      catch (facilities::WrongType ex) {
        return false;
      }
      return ((intVal >= m_minInt) && (intVal <= m_maxInt));
    }
    case TYPEvarchar:
    case TYPEchar:
      if (m_restrict == RESTRICTnone) return true;
      // for now, don't attempt to parse file path
      if (m_restrict == RESTRICTfile) return true;  
      if (!m_enum->choicesRequired()) return true;
    case TYPEenum: {
      unsigned nChoice = m_enum->getChoices().size();
      for (unsigned i = 0; i < nChoice; i++) {
        if (val == m_enum->getChoices()[i]) return true;
      }
      return false;
    }
    case TYPEdatetime:
    case TYPEtimestamp: {
      try {
        facilities::Timestamp aTime(val);
        if (m_restrict == RESTRICTinterval) {
          facilities::Timestamp min(m_min);
          facilities::Timestamp max(m_max);
          return ((min <= aTime) && (aTime <= max));
        }
        return true;
      }
      catch (facilities::BadTimeInput ex) {
        std::cerr << "From rdbModel::Datatype::okValue" << std::endl;
        std::cerr << ex.complaint << std::endl;
        return false;
      }

    }
    default:
      return false;
    }
  }

  bool Datatype::isCompatible(const Datatype* other) const {
    // The ten distinct types can be partitioned into 4 sorts: integer,
    // real, character, and date.  Call two types compatible if they're
    // in the same partition.
    return (findTOT(m_type) == findTOT(other->m_type));
  }
}
