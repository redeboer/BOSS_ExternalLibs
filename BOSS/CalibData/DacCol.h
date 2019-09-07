// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/DacCol.h,v 1.1.1.1 2005/10/17 06:13:32 maqm Exp $
#ifndef CalibData_DacCol_h
#define CalibData_DacCol_h
#include <vector>
namespace CalibData {

  /**   @class DacCol
      Keeps track of a collection of dac settings, typically used
      for collecting calibration data for a single range
   */
  class DacCol {

  public:
    DacCol(std::vector<unsigned>* vals=0);
    DacCol(std::vector<int>* vals); 
    ~DacCol() {}

    const std::vector<unsigned>* getDacs() const {return &m_dacs;}
    /// Deep copy.
    void update(const DacCol* other);

  private:
    std::vector<unsigned> m_dacs;
    
  };

}
#endif
