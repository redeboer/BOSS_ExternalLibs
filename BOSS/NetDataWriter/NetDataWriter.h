#ifndef NET_DATA_WRITER_H
#define NET_DATA_WRITER_H

#include "NetDataWriter/DimRpcWriter.h"
#include "DistBossUtil/PthrWriterBufPool.h"

typedef PthrWriterBufPool<DimRpcWriter, 4> NetDataWriter;

#endif
