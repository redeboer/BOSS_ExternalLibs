// $Header: /bes/bes/BossCvs/Calibration/calibUtil/calibUtil/ChannelStatusDef.h,v 1.1.1.1 2005/10/17 06:12:26 maqm Exp $ 
#ifndef CALIBUTIL_CHANNELSTATUSDEF_H
#define CALIBUTIL_CHANNELSTATUSDEF_H

#define eCALIBUTIL_nOnbdCalib  0
#define eCALIBUTIL_nOnbdTrig   1
#define eCALIBUTIL_nOnbdData   2

#define vCALIBUTIL_nOnbdCalib (1 << eCALIBUTIL_nOnbdCalib)
#define vCALIBUTIL_nOnbdTrig  (1 << eCALIBUTIL_nOnbdTrig)
#define vCALIBUTIL_nOnbdData  (1 << eCALIBUTIL_nOnbdData)

#endif
