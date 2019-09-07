#ifndef MAGFIELDREADER_H 
#define MAGFIELDREADER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimerSvc.h"

// Forward references
class IMagneticFieldSvc;

/** @class MagFieldReader MagFieldReader.h
 *  @param An Algorithm to read and plot magnetic filed maps
 *  @param for x and y kept constant and varying z. The x, y
 *  @param positions and the z range can be set in job options.
 *
 */

class MagFieldReader : public Algorithm {
public:
  /// Standard constructor
  MagFieldReader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MagFieldReader( ){ }; ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  StatusCode readField();
  
  // Pointer to the magnetic field service
  IMagneticFieldSvc* m_pIMF;

  // range and steps in z to get the magnetic field 
  double m_zMin, m_zMax, m_step;

  // range in x and y
  double m_xMin, m_xMax, m_yMin, m_yMax;

  NTuple::Tuple*        m_ntuple;
  NTuple::Tuple*        m_tuple1;
  NTuple::Tuple*        m_tuple2;
  NTuple::Tuple*        m_tuple3;
  NTuple::Tuple*        m_tuple4;
  NTuple::Item<float>   m_x, m_y, m_z, m_Bx, m_By, m_Bz, m_r;
  NTuple::Item<float>   m_x2, m_y2, m_z2, m_Bx2, m_By2, m_Bz2, m_r2;
  NTuple::Item<float>   m_x3, m_y3, m_z3, m_Bx3, m_By3, m_Bz3, m_r3, m_phi3;
  NTuple::Item<float> m_sigma_bx,m_sigma_by,m_sigma_bz;  
  NTuple::Item<float> m_time;

  std::string m_filename;

  IBesTimerSvc* m_timersvc;
  BesTimer* m_timer;
};
#endif // MAGFIELDREADER_H
