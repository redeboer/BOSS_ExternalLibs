#ifndef HOUGHGLOBAL_H
#define HOUGHGLOBAL_H
#include "MdcRawEvent/MdcDigi.h"
#include "MdcGeom/MdcDetector.h"
#include <vector>

//typedef std::vector<MdcDigi*> MdcDigiVec;

namespace Global{
//extern const MdcDetector* m_gm;
extern const MdcDetector* m_gm;
}

enum HoughTransformType{
  STANDERD 
};

enum detectorType{
  MDC, 
  CGEM
};

enum HoughHitType{
  DIGIWITHTRUTH, 
  MIDPOINT, 
  CRUDEDRIFT, 
  DRIFT2D,
  DRIFT3D
};
#endif
