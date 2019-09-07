#ifndef NET_DATA_READER_H
#define NET_DATA_READER_H

#include "NetDataReader/DimRpcReader.h"
#include "DistBossUtil/PthrReaderBufPool.h"

typedef PthrReaderBufPool<DimRpcReader, 4> NetDataReader;

#endif
