#include "RawEvent/RawDataUtil.h"

const double RawDataUtil::MDC_TIME_FACTOR = 0.09375;
const double RawDataUtil::MDC_CHARGE_FACTOR = 1.;

// const double RawDataUtil::TOF_TIME_FACTOR = 0.00293;
const double RawDataUtil::TOF_TIME_FACTOR = 12000./499.8/8./1024.;
const double RawDataUtil::TOF_CHARGE_FACTOR = 0.3; 

const double RawDataUtil::EMC_CHARGE_HIGH_MEASURE = 2500.;
const double RawDataUtil::EMC_CHARGE_MID_MEASURE = 625.;
const double RawDataUtil::EMC_CHARGE_LOW_MEASURE = 78.;
const double RawDataUtil::EMC_CHARGE_FACTOR = 1024;
const double RawDataUtil::EMC_TIME_FACTOR = 1;

